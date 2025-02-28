
#include "Define.h"
#include "Compression.h"
#include "DecompressionTreeBased.h"
#include "DecompressionstateMachine.h"


int main() {
    setbuf(stdout, NULL);  // 禁用输出缓冲区

    int num = 0;
    printf("Please select the operation you wish to perform: \n");
    printf("1. Compress files\n2. Decompress files\n3. Exit\n");
    while (1) {
        scanf("%d", &num);                                                // 选择功能
        if (num == 1) {
            Compress();
            break;
        }
        else if (num == 2) {
            int select;
            printf("Please select the decompress method: \n");
            printf("1. Tree-based\n2. State machine\n");
            while (1) {
                scanf("%d", &select);                                    // 选择解码方式
                if (select == 1) {
                    DecompressTreeBased();
                    break;
                }
                else if (select == 2) {
                    DecompressStateMachine();
                    break;
                }
                else {
                    printf("Invalid input. Please enter your selection again.\n");
                }
            }
            break;
        }
        else if (num == 3) {
            printf("Exiting program.\n");
            break;
        }
        else printf("Invalid input. Please enter your selection again.\n");
    }
    return EXIT_SUCCESS;
}
