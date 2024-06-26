cmake_minimum_required( VERSION 3.0 )
project (RubberDuck VERSION 1.0 LANGUAGES CXX)

set (CMAKE_BUILD_TYPE Debug)

##############设置目标文件生成位置#####################
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")

# define RUNTIME_INSTALL_DIR LIBRARY_INSTALL_DIR INCLUDE_INSTALL_DIR CONFIG_INSTALL_DIR
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
    