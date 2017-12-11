#include "buffer.h"

void init(cbuffer_t *buffer, int size) {
    buffer->size = size;
    buffer->start = 0;
    buffer->astart=0;
    buffer->estart=0;
    buffer->count = 0;
    buffer->element = (void**) malloc(sizeof(buffer->element)*size);
    buffer->tag = (void **) malloc(sizeof(buffer->tag) * size);
    /* allocated array of void pointers. Same as below */
    //buffer->element = malloc(sizeof(void *) * size);
     
}
 
int full(cbuffer_t *buffer) {
    int count =0;
    if(buffer->astart>buffer->estart){
         count = buffer->astart - buffer->estart;
    }else if (buffer->astart < buffer->estart){
         count = (buffer->size - buffer->estart) + buffer->astart;
    }
    if (count == buffer->size) { 
        return 1;
    } else {
        return 0;
    }
}
 
int empty(cbuffer_t *buffer) {
    if (buffer->estart == buffer->astart) {
        return 1;
    } else {
        return 0;
    }
}
     
void push(cbuffer_t *buffer, void *data, void * tag) {
    int index;
    if (full(buffer)) {
        //printf("Buffer overflow\n");
        return;
    } else {
        index = buffer->astart++;
        if (index >= buffer->size) {
            index = 0;
            buffer->astart = 0;
        }
        buffer->element[index] = data;
	buffer->tag[index] = tag;
        //char * temp = (char *) buffer->tag[index];
        //printf("%x \n", temp[0]);
    }
}
 
 
void * popqueue(cbuffer_t *buffer, char ** tag) {
    void * element;
    if (empty(buffer)) {
        //printf("Buffer underflow\n");
        return NULL;
    } else {
       /* FIFO implementation */
       element = buffer->element[buffer->estart];
       *tag = (char *) buffer->tag[buffer->estart];
       //printf("%p \n", *tag);
       buffer->estart++;
       if (buffer->estart == buffer->size) {
           buffer->estart = 0;
       }
        
       return element;
    }
}
 
void * popstack(cbuffer_t *buffer) {
    int index;
    if (empty(buffer)) {
        //printf("Buffer underflow\n");
        return NULL;
    } else {
        /* LIFO implementation */
        index = buffer->start + buffer->count - 1;
        if (index >= buffer->size) {
           index = buffer->count - buffer->size - 1;
        }      
        buffer->count--;
        return buffer->element[index];
    }
}
