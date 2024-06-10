cmake_minimum_required (VERSION 3.26)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("cppMarkdownTest")

find_path(CATCH2_INCLUDE_DIR NAMES catch.hpp PATHS {proper-location} OPTIONAL)

add_executable(cppMarkdownTest)
target_sources(cppMarkdownTest PRIVATE "main.cpp" "documenttest.cpp" "textentrytest.cpp" "paragraphtest.cpp" "headinigtest.cpp")

set_property(TARGET cppMarkdownTest PROPERTY CXX_STANDARD 14)

target_include_directories(cppMarkdownTest PRIVATE "${CATCH2_INCLUDE_DIR}")
target_include_directories(cppMarkdownTest PRIVATE "${CMAKE_SOURCE_DIR}/include")

target_link_libraries(cppMarkdownTest PRIVATE cppMarkdown)