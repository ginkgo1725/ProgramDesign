//
// Created by Ginkgo on 2025/2/22.
//
#include "Compression.h"
#include "HeapSort.h"
#include "HuffmanCode.h"
#include "AffineCrypt.h"
#include "CaeserCrypt.h"

void Compress() {
    char RawFileName[100];
    printf("Enter file name: \n");
    scanf("%s", RawFileName);
    clock_t start1 = clock();

    // 打开文件
    FILE *RawFile = fopen(RawFileName, "rb");
    if (RawFile == NULL) {
        perror("无法打开文件");
        return;
    }

    unsigned char *buffer = malloc(BUFFERSIZE);
    if (buffer == NULL) {
        perror("内存分配失败");
        fclose(RawFile);
        return;
    }

    printf("Please choose insert information or not: (y or n)\n");
    char insert;
    clock_t finish1 = clock();
    scanf(" %c", &insert);
    clock_t start2 = clock();

    FILE *InsertInfoFile = fopen("InsertInfo.txt", "w");
    if (InsertInfoFile == NULL) {
        perror("无法创建文件");
        return;  // 或处理错误
    }

    clock_t finish2 = 0, start3 = 0;
    if (insert == 'y') {
        char InsertInfoSender[30];
        char InsertInfoReceiver[30];

        finish2 = clock();
        scanf("%s", InsertInfoSender);
        scanf("%s", InsertInfoReceiver);
        start3 = clock();

        fwrite(InsertInfoSender, sizeof(char), strlen(InsertInfoSender), InsertInfoFile);
        fputs("\n", InsertInfoFile);
        fwrite(InsertInfoReceiver, sizeof(char), strlen(InsertInfoReceiver), InsertInfoFile);
        fputs("\n", InsertInfoFile);

        int ch;
        while ((ch = fgetc(RawFile)) != EOF) {
            fputc(ch, InsertInfoFile);
        }
        fclose(InsertInfoFile);
    }

    CharCount word[512] = {{0}};  //存储字符编码、字符频度、之后用来存储哈夫曼树

    int size = 0;  // 记录不同字符个数

    size_t TotalRawBytes;//总字节数量

    printf("Please choose encrypt or not: (y or n)\n");
    char choice;
    clock_t finish3 = clock();
    scanf(" %c", &choice);
    clock_t start4 = clock();

    char method;
    clock_t finish4 = 0, start5 = 0;
    if (choice == 'y') {
        printf("Please choose encrypt method: \n");
        printf("c: Caeser cipher\n");
        printf("a: Affine cipher\n");
        finish4 = clock();
        scanf(" %c", &method);
        start5 = clock();
    }


    int count[256] = {0};
    if (insert == 'y') {
        InsertInfoFile = fopen("InsertInfo.txt", "r");
        while ((TotalRawBytes = fread(buffer, 1, BUFFERSIZE, InsertInfoFile)) > 0) {   // 这里没单独处理，所以TotalRawBytes最后得到值为0
            for (int index = 0; index < TotalRawBytes; index++) {
                if (choice == 'y' && method == 'c') {  // 是否加密
                    buffer[index] = CaeserEncrypt(buffer[index]);
                }
                if (choice == 'y' && method == 'a') {  // 是否加密
                    buffer[index] = AffineEncrypt(buffer[index]);
                }
                count[buffer[index]]++;
            }
        }
    }
    else {
        while ((TotalRawBytes = fread(buffer, 1, BUFFERSIZE, RawFile)) > 0) {   // 这里没单独处理，所以TotalRawBytes最后得到值为0
            for (int index = 0; index < TotalRawBytes; index++) {
                if (choice == 'y' && method == 'c') {  // 是否加密
                    buffer[index] = CaeserEncrypt(buffer[index]);
                }
                if (choice == 'y' && method == 'a') {  // 是否加密
                    buffer[index] = AffineEncrypt(buffer[index]);
                }
                count[buffer[index]]++;
            }
        }
    }


    // 将记录下来的字符数据和字符频度存储到哈夫曼数组中
    for (int i = 0; i < 256; i++) {
        if (count[i] > 0) {          // 仅处理出现过的字符
            word[size].data = (unsigned char)i;  // 字符值
            word[size].count = count[i];         // 频度
            size++;                              // 更新有效字符数
        }
    }

    free(buffer);

    char* order = "count";  // 以频度为排序依据
    HeapSort(word, size, order); //堆排序按字符频度升序排序编码


/*
    // 显示词频
    for (int i = 0; i < size; i++) {
        printf("%02x ", word[i].data);
        printf("%d\n", word[i].count);
    }
*/

    // 下起哈夫曼编码区

    // 初始化哈夫曼数组
    for (int i = 0; i < size * 2 - 1; i++) {
        if (i >= size) word[i].count = 0;
        word[i].parent = word[i].leftchild = word[i].rightchild = -1;
    }


    CreateHuffmanTree(word, size);    // 建树

    CreateHuffmanCode(word, size);    // 获得哈夫曼编码


    fseek(InsertInfoFile, 0, SEEK_SET);    // 将指针回到文本开头
    fseek(RawFile, 0, SEEK_SET);    // 将指针回到文本开头

    if (insert == 'y') TextHuffmanCoding(word, InsertInfoFile, size, RawFileName, choice, method);    // 为更新后文本编码
    else TextHuffmanCoding(word, RawFile, size, RawFileName, choice, method);    // 为原文本编码


    //计算时间
    clock_t finish5 = clock();
    double time = (double)(finish5 - start5 + finish4 - start4 + finish3 - start3 + finish2 - start2 + finish1 - start1);
    printf("Compression time：\n%.4lfms", time);

    // 关闭文件
    fclose(RawFile);
    fclose(InsertInfoFile);
}