/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 *   Copyright 2020-2021 Couchbase, Inc.
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
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
