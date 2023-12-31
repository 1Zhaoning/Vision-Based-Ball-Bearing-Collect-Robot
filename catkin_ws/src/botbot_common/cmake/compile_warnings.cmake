# from here:
#
# https://github.com/lefticus/cppbestpractices/blob/master/02-Use_the_Tools_Avai
# lable.md
option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors" FALSE)

if(NOT ${CMAKE_BUILD_TYPE} STREQUAL "Release")
    set(CLANG_WARNINGS
    \ -Wall
    \ -Wextra # reasonable and standard
    \ -Wunused # warn on anything being unused
    )
    if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        set(CLANG_WARNINGS
            ${CLANG_WARNINGS}
            \ -Wshadow # warn the user if a variable declaration shadows one from a
                    # parent context
            \ -Wnon-virtual-dtor # warn the user if a class with virtual functions has a
                                    # non-virtual destructor. This helps catch hard to
                                    # track down memory errors
            \ -Wold-style-cast # warn for c-style casts
            \ -Wcast-align # warn for potential performance problem casts
            \ -Woverloaded-virtual # warn if you overload (not override) a virtual
                                    # function
            \ -Wpedantic # warn if non-standard C++ is used
            \ -Wconversion # warn on type conversions that may lose data
            #\ -Wsign-conversion # warn on sign conversions
            \ -Wnull-dereference # warn if a null dereference is detected
            \ -Wdouble-promotion # warn if float is implicit promoted to double
            \ -Wformat=2 # warn on security issues around functions that format output
                            # (ie printf)
        )
    endif()


    if (WARNINGS_AS_ERRORS)
        set(CLANG_WARNINGS ${CLANG_WARNINGS} -Werror)
    endif()
    
    set(GCC_WARNINGS
    ${CLANG_WARNINGS}
    \ -Wmisleading-indentation # warn if identation implies blocks where blocks
                            # do not exist
    \ -Wduplicated-cond # warn if if / else chain has duplicated conditions
    \ -Wlogical-op # warn about logical operations being used where bitwise were
                    # probably wanted
    )
    
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(PROJECT_WARNINGS ${CLANG_WARNINGS})
    else()
        set(PROJECT_WARNINGS ${GCC_WARNINGS})
    endif()
    
    string(REPLACE "\\" "  " PROJECT_WARNINGS ${PROJECT_WARNINGS})
    # target_compile_options(${project_name} INTERFACE ${PROJECT_WARNINGS})
    file(GLOB_RECURSE CXX_SRCS *.cpp *.hpp)
    set_source_files_properties(
      ${CXX_SRCS}
      PROPERTIES
      COMPILE_FLAGS ${PROJECT_WARNINGS} 
    )
endif()