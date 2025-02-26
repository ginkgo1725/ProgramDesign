//
// Created by Ginkgo on 2025/2/26.
//
#include "Tree.h"

int HexTransTree(unsigned char c) {   // 字符16进制转换10进制
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

CodeTree* CreateNode(char choice, char method) {   // 创建节点
    CodeTree* node = (CodeTree*) malloc(sizeof(CodeTree));
    if (node == NULL) {
        return NULL;
    }

    if (choice == 'y') {
        if (method == 'c') node->data = OFFSETCAESER;  // caeser
        if (method == 'a') node->data = KEYB;          // afiine
    }
    else node->data = 0;

    node->left = NULL;
    node->right = NULL;
    return node;
}

void CreateCodeTree (CodeTree *root, const char *code, unsigned char data, char choice, char method) {
    CodeTree* current = root;
    for (int i = 0; code[i] != '\0'; i++) {        // 根据字符编码（2进制）构建解码树
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

int FileDecodingTree(CodeTree *root, const char *FileBinCode, char *HfmFileName, int TotalRawBytes, clock_t *time, char choice, char method) {
    clock_t start1 = clock();
    unsigned char* text = malloc(TotalRawBytes + 1);
    if (!text) {
        perror("内存分配失败");
        return 0;
    }
    memset(text, 0, TotalRawBytes + 1);

    CodeTree *current = root;
    int pos = 0;
    int TotalCompressedBits = 0;

//    printf("@CompreesedFileDecoding FileBinCode: \n%llu\n", strlen(FileBinCode));  // TotalCompressedBits (8倍数版）


    printf("Please choose check or not: (y or n)\n");
    char check;
    clock_t finish1 = clock();
    scanf(" %c", &check);
    clock_t start2 = clock();
    printf("your check is: %c\n", check);

    char SenderInfo[30],ReceiverInfo[30];
    clock_t finish2 = clock();
    if (check == 'y') {
        printf("Please enter the Sender information: \n");
        scanf("%s", SenderInfo);
        printf("Please enter the Receiver information: \n");
        scanf("%s", ReceiverInfo);
    }
    clock_t start3 = clock();

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

        unsigned char DataTmp = current->data;
        if (choice == 'y' && method == 'c') {
            CodeTmp = OFFSETCAESER;
            DataTmp = CaeserDecrypt(DataTmp);
        }
        else if(choice == 'y' && method == 'a') {
            CodeTmp = KEYB;
            ModInverse = CalculateModularInverse(KEYA, 256);
            DataTmp = AffineDecrypt(DataTmp, ModInverse);
        }

        if (current != NULL && DataTmp != '\0') {   // 注意：这里current->data != () '\0'字符对应的数据，根据加密方式不同需要加以修改
            text[pos++] = DataTmp;
            if (check == 'y') {
                if (flag == 0 && DataTmp != '\n') DecompressedSenderInfo[SenderIndex++] = DataTmp;
                else if (flag == 1 && DataTmp != '\n') DecompressedReceiverInfo[ReceiverIndex++] = DataTmp;

                if (DataTmp == '\n') flag++;

                if (flag == 2 && bool == 1) {
                    DecompressedSenderInfo[SenderIndex] = '\0';
                    DecompressedReceiverInfo[ReceiverIndex] = '\0';
                    printf("Decompressed verification information：\n");
                    printf("Sender Info：%s\nReceiver Info：%s\n", (char*)DecompressedSenderInfo, (char*)DecompressedReceiverInfo);
                    if (strcmp(DecompressedSenderInfo, SenderInfo) != 0 || strcmp(DecompressedReceiverInfo, ReceiverInfo) != 0) {
                        printf("Verification failed, program terminated\n");
                        return 0;
                    }
                    else {
                        printf("Verification successful, continuing...\n");
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
 //   printf("@Decoding pos TotalRawBytes: %d\n", pos);
 //   printf("@Decoding TotalRawBytes %d\n", TotalRawBytes)

    text[TotalRawBytes] = '\0';

    printf("Text:\n");
    for(int index = 0; index < TotalRawBytes; index++) {
        printf("%02x", text[index]);
    }

    printf("\n");
    printf("%s\n", (char*)text);

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
    FILE *DecompressedFile = fopen(DecompressedFIleName, "w");
   // printf("\n%s\n", text);
    text[TotalRawBytes] = '\0';
    fwrite(text, 1, TotalRawBytes + 1, DecompressedFile);
    fclose(DecompressedFile);

    printf("\n");
    uint64_t ans = fnv1a_64(text, TotalRawBytes);
    printf("Hash value of the text before decompression: 0x%llx\n", ans);

    double CompressRate = ceil(TotalCompressedBits / 8.0) / TotalRawBytes;
    printf("Compression rate：%.4lf%%\n", CompressRate * 100);
    free(text);

    clock_t finish3 = clock();
    *time = finish3 - start3 + finish2 - start2 + finish1 -start1;
    return 1;
}
