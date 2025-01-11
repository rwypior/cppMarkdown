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

project ("cppMarkdown")
add_library(cppMarkdown)

set_property(TARGET cppMarkdown PROPERTY CXX_STANDARD 17)

target_compile_definitions(cppMarkdown
    PRIVATE
        $<$<CONFIG:Debug>:DEBUG>
)

include(cmake/CPM.cmake)
target_include_directories (cppMarkdown PUBLIC "${PROJECT_SOURCE_DIR}/include")

include(CTest)

add_subdirectory ("src")
add_subdirectory ("example")
add_subdirectory ("tests")

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT cppMarkdownExample)
