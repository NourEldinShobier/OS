
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define MAX 1000

struct SCOPE {
    int startPoint;
    int endPoint;
};

int unsortedArray[MAX] = {0};
int unsortedArrayLength = 0;

void loadArray() {
    FILE *file = fopen("input.txt", "r");
    fscanf(file, "%d", &unsortedArrayLength);

    for (int i = 0; i < unsortedArrayLength; ++i) {
        fscanf(file, "%d", &unsortedArray[i]);
    }

    fclose(file);
}

void merge(struct SCOPE *scope) {
    int startPoint = scope->startPoint;
    int endPoint = scope->endPoint;
    int middlePoint = (startPoint + endPoint) / 2;

    int leftSize = middlePoint - startPoint + 1;
    int rightSize = endPoint - middlePoint;

    /*-------------------------------------------*/

    int *leftAr = (int *) malloc(sizeof(int) * leftSize);
    int *rightAr = (int *) malloc(sizeof(int) * rightSize);

    // load values in left & right parts
    for (int i = 0; i < leftSize; i++) leftAr[i] = unsortedArray[i + startPoint];
    for (int i = 0; i < rightSize; i++) rightAr[i] = unsortedArray[i + middlePoint + 1];

    int k = startPoint, i = 0, j = 0;

    // merge left & right in ascending order
    while (i < leftSize && j < rightSize) {
        if (leftAr[i] <= rightAr[j])
            unsortedArray[k++] = leftAr[i++];
        else
            unsortedArray[k++] = rightAr[j++];
    }

    // insert left value in the array
    while (i < leftSize) unsortedArray[k++] = leftAr[i++];

    // insert right value in the array
    while (j < rightSize) unsortedArray[k++] = rightAr[j++];
}

void *mergeSort(void *args) {
    struct SCOPE *scope = (struct SCOPE *) args;

    if (scope->startPoint == scope->endPoint) return NULL;

    int middlePoint = (scope->startPoint + scope->endPoint) / 2;

    struct SCOPE leftScope = {scope->startPoint, middlePoint};
    struct SCOPE rightScope = {middlePoint + 1, scope->endPoint};

    pthread_t leftThread, rightThread;

    pthread_create(&leftThread, NULL, mergeSort, &leftScope);
    pthread_create(&rightThread, NULL, mergeSort, &rightScope);

    pthread_join(leftThread, NULL);
    pthread_join(rightThread, NULL);

    merge(scope);

    return NULL;
}

int main() {
    loadArray();

    struct SCOPE scope = {0, unsortedArrayLength - 1};
    pthread_t initThread;

    clock_t startTime = clock();

    pthread_create(&initThread, NULL, mergeSort, &scope);
    pthread_join(initThread, NULL);

    clock_t endTime = clock();

    for (int i = 0; i < 10; i++) printf("%d ", unsortedArray[i]);

    printf("\n\nelapsed time: %f", ((double)(endTime - startTime)));

    printf("\n");

    return 0;
}