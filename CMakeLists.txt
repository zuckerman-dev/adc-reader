cmake_minimum_required(VERSION 3.10)

project(adcr VERSION 0.0.1)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

option(ENABLE_API_DOCUMENTATION "Enable C++ API documentation" OFF)

set(ADS1256_HEADERS 
    include/adc/ads1256/ADS1256.h
    include/adc/ads1256/ADS1256Reader.h
)

set(ADS1263_HEADERS
    include/adc/ads1263/ADS1263.h
    include/adc/ads1263/ADS1263Reader.h
    include/adc/ads1263/dev_hardware_SPI.h
    include/adc/ads1263/RPI_sysfs_gpio.h
)

set(ADC_COMMON_HEADERS
    include/adc/AnalogDataReader.h
    include/DEV_Config.h
)

set(SRC_ADS1256 
    src/adc/ads1256/ADS1256.cpp
    src/adc/ads1256/ADS1256Reader.cpp
)

set(SRC_ADS1263 
    src/adc/ads1263/ADS1263.cpp
    src/adc/ads1263/ADS1263Reader.cpp
    src/adc/ads1263/dev_hardware_SPI.cpp
    src/adc/ads1263/RPI_sysfs_gpio.cpp
)

set(SRC_ADC 
    src/DEV_Config
)

set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(date REQUIRED)
find_package(spdlog REQUIRED)
find_package(Threads REQUIRED)
find_package(WiringPi REQUIRED)
find_package(CLI11 CONFIG REQUIRED)
find_package(bcm2835 CONFIG REQUIRED)

set(TARGET_ADC adc)
add_library(${TARGET_ADC} SHARED ${SRC_ADC})
target_link_libraries(${TARGET_ADC} PUBLIC bcm2835::bcm2835)
target_compile_definitions(${TARGET_ADC} PUBLIC -DRPI=1 -DUSE_BCM2835_LIB=1)
target_include_directories(${TARGET_ADC} 
        PRIVATE 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PUBLIC 
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/adc/${TARGET_ADC}/>
)

set(TARGET_ADS1263 ads1263)
add_library(${TARGET_ADS1263} SHARED ${SRC_ADS1263})
target_link_libraries(${TARGET_ADS1263} PUBLIC bcm2835::bcm2835 ${TARGET_ADC})
target_compile_definitions(${TARGET_ADS1263} PUBLIC -DRPI=1 -DUSE_BCM2835_LIB=1)
target_include_directories(${TARGET_ADS1263} 
        PRIVATE 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PUBLIC 
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/adc/${TARGET_ADS1263}//>
)

set(TARGET_ADS1256 ads1256)
add_library(${TARGET_ADS1256} SHARED ${SRC_ADS1256})
target_compile_definitions(${TARGET_ADS1256} PUBLIC -DRPI=1 -DUSE_BCM2835_LIB=1)
target_link_libraries(${TARGET_ADS1256} PUBLIC bcm2835::bcm2835 ${TARGET_ADC})
target_include_directories(${TARGET_ADS1256} 
        PRIVATE 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PUBLIC 
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/adc/${TARGET_ADS1256}//>
)

include(CMakePackageConfigHelpers)

set_property(TARGET ${TARGET_ADS1256} PROPERTY VERSION ${PROJECT_VERSION})
set_property(TARGET ${TARGET_ADS1256} PROPERTY SOVERSION ${PROJECT_VERSION_MAJOR})

set_property(TARGET ${TARGET_ADS1263} PROPERTY VERSION ${PROJECT_VERSION})
set_property(TARGET ${TARGET_ADS1263} PROPERTY SOVERSION ${PROJECT_VERSION_MAJOR})

set_property(TARGET ${TARGET_ADC} PROPERTY VERSION ${PROJECT_VERSION})
set_property(TARGET ${TARGET_ADC} PROPERTY SOVERSION ${PROJECT_VERSION_MAJOR})

