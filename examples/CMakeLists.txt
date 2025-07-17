include(CMakePrintHelpers)

macro(define_example name)
    add_executable(${name} ${name}.cxx)
    target_include_directories(${name} PUBLIC ${PROJECT_SOURCE_DIR})
    target_link_libraries(
            ${name}
            couchbase_cxx_encryption
            ${CXX_SDK_TARGET}
            taocpp::json
            spdlog::spdlog
    )
endmacro()

define_example(minimal)
