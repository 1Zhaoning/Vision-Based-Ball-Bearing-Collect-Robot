# Standard compile options and definitions, depending on device type and build type

add_compile_options(-std=c++17 -fdiagnostics-color)

if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    add_compile_options(-Ofast -march=native -fPIC -flto -fuse-linker-plugin)
elseif(${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
    add_compile_options(-g -Ofast -march=native -fPIC) #-ftime-trace)
else()
    add_compile_options(-g -Og -fno-inline -fno-omit-frame-pointer -fPIC)
endif()


# Turns off Eigen array bounds checking
# Will throw a runtime error if enabled ???
add_definitions(-DNDEBUG)

if(${CPU_VENDOR} MATCHES "GenuineIntel")
    MESSAGE(STATUS "CPU: Intel")
    add_compile_options(-m64)
elseif(${CPU_VENDOR} MATCHES "AuthenticAMD")
    MESSAGE(STATUS "CPU: AMD")
    add_compile_options(-m64)
else()
    MESSAGE(STATUS "CPU: ARM")
    add_definitions(-DARM)
endif()