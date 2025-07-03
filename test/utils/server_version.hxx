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

#pragma once

#include <cstdlib>
#include <string>

namespace test::utils
{

enum class server_edition {
  unknown,
  enterprise,
  community,
  columnar
};

enum class deployment_type {
  on_prem,
  capella,
  elixir
};

enum class server_config_profile {
  unknown,
  serverless
};

struct server_version {
  unsigned long major{ 0 };
  unsigned long minor{ 0 };
  unsigned long micro{ 0 };
  unsigned long build{ 0 };
  bool developer_preview{ false };
  server_edition edition{ server_edition::unknown };
  deployment_type deployment{ deployment_type::on_prem };
  server_config_profile profile{ server_config_profile::unknown };
  bool use_gocaves{ false };

  static auto parse(const std::string& str, deployment_type deployment) -> server_version;
};

} // namespace test::utils
