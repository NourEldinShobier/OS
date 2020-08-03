#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include<string.h>

#define MAX 2000

int FRAMES_NUM = 0;
int FAULTS_NUM = 0;

//region FIFO Variables

int queue[MAX] = {-1};

unsigned int Qsize = 0;
unsigned int Qrear = 0;
unsigned int Qfront = 0;

//endregion
//region FIFO Fns

int isQFull() {
    return (Qsize == FRAMES_NUM);
}

int isQEmpty() {
    return (Qsize == 0);
}

int enqueue(int data) {
    if (isQFull()) return 0;

    Qrear = (Qrear + 1) % FRAMES_NUM;
    Qsize++;
    queue[Qrear] = data;
    return 1;
}

int dequeue() {
    if (isQEmpty()) return INT_MIN;

    int data = queue[Qfront];
    Qfront = (Qfront + 1) % FRAMES_NUM;
    Qsize--;

    return data;
}

bool qContains(int value) {
    for (int i = 0; i < FRAMES_NUM; ++i) {
        if (queue[i] == -1) break;
        if (queue[i] == value) return true;
    }

    return false;
}

void fifoPrintSnapshot(int page, bool isFault) {
    printf("%02d", page);

    printf(" ");

    if (isFault) printf("F");
    else printf(" ");

    printf("   ");

    for (int i = 0; i < Qsize; ++i) {
        if (queue[i] == -1) break;
        else printf("%02d ", queue[i]);
    }
    printf("\n");
}

//endregion

//region LRU Variables

typedef struct node {
    int data;
    struct node *next;
} node;

node *head = NULL;

//endregion
//region LRU Fns

node *lruNewNode(int data, node *next) {
    node *newNode = (node *) malloc(sizeof(node));

    newNode->data = data;
    newNode->next = next;

    return newNode;
}

node *lruAddHead(int data) {
    node *newNode = lruNewNode(data, head);
    head = newNode;

    return head;
}

node *lruRemoveHead() {
    if (head == NULL) return NULL;

    node *front = head;
    head = head->next;
    front->next = NULL;

    if (front == head) head = NULL;

    free(front);
    return head;
}

node *lruRemoveTail() {
    if (head == NULL) return NULL;

    node *currentNode = head;
    node *prevNode = NULL;

    while (currentNode->next != NULL) {
        prevNode = currentNode;
        currentNode = currentNode->next;
    }

    if (prevNode != NULL) prevNode->next = NULL;
    if (currentNode == head) head = NULL;

    free(currentNode);

    return head;
}

node *lruDeleteNode(node *targetNode) {
    if (targetNode == NULL) return NULL;

    if (targetNode == head) return lruRemoveHead();
    if (targetNode->next == NULL) return lruRemoveTail();

    node *pointer = head;

    while (pointer != NULL) {
        if (pointer->next == targetNode) break;
        pointer = pointer->next;
    }

    if (pointer != NULL) {
        node *tmp = pointer->next;
        pointer->next = tmp->next;
        tmp->next = NULL;
        free(tmp);
    }

    return head;
}

node *lruFind(int data) {
    node *pointer = head;

    while (pointer != NULL) {
        if (pointer->data == data) return pointer;
        pointer = pointer->next;
    }

    return NULL;
}

int lruListSize() {
    node *pointer = head;
    int listSize = 0;

    while (pointer != NULL) {

        pointer = pointer->next;
        listSize++;
    }

    return listSize;
}

bool lruIsListFull() {
    return lruListSize() == FRAMES_NUM;
}

void lruPrintSnapshot(int page, bool isFault) {
    printf("%02d", page);

    printf(" ");

    if (isFault) printf("F");
    else printf(" ");

    printf("   ");

    // print list values
    node *node = head;

    for (int i = 0; i < lruListSize(); ++i) {
        if (node == NULL) break;

        printf("%02d ", node->data);
        node = node->next;
    }

    printf("\n");
}

//endregion

//region Clock Variables

int clockFrames[MAX] = {-1};
int flags[MAX] = {0};

int clockIndex = 0;

//endregion
//region Clock Fns

bool clockArrContains(int value) {
    for (int i = 0; i < FRAMES_NUM; ++i) {
        if (clockFrames[i] == -1) return false;
        if (clockFrames[i] == value) return true;
    }

    return false;
}

