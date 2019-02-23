#ifndef _BITMAP_SET_H_
#define _BITMAP_SET_H_

#include <stdlib.h>
#include <stdbool.h>

typedef struct bmset_Set bmset_Set_t;

typedef enum
{
    BMSET_RES_SUCCESS,
    BMSET_RES_ERROR_VALUE_RANGE,
    BMSET_RES_ERROR_INTERNAL,
} bmset_Result_t;

bmset_Set_t * bmset_create(ssize_t min_val, ssize_t max_val);
void bmset_destroy(bmset_Set_t *set);
bmset_Result_t bmset_is_element_of(bmset_Set_t *set, ssize_t value, bool *is_element);
bmset_Result_t bmset_add(bmset_Set_t *set, ssize_t value, bool *was_element);
bmset_Result_t bmset_remove(bmset_Set_t *set, ssize_t value, bool *was_element);

#endif // _BITMAP_SET_H_
