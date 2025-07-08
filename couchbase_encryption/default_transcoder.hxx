/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * Copyright (c) 2025 Couchbase, Inc.
 *
 * Use of this software is subject to the Couchbase Inc. Enterprise Subscription License Agreement
 * v7 which may be found at https://www.couchbase.com/ESLA01162020.
 */

#pragma once

#include <couchbase/codec/transcoder_traits.hxx>
#include <couchbase_encryption/transcoder.hxx>

namespace couchbase
{
namespace codec
{
class tao_json_serializer;
} // namespace codec

namespace crypto
{
using default_transcoder = transcoder<codec::tao_json_serializer>;
} // namespace crypto
} // namespace couchbase

#ifndef COUCHBASE_CXX_CLIENT_DOXYGEN
template<>
struct couchbase::codec::is_transcoder<couchbase::crypto::default_transcoder>
  : public std::true_type {
};

template<>
struct couchbase::codec::is_crypto_transcoder<couchbase::crypto::default_transcoder>
  : public std::true_type {
};
#endif
