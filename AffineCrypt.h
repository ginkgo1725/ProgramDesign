//
// Created by Ginkgo on 2025/2/25.
//

#ifndef AFFINECRYPT_H
#define AFFINECRYPT_H

int CalculateModularInverse(int key, int num);

unsigned char AffineEncrypt(unsigned char data);

unsigned char AffineDecrypt(unsigned char data, int KEYAModInverse);
#endif //AFFINECRYPT_H
