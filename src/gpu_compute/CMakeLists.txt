add_library(gpu_compute STATIC
    price_calculator.cpp
    cuda_kernels.cu
)

target_include_directories(gpu_compute PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

set_property(TARGET gpu_compute PROPERTY CUDA_ARCHITECTURES 75 80 86)

target_link_libraries(gpu_compute
    core
    ${CUDA_LIBRARIES}
    ${CUDA_CUBLAS_LIBRARIES}
)