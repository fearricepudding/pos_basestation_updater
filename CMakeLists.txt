PROJECT(BaseStationUpdater)
cmake_minimum_required(VERSION 2.8.11)

include_directories(include /usr/local/include)
link_directories(/usr/local/lib)

add_subdirectory(googletest  EXCLUDE_FROM_ALL)

file(GLOB baseStationUpdater_TEST 
	"test/testing.cpp"
)
add_executable(BSTest ${baseStationUpdater_TEST})

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


