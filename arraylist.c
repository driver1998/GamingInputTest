#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

#include "arraylist.h"

ArrayList *ArrayList_Init(size_t item_size, size_t size) {
    ArrayList *list = (ArrayList *)malloc(sizeof(ArrayList));
    assert(list != NULL);

    list->buf = malloc(item_size * size);
    assert(list->buf != NULL);

    list->count = 0;
    list->size = size;
    list->item_size = item_size;
    return list;
}

int ArrayList_Add(ArrayList *self, void *item) {
    if (self->count + 1 == self->size) {
        size_t new_size = 2 * self->size;
        void *new_buf = realloc(self->buf, new_size * self->item_size);
        assert(new_buf != NULL);
        self->buf = new_buf;
        self->size = new_size;
    }

    int i = self->count;
    memcpy((uint8_t*)self->buf + i * self->item_size, item, self->item_size);

    self->count++;
    return i;
}

void ArrayList_Get(ArrayList *self, int i, void *out_item) {
    memcpy(out_item, (uint8_t*)self->buf + i * self->item_size, self->item_size);
}

void ArrayList_GetRef(ArrayList *self, int i, void **out_item_ref) {
    *out_item_ref = (uint8_t*)self->buf + i * self->item_size;
}

void ArrayList_Del(ArrayList *self, int i) {
    int count_after_i = self->count - i - 1;
    void *dst = (uint8_t*)self->buf + i * self->item_size;
    void *src = (uint8_t*)self->buf + ((size_t)i + 1) * self->item_size;
    memmove(dst, src, count_after_i * self->item_size);
    self->count--;
}

int ArrayList_FindIndex(ArrayList *self,
                            int (*predicate)(void *p, void *extra_param),
                            void *extra_param) {
    int ret = -1;
    void *p;

    for (int i = 0; i < self->count; i++) {
        ArrayList_GetRef(self, i, &p);        
        if (predicate(p, extra_param)) {
            ret = i;
            break;
        }
    }
    return ret;
}

void ArrayList_Destroy(ArrayList *self) {
    free(self->buf);
    free(self);
}
