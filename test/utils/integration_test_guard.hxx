/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * Copyright (c) 2025 Couchbase, Inc.
 *
 * Use of this software is subject to the Couchbase Inc. Enterprise Subscription License Agreement
 * v7 which may be found at https://www.couchbase.com/ESLA01162020.
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
  explicit integration_test_guard(
    const std::shared_ptr<couchbase::crypto::manager>& crypto_manager);
  ~integration_test_guard();

  [[nodiscard]] auto cluster_version() const -> server_version;

  test_context ctx;
  couchbase::cluster cluster;
};
} // namespace test::utils
