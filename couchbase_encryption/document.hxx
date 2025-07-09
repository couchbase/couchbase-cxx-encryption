/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * Copyright (c) 2025 Couchbase, Inc.
 *
 * Use of this software is subject to the Couchbase Inc. Enterprise Subscription License Agreement
 * v7 which may be found at https://www.couchbase.com/ESLA01162020.
 */

#pragma once

#include <couchbase_encryption/encrypted_fields.hxx>
#include <couchbase_encryption/manager.hxx>

#include <optional>
#include <string>
#include <vector>

namespace couchbase::crypto
{
template<typename DocumentType>
class document
{
public:
  auto with_encrypted_field(std::vector<std::string> field_path,
                            std::optional<std::string> encryptor_alias = {}) -> document&
  {
    encrypted_fields_.emplace_back(
      encrypted_field{ std::move(field_path), std::move(encryptor_alias) });
    return *this;
  }

  auto clear_encrypted_fields() -> document&
  {
    encrypted_fields_.clear();
    return *this;
  }

  [[nodiscard]] auto content() const -> const DocumentType&
  {
    return content_;
  }

  [[nodiscard]] auto encrypted_fields() const -> const std::vector<encrypted_field>&
  {
    return encrypted_fields_;
  }

  static auto from(DocumentType doc) -> document
  {
    std::vector<encrypted_field> fields_to_encrypt{};
    if constexpr (has_encrypted_fields_v<DocumentType>) {
      fields_to_encrypt = DocumentType::encrypted_fields;
    }

    return document{ std::move(doc), std::move(fields_to_encrypt) };
  }

private:
  document(DocumentType content, std::vector<encrypted_field> encrypted_fields)
    : content_{ std::move(content) }
    , encrypted_fields_{ std::move(encrypted_fields) }
  {
  }

  DocumentType content_;
  std::vector<encrypted_field> encrypted_fields_{};
};
} // namespace couchbase::crypto
