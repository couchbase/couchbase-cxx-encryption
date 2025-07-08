/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * Copyright (c) 2025 Couchbase, Inc.
 *
 * Use of this software is subject to the Couchbase Inc. Enterprise Subscription License Agreement
 * v7 which may be found at https://www.couchbase.com/ESLA01162020.
 */

#pragma once

#include <map>
#include <optional>
#include <string>
#include <vector>

namespace couchbase::crypto
{
class encryption_result
{
public:
  encryption_result() = default;
  explicit encryption_result(std::string algorithm);
  explicit encryption_result(std::map<std::string, std::string> encrypted_node);

  [[nodiscard]] auto algorithm() const -> std::string;
  [[nodiscard]] auto get(const std::string& field_name) const -> std::optional<std::string>;
  [[nodiscard]] auto get_bytes(const std::string& field_name) const
    -> std::optional<std::vector<std::byte>>;
  [[nodiscard]] auto as_map() const -> std::map<std::string, std::string>;

  void put(std::string field_name, std::string value);
  void put(std::string field_name, std::vector<std::byte> value);

private:
  std::map<std::string, std::string> internal_;
};
} // namespace couchbase::crypto
