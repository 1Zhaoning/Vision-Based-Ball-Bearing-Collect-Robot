function(include_eigen)
    find_package(Eigen3)
    include_directories(SYSTEM
        ${EIGEN3_INCLUDE_DIR}
    )
endfunction()