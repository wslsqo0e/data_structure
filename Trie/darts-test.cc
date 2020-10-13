#include <stdio.h>
#include "str_darts.h"

int main(int argc, char* argv[])
{
  char* key[16];
  key[0] = "a";
  key[1] = "ab";
  key[2] = "ac";
  key[3] = "ace";
  key[4] = "aced";
  key[5] = "af";
  key[6] = "ag";
  key[7] = "ah";
  key[8] = "ai";
  key[9] = "aj";
  key[10] = "az";
  // key需要是字典序

  DARTS::DoubleArray _array;
  int ret = _array.build(11, key);
  LOG_ASSERT( ret == 0);

  int index = _array.exactMatchSearch<DARTS::DoubleArray::result_type>("aced");
  LOG_ASSERT(index == 4);

  DARTS::TrieDarts trie_array(11, key);
  ret = trie_array.build();
  LOG_ASSERT(ret == 0);

  int rr = trie_array.exactMatchSearch("aced");
  LOG_ASSERT(rr == 4);

  int rt[5];
  int rc = _array.commonPrefixSearch("acedbbc", rt, 4);
  LOG_ASSERT(rc == 4);
  LOG_ASSERT(rt[0] == 0);
  LOG_ASSERT(rt[1] == 2);
  LOG_ASSERT(rt[2] == 3);
  LOG_ASSERT(rt[3] == 4);

  DARTS::StrDarts str_darts;
  std::vector<std::string> dict = {"the", "thenland"};
  str_darts.init(dict);
  ret = str_darts.build();
  LOG_ASSERT(ret == 0);
  std::vector<std::string> token;
  bool is_word;
  token = {"t", "h"};
  int end = str_darts.contain_tokens(token, is_word);
  LOG_ASSERT(end == 2);
  LOG_ASSERT(is_word == false);
  token = {"t", "h", "e"};
  end = str_darts.contain_tokens(token, is_word);
  LOG_ASSERT(end == 3);
  LOG_ASSERT(is_word == true);
  token = {"t", "h", "e", "n", "l", "b"};
  end = str_darts.contain_tokens(token, is_word);
  LOG_ASSERT(end == 5);
  LOG_ASSERT(is_word == false);

  printf("save\n");
  rr = str_darts.save("T_Darts");
  printf("save\n");
  LOG_ASSERT(rr == 0);

  DARTS::StrDarts load_str_darts;
  printf("load\n");
  load_str_darts.load("T_Darts");
  printf("load\n");
  token = {"t", "h"};
  end = load_str_darts.contain_tokens(token, is_word);
  LOG_ASSERT(end == 2);
  LOG_ASSERT(is_word == false);
  token = {"t", "h", "e"};
  end = load_str_darts.contain_tokens(token, is_word);
  LOG_ASSERT(end == 3);
  LOG_ASSERT(is_word == true);
  token = {"t", "h", "e", "n", "l", "b"};
  end = load_str_darts.contain_tokens(token, is_word);
  LOG_ASSERT(end == 5);
  LOG_ASSERT(is_word == false);
  return 0;
}
