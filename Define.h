//
// Created by Ginkgo on 2025/2/19.
//

#ifndef DEFINE_H
#define DEFINE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tgmath.h>
#include <stdint.h>
#include <time.h>

#define MAXN 2147483647
#define BUFFERSIZE 1024 * 1024 * 1024
#define OFFSETCAESER 0x55
#define KEYA 13
#define KEYB 5

typedef struct {
    unsigned char data; //字符数据
    int count; //字符频度
    int parent, leftchild, rightchild;
    char code[256]; //字符编码
} CharCount, *HuffmanTree, HuffmanCode;

typedef struct CodeTree {
    unsigned char data; // 叶子节点字符
    struct CodeTree* left; // 0分支
    struct CodeTree* right; // 1分支
} CodeTree;


typedef struct StateChain {
    unsigned char data; // 叶子节点字符
    int next[2];
    int IsLeaf;
} StateChain;
#endif //DEFINE_H
