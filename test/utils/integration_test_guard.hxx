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

#include "test_context.hxx"

#include <couchbase/cluster.hxx>

#include <optional>
#include <string>
#include <vector>

namespace test::utils
{

class integration_test_guard
{
public:
  integration_test_guard();
  explicit integration_test_guard(const couchbase::cluster_options& opts);
  explicit integration_test_guard(const std::shared_ptr<couchbase::crypto::manager>& crypto_manager);
  ~integration_test_guard();

  [[nodiscard]] auto cluster_version() const -> server_version;

  test_context ctx;
  couchbase::cluster cluster;
};
} // namespace test::utils
