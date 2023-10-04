cmake_minimum_required(VERSION 3.5)
project(botbot_msgs)

## Find catkin macros and libraries
find_package(catkin REQUIRED
  COMPONENTS
    roscpp
    message_generation
    std_msgs
    geometry_msgs
    sensor_msgs
)

#######################################
## Declare ROS messages and services ##
#######################################

## Generate messages in the 'msg' folder
add_message_files(
  FILES
  VehicleState.msg
)

## Generate services in the 'srv' folder
# add_service_files(
#   FILES
#   ?.srv
# )

## Generate added messages and services with any dependencies listed here
generate_messages(
  DEPENDENCIES
    std_msgs
    geometry_msgs
    sensor_msgs
)

catkin_package(
#  INCLUDE_DIRS
#  LIBRARIES
  CATKIN_DEPENDS
    message_runtime
    std_msgs
    geometry_msgs
#  DEPENDS
)
