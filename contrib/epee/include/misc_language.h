#pragma once

#include <limits>
#include <boost/thread.hpp>
#include <boost/utility/value_init.hpp>
namespace epee
{
#define STD_TRY_BEGIN() try {

#define STD_TRY_CATCH(where_, ret_val) \
	} \
	catch (const std::exception  &e) \
	{ \
		LOG_ERROR("EXCEPTION: " << where_  << ", mes: "<< e.what());  \
		return ret_val; \
	} \
	catch (...) \
	{ \
		LOG_ERROR("EXCEPTION: " << where_ ); \
		return ret_val; \
	}



#define AUTO_VAL_INIT(v)   boost::value_initialized<decltype(v)>()

namespace misc_utils
{
	template<typename t_type>
		t_type get_max_t_val(t_type t)
		{
			return (std::numeric_limits<t_type>::max)();
		}

		
	template<typename t_iterator>
		t_iterator move_it_forward(t_iterator it, size_t count)
		{
			while(count--)
				it++;
			return it;
		}

    template<typename t_iterator>
    t_iterator move_it_backward(t_iterator it, size_t count)
    {
      while(count--)
        it--;
      return it;
    }


	// TEMPLATE STRUCT less
	template<class _Ty>
	struct less_as_pod
		: public std::binary_function<_Ty, _Ty, bool>
	{	// functor for operator<
		bool operator()(const _Ty& _Left, const _Ty& _Right) const
		{	// apply operator< to operands
			return memcmp(&_Left, &_Right, sizeof(_Left)) < 0;
		}
	};

  template<class _Ty>
  bool is_less_as_pod(const _Ty& _Left, const _Ty& _Right)
  {	// apply operator< to operands
      return memcmp(&_Left, &_Right, sizeof(_Left)) < 0;
  }
	

	inline
	bool sleep_no_w(long ms )
	{
		boost::this_thread::sleep( 
			boost::get_system_time() + 
			boost::posix_time::milliseconds( std::max<long>(ms,0) ) );
		
		return true;
	}

  template<class type_vec_type>
  type_vec_type median(std::vector<type_vec_type> &v)
  {
    if(v.empty())
      return boost::value_initialized<type_vec_type>();
    if(v.size() == 1)
      return v[0];

    size_t n = (v.size()) / 2;
    std::sort(v.begin(), v.end());
    //nth_element(v.begin(), v.begin()+n-1, v.end());
    if(v.size()%2)
    {//1, 3, 5...
      return v[n];
    }else 
    {//2, 4, 6...
      return (v[n-1] + v[n])/2;
    }

  }

  /************************************************************************/
  /*                                                                      */
  /************************************************************************/

  struct call_befor_die_base
  {
    virtual ~call_befor_die_base(){}
  };

  typedef boost::shared_ptr<call_befor_die_base> auto_scope_leave_caller;


  template<class t_scope_leave_handler>
  struct call_befor_die: public call_befor_die_base
  {
    t_scope_leave_handler m_func;
    call_befor_die(t_scope_leave_handler f):m_func(f)
    {}
    ~call_befor_die()
    {
      m_func();
    }
  };

  template<class t_scope_leave_handler>
  auto_scope_leave_caller create_scope_leave_handler(t_scope_leave_handler f)
  {
    auto_scope_leave_caller slc(new call_befor_die<t_scope_leave_handler>(f));
    return slc;
  }

}
}
