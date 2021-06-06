cmake_minimum_required(VERSION 3.10)

project(adcr VERSION 0.0.1)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

option(ENABLE_API_DOCUMENTATION "Enable C++ API documentation" OFF)

set(ADC_PUBLIC_HEADERS 
    include/adc/ads1256/ADS1256.h
    include/adc/ads1256/ADS1256Reader.h
    include/adc/ads1263/ADS1263.h
    include/adc/ads1263/ADS1263Reader.h
    include/adc/AnalogDataReader.h
)

set(ADC_SOURCES 
    src/adc/ads1256/ADS1256.cpp
    src/adc/ads1256/ADS1256Reader.cpp
    src/adc/ads1263/ADS1263.cpp
    src/adc/ads1263/ADS1263Reader.cpp
    src/adc/ads1263/dev_hardware_SPI.cpp
    src/adc/ads1263/RPI_sysfs_gpio.cpp
    src/adc/device_config.cpp
)

set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(date REQUIRED)
find_package(spdlog REQUIRED)
find_package(Threads REQUIRED)
find_package(WiringPi REQUIRED)
find_package(CLI11 REQUIRED)
find_package(bcm2835 REQUIRED)
find_package(concurrentqueue REQUIRED)

set(TARGET_ADC ${PROJECT_NAME})
add_library(${TARGET_ADC} SHARED ${ADC_SOURCES})
target_link_libraries(${TARGET_ADC} PUBLIC bcm2835::bcm2835)
target_compile_definitions(${TARGET_ADC} PUBLIC -DRPI=1 -DUSE_BCM2835_LIB=1)
target_include_directories(${TARGET_ADC} 
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/>
)

include(CMakePackageConfigHelpers)

set_property(TARGET ${TARGET_ADC} PROPERTY VERSION ${PROJECT_VERSION})
set_property(TARGET ${TARGET_ADC} PROPERTY SOVERSION ${PROJECT_VERSION_MAJOR})

install(TARGETS ${TARGET_ADC} 
    EXPORT 
        ${TARGET_ADC}Targets
    DESTINATION 
        ${CMAKE_INSTALL_LIBDIR}
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_ADC}/${TARGET_ADC}ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
    )
    
configure_package_config_file(${TARGET_ADC}Config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_ADC}/${TARGET_ADC}Config.cmake
    INSTALL_DESTINATION 
        ${CMAKE_INSTALL_LIBDIR}/cmake/${TARGET_ADC}/
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
        ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_ADC}/${TARGET_ADC}Config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_ADC}/${TARGET_ADC}ConfigVersion.cmake
    DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake/${TARGET_ADC}
    COMPONENT
        Devel
)

install(DIRECTORY 
        ${CMAKE_SOURCE_DIR}/include/
    DESTINATION 
        ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
    COMPONENT 
        Devel
)

set(TARGET_ADC_READER adc-reader)
add_executable(${TARGET_ADC_READER}  src/adcr/main.cpp)
target_compile_features(${TARGET_ADC_READER} PRIVATE cxx_std_17)
set_target_properties(${TARGET_ADC_READER} PROPERTIES OUTPUT_NAME adcr)
target_link_libraries(${TARGET_ADC_READER} 
    PRIVATE 
        ${TARGET_ADC} 
        date::date 
        Threads::Threads 
        CLI11::CLI11 
        spdlog::spdlog 
        concurrentqueue::concurrentqueue
        rt crypt m
)
target_include_directories(${TARGET_ADC_READER} 
    PRIVATE 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

install(TARGETS ${TARGET_ADC_READER} DESTINATION ${CMAKE_INSTALL_BINDIR})

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