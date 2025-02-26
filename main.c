/*
 * v5.7
 * 添加第二种加密解密算法（仿射加密）
 *
 * v5.6.5
 * 对结构做一点调整，添加了第二种解码方法（状态机）
 *
 * v5.6
 * 开始添加信息部分，完成
 * 添加信息、加密解密、验证信息部分成功结算画面
 *
 * v5.5.5
 * 开始加密解密部分
 * 完成caeser密码加密解密
 *
 * v5.5.1
 * 尝试优化压缩部分
 *
 * v5.5
 * 尝试改结构、加注释、完善代码逻辑
 *
 * v5.3
 * 尝试修复大文件读取问题
 * 修复成功结算
 * 解压缩部分成功结算界面
 * 加上了压缩率和耗时部分
 *
 * v5.2
 * 开启解压缩部分
 *
 * v5.1
 * 尝试将压缩部分放在函数中封装，包括输入文件名什么的
 * 封装成功结算界面
 *
 * v5
 * 尝试将两个结构体合并成一个  成功了
 * 文本压缩部分成功结算界面
*/

#include "Compression.h"
#include "DecompressionTreeBased.h"
#include "Define.h"
#include "StateMachine.h"


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
