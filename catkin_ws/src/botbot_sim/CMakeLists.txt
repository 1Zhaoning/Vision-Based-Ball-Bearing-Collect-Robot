cmake_minimum_required(VERSION 3.5)
project(botbot_sim)

# Determine path to mms_library to include files
execute_process(COMMAND rospack find botbot_common
    OUTPUT_VARIABLE MMS_CMAKE_DIR
    )
string(STRIP ${MMS_CMAKE_DIR} MMS_CMAKE_DIR)
string(APPEND MMS_CMAKE_DIR "/cmake")

include(${MMS_CMAKE_DIR}/project_settings.cmake)

## Find catkin and any catkin packages
find_package(catkin REQUIRED COMPONENTS
    roscpp
    std_msgs
    genmsg
    message_generation
    visualization_msgs
    botbot_common
    )

#include_eigen()

## Declare ROS messages and services
add_message_files(FILES MyCustomMessage.msg)

## Generate added messages and services
generate_messages(DEPENDENCIES
    std_msgs
    visualization_msgs
    )

## Declare a catkin package
catkin_package()

include_directories(SYSTEM
    include
    ${catkin_INCLUDE_DIRS}
    )

install(DIRECTORY
    launch
    DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
    )

# Create Project library (making it a library makes it easy to include in several different contexts
#    with their own main function, eg: unit_tests, rostests and main roslaunch exe).
add_library(${PROJECT_NAME}_lib STATIC
    src/AlgorithmNode.cpp
    src/AlgorithmNode.hpp
    )
target_link_libraries(${PROJECT_NAME}_lib
    ${catkin_LIBRARIES}
    pthread
    )
add_dependencies(${PROJECT_NAME}_lib
    ${catkin_EXPORTED_TARGETS}
    botbot_sim_generate_messages_cpp
    )

# Add main file and link to project
add_executable(main
    src/Main.cpp
    )
target_link_libraries(main
    ${PROJECT_NAME}_lib
    )


add_unit_tests(${PROJECT_NAME}_lib)

# Any rostests need to be individually added
