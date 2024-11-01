cmake_minimum_required(VERSION 3.29)
project(c_or_c_plus_plus)

set(CMAKE_CXX_STANDARD 14)

add_executable(c_or_c_plus_plus
    oop/behavioural/observer.cpp
    oop/behavioural/strategy.cpp
    oop/creational/builder.cpp
    oop/creational/factory.cpp
    oop/creational/singleton.cpp
    oop/structural/adapter.cpp
    oop/structural/decorator.cpp
    oop/structural/facade.cpp
    pointers-related/array_as_a_pointer.cpp
    pointers-related/multiple_pointers.cpp
    pointers-related/pointers.cpp
    pointers-related/reference.cpp
    pointers-related/single_pointer.cpp
    pointers-related/void_pointer.cpp
    array.cpp
    control_flow.cpp
    function.cpp
    hello-world.cpp
    iterators.cpp
    multiple_pointers.cpp
    namespaces.cpp
    oop.cpp
    pointers.cpp
    single_pointer.cpp
    user_input.cpp)
