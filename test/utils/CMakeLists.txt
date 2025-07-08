add_library(
        test_utils OBJECT
        binary.cxx
        integration_test_guard.cxx
        logger.cxx
        server_version.cxx
        test_context.cxx
        test_data.cxx
        crypto.cxx
)
set_target_properties(test_utils PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_link_libraries(
        test_utils
        PRIVATE project_options
        project_warnings
        Microsoft.GSL::GSL
        taocpp::json
        spdlog::spdlog
        Catch2::Catch2WithMain
)
target_include_directories(test_utils PRIVATE
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_BINARY_DIR}/generated
        ${PROJECT_SOURCE_DIR}/couchbase-cxx-client)
target_include_directories(test_utils PRIVATE ${PROJECT_BINARY_DIR}/generated)

if(MSVC)
    target_compile_definitions(test_utils PRIVATE _CRT_SECURE_NO_WARNINGS)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(test_utils PUBLIC -Wno-deprecated-declarations)
    endif()
endif()
