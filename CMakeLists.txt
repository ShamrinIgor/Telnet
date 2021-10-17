cmake_minimum_required(VERSION 3.16)

project(myTelnet)

add_executable(${PROJECT_NAME} "main.cpp")
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Boost REQUIRED COMPONENTS chrono coroutine system thread)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

find_package(Boost REQUIRED COMPONENTS system thread)
target_include_directories(${PROJECT_NAME} PRIVATE ${BOOST_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::chrono Boost::coroutine Boost::system Boost::thread)
