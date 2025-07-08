/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * Copyright (c) 2025 Couchbase, Inc.
 *
 * Use of this software is subject to the Couchbase Inc. Enterprise Subscription License Agreement
 * v7 which may be found at https://www.couchbase.com/ESLA01162020.
 */

#include "document_types/profile.hxx"
#include "test/utils/crypto.hxx"
#include "test/utils/test_data.hxx"
#include "test_helper_integration.hxx"

#include <couchbase/codec/tao_json_serializer.hxx>
#include <couchbase_encryption/aead_aes_256_cbc_hmac_sha512_provider.hxx>
#include <couchbase_encryption/default_manager.hxx>
#include <couchbase_encryption/default_transcoder.hxx>
#include <couchbase_encryption/encrypted_fields.hxx>
#include <couchbase_encryption/insecure_keyring.hxx>

#include <tao/json/value.hpp>

const auto KEY = test::utils::make_bytes({
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
});

auto
make_crypto_manager() -> std::shared_ptr<couchbase::crypto::default_manager>
{
  auto keyring = std::make_shared<couchbase::crypto::insecure_keyring>();
  keyring->add_key(couchbase::crypto::key("test-key", KEY));

  auto provider = couchbase::crypto::aead_aes_256_cbc_hmac_sha512_provider(keyring);

  auto manager = std::make_shared<couchbase::crypto::default_manager>();
  manager->register_default_encrypter(provider.encrypter_for_key("test-key"));
  manager->register_decrypter(provider.decrypter());

  return manager;
}

TEST_CASE("integration: get with encryption", "[integration]")
{
  test::utils::integration_test_guard integration{ make_crypto_manager() };

  const profile albert{ "this_guy_again", "Albert Einstein", 1879 };
  const auto doc_id = test::utils::uniq_id("albert");

  static_assert(couchbase::crypto::has_encrypted_fields_v<profile>,
                "profile should have encrypted_fields");

  const auto collection = integration.cluster.bucket(integration.ctx.bucket).default_collection();

  {
    const auto [err, res] =
      collection.upsert<couchbase::crypto::default_transcoder>(doc_id, albert).get();
    REQUIRE_NO_ERROR(err);
    REQUIRE_FALSE(res.cas().empty());
  }

  SECTION("get")
  {
    const auto [err, res] = collection.get(doc_id).get();
    REQUIRE_NO_ERROR(err);
    REQUIRE_FALSE(res.cas().empty());
    test::utils::ensure_field_is_encrypted(res.content_as<tao::json::value>(), "full_name");

    // Encrypted document cannot be deserialized as profile, as the full_name field will not be
    // found.
    try {
      const auto _ = res.content_as<profile>();
      FAIL("Expected an exception when trying to deserialize encrypted content with a non-crypto "
           "transcoder");
    } catch (const std::system_error& e) {
      REQUIRE(e.code() == couchbase::errc::common::decoding_failure);
    } catch (...) {
      FAIL("Expected std::system_error, but got a different exception type");
    }

    REQUIRE(res.content_as<profile, couchbase::crypto::default_transcoder>() == albert);
  }

  SECTION("get and touch")
  {
    const auto [err, res] = collection.get_and_touch(doc_id, std::chrono::hours(1)).get();
    REQUIRE_NO_ERROR(err);
    REQUIRE_FALSE(res.cas().empty());
    test::utils::ensure_field_is_encrypted(res.content_as<tao::json::value>(), "full_name");
    REQUIRE(res.content_as<profile, couchbase::crypto::default_transcoder>() == albert);
  }

  SECTION("get any replica")
  {
    const auto [err, res] = collection.get_any_replica(doc_id).get();
    REQUIRE_NO_ERROR(err);
    REQUIRE_FALSE(res.cas().empty());
    test::utils::ensure_field_is_encrypted(res.content_as<tao::json::value>(), "full_name");
    REQUIRE(res.content_as<profile, couchbase::crypto::default_transcoder>() == albert);
  }

  SECTION("get all replicas")
  {
    const auto [err, res] = collection.get_all_replicas(doc_id).get();
    REQUIRE_NO_ERROR(err);

    for (const auto& replica : res) {
      REQUIRE_FALSE(replica.cas().empty());
      test::utils::ensure_field_is_encrypted(replica.content_as<tao::json::value>(), "full_name");
      REQUIRE(replica.content_as<profile, couchbase::crypto::default_transcoder>() == albert);
    }
  }

  SECTION("get and lock")
  {
    const auto [err, res] = collection.get_and_lock(doc_id, std::chrono::seconds(1)).get();
    REQUIRE_NO_ERROR(err);
    REQUIRE_FALSE(res.cas().empty());
    test::utils::ensure_field_is_encrypted(res.content_as<tao::json::value>(), "full_name");
    REQUIRE(res.content_as<profile, couchbase::crypto::default_transcoder>() == albert);
  }
}

