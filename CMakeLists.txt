cmake_minimum_required(VERSION 3.19)
message(STATUS "CMake version ${CMAKE_VERSION}")

include(cmake/CPM.cmake)

cmake_policy(SET CMP0074 NEW)

project(
  couchbase_cxx_encryption
  VERSION "0.1.0"
  LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)

set(COUCHBASE_CXX_CLIENT_BUILD_DOCS FALSE)
set(COUCHBASE_CXX_CLIENT_BUILD_EXAMPLES FALSE)
set(COUCHBASE_CXX_CLIENT_BUILD_TESTS FALSE)
set(COUCHBASE_CXX_CLIENT_BUILD_TOOLS FALSE)
set(COUCHBASE_CXX_CLIENT_BUILD_STATIC TRUE)
set(COUCHBASE_CXX_CLIENT_BUILD_SHARED FALSE)
set(COUCHBASE_CXX_CLIENT_STATIC_BORINGSSL TRUE)
set(COUCHBASE_CXX_CLIENT_INSTALL FALSE)
add_subdirectory("${PROJECT_SOURCE_DIR}/couchbase-cxx-client")

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

if(NOT TARGET Catch2::Catch2WithMain)
    cpmaddpackage(
            NAME
            catch2
            VERSION
            3.4.0
            GITHUB_REPOSITORY
            "catchorg/catch2")
endif()

if(NOT TARGET taocpp::json)
    # https://github.com/taocpp/json/releases
    cpmaddpackage(
            NAME
            json
            GIT_TAG
            1.0.0-beta.14
            VERSION
            1.0.0-beta.14
            GITHUB_REPOSITORY
            "taocpp/json"
            OPTIONS
            "CMAKE_C_VISIBILITY_PRESET hidden"
            "CMAKE_CXX_VISIBILITY_PRESET hidden"
            "CMAKE_POSITION_INDEPENDENT_CODE ON"
            "BUILD_SHARED_LIBS OFF"
            "PEGTL_INSTALL ${COUCHBASE_CXX_CLIENT_INSTALL}"
            "PEGTL_INSTALL_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/pegtl"
            "PEGTL_INSTALL_DOC_DIR ${CMAKE_INSTALL_DATAROOTDIR}/doc/tao/pegtl"
            "PEGTL_BUILD_TESTS OFF"
            "PEGTL_BUILD_EXAMPLES OFF"
            "PEGTL_USE_BOOST_FILESYSTEM OFF"
            "TAOCPP_JSON_INSTALL ${COUCHBASE_CXX_CLIENT_INSTALL}"
            "TAOCPP_JSON_INSTALL_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/taocpp-json"
            "TAOCPP_JSON_INSTALL_DOC_DIR ${CMAKE_INSTALL_DATAROOTDIR}/doc/tao/json"
            "TAOCPP_JSON_BUILD_TESTS OFF"
            "TAOCPP_JSON_BUILD_EXAMPLES OFF")
endif()

if(NOT TARGET spdlog::spdlog)
    # https://github.com/gabime/spdlog/releases
    cpmaddpackage(
            NAME
            spdlog
            VERSION
            1.15.0
            GITHUB_REPOSITORY
            "gabime/spdlog"
            EXCLUDE_FROM_ALL ON
            OPTIONS
            "SPDLOG_INSTALL OFF"
            "BUILD_SHARED_LIBS OFF"
            "CMAKE_C_VISIBILITY_PRESET hidden"
            "CMAKE_CXX_VISIBILITY_PRESET hidden"
            "CMAKE_POSITION_INDEPENDENT_CODE ON"
            "NO_CMAKE_SYSTEM_PATH ON"
            "NO_CMAKE_INSTALL_PREFIX ON"
            "NO_CMAKE_SYSTEM_PACKAGE_REGISTRY ON"
            "SPDLOG_BUILD_SHARED OFF"
            "SPDLOG_FMT_EXTERNAL OFF")
endif()

add_library(couchbase_cxx_encryption STATIC ${couchbase_cxx_encryption_FILES})

set_target_properties(couchbase_cxx_encryption PROPERTIES LINKER_LANGUAGE CXX)

set(CXX_SDK_TARGET couchbase_cxx_client::couchbase_cxx_client)
if(NOT TARGET ${CXX_SDK_TARGET})
    set(CXX_SDK_TARGET couchbase_cxx_client::couchbase_cxx_client_static)
endif()

target_include_directories(couchbase_cxx_encryption PRIVATE SYSTEM
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/couchbase-cxx-client
)

target_link_libraries(couchbase_cxx_encryption PRIVATE
    ${CXX_SDK_TARGET}
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
