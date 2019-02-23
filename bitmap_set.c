#include "bitmap_set.h"
#include <pthread.h>
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
    bool use_lock;
    pthread_mutex_t lock;
    ssize_t min_val;
    ssize_t max_val;
    unsigned int data[];
};

static bool value_to_index(
    bmset_Set_t *set, ssize_t value, size_t *unsigned_index, uint8_t *bit_index)
{
    if (value > set->max_val || value < set->min_val)
        return false;

    size_t total_bit_index = value - set->min_val;
    *unsigned_index = total_bit_index / BITS_PER_UNSIGNED;
    *bit_index = total_bit_index % BITS_PER_UNSIGNED;
    return true;
}

bmset_Set_t *bmset_create(ssize_t min_val, ssize_t max_val, bool thread_safe)
{
    const ssize_t bits_required = 1 + (max_val - min_val);
    if (bits_required < 1)
        return NULL;
    const size_t unsigned_required =
        (bits_required / BITS_PER_UNSIGNED) + ((bits_required % BITS_PER_UNSIGNED) ? 1 : 0);
    bmset_Set_t *s = calloc(sizeof(bmset_Set_t) + unsigned_required, 1);
    if (!s)
        goto fail;

    s->use_lock = thread_safe;

    if (s->use_lock && pthread_mutex_init(&s->lock, NULL))
        goto fail;

    s->min_val = min_val;
    s->max_val = max_val;

    return s;

fail:
    if (s)
        free(s);
    return NULL;
}

void bmset_destroy(bmset_Set_t *set)
{
    if (set->use_lock)
        pthread_mutex_destroy(&set->lock);
    free(set);
}

static bmset_Result_t bmset_generic_op(
    bmset_Set_t *set, ssize_t value, enum operation op, bool *preexists)
{
    size_t unsigned_index;
    uint8_t bit_index;
    bmset_Result_t res = BMSET_RES_SUCCESS;

    if (!value_to_index(set, value, &unsigned_index, &bit_index)) {
        res = BMSET_RES_ERROR_VALUE_RANGE;
        goto done;
    }

    if (set->use_lock) {
        int pthread_res = pthread_mutex_lock(&set->lock);
        if (pthread_res) {
            res = BMSET_RES_ERROR_THREADING;
            goto done;
        }
    }

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
        res = BMSET_RES_ERROR_INTERNAL;
        break;
    }

    if (set->use_lock) {
        int pthread_res = pthread_mutex_unlock(&set->lock);
        if (pthread_res) {
            res = BMSET_RES_ERROR_THREADING;
        }
    }

done:
    return res;
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
