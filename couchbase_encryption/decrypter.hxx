/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * Copyright (c) 2025 Couchbase, Inc.
 *
 * Use of this software is subject to the Couchbase Inc. Enterprise Subscription License Agreement
 * v7 which may be found at https://www.couchbase.com/ESLA01162020.
 */

#pragma once

#include <couchbase_encryption/encryption_result.hxx>
#include <couchbase/error.hxx>

#include <string>
#include <utility>
#include <vector>

namespace couchbase::crypto
{
class decrypter
{
public:
  decrypter() = default;
  decrypter(const decrypter& other) = default;
  decrypter(decrypter&& other) = default;
  auto operator=(const decrypter& other) -> decrypter& = default;
  auto operator=(decrypter&& other) -> decrypter& = default;
  virtual ~decrypter() = default;

  [[nodiscard]] virtual auto decrypt(encryption_result encrypted)
    -> std::pair<error, std::vector<std::byte>> = 0;
  [[nodiscard]] virtual auto algorithm() const -> const std::string& = 0;
};
} // namespace couchbase::crypto
