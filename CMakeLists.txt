cmake_minimum_required(VERSION 3.26)
project(LearningCG)

set(CMAKE_CXX_STANDARD 17)

add_executable(LearningCG main.cpp
        mesh.cpp
        mesh.h
        mesh.h
        fileio.cpp
        fileio.h
        obb.cpp
        obb.h
        camera.cpp
        camera.h
)

find_package(glad CONFIG REQUIRED)
target_link_libraries(LearningCG PRIVATE glad::glad)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(LearningCG PRIVATE glfw)

find_package(glm CONFIG REQUIRED)
target_link_libraries(LearningCG PRIVATE glm::glm)

find_package(assimp CONFIG REQUIRED)
target_link_libraries(LearningCG PRIVATE assimp::assimp)

find_package(Eigen3 CONFIG REQUIRED)
target_link_libraries(LearningCG PRIVATE Eigen3::Eigen)