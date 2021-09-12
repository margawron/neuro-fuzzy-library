file(GLOB sources
        source/auxiliary/*
        source/common/*
        source/data/*
        source/descriptors/*
        source/dissimilarities/*
        source/gan/*
        source/granules/*
        source/heuristics/*
        source/implications/*
        source/memory/*
        source/metrics/*
        source/neuro-fuzzy/*
        source/owas/*
        source/partitions/*
        source/readers/*
        source/service/*
        source/tnorms/*)

file(GLOB exec_sources
        source/experiments/*
        source/data/*)

file(COPY source/data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_library(NeuroFuzzyLib ${sources})

target_include_directories(NeuroFuzzyLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/source)

add_executable(NeuroFuzzyExperiments
        ${sources}
        ${exec_sources}
        source/main.cpp)