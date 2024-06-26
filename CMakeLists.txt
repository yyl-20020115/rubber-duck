cmake_minimum_required( VERSION 3.0 )
project (RubberDuck VERSION 1.0 LANGUAGES CXX)

set (CMAKE_BUILD_TYPE Debug)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")

set (RUNTIME_INSTALL_DIR bin)
set (LIBRARY_INSTALL_DIR lib)
set (INCLUDE_INSTALL_DIR include)


if(WIN32)
    add_compile_options(/utf-8)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS _USE_MATH_DEFINES)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)
add_subdirectory(lib)
add_subdirectory(demos)
add_subdirectory(examples)

install(TARGETS RubberDuck
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

install(TARGETS Assembly FMS Inventory Philosopher QueueReplication 
        AbleBaker_3P AbleBaker_ES AbleBaker_PI DumpTruck_3P DumpTruck_ES DumpTruck_PI
        Philosopher_3P Philosopher_ES Philosopher_PI
        Queue Queue_PI2 Queue3P QueueBatch QueueES QueuePI QueueRandom Random RandomTest
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

file(GLOB headers "lib/*.h")
install(FILES ${headers} DESTINATION include)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/examples/
    DESTINATION ${CMAKE_INSTALL_PREFIX}/examples
    FILE_PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
    DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
    )
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/demos/
    DESTINATION ${CMAKE_INSTALL_PREFIX}/demos
    FILE_PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
    DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
    )
    
set(INCLUDE_DIRS include)
set(LIBRARIES lib)
set(LIB_DIR lib)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
	${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
	VERSION 1.1.1
	COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
	${PROJECT_SOURCE_DIR}/${PROJECT_NAME}Config.cmake.in
	${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
	INSTALL_DESTINATION ./
	PATH_VARS INCLUDE_DIRS LIBRARIES LIB_DIR
	INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}
)

install(
	FILES ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake ${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
	DESTINATION ./
)