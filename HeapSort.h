//
// Created by Ginkgo on 2025/2/19.
//
#ifndef HEAPSORT_H
#define HEAPSORT_H

#include "Define.h"

void swap(CharCount *a, CharCount *b);

void HeapAdjust(CharCount *arr, int start, int end, char* order);

void HeapSort(CharCount arr[], int len, char* order);

#endif //HEAPSORT_H
