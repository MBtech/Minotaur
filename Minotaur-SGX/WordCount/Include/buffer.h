#include <stdio.h>
#include <stdlib.h>
 
struct cbuffer {
    int size;
    int start;
    int estart;
    int astart;
    //int end;  // position of last element
    /* Tracking start and end of buffer would waste
     * one position. A full buffer would always have
     * to leave last position empty or otherwise
     * it would look empty. Instead this buffer uses
     * count to track if buffer is empty or full
     */
    int count; // number of elements in buffer
    /* Two ways to make buffer element type opaque
     * First is by using typedef for the element
     * pointer. Second is by using void pointer.
     */
    /* different types of buffer: 
    int *element;   // array of integers
    char *element;  // array of characters 
    void *element;  // array of void type (could cast to int, char, etc)
    char **element; //array of char pointers (array of strings)
    void **element; // array of void pointers
    Choosing array of void pointers since it's the most flexible */
    void **element;
    void ** tag;
};
 
typedef struct cbuffer cbuffer_t;

void init(struct cbuffer *buffer, int size);

int full(cbuffer_t *buffer);

int empty(cbuffer_t *buffer);

void push(cbuffer_t *buffer, void *data, void * tag);

void * popqueue(cbuffer_t *buffer, char **tag);

void * popstack(cbuffer_t *buffer); 
