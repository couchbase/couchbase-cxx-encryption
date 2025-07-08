/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * Copyright (c) 2025 Couchbase, Inc.
 *
 * Use of this software is subject to the Couchbase Inc. Enterprise Subscription License Agreement
 * v7 which may be found at https://www.couchbase.com/ESLA01162020.
 */

#pragma once

#include <string>
#include <vector>

namespace couchbase::crypto
{
class key
{
public:
  key() = default;
  key(std::string id, std::vector<std::byte> bytes);

  [[nodiscard]] auto id() const -> const std::string&;
  [[nodiscard]] auto bytes() const -> const std::vector<std::byte>&;

private:
  std::string id_;
  std::vector<std::byte> bytes_;
};
} // namespace couchbase::crypto
