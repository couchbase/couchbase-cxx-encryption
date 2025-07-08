/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * Copyright (c) 2025 Couchbase, Inc.
 *
 * Use of this software is subject to the Couchbase Inc. Enterprise Subscription License Agreement
 * v7 which may be found at https://www.couchbase.com/ESLA01162020.
 */

#include "test_context.hxx"

#include <spdlog/details/os.h>

#include <cstring>

namespace test::utils
{
auto
test_context::load_from_environment() -> test_context
{
  test_context ctx{};

  if (auto var = spdlog::details::os::getenv("TEST_CONNECTION_STRING"); !var.empty()) {
    ctx.connection_string = var;
  }

  if (auto var = spdlog::details::os::getenv("TEST_USERNAME"); !var.empty()) {
    ctx.username = var;
  }

  if (auto var = spdlog::details::os::getenv("TEST_PASSWORD"); !var.empty()) {
    ctx.password = var;
  }

  if (auto var = spdlog::details::os::getenv("TEST_CERTIFICATE_PATH"); !var.empty()) {
    ctx.certificate_path = var;
  }

  if (auto var = spdlog::details::os::getenv("TEST_KEY_PATH"); !var.empty()) {
    ctx.key_path = var;
  }

  if (auto var = spdlog::details::os::getenv("TEST_BUCKET"); !var.empty()) {
    ctx.bucket = var;
  }

  if (auto var = spdlog::details::os::getenv("OTHER_TEST_BUCKET"); !var.empty()) {
    ctx.other_bucket = var;
  }

  if (auto var = spdlog::details::os::getenv("TEST_DNS_NAMESERVER"); !var.empty()) {
    ctx.dns_nameserver = var;
  }

  if (auto var = spdlog::details::os::getenv("TEST_DNS_PORT"); !var.empty()) {
    ctx.dns_port = std::stol(var);
  }

  if (auto var = spdlog::details::os::getenv("TEST_DEPLOYMENT_TYPE"); !var.empty()) {
    if (var == "on_prem") {
      ctx.deployment = deployment_type::on_prem;
    } else if (var == "capella") {
      ctx.deployment = deployment_type::capella;
    }
  }

  if (auto var = spdlog::details::os::getenv("TEST_SERVER_VERSION"); !var.empty()) {
    ctx.version = server_version::parse(var, ctx.deployment);
  }

  if (auto var = spdlog::details::os::getenv("TEST_DEVELOPER_PREVIEW"); !var.empty()) {
    if (var == "true" || var == "yes" || var == "1") {
      ctx.version.developer_preview = true;
    } else if (var == "false" || var == "no" || var == "0") {
      ctx.version.developer_preview = false;
    }
  }

  if (auto var = spdlog::details::os::getenv("TEST_NUMBER_OF_IO_THREADS"); !var.empty()) {
    ctx.number_of_io_threads = std::stoul(var);
  }

  if (auto var = spdlog::details::os::getenv("TEST_USE_GOCAVES"); !var.empty()) {
    if (var == "true" || var == "yes" || var == "1") {
      ctx.version.use_gocaves = true;
    } else if (var == "false" || var == "no" || var == "0") {
      ctx.version.use_gocaves = false;
    }
  }

  if (auto var = spdlog::details::os::getenv("TEST_USE_WAN_DEVELOPMENT_PROFILE"); !var.empty()) {
    if (var == "true" || var == "yes" || var == "1") {
      ctx.use_wan_development_profile = true;
    } else if (var == "false" || var == "no" || var == "0") {
      ctx.use_wan_development_profile = false;
    }
  }

  // Always use WAN profile for Capella
  if (ctx.deployment == deployment_type::capella) {
    ctx.use_wan_development_profile = true;
  }

  return ctx;
}

auto
test_context::build_options() const -> couchbase::cluster_options
{
  auto options{ [this] {
    if (certificate_path.empty()) {
      return couchbase::cluster_options{
        couchbase::password_authenticator{ username, password },
      };
    }
    return couchbase::cluster_options{
      couchbase::certificate_authenticator{ certificate_path, key_path },
    };
  }() };

  if (use_wan_development_profile) {
    options.apply_profile("wan_development");
  }
  if (dns_nameserver.has_value() && dns_port.has_value()) {
    options.dns().nameserver(dns_nameserver.value(), dns_port.value());
  } else if (dns_nameserver.has_value()) {
    options.dns().nameserver(dns_nameserver.value());
  }

  return options;
}
} // namespace test::utils
