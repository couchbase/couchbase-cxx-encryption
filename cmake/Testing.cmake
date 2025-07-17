if(NOT TARGET Catch2::Catch2WithMain)
    cpmaddpackage(
            NAME
            catch2
            VERSION
            3.8.1
            GITHUB_REPOSITORY
            "catchorg/catch2")
endif()

list(APPEND CMAKE_MODULE_PATH "${catch2_SOURCE_DIR}/extras")

include(CTest)
include(Catch)

define_property(
        GLOBAL
        PROPERTY COUCHBASE_ENCRYPTION_INTEGRATION_TESTS
        BRIEF_DOCS "list of integration tests"
        FULL_DOCS "list of integration tests targets")
set_property(GLOBAL PROPERTY COUCHBASE_ENCRYPTION_INTEGRATION_TESTS "")

macro(integration_test name)
    add_executable(test_integration_${name} "${PROJECT_SOURCE_DIR}/test/test_integration_${name}.cxx")
    target_include_directories(test_integration_${name} PRIVATE
            ${PROJECT_SOURCE_DIR}
            ${PROJECT_SOURCE_DIR}/couchbase-cxx-client
            ${PROJECT_BINARY_DIR}/generated
            ${PROJECT_BINARY_DIR}/generated_$<CONFIG>)
    target_link_libraries(
            test_integration_${name}
            couchbase_cxx_encryption
            ${CXX_SDK_TARGET}
            Catch2::Catch2WithMain
            Microsoft.GSL::GSL
            taocpp::json
            spdlog::spdlog
            test_utils)
    catch_discover_tests(
            test_integration_${name}
            PROPERTIES
            SKIP_REGULAR_EXPRESSION
            "SKIP"
            LABELS
            "integration")
    set_property(GLOBAL APPEND PROPERTY COUCHBASE_INTEGRATION_TESTS "test_integration_${name}")
endmacro()

define_property(
        GLOBAL
        PROPERTY COUCHBASE_UNIT_TESTS
        BRIEF_DOCS "list of unit tests"
        FULL_DOCS "list of unit tests targets")
set_property(GLOBAL PROPERTY COUCHBASE_UNIT_TESTS "")
macro(unit_test name)
    add_executable(test_unit_${name} "${PROJECT_SOURCE_DIR}/test/test_unit_${name}.cxx")
    target_include_directories(test_unit_${name} PRIVATE
            ${PROJECT_SOURCE_DIR}
            ${PROJECT_SOURCE_DIR}/couchbase-cxx-client
            ${PROJECT_BINARY_DIR}/generated
            ${PROJECT_BINARY_DIR}/generated_$<CONFIG>)
    target_link_libraries(
            test_unit_${name}
            project_options
            project_warnings
            couchbase_cxx_encryption
            Catch2::Catch2WithMain
            Microsoft.GSL::GSL
            taocpp::json
            spdlog::spdlog
            ${CXX_SDK_TARGET}
            test_utils)
    catch_discover_tests(
            test_unit_${name}
            PROPERTIES
            SKIP_REGULAR_EXPRESSION
            "SKIP"
            LABELS
            "unit")
    set_property(GLOBAL APPEND PROPERTY COUCHBASE_UNIT_TESTS "test_unit_${name}")
endmacro()

add_subdirectory(${PROJECT_SOURCE_DIR}/test)

get_property(integration_targets GLOBAL PROPERTY COUCHBASE_ENCRYPTION_INTEGRATION_TESTS)
add_custom_target(build_integration_tests DEPENDS ${integration_targets})

get_property(unit_targets GLOBAL PROPERTY COUCHBASE_ENCRYPTION_UNIT_TESTS)
add_custom_target(build_unit_tests DEPENDS ${unit_targets})
