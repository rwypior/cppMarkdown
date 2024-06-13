cmake_minimum_required (VERSION 3.26)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("cppMarkdownTest")

add_executable(cppMarkdownTest)
target_sources(cppMarkdownTest PRIVATE "main.cpp" "documenttest.cpp" "textentrytest.cpp" "paragraphtest.cpp" "headingtest.cpp" "blockquotetest.cpp" "listtest.cpp")

set_property(TARGET cppMarkdownTest PROPERTY CXX_STANDARD 14)

target_include_directories(cppMarkdownTest PRIVATE "${CMAKE_SOURCE_DIR}/extern")
target_include_directories(cppMarkdownTest PRIVATE "${CMAKE_SOURCE_DIR}/include")

target_link_libraries(cppMarkdownTest PRIVATE cppMarkdown)