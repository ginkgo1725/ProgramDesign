//
// Created by Ginkgo on 2025/2/22.
//
#include "DecompressionStateMachine.h"
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
    for (int index = 0; index < size * 2 - 1; index++) {    // 初始化
        (*states)[index].data = 0;
        (*states)[index].next[0] = -1;
        (*states)[index].next[1] = -1;
        (*states)[index].IsLeaf = 0;
    }

    int StateCount = 1;
    for (int i = 0; i < size; i++) {
        int CurrentState = 0;
        for (int j = 0; table[i].code[j] != '\0'; j++) {   // 根据编码表将编码储存在顺序数组中
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

    return StateCount;
}

int FileDecodingStateMachine(StateChain *states, const char *FileBinCode, char *HfmFileName, int TotalRawBytes, clock_t *time, char choice, char method) {
    clock_t start1 = clock();
    unsigned char* text = malloc(TotalRawBytes + 1);
    if (!text) {
        perror("内存分配失败");
        free(text);
        return 0;
    }
    memset(text, 0, TotalRawBytes + 1);

    int pos = 0;
    int TotalCompressedBits = 0;

    printf("@CompressedFileDecoding FileBinCode: \n%llu\n", strlen(FileBinCode));  // TotalCompressedBits (8倍数版）


    printf("Please choose check or not: (y or n)\n");
    char check = 0;
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

    int CurrentState = 0;

    for (int index = 0; FileBinCode[index] != '\0'; index++) {
        int bit = FileBinCode[index] - '0';
        CurrentState = states[CurrentState].next[bit];

        unsigned char DataTmp = states[CurrentState].data;
        if (choice == 'y' && method == 'c') {
            CodeTmp = OFFSETCAESER;
            DataTmp = CaeserDecrypt(DataTmp);
        }
        else if(choice == 'y' && method == 'a') {
            CodeTmp = KEYB;
            ModInverse = CalculateModularInverse(KEYA, 256);
            DataTmp = AffineDecrypt(DataTmp, ModInverse);
        }

        if (CurrentState != -1 && states[CurrentState].IsLeaf) {   // 注意：这里current->data != () '\0'字符对应的数据，根据加密方式不同需要加以修改
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

            CurrentState = 0;
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


    //写入文件
    FILE *DecompressedFile = fopen(DecompressedFIleName, "wb");
   // printf("\n%s\n", text);
    text[TotalRawBytes] = '\0';
    fwrite(text, 1, TotalRawBytes + 1, DecompressedFile);
    fclose(DecompressedFile);

    printf("\n");
    printf("WPL: \n%d\n", TotalCompressedBits);
    uint64_t ans = fnv1a_64(text, TotalRawBytes);
    printf("Hash value of the text before decompression: \n0x%llx\n", ans);

 //   printf("@Decoding index/TotalRawBytes %d\n", pos);
 //   printf("@Decoding total/TotalCompressedBits %d\n", TotalCompressedBits);
    double CompressRate = ceil(TotalCompressedBits / 8.0) / TotalRawBytes;
    printf("Compression：\n%.4lf%%", CompressRate * 100);
    free(text);

    clock_t finish3 = clock();
    *time = finish3 - start3 + finish2 - start2 + finish1 -start1;
    return 1;
}

void DecompressStateMachine() {
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
        TotalRawBytes = TotalRawBytes * 10 + HexTransMachine(line[pos]);
    }
    //   printf("@decompress TotalRawBytes：%d\n",TotalRawBytes);

    while (fgets(line, sizeof(line), CodeFile) != NULL) {
        //保存编码
        retree[size].data = HexTransMachine(line[2]) * 16 + HexTransMachine(line[3]);

        //保存huffmancode长度
        retree[size].count = HexTransMachine(line[7]) * 16 + HexTransMachine(line[8]);

        int ByteSize = ceil(retree[size].count / 8.0);

        // 得到十进制的编码
        int DecCode = 0;
        for (int i = 0; i < ByteSize; i++) {
            DecCode = DecCode * 256 + (HexTransMachine(line[12 + i * 5]) * 16 + HexTransMachine(line[13 + i * 5]));
        }
        for (int i = retree[size].count; i < ByteSize * 8; i++) {  // 得到十进制的编码
            DecCode = DecCode / 2;
        }

        //十进制转二进制储存进数组中
        for (int i = retree[size].count - 1; i >= 0; i--) {
            retree[size].code[i] = (char)(DecCode % 2 + '0');
            DecCode = DecCode / 2;
        }
        retree[size].code[retree[size].count] = '\0';

        size++;

    }


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

    StateChain* root;
    BuildStateMachine(&root, retree, size);
    // 到此为止code.txt处理完毕


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


    clock_t TmpTime = 0;
    clock_t finish5= clock();
    //下面开始解码
    int success = FileDecodingStateMachine(root, FileBinCode, HfmFileName, TotalRawBytes, &TmpTime, choice, method);
    clock_t start6 = clock();

    if (!success) {
        free(buffer);
        fclose(HfmFile);
        fclose(CodeFile);
        return;
    }

    //   printf("@decompress 压缩字节数 TotalCompressedBytes：%llu\n", TotalCompressedBytes);

    clock_t finish6 = clock();
    double time = finish6 - start6 + finish5 - start5 + finish4 - start4 + finish3 + TmpTime - start2 + finish1 - start1;
    printf("Decompression time：%lfms\n", time);
    free(buffer);
    fclose(HfmFile);
    fclose(CodeFile);
}