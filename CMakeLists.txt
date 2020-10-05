PROJECT(BaseStationUpdater)
cmake_minimum_required(VERSION 2.8.11)

include_directories(include /usr/local/include)
link_directories(/usr/local/lib)

find_package(nlohmann_json 3.2.0 REQUIRED)

file(GLOB baseStationUpdater_SRC
	"src/*.cpp"
)

add_executable(baseStationUpdater
	${baseStationUpdater_SRC}
)

target_link_libraries(baseStationUpdater LINK_PUBLIC pthread cppcms dl pcre icui18n booster crypto z usb-1.0 ssl nlohmann_json::nlohmann_json)


