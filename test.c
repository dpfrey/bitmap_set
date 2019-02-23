#include "bitmap_set.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

int main(int argc)
{
    bmset_Set_t *set = bmset_create(INT16_MIN, INT16_MAX);
    assert(set);

    bool was_element;
    assert(bmset_add(set, 10, &was_element) == BMSET_RES_SUCCESS);
    assert(!was_element);
    assert(bmset_add(set, -9, &was_element) == BMSET_RES_SUCCESS);
    assert(!was_element);
    assert(bmset_add(set, INT16_MAX + 1, &was_element) == BMSET_RES_ERROR_VALUE_RANGE);
    assert(bmset_add(set, 10, &was_element) == BMSET_RES_SUCCESS);
    assert(was_element);
    assert(bmset_remove(set, 10, &was_element) == BMSET_RES_SUCCESS);
    assert(was_element);
    assert(bmset_remove(set, 11, &was_element) == BMSET_RES_SUCCESS);
    assert(!was_element);
    bmset_destroy(set);

    return 0;
}