install(TARGETS ${TARGET_ADS1256} 
    EXPORT ${TARGET_ADS1256}Targets
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

install(TARGETS ${TARGET_ADS1263} 
    EXPORT ${TARGET_ADS1263}Targets
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(TARGETS ${TARGET_ADC} 
    EXPORT ${TARGET_ADC}Targets
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_ADS1256}/${TARGET_ADS1256}ConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
    )

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_ADS1263}/${TARGET_ADS1263}ConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
    )

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_ADC}/${TARGET_ADC}ConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
    )
    
configure_package_config_file(adcConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_ADS1256}/${TARGET_ADS1256}Config.cmake
        INSTALL_DESTINATION 
            ${CMAKE_INSTALL_LIBDIR}/cmake/${TARGET_ADS1256}/
)
  
configure_package_config_file(adcConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_ADS1263}/${TARGET_ADS1263}Config.cmake
        INSTALL_DESTINATION 
            ${CMAKE_INSTALL_LIBDIR}/cmake/${TARGET_ADS1263}/
)

configure_package_config_file(adcConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_ADC}/${TARGET_ADC}Config.cmake
        INSTALL_DESTINATION 
            ${CMAKE_INSTALL_LIBDIR}/cmake/${TARGET_ADC}/
)

install(EXPORT ${TARGET_ADS1256}Targets
    FILE
        ${TARGET_ADS1256}Targets.cmake
    NAMESPACE 
        "adc::"
    DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake/${TARGET_ADS1256}
    COMPONENT
        Devel
)
    
install(EXPORT ${TARGET_ADS1263}Targets
    FILE
        ${TARGET_ADS1263}Targets.cmake
    NAMESPACE 
        "adc::"
    DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake/${TARGET_ADS1263}
    COMPONENT
        Devel
)

install(EXPORT ${TARGET_ADC}Targets
    FILE
        ${TARGET_ADC}Targets.cmake
    NAMESPACE 
        "adc::"
    DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake/${TARGET_ADC}
    COMPONENT
        Devel
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_ADS1256}/${TARGET_ADS1256}Config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_ADS1256}/${TARGET_ADS1256}ConfigVersion.cmake"
    DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake/${TARGET_ADS1256}
    COMPONENT
        Devel
)
  
install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_ADS1263}/${TARGET_ADS1263}Config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_ADS1263}/${TARGET_ADS1263}ConfigVersion.cmake"
    DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake/${TARGET_ADS1263}
    COMPONENT
        Devel
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_ADC}/${TARGET_ADC}Config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_ADC}/${TARGET_ADC}ConfigVersion.cmake"
    DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake/${TARGET_ADC}
    COMPONENT
        Devel
)

install(FILES ${ADS1256_HEADERS} 
    DESTINATION 
        ${CMAKE_INSTALL_INCLUDEDIR}/adc/${TARGET_ADS1256}/
    COMPONENT 
        Devel
)
      
install(FILES ${ADS1263_HEADERS} 
    DESTINATION 
        ${CMAKE_INSTALL_INCLUDEDIR}/adc/${TARGET_ADS1263}/
    COMPONENT 
        Devel
)

install(FILES ${ADC_COMMON_HEADERS} 
    DESTINATION 
        ${CMAKE_INSTALL_INCLUDEDIR}/adc/
    COMPONENT 
        Devel
)

add_executable(${PROJECT_NAME}  src/adcr/main.cpp)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
        ${TARGET_ADS1256} 
        ${TARGET_ADS1263} 
        date::date 
        Threads::Threads 
        CLI11::CLI11 
        spdlog::spdlog 
        rt crypt m
)
target_include_directories(${PROJECT_NAME} PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})

if(ENABLE_API_DOCUMENTATION)
    find_package(Doxygen
                REQUIRED dot
                OPTIONAL_COMPONENTS mscgen dia)

    set(DOXYGEN_GENERATE_HTML YES)
    
    doxygen_add_docs(
        doxygen
        ${PROJECT_SOURCE_DIR}/include
        COMMENT "Generate C++ API documentation"
    )                
endif()

set(CPACK_PACKAGE_NAME "adc")
set(CPACK_PACKAGE_VENDOR "Mikhail Tcukerman <michael@zuckerman.dev>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Analog to digital convertion library")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER ${CPACK_PACKAGE_VENDOR})
set(CPACK_GENERATOR "RPM;DEB")

include(CPack)