/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * Copyright (c) 2025 Couchbase, Inc.
 *
 * Use of this software is subject to the Couchbase Inc. Enterprise Subscription License Agreement
 * v7 which may be found at https://www.couchbase.com/ESLA01162020.
 */

#pragma once

#include <couchbase/codec/codec_flags.hxx>
#include <couchbase/codec/encoded_value.hxx>
#include <couchbase_encryption/document.hxx>
#include <couchbase_encryption/manager.hxx>
#include <couchbase/error_codes.hxx>

namespace couchbase::crypto
{
namespace internal
{
auto
encrypt(const codec::binary& raw,
        const std::vector<encrypted_field>& encrypted_fields,
        const std::shared_ptr<manager>& crypto_manager) -> std::pair<error, codec::binary>;

auto
decrypt(const codec::binary& encrypted, const std::shared_ptr<manager>& crypto_manager)
  -> std::pair<error, codec::binary>;
} // namespace internal

template<typename Serializer>
class transcoder
{
public:
  template<typename DocumentType>
  static auto encode(const document<DocumentType>& document,
                     const std::shared_ptr<manager>& crypto_manager) -> codec::encoded_value
  {
    if (crypto_manager == nullptr) {
      throw std::system_error(errc::field_level_encryption::generic_cryptography_failure,
                              "crypto manager is not set, cannot use transcoder with FLE");
    }
    auto data = Serializer::serialize(document.content());

    auto [err, encrypted_data] =
      internal::encrypt(data, document.encrypted_fields(), crypto_manager);
    if (err) {
      throw std::system_error(err.ec(), "Failed to encrypt document: " + err.message());
    }
    return { std::move(encrypted_data), codec::codec_flags::json_common_flags };
  }

  template<typename Document>
  static auto encode(Document document, const std::shared_ptr<manager>& crypto_manager)
    -> codec::encoded_value
  {
    if (crypto_manager == nullptr) {
      throw std::system_error(errc::field_level_encryption::generic_cryptography_failure,
                              "crypto manager is not set, cannot use transcoder with FLE");
    }
    auto data = Serializer::serialize(document);

    static const std::vector<encrypted_field> no_fields_to_encrypt{};
    constexpr const std::vector<encrypted_field>& encrypted_fields =
      has_encrypted_fields_v<Document> ? Document::encrypted_fields : no_fields_to_encrypt;

    auto [err, encrypted_data] = internal::encrypt(data, encrypted_fields, crypto_manager);
    if (err) {
      throw std::system_error(err.ec(), "Failed to encrypt document: " + err.message());
    }
    return { std::move(encrypted_data), codec::codec_flags::json_common_flags };
  }

  template<typename Document>
  static auto decode(const codec::encoded_value& encoded,
                     const std::shared_ptr<manager>& crypto_manager) -> Document
  {
    if (crypto_manager == nullptr) {
      throw std::system_error(errc::field_level_encryption::generic_cryptography_failure,
                              "crypto manager is not set, cannot use transcoder with FLE");
    }
    if (!codec::codec_flags::has_common_flags(encoded.flags,
                                              codec::codec_flags::json_common_flags)) {
      throw std::system_error(
        errc::common::decoding_failure,
        "crypto::transcoder expects document to have JSON common flags, flags=" +
          std::to_string(encoded.flags));
    }

    auto [err, decrypted_data] = internal::decrypt(encoded.data, crypto_manager);
    if (err) {
      throw std::system_error(err.ec(), "Failed to decrypt document: " + err.message());
    }
    return Serializer::template deserialize<Document>(decrypted_data);
  }
};
} // namespace couchbase::crypto
