include(CMakePrintHelpers)

macro(define_example name)
    add_executable(${name} ${name}.cxx)
    target_include_directories(${name} PUBLIC ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/couchbase-cxx-client)
    target_link_libraries(
            ${name}
            ${CXX_SDK_TARGET}
            couchbase_cxx_encryption
            taocpp::json
            spdlog::spdlog
    )
endmacro()

define_example(minimal)
