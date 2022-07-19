#ifndef ARRAYLIST_H
#define ARRAYLIST_H

#include <stdint.h>


typedef struct ArrayList {
    int count;
    size_t size;
    size_t item_size;
    void *buf;
} ArrayList;

ArrayList *ArrayList_Init(size_t item_size, size_t size);
int ArrayList_Add(ArrayList *self, void *item);
void ArrayList_Get(ArrayList *self, int i, void *out_item);
void ArrayList_GetRef(ArrayList *self, int i, void **out_item_ref);
void ArrayList_Del(ArrayList *self, int i);
int ArrayList_FindIndex(ArrayList *self,
                             int (*predicate)(void *p, void *extra_param),
                             void *extra_param);
void ArrayList_Destroy(ArrayList *self);

#endif