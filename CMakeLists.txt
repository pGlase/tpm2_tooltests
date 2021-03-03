cmake_minimum_required(VERSION 3.16)
project(tpm2_tooltests)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)
find_package(OpenSSL "1.1.1" REQUIRED)
find_library(tss2-esys REQUIRED)
#needed if fapi is needed
#find_library(tss2-fapi REQUIRED)
#find_package(json-c REQUIRED)

add_executable(tpm2_tooltests main.cpp)
target_link_libraries(tpm2_tooltests PRIVATE pthread tss2-esys OpenSSL::Crypto)
install(TARGETS tpm2_tooltests DESTINATION bin)
