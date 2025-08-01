/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * Copyright (c) 2025 Couchbase, Inc.
 *
 * Use of this software is subject to the Couchbase Inc. Enterprise Subscription License Agreement
 * v7 which may be found at https://www.couchbase.com/ESLA01162020.
 */

#pragma once

#include <cstdint>
#include <string>

namespace test::utils
{

enum class server_edition : std::uint8_t {
  unknown,
  enterprise,
  community,
  columnar
};

enum class deployment_type : std::uint8_t {
  on_prem,
  capella,
};

enum class server_feature : std::uint8_t {
  range_scan,
};

struct server_version {
  unsigned long major{ 0 };
  unsigned long minor{ 0 };
  unsigned long micro{ 0 };
  unsigned long build{ 0 };
  bool developer_preview{ false };
  server_edition edition{ server_edition::unknown };
  deployment_type deployment{ deployment_type::on_prem };
  bool use_gocaves{ false };

  static auto parse(const std::string& str, deployment_type deployment) -> server_version;

  [[nodiscard]] auto supports_feature(server_feature feature) const -> bool;
};

} // namespace test::utils
