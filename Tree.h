//
// Created by Ginkgo on 2025/2/26.
//

// 根据编码构造树结构、根据树结构解码文本

#ifndef TREE_H
#define TREE_H
#include "Define.h"
#include "CaeserCrypt.h"
#include "AffineCrypt.h"
#include "Hash.h"

int HexTransTree(unsigned char c);
void FreeCodeTree(CodeTree* root);
CodeTree* CreateNode(char choice, char method);
void CreateCodeTree (CodeTree *root, const char *code, unsigned char data, char choice, char method);
int FileDecodingTree(CodeTree *root, const char *FileBinCode, char *HfmFileName, int TotalRawBytes, clock_t *time, char choice, char method);
#endif //TREE_H
