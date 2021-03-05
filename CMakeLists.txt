cmake_minimum_required(VERSION 3.10)

project(adcr VERSION 0.0.1)

set(SRC_ADS1256 
    src/adc/ads1256/ADS1256.cpp
    src/DEV_Config.cpp
)

set(SRC 
    src/main.cpp
)

set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)
find_package(WiringPi REQUIRED)
find_package(CLI11 CONFIG REQUIRED)
find_package(bcm2835 CONFIG REQUIRED)

set(TARGET_ADS1256 ads1256)
add_library(${TARGET_ADS1256} SHARED ${SRC_ADS1256})
target_link_libraries(${TARGET_ADS1256} PUBLIC bcm2835::bcm2835)
target_include_directories(${TARGET_ADS1256} 
        PRIVATE 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PUBLIC 
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/adc/${TARGET_ADS1256}/${PROJECT_VERSION_MAJOR}/>
)

include(CMakePackageConfigHelpers)

set_property(TARGET ${TARGET_ADS1256} PROPERTY VERSION ${PROJECT_VERSION})
set_property(TARGET ${TARGET_ADS1256} PROPERTY SOVERSION ${PROJECT_VERSION_MAJOR})

install(TARGETS ${TARGET_ADS1256} 
    EXPORT ${TARGET_ADS1256}Targets
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/adc/${TARGET_ADS1256}/${TARGET_ADS1256}ConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
    )

configure_package_config_file(adcConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/adc/${TARGET_ADS1256}/${TARGET_ADS1256}Config.cmake
        INSTALL_DESTINATION 
            ${CMAKE_INSTALL_LIBDIR}/cmake/adc/${TARGET_ADS1256}/
)
  
install(EXPORT ${TARGET_ADS1256}Targets
    FILE
        ${TARGET_ADS1256}Targets.cmake
    NAMESPACE 
        "adc::"
    DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake/adc/${TARGET_ADS1256}
    COMPONENT
        Devel
)
    
install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/adc/${TARGET_ADS1256}/${TARGET_ADS1256}Config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/adc/${TARGET_ADS1256}/${TARGET_ADS1256}ConfigVersion.cmake"
    DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake/adc/${TARGET_ADS1256}
    COMPONENT
        Devel
)
  
install(FILES ${HEADERS} 
    DESTINATION 
        ${CMAKE_INSTALL_INCLUDEDIR}/adc/${TARGET_ADS1256}/${PROJECT_VERSION_MAJOR}/
    COMPONENT 
        Devel
)
      
add_executable(${PROJECT_NAME} ${SRC})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_link_libraries(${PROJECT_NAME} PRIVATE ${TARGET_ADS1256} Threads::Threads CLI11::CLI11 rt crypt m)
target_include_directories(${PROJECT_NAME} PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})

include(CPack)
