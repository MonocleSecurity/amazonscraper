cmake_minimum_required(VERSION 3.21)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

project(amazonscraper)

find_package(Boost REQUIRED COMPONENTS iostreams regex)
find_package(OpenSSL REQUIRED)
find_package(pugixml CONFIG REQUIRED)

macro(get_win_hex outvar)
  string(REGEX MATCH "^([0-9]+)\\.([0-9]+)" ${outvar} ${CMAKE_SYSTEM_VERSION})
  math(EXPR ${outvar} "(${CMAKE_MATCH_1} << 8) + ${CMAKE_MATCH_2}" OUTPUT_FORMAT HEXADECIMAL)
endmacro()

if(WIN32)
  get_win_hex(winver)
  add_compile_definitions(_WIN32_WINNT=${winver})
endif()

########## amazonscraper ##########

add_executable(amazonscraper
main.hpp
main.cpp)

set_property(TARGET amazonscraper PROPERTY CXX_STANDARD 20)

##### INCLUDES #####

include_directories(amazonscraper .)

##### LIBRARIES #####

target_link_libraries(amazonscraper PRIVATE Boost::regex)
target_link_libraries(amazonscraper PRIVATE Boost::iostreams)
target_link_libraries(amazonscraper PRIVATE OpenSSL::SSL OpenSSL::Crypto)
target_link_libraries(amazonscraper PRIVATE pugixml::shared pugixml::pugixml)
