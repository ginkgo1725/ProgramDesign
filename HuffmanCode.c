//
// Created by Ginkgo on 2025/2/20.
//
#include "HuffmanCode.h"

void CreateHuffmanTree(HuffmanTree HT, int size) {    // size: 不同字符个数
    if (size == 1) return;

    for (int i = size; i < size * 2 - 1; i++) {
        int LeftMin = MAXN, RightMin = MAXN;    // 记录权值
        int LeftNode = -1, RightNode = -1;
        for (int j = 0; j < i; j++) {
            if (HT[j].parent != -1) continue;    // 跳过已有父节点的节点
            //根据规则选择节点建树
            if (HT[j].count < LeftMin) {
                RightMin = LeftMin;
                RightNode = LeftNode;
                LeftMin = HT[j].count;
                LeftNode = j;
                continue;
            }
            if (HT[j].count == LeftMin && HT[LeftNode].data > HT[j].data) {
                RightMin = LeftMin;
                RightNode = LeftNode;
                LeftMin = HT[j].count;
                LeftNode = j;
                continue;

            }
            if (HT[j].count < RightMin) {
                RightMin = HT[j].count;
                RightNode = j;
                continue;
            }
            if (HT[j].count == RightMin && HT[RightNode].data > HT[j].data) {
                    RightMin = HT[j].count;
                    RightNode = j;
            }
        }

        HT[LeftNode].parent = i;
        HT[RightNode].parent = i;
        HT[i].leftchild = LeftNode;
        HT[i].rightchild = RightNode;
        HT[i].count = HT[LeftNode].count + HT[RightNode].count;
        //合并节点后data值以便后续比较
        HT[i].data = (HT[LeftNode].data > HT[RightNode].data) ? HT[LeftNode].data : HT[RightNode].data;
    }
}

void CreateHuffmanCode(HuffmanCode *code, int size) {
    char TmpCode[256] = {0};
    int start = 0;

    // 从叶子节点向根查找得到2进制编码
    for (int i = 0; i < size; i++) {
        memset(TmpCode, 0, 256);    // 清空中间字符数组内容
        start = 255;
        TmpCode[start] = '\0';
        int current = i, next = code[i].parent;    // 当前节点、它的双亲节点
        while (next != -1) {
            start--;
            if (code[next].leftchild == current) {
                TmpCode[start] = '0';
            }
            else if (code[next].rightchild == current) {
                TmpCode[start] = '1';
            }
            current = next;
            next = code[current].parent;
        }
        strcpy(code[i].code, &TmpCode[start]);    // 将编码存入哈夫曼数组中
    }

    //计算WPL值
    size_t WPL = 0, BitSize = 0;
    for (int i = 0; i < size; i++) {
        BitSize = strlen(code[i].code);
        WPL = WPL + code[i].count * BitSize;
    }

  //  printf("@CreateHuffmanCode.c WPL: %llu\n", WPL);


    FILE *CodeFile = fopen("code.txt", "wb");
    if (CodeFile == NULL) {
        perror("无法打开文件");
        fclose(CodeFile);
    }

    int TotalRawBytes = code[size * 2 - 2].count;
  //  printf("@CreateHuffmanCode.c TotalRawBytes: %d\n", TotalRawBytes);    // 总字符频度(总字节数量)

    fprintf(CodeFile, "%d\n", TotalRawBytes);    // 文件输出

    char* order = "data";
    HeapSort(code, size, order);    // 根据字符数据排序编码

  //  printf("@HuffmanCode 堆排序后\n");

    // 将2进制编码转化为8bit位编码
    memset(TmpCode, 0, 256);
    for (int i = 0; i < size; i++) {    //不足8位的补齐成8位
        BitSize = strlen(code[i].code);
        strcpy(TmpCode, code[i].code);
        int BitSizeFilled = ceil(BitSize / 8.0) * 8;    // 编码长度（位）
        int ByteSize = BitSizeFilled / 8;    //编码长度（字节）

        if (BitSize % 8 != 0) {    // 不足8位在末尾补0
            size_t pos = 0;
            for (pos = BitSize; pos < BitSizeFilled; pos++) {
                TmpCode[pos] = '0';
            }
            TmpCode[pos] = '\0';
        }

        unsigned char bit[ByteSize];
        memset(bit, 0, ByteSize * sizeof(unsigned char));

        fprintf(CodeFile, "0x%02x 0x%02zx ", code[i].data, BitSize);  // 字符数据 编码长度

        for (int k = 0; k < ByteSize; k++) {
            for (int j = 0; j < 8; j++) {
                bit[k] = (bit[k] << 1) | (TmpCode[k * 8 + j] - '0');
            }
            fprintf(CodeFile, "0x%02x ", bit[k]);  // 编码
        }
        fprintf(CodeFile, "\n");
    }
    fclose(CodeFile);
}

