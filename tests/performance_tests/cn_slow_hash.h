#pragma once

#include "crypto/crypto.h"
#include "cryptonote_core/cryptonote_basic.h"

class test_cn_slow_hash
{
public:
  static const size_t loop_count = 10;

#pragma pack(push, 1)
  struct data_t
  {
    char data[13];
  };
#pragma pack(pop)

  static_assert(13 == sizeof(data_t), "Invalid structure size");

  bool init()
  {
    if (!epee::string_tools::hex_to_pod("63617665617420656d70746f72", m_data))
      return false;

    if (!epee::string_tools::hex_to_pod("bbec2cacf69866a8e740380fe7b818fc78f8571221742d729d9d02d7f8989b87", m_expected_hash))
      return false;

    return true;
  }

  bool test()
  {
    crypto::hash hash;
    crypto::cn_slow_hash_2m(&m_data, sizeof(m_data), hash);
    return hash == m_expected_hash;
  }

private:
  data_t m_data;
  crypto::hash m_expected_hash;
};
