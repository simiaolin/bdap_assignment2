cmake_minimum_required(VERSION 3.1)
project(Test)

set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_FLAGS “${CMAKE_CXX_FLAGS} -stdlib=libstdc++”)
set(CMAKE_BUILD_TYPE Debug)

# find_package(DecisionTree 0.1 REQUIRED)
# if(DecisionTree_FOUND)
#     message("-- DecisionTree version: ${DecisionTree_VERSION}")
# endif()

set(Boost_INCLUDE_DIR /Users/ary/Desktop/bdap/boost/boost_1_75_0/)
set(Boost_LIBRARIES /Users/ary/Desktop/bdap/boost/boost_1_75_0/stage/lib/)


#set(Boost_INCLUDE_DIRS /Users/ary/Desktop/bdap/boost/boost_1_75_0/)

#include_directories(/Users/ary/Desktop/bdap/boost/boost_1_75_0/include/)
#link_directories(/Users/ary/Desktop/bdap/boost/boost_1_75_0/stage/lib)

find_package(Threads REQUIRED)
find_package(Boost COMPONENTS timer chrono REQUIRED)

set (FILES
        ../lib/src/DataReader.cpp
        ../lib/src/DecisionTree.cpp
        ../lib/src/Bagging.cpp
        ../lib/src/Question.cpp
        ../lib/src/Leaf.cpp
        ../lib/src/Node.cpp
        ../lib/src/Calculations.cpp
        ../lib/src/TreeTest.cpp)

# add_executable(ImportTest import_test.cpp)
# target_include_directories(ImportTest PUBLIC ../lib/include ${Boost_INCLUDE_DIRS})
# target_compile_options(ImportTest PRIVATE -Wall -Weffc++ -Wpedantic)
# target_link_libraries(ImportTest DecisionTree Threads::Threads ${Boost_LIBRARIES})


add_executable(DecisionTreeTest decision_tree_tester.cpp ${FILES})
target_compile_options(DecisionTreeTest PRIVATE -Wall -Weffc++ -Wpedantic)
target_include_directories(DecisionTreeTest PUBLIC ../lib/include ${Boost_INCLUDE_DIRS})
target_link_libraries(DecisionTreeTest Threads::Threads ${Boost_LIBRARIES})

add_executable(BaggingTest bagging_tester.cpp ${FILES})
target_compile_options(BaggingTest PRIVATE -Wall -Weffc++ -Wpedantic)
target_include_directories(BaggingTest PUBLIC ../lib/include ${Boost_INCLUDE_DIRS})
target_link_libraries(BaggingTest Threads::Threads ${Boost_LIBRARIES})

add_executable(MyMainTest mytest.cpp)
