cmake_minimum_required(VERSION 3.9)
project(botbot_common)

include(${CMAKE_SOURCE_DIR}/cmake/project_settings.cmake)

find_package(catkin REQUIRED COMPONENTS 
    roscpp
    rospy
)

include_eigen()

find_package(Boost REQUIRED COMPONENTS system)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS} )

# catkin_python_setup()

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES ${PROJECT_NAME}
  # CATKIN_DEPENDS other_catkin_pkg
  # DEPENDS system_lib
)

include_directories(
  include
	SYSTEM ${catkin_INCLUDE_DIRS}
)

add_library(${PROJECT_NAME} STATIC 
            include/botbot_common/VehicleParams.hpp
            include/botbot_common/CsvWriter.hpp
            src/SignalFilter.cpp
            src/TicToc.cpp
            src/MathUtils.cpp
            src/ThreadPool.cpp
            src/VectorXY.cpp)

#target_compile_options(${PROJECT_NAME} PRIVATE -Werror)

# # Not required
# link_eigen(botbot_common)
target_link_libraries(${PROJECT_NAME}
		${catkin_LIBRARIES}
		pthread
)

## Mark hpp header files for installation
install(DIRECTORY include
  DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION})

add_unit_tests(${PROJECT_NAME})

#catkin_install_python(PROGRAMS scripts/test
#  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION})
