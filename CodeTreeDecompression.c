//
// Created by Ginkgo on 2025/2/22.
//
#include "CodeTreeDecompression.h"
#include "AffineCrypt.h"
#include "CaeserCrypt.h"
#include "hash.h"

int HexTransTree(unsigned char c) {
    int trans = 0;
    if (c >= '0' && c <= '9') {
        trans = c - '0';
    }
    else if (c >= 'A' && c <= 'F') {
        trans = c - 'A' + 10;
    }
    else if (c >= 'a' && c <= 'f') {
        trans = c - 'a' + 10;
    }
    return trans;
}

void FreeCodeTree(CodeTree* root) {
    if (root == NULL) return;
    FreeCodeTree(root->left);
    FreeCodeTree(root->right);
    free(root);
}

CodeTree* CreateNode(char choice, char method) {
    CodeTree* node = (CodeTree*) malloc(sizeof(CodeTree));
    if (node == NULL) {
        return NULL;
    }
    if (choice == 'y' && method == 'c') {         // caeser
        node->data = 0x55;
    }
    else if (choice == 'y' && method == 'a') {    // affine
        node->data = KEYB;
    }

    else node->data = 0;
    node->left = NULL;
    node->right = NULL;
    return node;
}

void CreateCodeTree (CodeTree *root, const char *code, unsigned char data, char choice, char method) {
    CodeTree* current = root;
    for (int i = 0; code[i] != '\0'; i++) {
        char bit = code[i];
        if (bit == '0') {
            if (current->left == NULL) {
                current->left = CreateNode(choice, method);
            }
            current = current->left;
        } else if (bit == '1') {
            if (current->right == NULL) {
                current->right = CreateNode(choice, method);
            }
            current = current->right;
        }
    }
    current->data = data;
}