bool clockIsArrFull() {
    for (int i = 0; i < FRAMES_NUM; ++i) {
        if (clockFrames[i] == -1) return false;
    }

    return true;
}

int clockArrFind(int value) {
    for (int i = 0; i < FRAMES_NUM; ++i) {
        if (clockFrames[i] == value) return i;
    }

    return -1;
}

void clockInit() {
    for (int i = 0; i < MAX; ++i) clockFrames[i] = -1;
    for (int j = 0; j < MAX; ++j) flags[j] = 0;
}

void clockPrintSnapshot(int page, bool isFault) {
    printf("%02d", page);

    printf(" ");

    if (isFault) printf("F");
    else printf(" ");

    printf("   ");

    for (int i = 0; i < FRAMES_NUM; ++i) {
        if (clockFrames[i] == -1) break;
        printf("%02d ", clockFrames[i]);
    }

    printf("\n");
}

//endregion

//region Optimal Variables

int optimalArr[MAX] = {-1};

//endregion
//region Optimal Fns

void optimalInit() {
    for (int i = 0; i < MAX; ++i) optimalArr[i] = -1;
}

bool optimalArrContains(int value) {
    for (int i = 0; i < FRAMES_NUM; ++i) {
        if (optimalArr[i] == -1) return false;
        if (optimalArr[i] == value) return true;
    }

    return false;
}

bool optimalIsArrFull() {
    for (int i = 0; i < FRAMES_NUM; ++i) {
        if (optimalArr[i] == -1) return false;
    }

    return true;
}

void optimalPrintSnapshot(int page, bool isFault) {
    printf("%02d", page);

    printf(" ");

    if (isFault) printf("F");
    else printf(" ");

    printf("   ");

    for (int i = 0; i < FRAMES_NUM; ++i) {
        if (optimalArr[i] == -1) break;
        printf("%02d ", optimalArr[i]);
    }

    printf("\n");
}

//endregion
//region Main Fns

void printTableHeader(char *policy);

void printTableFooter();

void fifoPolicy(int *requests, int n);

void lruPolicy(int *requests, int n);

void clockPolicy(int *requests, int n);

void optimalPolicy(int *requests, int n);

int findFrameThatNeverOccurs(int *requests, int index, int length);

int findOptimalReplacement(int *requests, int index, int length);
//endregion

int main() {
    char policyName[10];
    int refrences[MAX];
    int length = 0;

    scanf("%d", &FRAMES_NUM);
    scanf("%s", policyName);

    int num = 0;
    while (num != -1) {
        scanf("%d", &num);
        if (num == -1) break;
        refrences[length] = num;
        length++;
    }

    printTableHeader(policyName);
    if (strcmp("FIFO", policyName) == 0) fifoPolicy(refrences, length);
    else if (strcmp("LRU", policyName) == 0) lruPolicy(refrences, length);
    else if (strcmp("OPTIMAL", policyName) == 0) optimalPolicy(refrences, length);
    else if (strcmp("CLOCK", policyName) == 0) clockPolicy(refrences, length);
    printTableFooter();

    return 0;
}

void printTableHeader(char *policy) {
    printf("\nReplacement Policy = %s\n", policy);
    printf("-------------------------------------\n");
    printf("Page   Content of Frames\n");
    printf("----   -----------------\n");
}

void printTableFooter() {
    printf("-------------------------------------\n");
    printf("Number of page faults = %d\n", FAULTS_NUM);
}

void fifoPolicy(int *requests, int n) {
    // init Q
    Qsize = 0;
    Qrear = FRAMES_NUM - 1;
    Qfront = 0;
    //------------------//
    int requests_num = n;

    for (int i = 0; i < requests_num; ++i) {
        if (!isQFull()) {
            if (!qContains(requests[i])) {
                enqueue(requests[i]);
                fifoPrintSnapshot(requests[i], false);
            } else {
                fifoPrintSnapshot(requests[i], false);
            }
        } else {
            if (!qContains(requests[i])) {
                dequeue();
                enqueue(requests[i]);
                fifoPrintSnapshot(requests[i], true);
                FAULTS_NUM++;
            } else {
                fifoPrintSnapshot(requests[i], false);
            }
        }
    }
}

