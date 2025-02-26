//
// Created by Ginkgo on 2025/2/19.
//
#include "HeapSort.h"

void swap (CharCount *a, CharCount *b) {
    CharCount temp = *b;
    *b = *a;
    *a = temp;
}

void HeapAdjust (CharCount *arr, int start, int end, char* order) {
    int parent = start;
    int child = parent * 2 + 1;

    if (order[0] == 'd') {
        while (child <= end) {
            // 找出左右子节点中较大的那个
            if (child + 1 <= end) {
                if (arr[child].data < arr[child + 1].data) {
                    child++;
                }
            }
            // 如果父节点小于等于子节点，无需调整
            if (arr[parent].data > arr[child].data) {
                break;
            }

            // 交换父节点和子节点
            swap(&arr[parent], &arr[child]);
            // 继续向下调整
            parent = child;
            child = parent * 2 + 1;
        }
    }
    else {
        while (child <= end) {
            // 找出左右子节点中较大的那个，一样频度就选数据大的
            if (child + 1 <= end) {
                if (arr[child].count < arr[child + 1].count || arr[child].count == arr[child + 1].count && arr[child].data < arr[child + 1].data) {
                    child++;
                }
            }
            // 如果父节点大于等于子节点，无需调整
            if (arr[parent].count > arr[child].count || (arr[parent].count == arr[child].count && arr[parent].data > arr[child].data)) {
                break;
            }

            // 交换父节点和子节点
            swap(&arr[parent], &arr[child]);
            // 继续向下调整
            parent = child;
            child = parent * 2 + 1;
        }
    }
}

void HeapSort (CharCount arr[], int len, char* order) {
    int i;
    for (i = len / 2 - 1; i >= 0; i--)
        HeapAdjust(arr, i, len - 1, order);
    for (i = len - 1; i > 0; i--) {
        swap(&arr[0], &arr[i]);
        HeapAdjust(arr, 0, i - 1, order);
    }
}