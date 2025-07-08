/* -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * Copyright (c) 2025 Couchbase, Inc.
 *
 * Use of this software is subject to the Couchbase Inc. Enterprise Subscription License Agreement
 * v7 which may be found at https://www.couchbase.com/ESLA01162020.
 */

#include "integration_test_guard.hxx"

#include "logger.hxx"

namespace test::utils
{
integration_test_guard::integration_test_guard()
    : ctx{ test_context::load_from_environment() }
{
    init_logger();
    auto [err, c] = couchbase::cluster::connect(ctx.connection_string, ctx.build_options()).get();
    if (err) {
        throw std::system_error(err.ec(), err.message());
    }
    cluster = std::move(c);
}

integration_test_guard::integration_test_guard(const couchbase::cluster_options& opts)
    : ctx{ test_context::load_from_environment() }
{
    init_logger();
    auto [err, c] = couchbase::cluster::connect(ctx.connection_string, opts).get();
    if (err) {
        throw std::system_error(err.ec(), err.message());
    }
    cluster = std::move(c);
}

integration_test_guard::integration_test_guard(const std::shared_ptr<couchbase::crypto::manager>& crypto_manager)
    : ctx{ test_context::load_from_environment() }
{
    init_logger();
    auto opts = ctx.build_options();
    opts.crypto_manager(crypto_manager);
    auto [err, c] = couchbase::cluster::connect(ctx.connection_string, opts).get();
    if (err) {
        throw std::system_error(err.ec(), err.message());
    }
    cluster = std::move(c);
}

integration_test_guard::~integration_test_guard()
{
    cluster.close().get();
}

auto
integration_test_guard::cluster_version() const -> server_version
{
    return ctx.version;
}
} // namespace test::utils
