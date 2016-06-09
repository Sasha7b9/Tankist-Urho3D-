project(Tankist)

cmake_minimum_required(VERSION 2.6)

set (CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

file (GLOB CPPS "../source/Tankist/Tankist/src/*.cpp" "../source/Tankist/common/*.cpp")

add_definitions(-O3 -std=c++11 -march=native -ffast-math -pthread -Wno-invalid-offsetof -D_SERVER_=1)

include_directories(../../urho3D/build/include/ ../../urho3D/build/include/Urho3D/ThirdParty ../source/Tankist/Tankist/src)

add_executable(Tankist ${CPPS})

target_link_libraries(Tankist ${CMAKE_CURRENT_SOURCE_DIR}/build/libUrho3D.a -ldl -lm -lrt -lGL -pthread)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
