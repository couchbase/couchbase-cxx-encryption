/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * Copyright (c) 2025 Couchbase, Inc.
 *
 * Use of this software is subject to the Couchbase Inc. Enterprise Subscription License Agreement
 * v7 which may be found at https://www.couchbase.com/ESLA01162020.
 */

#include <couchbase/logger.hxx>

#include <spdlog/details/os.h>
#include <spdlog/spdlog.h>

namespace test::utils
{
namespace
{
auto
level_from_str(const std::string& str) -> couchbase::logger::log_level
{
  switch (spdlog::level::from_str(str)) {
  case spdlog::level::level_enum::trace:
    return couchbase::logger::log_level::trace;
  case spdlog::level::level_enum::debug:
    return couchbase::logger::log_level::debug;
  case spdlog::level::level_enum::info:
    return couchbase::logger::log_level::info;
  case spdlog::level::level_enum::warn:
    return couchbase::logger::log_level::warn;
  case spdlog::level::level_enum::err:
    return couchbase::logger::log_level::error;
  case spdlog::level::level_enum::critical:
    return couchbase::logger::log_level::critical;
  case spdlog::level::level_enum::off:
    return couchbase::logger::log_level::off;
  default:
    break;
  }
  // return highest level if we don't understand
  return couchbase::logger::log_level::trace;
}
} // namespace

void
init_logger()
{
  static bool initialized = false;

  if (!initialized) {
    couchbase::logger::initialize_console_logger();
    if (auto env_val = spdlog::details::os::getenv("TEST_LOG_LEVEL"); !env_val.empty()) {
      couchbase::logger::set_level(level_from_str(env_val));
    }

    initialized = true;
  }
}
} // namespace test::utils
