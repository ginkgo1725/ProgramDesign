//
// Created by Ginkgo on 2025/2/22.
//
#include "DecompressionTreeBased.h"

void DecompressTreeBased() {
    char CodeFileName[100];
    printf("Please enter the codefile name: \n");
    scanf("%s", CodeFileName);
    clock_t start1 = clock();

    // 打开文件
    FILE *CodeFile = fopen(CodeFileName, "rb");
    if (CodeFile == NULL) {
        perror("无法打开文件");
        return;
    }

    //先将code.txt处理成编码数组
    unsigned char line[200];
    CharCount retree[512];
    int size = 0; //不同字符个数

    fgets(line, sizeof(line), CodeFile); //除去第一行的干扰
    int TotalRawBytes = 0;
    for (int pos = 0; line[pos] != '\r' && line[pos] != '\n'; pos++) {
        TotalRawBytes = TotalRawBytes * 10 + HexTransTree(line[pos]);
    }
 //   printf("@decompress TotalRawBytes：%d\n",TotalRawBytes);

    while (fgets(line, sizeof(line), CodeFile) != NULL) {
        //保存编码
        retree[size].data = HexTransTree(line[2]) * 16 + HexTransTree(line[3]);

        //保存huffmancode长度
        retree[size].count = HexTransTree(line[7]) * 16 + HexTransTree(line[8]);

        int ByteSize = ceil(retree[size].count / 8.0);

        // 得到十进制的编码
        int DecCode = 0;
        for (int i = 0; i < ByteSize; i++) {
            DecCode = DecCode * 256 + (HexTransTree(line[12 + i * 5]) * 16 + HexTransTree(line[13 + i * 5]));  // 得到十进制的编码（含填充）
        }
        for (int i = retree[size].count; i < ByteSize * 8; i++) {
            DecCode = DecCode / 2;                                   // 得到十进制的编码（去填充）
        }

        //十进制转二进制储存进数组中
        for (int i = retree[size].count - 1; i >= 0; i--) {
            retree[size].code[i] = (char)(DecCode % 2 + '0');
            DecCode = DecCode / 2;
        }
        retree[size].code[retree[size].count] = '\0';

        size++;

    }
   // printf("\n%d\n", index);

    char HfmFileName[100];
    printf("Please enter the filename to decompress: \n");
    clock_t finish1 = clock();
    scanf("%s", HfmFileName);
    clock_t start2 = clock();



    printf("Please choose encrypt or not: (y or n)\n");
    char choice;
    clock_t finish3 = clock();
    scanf(" %c", &choice);
    clock_t start4 = clock();
    char method;
    clock_t finish4 = 0, start5 = 0;
    if (choice == 'y') {
        printf("Please choose encrypt method: (caeser(c) or affine(a))\n");
        finish4 = clock();
        scanf(" %c", &method);
        start5 = clock();
    }

    CodeTree* root = CreateNode(choice, method);
    if (root == NULL) {
        perror("内存分配失败");
        return;
    }
    for (int i = 0; i < size; i++) {
        //printf("%x %s\n", retree[i].data, retree[i].code);
        CreateCodeTree(root, retree[i].code, retree[i].data, choice, method);  //建code树
    }

    // 到此为止code.txt处理完毕

    //下面开始 重建哈夫曼树

    //下接 hfmfile.hfm 的处理
    // 打开文件
    FILE *HfmFile = fopen(HfmFileName, "rb");
    if (HfmFile == NULL) {
        perror("无法打开文件");
        return;
    }
    // 一次性读取整个文件（需获取文件大小）
    fseek(HfmFile, 0, SEEK_END);
    size_t BufferSize = ftell(HfmFile);
    fseek(HfmFile, 0, SEEK_SET);

    unsigned char *buffer = malloc(BufferSize);
    if (buffer == NULL) {
        perror("内存分配失败");
        fclose(HfmFile);
        return;
    }

    //总字节数量（压缩后）
    size_t TotalCompressedBytes = fread(buffer, 1, BufferSize, HfmFile);  //一次性读取


 //   printf("@decompress TotalCompressedBytes: %llu\n", TotalCompressedBytes);
    // 把1byte变成8bit
    char *FileBinCode = malloc(TotalCompressedBytes * 8 + 1);  // 储存压缩后2进制文本编码

    if (FileBinCode == NULL) {
        perror("内存分配失败");
        free(buffer);
        fclose(HfmFile);
        return;
    }

    memset(FileBinCode, 0, TotalCompressedBytes * 8 + 1);  // 初始化
    for (size_t tmp = 0; tmp < TotalCompressedBytes; tmp++) {
        for (int i = 0; i < 8; i++) {  // 低位到高位
            FileBinCode[tmp * 8 + i] = (char)((buffer[tmp] >> (7 - i) & 1) + '0');
        }
    }

    FileBinCode[TotalCompressedBytes * 8] = '\0';
//    printf("@decompress 压缩位数 WPL 填充为8倍数的：%llu\n", strlen(FileBinCode));


    clock_t TmpTime = 0;
    clock_t finish5= clock();
    //下面开始解码，按树状结构依次查找
    int success = FileDecodingTree(root, FileBinCode, HfmFileName, TotalRawBytes, &TmpTime, choice, method);
    clock_t start6 = clock();
    if (!success) {
        free(buffer);
        fclose(HfmFile);
        fclose(CodeFile);
        FreeCodeTree(root);
        return;
    }
 //   printf("@decompress 压缩字节数 TotalCompressedBytes：%llu\n", TotalCompressedBytes);

    clock_t finish6 = clock();
    double time = finish6 - start6 + finish5 - start5 + finish4 - start4 + finish3 + TmpTime - start2 + finish1 - start1;
    printf("Decompression time：%lfms\n", time);
    free(buffer);
    fclose(HfmFile);
    fclose(CodeFile);
    FreeCodeTree(root);
}