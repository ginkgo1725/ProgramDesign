//
// Created by Ginkgo on 2025/2/25.
//

#include "Define.h"
#include "AffineCrypt.h"

int CalculateModularInverse(int key, int num) {
    int raw = num, tmp, q;
    int x = 0, y = 1;
    if (num == 1)
        return 0;
    while (key > 1) {
        q = key / num;   // 商
        tmp = num;
        num = key % num;   // 余数
        key = tmp;
        tmp = x;
        x = y - q * x;
        y = tmp;
    }
    if (y < 0)
        y += raw;
    return (key == 1) ? y : -1;  // 返回-1表示无逆元
}

unsigned char AffineEncrypt(unsigned char data) {
    return (KEYA * data + KEYB) % 0x100;
}

unsigned char AffineDecrypt(unsigned char data, int KEYAModInverse) {
    return (KEYAModInverse * (data - KEYB + 0x100)) % 0x100;
}
