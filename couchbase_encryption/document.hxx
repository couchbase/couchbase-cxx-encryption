/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * Copyright (c) 2025 Couchbase, Inc.
 *
 * Use of this software is subject to the Couchbase Inc. Enterprise Subscription License Agreement
 * v7 which may be found at https://www.couchbase.com/ESLA01162020.
 */

#pragma once

#include <couchbase_encryption/encrypted_fields.hxx>

#include <optional>
#include <string>
#include <vector>

namespace couchbase::crypto
{
/**
 * Represents a document that can be encrypted using the Couchbase Field Level Encryption (FLE). It
 * acts as a wrapper around the underlying document type and allows specifying which fields should
 * be encrypted.
 *
 * It must be used in conjunction with a crypto manager and a crypto transcoder:
 * @see couchbase::crypto::manager
 * @see couchbase::crypto::default_manager
 * @see couchbase::crypto::transcoder
 * @see couchbase::crypto::default_transcoder
 *
 * It is an alternative to specifying the fields to be encrypted directly on a document type.
 * @see couchbase::crypto::has_encrypted_fields
 *
 * @tparam DocumentType the type of the underlying document that will be encrypted
 *
 * @note Documents cannot be deserialized directly into this type, as the encrypter aliases
 * that were used to encrypt each field cannot be determined from the encrypted document. Instead,
 * they should be deserialized into the underlying DocumentType.
 *
 * @since 1.0.0
 * @committed
 */
template<typename DocumentType>
class document
{
public:
  /**
   * Specifies that the field at the given path should be encrypted using the given encryptor alias.
   *
   * The encrypter_alias refers to the alias a couchbase::crypto::encrypter was registered with
   * the couchbase::crypto::manager. If no encrypter alias is specified, the default encrypter is
   * used.
   *
   * @param field_path the path to the field that should be encrypted as it appears in the
   * serialized document, e.g. {"address", "street"}
   * @param encrypter_alias the alias of the encrypter that should be used to encrypt the field
   * @return this document, for chaining purposes
   *
   * @since 1.0.0
   * @committed
   */
  auto with_encrypted_field(std::vector<std::string> field_path,
                            std::optional<std::string> encrypter_alias = {}) -> document&
  {
    encrypted_fields_.emplace_back(
      encrypted_field{ std::move(field_path), std::move(encrypter_alias) });
    return *this;
  }

  /**
   * Clears the list of fields that should be encrypted for this document.
   *
   * @return this document, for chaining purposes
   *
   * @since 1.0.0
   * @committed
   */
  auto clear_encrypted_fields() -> document&
  {
    encrypted_fields_.clear();
    return *this;
  }

  /**
   * Returns the unencrypted content of the document.
   *
   * @return content of the document
   *
   * @since 1.0.0
   * @committed
   */
  [[nodiscard]] auto content() const -> const DocumentType&
  {
    return content_;
  }

  /**
   * Returns the list of fields that should be encrypted for this document.
   *
   * @return fields to be encrypted
   *
   * @since 1.0.0
   * @committed
   */
  [[nodiscard]] auto encrypted_fields() const -> const std::vector<encrypted_field>&
  {
    return encrypted_fields_;
  }

  /**
   * Creates a new instance of a crypto document from the given document. This allows specifying
   * fields that should be encrypted on this document.
   *
   * @param doc the document to wrap in a crypto document
   * @return a new instance of a crypto document
   *
   * @since 1.0.0
   * @committed
   */
  static auto from(DocumentType doc) -> document
  {
    std::vector<encrypted_field> fields_to_encrypt{};
    if constexpr (has_encrypted_fields_v<DocumentType>) {
      fields_to_encrypt = DocumentType::encrypted_fields;
    }

    return document{ std::forward<DocumentType>(doc), std::move(fields_to_encrypt) };
  }

private:
  document(DocumentType doc, std::vector<encrypted_field> encrypted_fields)
    : content_(std::forward<DocumentType>(doc))
    , encrypted_fields_{ std::move(encrypted_fields) }
  {
  }

  DocumentType content_;
  std::vector<encrypted_field> encrypted_fields_{};
};
} // namespace couchbase::crypto
