//
// Created by Ginkgo on 2025/2/22.
//
#include "StateMachine.h"
#include "AffineCrypt.h"
#include "CaeserCrypt.h"
#include "hash.h"

int HexTransMachine(unsigned char c) {
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

int BuildStateMachine (StateChain **states, HuffmanCode *table, int size) {
    // 初始状态数组大小
    *states = malloc((size * 2 - 1) * sizeof(StateChain));
    for (int index = 0; index < size * 2 - 1; index++) {
        (*states)[index].data = 0;
        (*states)[index].next[0] = -1;
        (*states)[index].next[1] = -1;
        (*states)[index].IsLeaf = 0;
    }

    int StateCount = 1;
    for (int i = 0; i < size; i++) {
        int CurrentState = 0;
        //printf("%2d %02x %s\n", i, table[i].data, table[i].code);
        for (int j = 0; table[i].code[j] != '\0'; j++) {
            int bit = table[i].code[j] - '0';

            if ((*states)[CurrentState].next[bit] == -1) {
                (*states)[CurrentState].next[bit] = StateCount++;
            }

            // 进入下一个状态
            CurrentState = (*states)[CurrentState].next[bit];
        }
        (*states)[CurrentState].IsLeaf = 1;
        (*states)[CurrentState].data = table[i].data;
    }
/*
    printf("\nStatecount: %d\n", StateCount);
    for (int i = 0; i < StateCount; i++) {
        printf("%2d ", i);
        printf("%02x %d %d %d\n", (*states)[i].data, (*states)[i].next[0], (*states)[i].next[1], (*states)[i].IsLeaf);
    }
    */
    return StateCount;
}

int CompressedFileDecodingStateMachine(StateChain *states, const char *FileBinCode, char *HfmFileName, int TotalRawBytes, char choice, char method) {

   // unsigned char text[TotalRawBytes + 1];
    unsigned char* text = malloc(TotalRawBytes + 1);
    if (!text) {
        perror("内存分配失败");
        return 0;
    }
    memset(text, 0, TotalRawBytes + 1);

    int pos = 0;
    int TotalCompressedBits = 0;

    printf("@CompressedFileDecoding FileBinCode: \n%llu\n", strlen(FileBinCode));  // TotalCompressedBits (8倍数版）



    printf("Please choose check or not: (y or n)\n");
    char check = 0;
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

    int CurrentState = 0;

    for (int index = 0; FileBinCode[index] != '\0'; index++) {
        int bit = FileBinCode[index] - '0';
        CurrentState = states[CurrentState].next[bit];

        unsigned char DataTmp;
        DataTmp = states[CurrentState].data;
        if (choice == 'y' && method == 'c') {
            CodeTmp = 0x55;
            DataTmp = CaeserDecrypt(DataTmp);
        }
        else if(choice == 'y' && method == 'a') {
            CodeTmp = KEYB;
            ModInverse = CalculateModularInverse(KEYA, 256);
            DataTmp = AffineDecrypt(DataTmp, ModInverse);
        }

        if (CurrentState != -1 && states[CurrentState].IsLeaf) {   // 注意：这里current->data != () '\0'字符对应的数据，根据加密方式不同需要加以修改
            text[pos++] = states[CurrentState].data;

            if (check == 'y') {
                if (states[CurrentState].data == '\n' + CodeTmp) flag++;

                if (flag == 0) DecompressedSenderInfo[SenderIndex++] = DataTmp;
                else if (flag == 1 && states[CurrentState].data != '\n' + CodeTmp) DecompressedReceiverInfo[ReceiverIndex++] = (states[CurrentState].data - CodeTmp + 0x100) % 0x100;

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

            CurrentState = 0;
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
            text[index] = (text[index] - 0x55 + 0x100) % 0x100;
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

void DecompressStateMachine() {
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
        TotalRawBytes = TotalRawBytes * 10 + HexTransMachine(line[pos]);
    }
    //   printf("@decompress TotalRawBytes：%d\n",TotalRawBytes);

    while (fgets(line, sizeof(line), CodeFile) != NULL) {
        //保存编码
        retree[size].data = HexTransMachine(line[2]) * 16 + HexTransMachine(line[3]);
        //printf("%02x ", retree[index].data);  //验证
        //保存huffmancode长度
        retree[size].count = HexTransMachine(line[7]) * 16 + HexTransMachine(line[8]);
        // printf("%d ", retree[index].count);  //验证
        int ByteSize = ceil(retree[size].count / 8.0);
        //printf("%d ", length);

        // 得到十进制的编码
        int DecCode = 0;
        for (int i = 0; i < ByteSize; i++) {
            DecCode = DecCode * 256 + (HexTransMachine(line[12 + i * 5]) * 16 + HexTransMachine(line[13 + i * 5]));
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

    StateChain* root;

    int StateCount = BuildStateMachine(&root, retree, size);

    /*
    printf("%d\n", StateCount);
    for (int i = 0; i < StateCount; i++) {
        printf("%02x %d %d %d\n", root[i].data, root[i].next[0], root[i].next[1], root[i].IsLeaf);
    }
    */
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


    //下面开始解码
    CompressedFileDecodingStateMachine(root, FileBinCode, HfmFileName, TotalRawBytes, choice, method);


    //   printf("@decompress 压缩字节数 TotalCompressedBytes：%llu\n", TotalCompressedBytes);

    clock_t finish2 = clock();
    double time = (double)(finish2 - start2 + finish1 - start1) ;
    printf("@decompress 解压缩耗时：%.4lfms\n", time);
    free(buffer);
    fclose(HfmFile);
    fclose(CodeFile);
}