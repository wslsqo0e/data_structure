#include "log_spd.h"

int main(int argc, char* argv[])
{
  LOG_SET_LEVEL(level_trace);
  // LOG_WARN("hello world, it's a warn");
  SPDLOG_WARN("HELLO WORLD");
  SPDLOG_ERROR("HELLO WORLD");
  SPDLOG_TRACE("Hello world");
  SPDLOG_DEBUG("Hello world");
  return 0;
}
