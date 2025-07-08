/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * Copyright (c) 2025 Couchbase, Inc.
 *
 * Use of this software is subject to the Couchbase Inc. Enterprise Subscription License Agreement
 * v7 which may be found at https://www.couchbase.com/ESLA01162020.
 */
#include "server_version.hxx"

#include <regex>

namespace test::utils
{
auto
server_version::parse(const std::string& str, const deployment_type deployment) -> server_version
{
  std::regex version_regex(R"((\d+).(\d+).(\d+)(-(\d+))?(-(.+))?)");
  std::smatch version_match{};
  server_version ver{};
  ver.deployment = deployment;
  if (std::regex_match(str, version_match, version_regex) && version_match.ready()) {
    ver.major = std::stoul(version_match[1]);
    ver.minor = std::stoul(version_match[2]);
    if (version_match.length(3) > 0) {
      ver.micro = std::stoul(version_match[3]);
      if (version_match.length(5) > 0) {
        ver.build = std::stoul(version_match[5]);
        if (version_match.length(7) > 0) {
          if (version_match[7] == "enterprise") {
            ver.edition = server_edition::enterprise;
          } else if (version_match[7] == "community") {
            ver.edition = server_edition::community;
          } else if (version_match[7] == "columnar") {
            ver.edition = server_edition::columnar;
          }
        }
      }
    }
  } else {
    ver.major = 6;
    ver.minor = 6;
    ver.micro = 0;
  }
  return ver;
}

auto
server_version::supports_feature(const server_feature feature) const -> bool
{
  switch (feature) {
    case server_feature::range_scan:
      return major > 7 || (major == 7 && minor >= 6);
    default:
      throw std::invalid_argument("Not known which version supports this feature");
  }
}
} // namespace test::utils