int CompressedFileDecodingTree(CodeTree *root, const char *FileBinCode, char *HfmFileName, int TotalRawBytes, char choice, char method) {

   // unsigned char text[TotalRawBytes + 1];
    unsigned char* text = malloc(TotalRawBytes + 1);
    if (!text) {
        perror("内存分配失败");
        return 0;
    }
    memset(text, 0, TotalRawBytes + 1);

    CodeTree *current = root;
    int pos = 0;
    int TotalCompressedBits = 0;

    printf("@CompreesedFileDecoding FileBinCode: \n%llu\n", strlen(FileBinCode));  // TotalCompressedBits (8倍数版）


    printf("Please choose check or not: (y or n)\n");
    char check;
    scanf(" %c", &check);
    printf("your check is: %c\n", check);

    char SenderInfo[30],ReceiverInfo[30];
    if (check == 'y') {
        printf("Please enter the Sender and Receiver information: \n");
        scanf("%s %s", SenderInfo, ReceiverInfo);
    }

    int flag = 0, bool = 1;
    unsigned char DecompressedSenderInfo[30], DecompressedReceiverInfo[30];
    int SenderIndex = 0, ReceiverIndex = 0;
    int CodeTmp = 0;
    int ModInverse = 0;

    for (int index = 0; FileBinCode[index] != '\0'; index++) {
        if (FileBinCode[index] == '0') {
            if (current->left == NULL) break;
            current = current->left;
        }
        else if (FileBinCode[index] == '1') {
            if (current->right == NULL) break;
            current = current->right;
        }

        unsigned char DataTmp;
        DataTmp = current->data;
        if (choice == 'y' && method == 'c') {
            CodeTmp = 0x55;
            DataTmp = CaeserDecrypt(DataTmp);
        }
        else if(choice == 'y' && method == 'a') {
            CodeTmp = KEYB;
            ModInverse = CalculateModularInverse(KEYA, 256);
            DataTmp = AffineDecrypt(DataTmp, ModInverse);
        }

        if (current != NULL && current->data != CodeTmp) {   // 注意：这里current->data != () '\0'字符对应的数据，根据加密方式不同需要加以修改
            text[pos++] = current->data;
            if (check == 'y') {
                if (current->data == '\n' + CodeTmp) flag++;

                if (flag == 0) DecompressedSenderInfo[SenderIndex++] = DataTmp;
                else if (flag == 1 && current->data != '\n' + CodeTmp) DecompressedReceiverInfo[ReceiverIndex++] = DataTmp;

                if (flag == 2 && bool == 1) {
                    DecompressedSenderInfo[SenderIndex] = '\0';
                    DecompressedReceiverInfo[ReceiverIndex] = '\0';
                    printf("%llu %llu ", strlen(SenderInfo), strlen(ReceiverInfo));
                    printf("%s %s ", SenderInfo, ReceiverInfo);
                    printf("%d %d ", SenderIndex, ReceiverIndex);
                    printf("\n");
                    printf("解压出的验证信息如下：\n");
                    printf("发件人信息：%s\n收件人信息：%s\n", DecompressedSenderInfo, DecompressedReceiverInfo);
                    if (strcmp(DecompressedSenderInfo, SenderInfo) != 0 || strcmp(DecompressedReceiverInfo, ReceiverInfo) != 0) {
                        printf("验证失败，不解压\n");
                        return 0;
                    }
                    else {
                        printf("nothing happened...\n");
                    }
                    bool = 0;
                }
            }
            current = root;
        }

        TotalCompressedBits++;  // 记录WPL

        if (pos == TotalRawBytes) break;

    }
 //   printf("@Decoding TotalCompressedBits / WPL%d\n", TotalCompressedBits);
    printf("@Decoding pos TotalRawBytes: %d\n", pos);
 //   printf("@Decoding TotalRawBytes %d\n", TotalRawBytes)
 /*   ;
    text = (unsigned char*)realloc(text, pos + 1);
    text[TotalRawBytes] = '\0';
*/


    text[TotalRawBytes] = '\0';
/*
    for(int index = 0; index < TotalRawBytes; index++) {
        printf("%02x", text[index]);
    }
    printf("\n");
    printf("%s\n", text);
*/

    if (choice == 'y' && method == 'c') {
        for (int index = 0; index < TotalRawBytes; index++) {
            text[index] = (text[index] - 0x55 + 0x100) % 0x100;       // 解密文本
        }
    }
    else if (choice == 'y' && method == 'a') {
        for (int index = 0; index < TotalRawBytes; index++) {
            text[index] = AffineDecrypt(text[index], ModInverse);       // 解密文本
        }
    }
    //printf("%s\n", text);
/*
    for(int index = 0; index < TotalRawBytes; index++) {
        printf("%02x", text[index]);
    }
*/
    size_t FileNameLen = strlen(HfmFileName);
    char DecompressedFIleName[FileNameLen + 2];
    strncpy(DecompressedFIleName, HfmFileName, FileNameLen -4);
    DecompressedFIleName[FileNameLen - 4] = '\0';
    strcat(DecompressedFIleName, "_j.txt");
    /*
    char DecompressedFIleName[FileNameLen];
    strcpy(DecompressedFIleName, HfmFileName);
    DecompressedFIleName[FileNameLen] = '\0';
    DecompressedFIleName[FileNameLen - 1] = 't';
    DecompressedFIleName[FileNameLen - 2] = 'x';
    DecompressedFIleName[FileNameLen - 3] = 't';
    DecompressedFIleName[FileNameLen - 4] = '.';
    DecompressedFIleName[FileNameLen - 5] = 'j';
    DecompressedFIleName[FileNameLen - 6] = '_';
*/

    //写入文件
    FILE *DecompressedFile = fopen(DecompressedFIleName, "wb");
   // printf("\n%s\n", text);
    text[TotalRawBytes] = '\0';
    fwrite(text, 1, TotalRawBytes + 1, DecompressedFile);
    fclose(DecompressedFile);

    printf("\n");
    uint64_t ans = fnv1a_64(text, TotalRawBytes);
    printf("解压缩后文本Hash值: 0x%llx\n", ans);

 //   printf("@Decoding index/TotalRawBytes %d\n", pos);
 //   printf("@Decoding total/TotalCompressedBits %d\n", TotalCompressedBits);
    double CompressRate = ceil(TotalCompressedBits / 8.0) / TotalRawBytes;
    printf("@Decoding 压缩率：%.4lf%%\n", CompressRate * 100);
    free(text);

    return 1;
}

