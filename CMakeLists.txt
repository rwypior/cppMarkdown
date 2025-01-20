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

include(cmake/CPM.cmake)
target_include_directories (cppMarkdown PRIVATE "${PROJECT_SOURCE_DIR}/include")

add_subdirectory ("src")

if(${BUILD_TESTING})
    message("Building unit tests")

    include(CTest)
    add_subdirectory ("tests")
endif()

install(
        TARGETS cppMarkdown EXPORT cppMarkdownTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
)

install(
        DIRECTORY include/
        DESTINATION include/cppMarkdown
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/cppMarkdown/cppMarkdownConfigVersion.cmake"
        VERSION 1.0.0
        COMPATIBILITY AnyNewerVersion
)

export(EXPORT cppMarkdownTargets
        FILE "${CMAKE_CURRENT_BINARY_DIR}/cppMarkdown/cppMarkdownTargets.cmake"
        NAMESPACE cppmd::
)

configure_file(cmake/cppMarkdownConfig.cmake
        "${CMAKE_CURRENT_BINARY_DIR}/cppMarkdown/cppMarkdownConfig.cmake"
        COPYONLY
)

set(ConfigPackageLocation lib/cmake/cppMarkdown)

install(EXPORT cppMarkdownTargets
        FILE cppMarkdownTargets.cmake
        NAMESPACE cppmd::
        DESTINATION ${ConfigPackageLocation}
)
install(FILES "cmake/cppMarkdownConfig.cmake" "${CMAKE_CURRENT_BINARY_DIR}/cppMarkdown/cppMarkdownConfigVersion.cmake"
        DESTINATION ${ConfigPackageLocation}
)

export(PACKAGE cppMarkdown)