void lruPolicy(int *requests, int n) {
    int requests_num = n;

    for (int i = 0; i < requests_num; ++i) {
        if (!lruIsListFull()) {
            if (!lruFind(requests[i])) {
                lruAddHead(requests[i]);
                lruPrintSnapshot(requests[i], false);
            } else {
                lruDeleteNode(lruFind(requests[i]));
                lruAddHead(requests[i]);
                lruPrintSnapshot(requests[i], false);
            }
        } else {
            if (!lruFind(requests[i])) {
                lruRemoveTail();
                lruAddHead(requests[i]);
                lruPrintSnapshot(requests[i], true);
                FAULTS_NUM++;
            } else {
                lruDeleteNode(lruFind(requests[i]));
                lruAddHead(requests[i]);
                lruPrintSnapshot(requests[i], false);
            }
        }
    }
}

void clockPolicy(int *requests, int n) {
    clockInit();

    int requests_num = n;

    for (int i = 0; i < requests_num; ++i) {
        if (!clockIsArrFull()) {
            if (clockArrContains(requests[i])) {
                int correspondingIndex = clockArrFind(requests[i]);
                flags[correspondingIndex] = 1;
                clockPrintSnapshot(requests[i], false);
            } else {
                for (int j = 0; j < FRAMES_NUM; ++j) {
                    if (clockFrames[j] == -1) {
                        clockFrames[j] = requests[i];
                        break;
                    }
                }

                clockPrintSnapshot(requests[i], false);
            }
        } else {
            if (clockArrContains(requests[i])) {
                int correspondingIndex = clockArrFind(requests[i]);
                flags[correspondingIndex] = 1;
                clockPrintSnapshot(requests[i], false);
            } else {
                while (1) {
                    if (flags[clockIndex] == 0) {
                        clockFrames[clockIndex] = requests[i];
                        clockIndex = (clockIndex + 1) % FRAMES_NUM;
                        clockPrintSnapshot(requests[i], true);
                        FAULTS_NUM++;
                        break;
                    } else {
                        flags[clockIndex] = 0;
                        clockIndex = 0;
                    }
                }
            }
        }
    }
}

// OPTIMAL

void optimalPolicy(int *requests, int n) {
    optimalInit();

    int requests_num = n;

    for (int i = 0; i < requests_num; ++i) {
        if (!optimalIsArrFull()) {
            if (optimalArrContains(requests[i])) {
                optimalPrintSnapshot(requests[i], false);
            } else {
                for (int j = 0; j < FRAMES_NUM; ++j) {
                    if (optimalArr[j] == -1) {
                        optimalArr[j] = requests[i];
                        break;
                    }
                }

                optimalPrintSnapshot(requests[i], false);
            }
        } else {
            if (optimalArrContains(requests[i])) {
                optimalPrintSnapshot(requests[i], false);
            } else {
                int replacementIndex = findOptimalReplacement(requests, i + 1, requests_num);
                optimalArr[replacementIndex] = requests[i];
                optimalPrintSnapshot(requests[i], true);
                FAULTS_NUM++;
            }
        }
    }
}

int findFrameThatNeverOccurs(int *requests, int index, int length) {
    for (int i = 0; i < FRAMES_NUM; ++i) {
        int frame = optimalArr[i];
        int count = 0;

        for (int j = index; j < length; ++j) {
            if (requests[j] == frame) count++;
        }

        if (count == 0) return i;
    }

    return -1;
}

int findOptimalReplacement(int *requests, int index, int length) {
    int frameNeverOccurs = findFrameThatNeverOccurs(requests, index, length);
    if (frameNeverOccurs != -1) return frameNeverOccurs;

    int frameToPeplace = 0;
    int maxTimeTillUse = 0;

    for (int i = 0; i < FRAMES_NUM; ++i) {
        int frame = optimalArr[i];
        int timeTillUse = 0;

        for (int j = index; j < length; ++j) {
            if (frame == requests[j]) {
                timeTillUse = j - index;
            }
        }

        if (timeTillUse > maxTimeTillUse) {
            maxTimeTillUse = timeTillUse;
            frameToPeplace = i;
        }
    }

    return frameToPeplace;
}