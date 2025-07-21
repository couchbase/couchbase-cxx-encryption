/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * Copyright (c) 2025 Couchbase, Inc.
 *
 * Use of this software is subject to the Couchbase Inc. Enterprise Subscription License Agreement
 * v7 which may be found at https://www.couchbase.com/ESLA01162020.
 */

#pragma once

/**
 * @mainpage
 *
 * @note You may read about related Couchbase software at https://docs.couchbase.com/
 *
 * This library must be used in conjunction with the Couchbase C++ SDK. Documentation for the SDK
 * can be found at https://docs.couchbase.com/cxx-sdk/current/hello-world/overview.html and the API
 * reference at https://docs.couchbase.com/sdk-api/couchbase-cxx-client/.
 *
 * Use of this library is subject to the Couchbase Inc. Enterprise Subscription License Agreement
 * v7 which may be found at https://www.couchbase.com/ESLA01162020.
 *
 * The following examples show different ways the library can be used to encrypt document fields:
 *
 * * @ref user_defined_type.cxx - example of how to specify fields that should be encrypted in a user-defined type.
 *
 * * @ref custom_transcoder.cxx - example of performing encryption and decryption of documents using a custom transcoder.
 *
 * * @ref crypto_document.cxx - example of how to specify fields that should be encrypted on any document type on an ad hoc basis.
 *
 * @example user_defined_type.cxx
 * Encryption for a user-defined type.
 *
 * @example custom_transcoder.cxx
 * Encryption using a custom transcoder.
 *
 * @example crypto_document.cxx
 * Encryption by specifying fields to encrypt on an ad hoc basis.
 */
