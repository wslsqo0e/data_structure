#include "Logger/log_spd.h"
#include "base_utils.h"

void test_Q2B_B2Q() {
  std::string input = "a b c";
  std::string output = B2Q(input);
  LOG_ASSERT(output == "ａ　ｂ　ｃ");
  input = Q2B(output);
  LOG_ASSERT(input == "a b c");
}

int main(int argc, char* argv[]) {
  test_Q2B_B2Q();
}
