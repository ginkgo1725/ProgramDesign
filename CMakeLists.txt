cmake_minimum_required(VERSION 3.29)
project(TextFile_CompressionAndRecovery__v5 C)

set(CMAKE_C_STANDARD 11)

add_executable(TextFile_CompressionAndRecovery__v5 main.c
        Hash.c
        Hash.h
        HeapSort.c
        HeapSort.h
        HuffmanCode.c
        HuffmanCode.h
        Define.h
        Compression.h
        Compression.c
        DecompressionTreeBased.h
        DecompressionTreeBased.c
        StateMachine.c
        StateMachine.h
        CaeserCrypt.c
        AffineCrypt.c
        CaeserCrypt.h
        AffineCrypt.h
        Tree.c
        Tree.h)