void DecompressTreebased() {
    FILE *CodeFile;
    unsigned char CodeFileName[100];

    printf("Please enter the codefile name: \n");
    scanf("%s", CodeFileName);

    clock_t start1 = clock();

    // 打开文件
    CodeFile = fopen(CodeFileName, "rb");
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
        //printf("%02x ", retree[index].data);  //验证
        //保存huffmancode长度
        retree[size].count = HexTransTree(line[7]) * 16 + HexTransTree(line[8]);
        // printf("%d ", retree[index].count);  //验证
        int ByteSize = ceil(retree[size].count / 8.0);
        //printf("%d ", length);

        // 得到十进制的编码
        int DecCode = 0;
        for (int i = 0; i < ByteSize; i++) {
            DecCode = DecCode * 256 + (HexTransTree(line[12 + i * 5]) * 16 + HexTransTree(line[13 + i * 5]));
        }
        for (int i = retree[size].count; i < ByteSize * 8; i++) {  // 得到十进制的编码
            DecCode = DecCode / 2;
        }
        //printf("%d ", DecCode);

        //十进制转二进制储存进数组中
        for (int i = retree[size].count - 1; i >= 0; i--) {
            retree[size].code[i] = (char)(DecCode % 2 + '0');
            DecCode = DecCode / 2;
        }
        retree[size].code[retree[size].count] = '\0';

        size++;

    }
   // printf("\n%d\n", index);


    FILE *HfmFile;
    char HfmFileName[100];


    printf("Please enter the filename to decompress: \n");
    clock_t finish1 = clock();
    scanf("%s", HfmFileName);
    clock_t start2 = clock();


    printf("Please choose encrypt or not: (y or n)\n");
    char choice;
    scanf(" %c", &choice);
    char method;
    if (choice == 'y') {
        printf("Please choose encrypt method: (caeser(c) or )\n");
        scanf(" %c", &method);
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
    HfmFile = fopen(HfmFileName, "rb");
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


    for (size_t tmp = 0; tmp < TotalCompressedBytes; tmp++) {
          //  printf("0x%02x ", buffer[tmp]);
    }

    for (size_t tmp = 0; tmp < TotalCompressedBytes; tmp++) {
     //   printf("%x ", buffer[tmp]);
    }

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
            FileBinCode[tmp * 8 + i] = (buffer[tmp] >> (7 - i) & 1) + '0';
        }
    }

    FileBinCode[TotalCompressedBytes * 8] = '\0';
//    printf("@decompress 压缩位数 WPL 填充为8倍数的：%llu\n", strlen(FileBinCode));




    int status;
    //下面开始解码，按树状结构依次查找
    status = CompressedFileDecodingTree(root, FileBinCode, HfmFileName, TotalRawBytes, choice, method);

    if (!status) {
        free(buffer);
        fclose(HfmFile);
        fclose(CodeFile);
        FreeCodeTree(root);
        return;
    }
 //   printf("@decompress 压缩字节数 TotalCompressedBytes：%llu\n", TotalCompressedBytes);

    clock_t finish2 = clock();
    double time = (double)(finish2 - start2 + finish1 - start1) ;
    printf("@decompress 解压缩耗时：%.4lfms\n", time);
    free(buffer);
    fclose(HfmFile);
    fclose(CodeFile);
    FreeCodeTree(root);
}