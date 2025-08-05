/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * Copyright (c) 2025 Couchbase, Inc.
 *
 * Use of this software is subject to the Couchbase Inc. Enterprise Subscription License Agreement
 * v7 which may be found at https://www.couchbase.com/ESLA01162020.
 */

#pragma once

#include <optional>
#include <string>
#include <vector>

namespace couchbase::crypto
{
/**
 * Represents an individual field that should be encrypted in a document.
 *
 * @note Only fields of JSON objects can be encrypted.
 */
struct encrypted_field {
  /**
   * The path to the field that should be encrypted, as it appears in the serialized document
   * e.g. {"address", "street"}.
   *
   * @since 1.0.0
   * @committed
   */
  std::vector<std::string> field_path;

  /**
   * The alias of the encrypter that should be used to encrypt the field.
   *
   * The encrypter_alias refers to the alias a couchbase::crypto::encrypter was registered with
   * the couchbase::crypto::manager. If no encrypter alias is specified, the default encrypter is
   * used.
   *
   * @since 1.0.0
   * @committed
   */
  std::optional<std::string> encrypter_alias{};

  auto operator==(const encrypted_field& other) const -> bool
  {
    return field_path == other.field_path && encrypter_alias == other.encrypter_alias;
  }
};

template<typename Document, typename = void>
struct has_encrypted_fields : std::false_type {
};

template<typename Document>
struct has_encrypted_fields<Document, std::void_t<decltype(Document::encrypted_fields)>>
  : std::is_same<decltype(Document::encrypted_fields),
                 const std::vector<couchbase::crypto::encrypted_field>> {
};

template<typename Document>
constexpr bool has_encrypted_fields_v = has_encrypted_fields<Document>::value;

} // namespace couchbase::crypto
