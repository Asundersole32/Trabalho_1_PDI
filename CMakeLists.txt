cmake_minimum_required(VERSION 3.10)
project(CG_PDI LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)
find_package(GLUT   REQUIRED)
find_package(OpenCV REQUIRED)

add_executable(glut_demo main_glut.cpp)
target_link_libraries(glut_demo GL GLU GLUT)

add_executable(cv_demo main_opencv.cpp)
target_link_libraries(cv_demo ${OpenCV_LIBS})