TEST_CASE("integration: mutate with encryption", "[integration]")
{
  test::utils::integration_test_guard integration{ make_crypto_manager() };

  const profile albert{ "this_guy_again", "Albert Einstein", 1879 };
  const auto collection = integration.cluster.bucket(integration.ctx.bucket).default_collection();
  const auto id = test::utils::uniq_id("albert");

  SECTION("insert")
  {
    SECTION("future API")
    {
      const auto [err, res] =
        collection.insert<couchbase::crypto::default_transcoder>(id, albert).get();
      REQUIRE_NO_ERROR(err);
      REQUIRE_FALSE(res.cas().empty());
    }

    SECTION("callback API")
    {
      auto barrier =
        std::make_shared<std::promise<std::pair<couchbase::error, couchbase::mutation_result>>>();
      auto f = barrier->get_future();
      collection.insert<couchbase::crypto::default_transcoder>(
        id, albert, {}, [barrier](auto err, auto res) {
          barrier->set_value({ std::move(err), std::move(res) });
        });
      const auto [err, res] = f.get();
      REQUIRE_NO_ERROR(err);
      REQUIRE_FALSE(res.cas().empty());
    }
  }

  SECTION("replace")
  {
    // Upsert without encryption
    {
      const auto [err, res] = collection.upsert(id, albert).get();
      REQUIRE_NO_ERROR(err);
      REQUIRE_FALSE(res.cas().empty());
    }

    // Verify document is unencrypted
    {
      const auto [err, res] = collection.get(id).get();
      REQUIRE(res.content_as<profile>() == albert);
    }

    // Replace with document that has encrypted field
    SECTION("future API")
    {
      const auto [err, res] =
        collection.replace<couchbase::crypto::default_transcoder>(id, albert).get();
      REQUIRE_NO_ERROR(err);
      REQUIRE_FALSE(res.cas().empty());
    }

    SECTION("callback API")
    {
      auto barrier =
        std::make_shared<std::promise<std::pair<couchbase::error, couchbase::mutation_result>>>();
      auto f = barrier->get_future();
      collection.replace<couchbase::crypto::default_transcoder>(
        id, albert, {}, [barrier](auto err, auto res) {
          barrier->set_value({ std::move(err), std::move(res) });
        });
      const auto [err, res] = f.get();
      REQUIRE_NO_ERROR(err);
      REQUIRE_FALSE(res.cas().empty());
    }
  }

  SECTION("upsert")
  {
    SECTION("future API")
    {
      const auto [err, res] =
        collection.upsert<couchbase::crypto::default_transcoder>(id, albert).get();
      REQUIRE_NO_ERROR(err);
      REQUIRE_FALSE(res.cas().empty());
    }

    SECTION("callback API")
    {
      auto barrier =
        std::make_shared<std::promise<std::pair<couchbase::error, couchbase::mutation_result>>>();
      auto f = barrier->get_future();
      collection.upsert<couchbase::crypto::default_transcoder>(
        id, albert, {}, [barrier](auto err, auto res) {
          barrier->set_value({ std::move(err), std::move(res) });
        });
      const auto [err, res] = f.get();
      REQUIRE_NO_ERROR(err);
      REQUIRE_FALSE(res.cas().empty());
    }
  }

  // Verify document is encrypted
  {
    const auto [err, res] = collection.get(id).get();
    REQUIRE_NO_ERROR(err);
    REQUIRE_FALSE(res.cas().empty());
    test::utils::ensure_field_is_encrypted(res.content_as<tao::json::value>(), "full_name");
    REQUIRE(res.content_as<profile, couchbase::crypto::default_transcoder>() == albert);

    // Encrypted document cannot be deserialized as profile, as the full_name field will not be
    // found.
    try {
      const auto _ = res.content_as<profile>();
      FAIL("Expected an exception when trying to deserialize encrypted content with a non-crypto "
           "transcoder");
    } catch (const std::system_error& e) {
      REQUIRE(e.code() == couchbase::errc::common::decoding_failure);
    } catch (...) {
      FAIL("Expected std::system_error, but got a different exception type");
    }
  }
}

TEST_CASE("integration: range scan with encryption", "[integration]")
{
  test::utils::integration_test_guard integration{ make_crypto_manager() };

  if (!integration.ctx.version.supports_feature(test::utils::server_feature::range_scan)) {
    SKIP("Server does not support range scan");
  }

  auto collection = integration.cluster.bucket(integration.ctx.bucket).default_collection();

  const auto prefix = test::utils::uniq_id("crypto-scan");
  std::map<std::string, profile> profiles;
  couchbase::mutation_state state;
  for (std::uint32_t i = 0; i < 10; ++i) {
    std::string key{ prefix + "-" + std::to_string(i) };
    const profile value{ "user" + std::to_string(i), "User " + std::to_string(i), 1950 + i };
    profiles[key] = value;
    const auto [err, res] =
      collection.upsert<couchbase::crypto::default_transcoder>(key, value).get();
    REQUIRE_NO_ERROR(err);
    state.add(res);
  }

  {
    auto [err, res] =
      collection
        .scan(couchbase::prefix_scan{ prefix }, couchbase::scan_options().consistent_with(state))
        .get();
    REQUIRE_NO_ERROR(err);
    std::size_t item_count{ 0 };
    for (const auto& [iter_err, item] : res) {
      REQUIRE_NO_ERROR(err);
      REQUIRE_FALSE(item.id_only());

      ++item_count;

      {
        // Check that we get the decrypted content when using the crypto transcoder
        const auto content = item.content_as<profile, couchbase::crypto::default_transcoder>();
        REQUIRE(profiles.at(item.id()) == content);
      }

      {
        // Check that we get the encrypted content when using a non-crypto transcoder
        const auto encrypted_content = item.content_as<tao::json::value>();
        test::utils::ensure_field_is_encrypted(encrypted_content, "full_name");

        // Check that the unencrypted fields match
        REQUIRE(encrypted_content.at("username").get_string() == profiles.at(item.id()).username);
        REQUIRE(encrypted_content.at("birth_year").get_unsigned() ==
                profiles.at(item.id()).birth_year);
      }
    }

    REQUIRE(10 == item_count);
  }
}
