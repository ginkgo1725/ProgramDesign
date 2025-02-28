//
// Created by Ginkgo on 2025/2/25.
//

// 状态机解码文本

#include "Define.h"

#ifndef STATUSMACHINE_H
#define STATUSMACHINE_H

int HexTransMachine(unsigned char c);

int BuildStateMachine (StateChain **states, HuffmanCode *table, int size);

int FileDecodingStateMachine(StateChain *states, const char *FileBinCode, char *HfmFileName, int TotalRawBytes, clock_t *time, char choice, char method);

void DecompressStateMachine();

#endif //STATUSMACHINE_H
