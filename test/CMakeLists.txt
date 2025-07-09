add_subdirectory(utils)

unit_test(crypto_transcoder)
unit_test(aead_aes_256_cbc_hmac_sha512_provider)
unit_test(keyring)
unit_test(crypto_document)
integration_test(crypto_transcoder)
