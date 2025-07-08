/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * Copyright (c) 2025 Couchbase, Inc.
 *
 * Use of this software is subject to the Couchbase Inc. Enterprise Subscription License Agreement
 * v7 which may be found at https://www.couchbase.com/ESLA01162020.
 */

#include <couchbase_encryption/document.hxx>

namespace couchbase::crypto
{
document::document(codec::binary raw, std::vector<encrypted_field> encrypted_fields)
  : raw_{ std::move(raw) }
  , encrypted_fields_{ std::move(encrypted_fields) }
{
}

void
document::add_encrypted_field(std::vector<std::string> path,
                              std::optional<std::string> encrypter_alias)
{
  encrypted_fields_.emplace_back(encrypted_field{ std::move(path), std::move(encrypter_alias) });
}

auto
document::encrypted_fields() const -> const std::vector<encrypted_field>&
{
  return encrypted_fields_;
}

auto
document::raw() const -> const codec::binary&
{
  return raw_;
}
} // namespace couchbase::crypto
