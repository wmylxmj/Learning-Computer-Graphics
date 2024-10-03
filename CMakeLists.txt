cmake_minimum_required(VERSION 3.29)
project(CppCG)

set(CMAKE_CXX_STANDARD 17)

add_executable(CppCG main.cpp
        mesh.cpp
        mesh.h
        camera.cpp
        camera.h
        shader.cpp
        shader.h
        fileio.cpp
        fileio.h
        obb.cpp
        obb.h
        bvh.cpp
        bvh.h
        svpng.inc
        sobol.cpp
        sobol.h)


find_package(glad CONFIG REQUIRED)
target_link_libraries(CppCG PRIVATE glad::glad)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(CppCG PRIVATE glfw)

find_package(glm CONFIG REQUIRED)
target_link_libraries(CppCG PRIVATE glm::glm)

find_package(assimp CONFIG REQUIRED)
target_link_libraries(CppCG PRIVATE assimp::assimp)

find_package(Eigen3 CONFIG REQUIRED)
target_link_libraries(CppCG PRIVATE Eigen3::Eigen)

find_package(OpenMP REQUIRED)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")