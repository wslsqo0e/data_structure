#include "log_base.h"
#include "log_trival.h"

int g_log_level = 7;

void AssertFailure_(const char *func, const char *file, int32_t line, const char *cond_str)
{
  std::cerr << "Assertion failed: (" << cond_str << ")" << std::endl;
  fflush(NULL);
  std::abort();
}
