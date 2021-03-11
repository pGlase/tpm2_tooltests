cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_BUILD_TYPE Debug)

find_package(Threads REQUIRED)
find_package(OpenSSL "1.1.1" REQUIRED)
find_library(tss2-esys REQUIRED)
find_library(tss2-rc REQUIRED)
find_library(spdlog REQUIRED)
#needed if fapi is needed
#find_library(tss2-fapi REQUIRED)
#find_package(json-c REQUIRED)

add_executable(tpm2_tooltests main.cpp Tpm2ContextBundle.cpp Tpm2Capabilites.cpp Tpm2Random.cpp)
target_link_libraries(tpm2_tooltests PRIVATE pthread tss2-esys tss2-rc OpenSSL::Crypto spdlog)
add_compile_options(-Wall -Wextra -pedantic -Werror)
install(TARGETS tpm2_tooltests DESTINATION bin)
