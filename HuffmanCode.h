//
// Created by Ginkgo on 2025/2/20.
//

#include "Define.h"
#include "Hash.h"
#include "HeapSort.h"
#include "CaeserCrypt.h"
#include "AffineCrypt.h"

#ifndef HUFFMANCODE_H
#define HUFFMANCODE_H

void CreateHuffmanTree (HuffmanTree HT, int size);

void CreateHuffmanCode (HuffmanCode *Code, int size);

void TextHuffmanCoding (HuffmanCode *code, FILE *RawFile, int size, char filename[], char choice, char method);

#endif //HUFFMANCODE_H
