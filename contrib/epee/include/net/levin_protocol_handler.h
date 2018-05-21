#ifndef _LEVIN_PROTOCOL_HANDLER_H_
#define _LEVIN_PROTOCOL_HANDLER_H_

#include <boost/uuid/uuid_generators.hpp>
#include "levin_base.h"

namespace epee
{
namespace levin
{
  template<class t_connection_context = net_utils::connection_context_base>
	struct protocl_handler_config
	{
		levin_commands_handler<t_connection_context>* m_pcommands_handler;
	};

  template<class t_connection_context = net_utils::connection_context_base>
	class protocol_handler
	{
	public:
    typedef t_connection_context connection_context;
		typedef protocl_handler_config<t_connection_context> config_type;

		protocol_handler(net_utils::i_service_endpoint* psnd_hndlr, config_type& config, t_connection_context& conn_context);
		virtual ~protocol_handler(){}

		virtual bool handle_recv(const void* ptr, size_t cb);

		bool after_init_connection(){return true;}
	private:
		enum connection_data_state
		{
			conn_state_reading_head,
			conn_state_reading_body
		};


		config_type& m_config;
		t_connection_context& m_conn_context;
		net_utils::i_service_endpoint* m_psnd_hndlr; 
		std::string m_cach_in_buffer;
		connection_data_state m_state;
		bucket_head m_current_head;
	};

  template<class t_connection_context>
	protocol_handler<t_connection_context>::protocol_handler(net_utils::i_service_endpoint* psnd_hndlr, config_type& config, t_connection_context& conn_context):
                  m_config(config), 
                  m_conn_context(conn_context),
                  m_psnd_hndlr(psnd_hndlr), 
                  m_state(conn_state_reading_head), 
                  m_current_head(bucket_head()) 
	{}

  template<class t_connection_context>
	bool protocol_handler<t_connection_context>::handle_recv(const void* ptr, size_t cb)
	{
		if(!m_config.m_pcommands_handler)
		{
			LOG_ERROR("Command handler not set!");
			return false;
		}
		m_cach_in_buffer.append((const char*)ptr, cb);

		bool is_continue = true;
		while(is_continue)
		{
			switch(m_state)
			{
			case conn_state_reading_head:
				if(m_cach_in_buffer.size() < sizeof(bucket_head))
				{
					if(m_cach_in_buffer.size() >= sizeof(uint64_t) && *((uint64_t*)m_cach_in_buffer.data()) != LEVIN_SIGNATURE)
					{
						LOG_ERROR("Signature missmatch on accepted connection");
						return false;
					}
					is_continue = false;
					break;
				}
				{
					bucket_head* phead = (bucket_head*)m_cach_in_buffer.data();
					if(LEVIN_SIGNATURE != phead->m_signature)
					{
						LOG_ERROR("Signature missmatch on accepted connection");
						return false;
					}
					m_current_head = *phead;
				}
				m_cach_in_buffer.erase(0, sizeof(bucket_head));
				m_state = conn_state_reading_body;
				break;
			case conn_state_reading_body:
				if(m_cach_in_buffer.size() < m_current_head.m_cb)
				{
					is_continue = false;
					break;
				}
				{
					std::string buff_to_invoke;
					if(m_cach_in_buffer.size()  == m_current_head.m_cb)
						buff_to_invoke.swap(m_cach_in_buffer);
					else
					{
						buff_to_invoke.assign(m_cach_in_buffer, 0, (std::string::size_type)m_current_head.m_cb);
						m_cach_in_buffer.erase(0, (std::string::size_type)m_current_head.m_cb);
					}


					if(m_current_head.m_have_to_return_data)
					{
						std::string return_buff;
						m_current_head.m_return_code = m_config.m_pcommands_handler->invoke(m_current_head.m_command, buff_to_invoke, return_buff, m_conn_context);
						m_current_head.m_cb = return_buff.size();
						m_current_head.m_have_to_return_data = false;
						std::string send_buff((const char*)&m_current_head, sizeof(m_current_head));
						send_buff += return_buff;

						if(!m_psnd_hndlr->do_send(send_buff.data(), send_buff.size()))
							return false;

					}
					else
						m_config.m_pcommands_handler->notify(m_current_head.m_command, buff_to_invoke, m_conn_context);
				}
				m_state = conn_state_reading_head;
				break;
			default:
				LOG_ERROR("Undefined state in levin_server_impl::connection_handler, m_state=" << m_state);
				return false;
			}
		}

		return true;
	}







}
}




#endif //_LEVIN_PROTOCOL_HANDLER_H_
