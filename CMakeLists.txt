cmake_minimum_required(VERSION 3.19)
message(STATUS "CMake version ${CMAKE_VERSION}")

include(cmake/CPM.cmake)

cmake_policy(SET CMP0074 NEW)

project(
  couchbase_cxx_encryption
  VERSION "0.1.0"
  LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)

set(couchbase_cxx_encryption_FILES
        couchbase_encryption/impl/utils/base64.cc
        couchbase_encryption/impl/utils/json.cxx
        couchbase_encryption/impl/aead_aes_256_cbc_hmac_sha512_provider.cxx
        couchbase_encryption/impl/default_manager.cxx
        couchbase_encryption/impl/encryption_result.cxx
        couchbase_encryption/impl/insecure_keyring.cxx
        couchbase_encryption/impl/key.cxx
        couchbase_encryption/impl/transcoder.cxx
)

add_library(couchbase_cxx_encryption STATIC ${couchbase_cxx_encryption_FILES})

set_target_properties(couchbase_cxx_encryption PROPERTIES LINKER_LANGUAGE CXX)

find_package(couchbase_cxx_client)
if (couchbase_cxx_client_FOUND)
    find_package(taocpp-json REQUIRED)
    find_package(spdlog REQUIRED)
else()
    cpmaddpackage(
            NAME
            couchbase_cxx_client
            GIT_TAG
            6b9cadd7a72bb78c809cffab338957e3114138fd
            VERSION
            1.2.0
            GITHUB_REPOSITORY
            "couchbase/couchbase-cxx-client"
            OPTIONS
            "COUCHBASE_CXX_CLIENT_INSTALL ON"
            "COUCHBASE_CXX_CLIENT_BUILD_SHARED ON"
            "COUCHBASE_CXX_CLIENT_BUILD_STATIC OFF")
endif()

set(CXX_SDK_TARGET couchbase_cxx_client::couchbase_cxx_client)

target_include_directories(couchbase_cxx_encryption
        PRIVATE SYSTEM
        ${PROJECT_SOURCE_DIR}
        ${couchbase_cxx_client_SOURCE_DIR}
)

target_link_libraries(couchbase_cxx_encryption
        PUBLIC
        ${CXX_SDK_TARGET}
        PRIVATE
        Microsoft.GSL::GSL
        spdlog::spdlog
        taocpp::json
)

option(COUCHBASE_CXX_ENCRYPTION_BUILD_EXAMPLES "Build example programs" ON)
if(COUCHBASE_CXX_ENCRYPTION_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

option(COUCHBASE_CXX_ENCRYPTION_BUILD_TESTS "Build test programs" ON)
if(COUCHBASE_CXX_ENCRYPTION_BUILD_TESTS)
    include(cmake/Testing.cmake)
endif()
