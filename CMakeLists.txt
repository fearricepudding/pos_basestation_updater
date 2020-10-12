PROJECT(BaseStationUpdater)
cmake_minimum_required(VERSION 2.8.11)

include_directories(include /usr/local/include)
link_directories(/usr/local/lib)

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${gtest_SOURCE_DIR}/include")
endif()

# Now simply link against gtest or gtest_main as needed. Eg
add_executable(BSTest 
	"test/testing.cpp"
	"src/version.cpp"
	"src/main.h"
)
target_link_libraries(BSTest gtest_main)
add_test(NAME example_test COMMAND BSTest)

file(GLOB baseStationUpdater_SRC 
	"src/logger.cpp"
	"src/main.cpp"
	"src/version.cpp"
)
add_executable(baseStationUpdater ${baseStationUpdater_SRC})

file(GLOB baseStationUpdaterCli
	"src/cli.cpp"
)
add_executable(bsupdater ${baseStationUpdaterCli})

target_link_libraries(baseStationUpdater LINK_PUBLIC pthread cppcms dl pcre icui18n booster crypto z ssl jsoncpp curl)


