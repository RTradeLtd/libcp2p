#! /bin/bash

function release_build() {
    if [[ $(cat build/.last_build) != "release" ]]; then
        echo "detecting different build mode, recreating build dir"
        rm -rf build && mkdir build
    fi
    echo "building libcp2p in release mode"
    cd build
    cmake ..
    cmake -build .
    make
    echo "release" > .last_build
}

function debug_build() {
    if [[ $(cat build/.last_build) != "debug" ]]; then
        echo "detecting different build mode, recreating build dir"
        rm -rf build && mkdir build
    fi
    echo "building libcp2p in debug mode"
    cd build
    cmake -D CMAKE_BUILD_TYPE=Debug ..
    cmake -D CMAKE_BUILD_TYPE=Debug -build . 
    make
    echo "debug" > .last_build
}

function analyze_build() {
    if [[ $(cat build/.last_build) != "analyze" ]]; then
        echo "detecting different build mode, recreating build dir"
        rm -rf build && mkdir build
    fi
    echo "building libcp2p in static analysis mode"
    cd build
    cmake -D CMAKE_BUILD_TYPE=Analyze ..
    cmake -D CMAKE_BUILD_TYPE=Analyze -build . 
    make
    echo "analyze" > .last_build
}

function ci_build() {
    if [[ $(cat build/.last_build) != "ci" ]]; then
        echo "detecting different build mode, recreating build dir"
        rm -rf build && mkdir build
    fi
    echo "building libcp2p in ci mode"
    cd build
    cmake -D CMAKE_BUILD_TYPE=CI ..
    cmake -D CMAKE_BUILD_TYPE=CI -build . 
    make
    echo "ci" > .last_build    
}

if [[ ! -d "build" ]]; then
    mkdir build
fi


case "$1" in
    ci)
        ci_build
        ;;
    debug)
        debug_build
        ;;
    analyze)
        analyze_build
        ;;
    *)
        release_build
        ;;
esac
