/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * Copyright (c) 2025 Couchbase, Inc.
 *
 * Use of this software is subject to the Couchbase Inc. Enterprise Subscription License Agreement
 * v7 which may be found at https://www.couchbase.com/ESLA01162020.
 */

#pragma once

#include <couchbase/codec/encoded_value.hxx>
#include <couchbase_encryption/encrypted_fields.hxx>
#include <couchbase_encryption/manager.hxx>
#include <couchbase/error.hxx>

namespace couchbase::crypto
{
class document
{
public:
  explicit document(codec::binary raw, std::vector<encrypted_field> encrypted_fields = {});

  void add_encrypted_field(std::vector<std::string> path,
                           std::optional<std::string> encrypter_alias = {});

  [[nodiscard]] auto raw() const -> const codec::binary&;
  [[nodiscard]] auto encrypted_fields() const -> const std::vector<encrypted_field>&;

private:
  codec::binary raw_;
  std::vector<encrypted_field> encrypted_fields_{};
};
} // namespace couchbase::crypto
