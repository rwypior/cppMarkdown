cmake_minimum_required (VERSION 3.26)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_DEBUG_POSTFIX "d")
else()
    set(CMAKE_DEBUG_POSTFIX "")
endif()

project ("cppMarkdownExample")
add_executable(cppMarkdownExample "example.cpp")

set_property(TARGET cppMarkdownExample PROPERTY CXX_STANDARD 14)

target_include_directories (cppMarkdownExample PUBLIC "${CMAKE_SOURCE_DIR}/include")
target_link_libraries (cppMarkdownExample cppMarkdown)