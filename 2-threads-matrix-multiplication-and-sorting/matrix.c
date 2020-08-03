
#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
#include<stdlib.h>
#include <stdbool.h>

#define MAX 1000

struct Location {
    int i;
    int j;
};

int aMat[MAX][MAX];
int bMat[MAX][MAX];

int resultMat[MAX][MAX];

int aRows, aColumns, bRows, bColumns;

void loadMatrices() {
    FILE *file = fopen("input.txt", "r");

    // reading first matrix
    fscanf(file, "%d %d", &aRows, &aColumns);

    for (int  i = 0; i < aRows; ++i) {
        for (int j = 0; j < aColumns; ++j) {
            fscanf(file, "%d", &aMat[i][j]);
        }
    }

    // reading second matrix
    fscanf(file, "%d %d", &bRows, &bColumns);

    for (int  i = 0; i < bRows; ++i) {
        for (int j = 0; j < bColumns; ++j) {
            fscanf(file, "%d", &bMat[i][j]);
        }
    }

    fclose(file);
}

void writeResult(){
    FILE *file = fopen("output.txt", "w");

    fprintf(file, "%d %d\n", aRows, bColumns);


    //print results
    for (int i = 0; i < aRows; i++) {
        for (int j = 0; j < bColumns; j++) {
            fprintf(file,"%d ", resultMat[i][j]);
        }
        fprintf(file,"\n");
    }

    fclose(file);
}

/*// rows
void *multiply(void *arg) {
    struct Location location = *(struct Location *) (arg);;

    int sum = 0;
    for (int j = 0; j < bColumns; ++j) {
        for (int k = 0; k < aColumns; ++k) {
            sum += aMat[location.i][k] * bMat[k][j];
        }

        resultMat[location.i][j] = sum;
        sum = 0;
    }

    return NULL;
}

// rows
int main() {
    loadMatrices();
    pthread_t threads[aRows];

    clock_t startTime = clock();

    for (int i = 0; i < aRows; ++i) {

        struct Location location = {i};
        struct Location *currentLocation;

        currentLocation = (struct Location *) malloc(sizeof(struct Location));
        *currentLocation = location;

        pthread_create(&threads[i], NULL, multiply, (void *) (currentLocation));
    }

    //joining all threads
    for (int i = 0; i < aRows; ++i) {
        pthread_join(threads[i], NULL);
    }

    clock_t endTime = clock();

    printf("\n\nelapse time: %f", ((double)(endTime - startTime)));
    writeResult();

    return 0;
}*/


// cells
void *multiply(void *arg) {

    struct Location location = *(struct Location *) (arg);;

    int sum = 0;

    for (int k = 0; k < aColumns; ++k) {
        sum += aMat[location.i][k] * bMat[k][location.j];
    }

    resultMat[location.i][location.j] = sum;

    return NULL;
}

// cells
int main() {
    loadMatrices();
    pthread_t threads[aRows][bColumns];

    clock_t startTime = clock();

    for (int i = 0; i < aRows; ++i) {
        for (int j = 0; j < bColumns; ++j) {
            struct Location location = {i, j};
            struct Location *currentLocation;

            currentLocation = (struct Location *) malloc(sizeof(struct Location));
            *currentLocation = location;

            pthread_create(&threads[i][j], NULL, multiply, (void *) (currentLocation));
        }
    }

    //joining all threads
    for (int i = 0; i < aRows; ++i) {
        for (int j = 0; j < bColumns; ++j) {
            pthread_join(threads[i][j], NULL);
        }
    }

    clock_t endTime = clock();

    printf("\n\nelapsed time: %f", ((double)(endTime - startTime)));
    writeResult();

    return 0;
}
