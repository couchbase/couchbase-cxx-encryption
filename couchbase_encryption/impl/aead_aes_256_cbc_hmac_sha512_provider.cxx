/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 *     Copyright 2025. Couchbase, Inc.
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */

#include <couchbase_encryption/aead_aes_256_cbc_hmac_sha512_provider.hxx>

#include <couchbase/error_codes.hxx>
#include <couchbase/crypto/internal.hxx>

#include "utils/base64.h"

#include <spdlog/fmt/bundled/format.h>

#include <stdexcept>

namespace couchbase::crypto
{
aead_aes_256_cbc_hmac_sha512_provider::aead_aes_256_cbc_hmac_sha512_provider(
  std::shared_ptr<keyring> keyring)
  : keyring_(std::move(keyring))
{
}

auto
aead_aes_256_cbc_hmac_sha512_provider::encrypter_for_key(const std::string& key_id) const
  -> std::shared_ptr<encrypter>
{
  return std::make_shared<aead_aes_256_cbc_hmac_sha512_encrypter>(key_id, keyring_);
}

auto
aead_aes_256_cbc_hmac_sha512_provider::decrypter() const -> std::shared_ptr<crypto::decrypter>
{
  return std::make_shared<aead_aes_256_cbc_hmac_sha512_decrypter>(keyring_);
}

aead_aes_256_cbc_hmac_sha512_encrypter::aead_aes_256_cbc_hmac_sha512_encrypter(
  std::string key_id,
  std::shared_ptr<keyring> keyring)
  : keyring_{ std::move(keyring) }
  , key_id_{ std::move(key_id) }
{
}

auto
aead_aes_256_cbc_hmac_sha512_encrypter::encrypt(std::vector<std::byte> plaintext)
  -> std::pair<error, encryption_result>
{
  auto [key_err, key] = keyring_->get(key_id_);
  if (key_err) {
    return { key_err, {} };
  }

  auto [iv_err, iv] = couchbase::crypto::internal::generate_initialization_vector();
  if (iv_err) {
    return { iv_err, {} };
  }

  auto [enc_err, ciphertext] =
    couchbase::crypto::internal::aead_aes_256_cbc_hmac_sha512::encrypt(key.bytes(), iv, plaintext, {});
  if (enc_err) {
    return { enc_err, {} };
  }

  auto res = encryption_result(aead_aes_256_cbc_hmac_sha512_provider::algorithm_name);
  res.put("kid", key_id_);
  res.put("ciphertext", impl::utils::base64::encode(ciphertext));

  return { {}, res };
}

aead_aes_256_cbc_hmac_sha512_decrypter::aead_aes_256_cbc_hmac_sha512_decrypter(
  std::shared_ptr<keyring> keyring)
  : keyring_{ std::move(keyring) }
{
}

auto
aead_aes_256_cbc_hmac_sha512_decrypter::decrypt(encryption_result encrypted)
  -> std::pair<error, std::vector<std::byte>>
{
  const auto key_id = encrypted.get("kid");
  if (!key_id.has_value()) {
    return { error{ errc::field_level_encryption::decryption_failure,
                    "failed to get key ID from document" },
             {} };
  }
  std::optional<std::vector<std::byte>> ciphertext{};
  try {
    ciphertext = encrypted.get_bytes("ciphertext");
  } catch (const std::invalid_argument& e) {
    return { error{ errc::field_level_encryption::invalid_ciphertext,
                    fmt::format("ciphertext could not be decoded: {}", e.what()) },
             {} };
  }
  if (!ciphertext.has_value()) {
    return { error{ errc::field_level_encryption::decryption_failure,
                    "failed to get ciphertext from document" },
             {} };
  }
  const auto [key_err, key] = keyring_->get(key_id.value());
  if (key_err) {
    return { key_err, {} };
  }

  return couchbase::crypto::internal::aead_aes_256_cbc_hmac_sha512::decrypt(
    key.bytes(), ciphertext.value(), {});
}

auto
aead_aes_256_cbc_hmac_sha512_decrypter::algorithm() const -> const std::string&
{
  return aead_aes_256_cbc_hmac_sha512_provider::algorithm_name;
}
} // namespace couchbase::crypto
