cmake_minimum_required(VERSION 3.10)

project(adcr VERSION 1.0.0)

set(SRC 
 src/ADS1256.cpp
 src/DEV_Config.cpp
 src/main.cpp
)

set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)
find_package(WiringPi REQUIRED)
find_package(CLI11 CONFIG REQUIRED)
find_package(bcm2835 CONFIG REQUIRED)

add_executable(${PROJECT_NAME} ${SRC})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads bcm2835::bcm2835 CLI11::CLI11 rt crypt m)
target_include_directories(${PROJECT_NAME} PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})