void TextHuffmanCoding(HuffmanCode *code, FILE *RawFile, int size, char filename[], char choice, char method) {
    size_t TotalCompressedBits = 0, BitSize = 0;
    for (int i = 0; i < size; i++) {
        BitSize = strlen(code[i].code);
        TotalCompressedBits = TotalCompressedBits + code[i].count * BitSize;
    }
    int TotalCompressedBytes = ceil(TotalCompressedBits / 8.0);  // 压缩后文本总字节

    unsigned char *buffer = malloc(BUFFERSIZE);
    if (buffer == NULL) {
        perror("内存分配失败");
        fclose(RawFile);
        return;
    }

    // 将编码写入.hfm文件
    size_t FileNameLen = strlen(filename);
    filename[FileNameLen - 1] = 'm';
    filename[FileNameLen - 2] = 'f';
    filename[FileNameLen - 3] = 'h';
    FILE *CompressedFile = fopen(filename, "wb");
    if (CompressedFile == NULL) {
        perror("无法打开文件");
        free(buffer);
        return;
    }

    unsigned char *HexCode = malloc(TotalCompressedBytes);
    if (HexCode == NULL) {
        perror("内存分配失败");
        free(buffer);
        free(CompressedFile);
        fclose(RawFile);
        return;
    }
    memset(HexCode, 0, TotalCompressedBytes);    // 初始化清0

    uint8_t TmpByte = 0;
    int BitCount = 0;
    size_t HexPos = 0;

    fseek(RawFile, 0, SEEK_SET);
    size_t TotalRawBytes = fread(buffer, 1, BUFFERSIZE, RawFile);

    //hash一下
    uint64_t RawHash = fnv1a_64(buffer, TotalRawBytes);

    for (size_t tmp = 0; tmp < TotalRawBytes; tmp++) {
        unsigned char TmpChar = buffer[tmp];
        // 加密操作
        if (choice == 'y') {
            if (method == 'c') TmpChar = CaeserEncrypt(TmpChar);
            if (method == 'a') TmpChar = AffineEncrypt(TmpChar);
        }

        for (int i = 0; i < size; i++) {
            if (TmpChar == code[i].data) {                                  // 在编码表中查找对应字符进行编码
                char *CurrentCode = code[i].code;
                size_t CodeLen = strlen(code[i].code);
                for (size_t pos = 0; pos < CodeLen; pos++) {
                    // 填充字节
                    TmpByte = (TmpByte << 1) | (CurrentCode[pos] - '0');    // 将code转化为二进制
                    BitCount++;
                    if (BitCount == 8) {                                    // 满8位存入数组中并写入.hfm文件中
                        HexCode[HexPos++] = TmpByte;
                        fwrite(&TmpByte, 1, 1, CompressedFile);    // 写入文本
                        TmpByte = 0;                                        //  清零重置，等待下一个8位
                        BitCount = 0;
                    }
                }
                break;
            }
        }
    }

    // 处理剩余位
    if (BitCount > 0) {    // 大于8说明需要填充
        TmpByte <<= (8 - BitCount);
        HexCode[HexPos++] = TmpByte;
        fwrite(&TmpByte, 1, 1, CompressedFile);
    }
    HexCode[HexPos] = '\0';

    fclose(RawFile);
    fclose(CompressedFile);

    printf("WPL: \n%llu\n", TotalCompressedBits);
    printf("Compressed size (in bytes): \n%d\n", TotalCompressedBytes);
    

    if (strcmp(filename, "example.hfm") == 0) {
        printf("Total compressed value: \n");
        for (int index = 0; index < TotalCompressedBytes; index++) {
            printf("0x%02x ", HexCode[index]);
        }
        printf("\n");
    }
    else {
        printf("Compressed value (last %d bytes): \n", (TotalCompressedBytes >= 16) ? 16 : TotalCompressedBytes);
        const int start = (TotalCompressedBytes >= 16) ? (TotalCompressedBytes - 16) : 0;
        for (int index = start; index < TotalCompressedBytes; index++) {
            printf("0x%02x ", HexCode[index]);
        }
        printf("\n");
    }

    //hash一下
    uint64_t CompressedHash = fnv1a_64(HexCode, TotalCompressedBytes);

    printf("Hash value of the text before compression: \n0x%llx\n", RawHash);
    printf("Hash value of the text after compression: \n0x%llx\n", CompressedHash);

    free(buffer);
}