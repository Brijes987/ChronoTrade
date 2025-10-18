add_executable(matching_engine_benchmark
    matching_engine_benchmark.cpp
)

target_link_libraries(matching_engine_benchmark
    matching_engine
    benchmark::benchmark
    benchmark::benchmark_main
)

add_executable(market_data_benchmark
    market_data_benchmark.cpp
)

target_link_libraries(market_data_benchmark
    market_data
    benchmark::benchmark
    benchmark::benchmark_main
)

add_executable(gpu_calculations_benchmark
    gpu_calculations_benchmark.cpp
)

target_link_libraries(gpu_calculations_benchmark
    gpu_compute
    benchmark::benchmark
    benchmark::benchmark_main
)