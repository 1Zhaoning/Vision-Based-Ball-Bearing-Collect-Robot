# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "Setting build type to 'RelWithDebInfo' as none was specified.")
    set(CMAKE_BUILD_TYPE "RelWithDebInfo"
        CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui, ccmake
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo")
endif()
message(STATUS "Build type: " ${CMAKE_BUILD_TYPE})

# include other files
execute_process(COMMAND rospack find botbot_common
    OUTPUT_VARIABLE MMS_CMAKE_DIR
)
string(STRIP ${MMS_CMAKE_DIR} MMS_CMAKE_DIR)
string(APPEND MMS_CMAKE_DIR "/cmake")

# Detect CPU Architecture
execute_process(COMMAND lscpu
    COMMAND grep Vendor
    COMMAND cut -f2- -d:
    OUTPUT_VARIABLE CPU_VENDOR
)

# Speeds up compilation through caching
find_program(CCACHE ccache)
if(CCACHE)
    message(STATUS "Using CCACHE")
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
else()
    message(WARNING "CCACHE not found, using default compiler. [sudo apt install ccache]")
endif()

# Generate compile_commands.json to make it easier to work with clang based tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

## Causes warnings when linking to gtest, so disabled in favour of compile option version
# cmake_policy(SET CMP0069 NEW)
# option(ENABLE_IPO
#        "Enable Iterprocedural Optimization, aka Link Time Optimization (LTO)"
#        ON)

# if(ENABLE_IPO)  
#   include(CheckIPOSupported)
#   check_ipo_supported(RESULT result OUTPUT output)
#   if(result)
#     set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
#   else()
#     message(SEND_ERROR "IPO is not supported: ${output}")
#   endif()
# endif()

# Will set global compile options inc CPU type
include(${MMS_CMAKE_DIR}/compile_options.cmake)

# Eigen related utility funcs (requires CPU type to be set)
include(${MMS_CMAKE_DIR}/eigen_function.cmake)

# Will set warnings for all source files
include(${MMS_CMAKE_DIR}/compile_warnings.cmake)

# Unit test compile func
include(${MMS_CMAKE_DIR}/unit_tests.cmake)
