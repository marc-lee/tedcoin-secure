#pragma once 
#include <cstddef>

namespace boost
{
  namespace serialization
  {


    template<class archive_t>
    void serialize(archive_t & ar, cryptonote::blockchain_storage::transaction_chain_entry& te, const unsigned int version)
    {
      ar & te.tx;
      ar & te.m_keeper_block_height;
      size_t dummy;
      ar & dummy; // formerly te.m_blob_size;
      ar & te.m_global_output_indexes;
    }

    template<class archive_t>
    void serialize(archive_t & ar, cryptonote::blockchain_storage::block_extended_info& ei, const unsigned int version)
    {
      ar & ei.bl;
      ar & ei.height;
      ar & ei.cumulative_difficulty;
      ar & ei.block_cumulative_size;
      ar & ei.already_generated_coins;
    }

  }
}
