//
// Created by Ginkgo on 2025/2/25.
//

#include "CaeserCrypt.h"

unsigned char CaeserEncrypt(unsigned char data) {
    return (data + 0x55) % 0x100;
}

unsigned char CaeserDecrypt(unsigned char data) {
    return (data - 0x55 + 0x100) % 0x100;
}