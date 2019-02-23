#include "bitmap_set.h"

#include <stdint.h>

#define BITS_PER_UNSIGNED (sizeof(unsigned int) * 8)

enum operation
{
    OP_NONE,
    OP_ADD,
    OP_REMOVE,
};

struct bmset_Set
{
    ssize_t min_val;
    ssize_t max_val;
    unsigned int data[];
};

static bool value_to_index(bmset_Set_t *set, ssize_t value, size_t *unsigned_index, uint8_t *bit_index)
{
    if (value > set->max_val || value < set->min_val)
        return false;

    size_t total_bit_index = value - set->min_val;
    *unsigned_index = total_bit_index / BITS_PER_UNSIGNED;
    *bit_index = total_bit_index % BITS_PER_UNSIGNED;
    return true;
}

bmset_Set_t *bmset_create(ssize_t min_val, ssize_t max_val)
{
    const ssize_t bits_required = 1 + (max_val - min_val);
    if (bits_required < 1)
        return NULL;
    const size_t unsigned_required =
        (bits_required / BITS_PER_UNSIGNED) + ((bits_required % BITS_PER_UNSIGNED) ? 1 : 0);
    bmset_Set_t *s = calloc(sizeof(bmset_Set_t) + unsigned_required, 1);
    if (!s)
        return NULL;

    s->min_val = min_val;
    s->max_val = max_val;

    return s;
}

void bmset_destroy(bmset_Set_t *set)
{
    free(set);
}

static bmset_Result_t bmset_generic_op(bmset_Set_t *set, ssize_t value, enum operation op, bool *preexists)
{
    size_t unsigned_index;
    uint8_t bit_index;

    if (!value_to_index(set, value, &unsigned_index, &bit_index))
        return BMSET_RES_ERROR_VALUE_RANGE;

    *preexists = (set->data[unsigned_index] & (1 << bit_index)) != 0;

    switch (op)
    {
    case OP_NONE:
        break;

    case OP_ADD:
        if (!*preexists)
            set->data[unsigned_index] |= (1 << bit_index);
        break;

    case OP_REMOVE:
        if (*preexists)
            set->data[unsigned_index] &= (~(1 << bit_index));
        break;

    default:
        return BMSET_RES_ERROR_INTERNAL;
    }

    return BMSET_RES_SUCCESS;
}

bmset_Result_t bmset_is_element_of(bmset_Set_t *set, ssize_t value, bool *is_element)
{
    return bmset_generic_op(set, value, OP_NONE, is_element);
}

bmset_Result_t bmset_add(bmset_Set_t *set, ssize_t value, bool *was_element)
{
    return bmset_generic_op(set, value, OP_ADD, was_element);
}

bmset_Result_t bmset_remove(bmset_Set_t *set, ssize_t value, bool *was_element)
{
    return bmset_generic_op(set, value, OP_REMOVE, was_element);
}
