message(STATUS "Conan: Using CMakeDeps conandeps_legacy.cmake aggregator via include()")
message(STATUS "Conan: It is recommended to use explicit find_package() per dependency instead")

find_package(GSL)
find_package(nlohmann_json)
find_package(NLopt)
find_package(cmaes)
find_package(Eigen3)
find_package(Catch2)
find_package(benchmark)

set(CONANDEPS_LEGACY  GSL::gsl  nlohmann_json::nlohmann_json  NLopt::nlopt  libcmaes::cmaes  Eigen3::Eigen  Catch2::Catch2WithMain  benchmark::benchmark_main )