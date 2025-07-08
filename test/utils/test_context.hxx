/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * Copyright (c) 2025 Couchbase, Inc.
 *
 * Use of this software is subject to the Couchbase Inc. Enterprise Subscription License Agreement
 * v7 which may be found at https://www.couchbase.com/ESLA01162020.
 */
#pragma once

#include "server_version.hxx"

#include <couchbase/cluster_options.hxx>

#include <string>

namespace test::utils
{
struct test_context {
  std::string connection_string{ "couchbase://localhost" };
  std::string username{ "Administrator" };
  std::string password{ "password" };
  std::string certificate_path{};
  std::string key_path{};
  std::string bucket{ "default" };
  server_version version{ 6, 6, 0 };
  deployment_type deployment{ deployment_type::on_prem };
  std::optional<std::string> dns_nameserver{};
  std::optional<std::uint16_t> dns_port{};
  std::size_t number_of_io_threads{ 1 };
  std::string other_bucket{ "secBucket" };
  bool use_wan_development_profile{ false };

  [[nodiscard]] auto build_options() const -> couchbase::cluster_options;

  static auto load_from_environment() -> test_context;
};

} // namespace test::utils
