cmake_minimum_required (VERSION 3.26)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("cppMarkdownTest")

add_executable(cppMarkdownTest)
target_sources(cppMarkdownTest PRIVATE 
    "main.cpp" "documenttest.cpp" "textentrytest.cpp" "paragraphtest.cpp" "headingtest.cpp" 
    "blockquotetest.cpp" "listtest.cpp" "codetest.cpp" "linetest.cpp" "linebreaktest.cpp" 
    "elementcontainertest.cpp" "ext/tabletest.cpp" "commontest.cpp" "referencetest.cpp"  "htmltest.cpp")
add_dependencies(cppMarkdownTest cppMarkdown)

set_property(TARGET cppMarkdownTest PROPERTY CXX_STANDARD 17)

target_compile_definitions(cppMarkdownTest
    PRIVATE
        $<$<CONFIG:Debug>:DEBUG>
)

find_package(Catch2 3.6.0 REQUIRED)

target_include_directories(cppMarkdownTest PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_include_directories(cppMarkdownTest PRIVATE "${CMAKE_SOURCE_DIR}/src")

target_link_libraries(cppMarkdownTest PRIVATE cppMarkdown)
target_link_libraries(cppMarkdownTest PRIVATE Catch2::Catch2WithMain)

list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
include(Catch)
catch_discover_tests(cppMarkdownTest)
