/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Krzysztof Blazewicz
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "py/obj.h"

typedef struct _mp_obj_star_t {
    mp_obj_base_t base;
    mp_obj_t arg;
} mp_obj_star_t;

mp_obj_t mp_obj_star_get(mp_obj_t *self_in) {
    mp_obj_star_t *self = (mp_obj_star_t*)self_in;
    return self->arg;
}

const mp_obj_type_t mp_type_star = {
    { &mp_type_type },
};

mp_obj_t mp_obj_new_star(mp_obj_t arg) {
    mp_obj_star_t *o = m_new_obj(mp_obj_star_t);
    o->base.type = &mp_type_star;
    o->arg = arg;
    return o;
}

#if 0
// TODO: there must already be an equivalent of this
typedef _mp_arglist_t {
    mp_obj_t *args;
    size_t len;
    size_t alloc;
} mp_arglist_t;


STATIC void prepare_arglist(mp_arglist_t *arglist, size_t threshold_sz, size_t new_sz) {
    if (arglist->alloc < threshold_sz) {
        arglist->args = m_renew(mp_obj_t, arglist->alloc, new_sz);
        arglist->alloc = new_sz;
    }
}


STATIC void unpack_single_star(mp_obj_star_t *self, mp_arglist_t *arglist) {

    if (MP_OBJ_IS_TYPE(self->arg, &mp_type_tuple) || MP_OBJ_IS_TYPE(self->arg, &mp_type_list) {
        // optimize for tuple and list

        size_t len;
        mp_obj_t *items;
        mp_obj_get_array(self->arg, &len, &items);

        prepare_arglist(arglist, arglist->len + len, arglist->len + len);
        m_seq_copy(arglist->args + arglist->len, items, len, mp_obj_t);
        arglist->len += len;
    } else {
        // generic iterator

        mp_obj_iter_buf_t iter_buf;
        // mp_getiter will raise TypeError it wrapped object is not an iterable
        mp_obj_t iterable = mp_getiter(self->arg, &iter_buf);
        mp_obj_t item;
        while ((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
            // we don't know how many elements will the iterable yield
            // so lets double the array size on overflow to limit the number
            // of renewals
            prepare_arglist(arglist, arglist->len, arglist->alloc*2);
            arglist->args[arglist->len++] = item;
        }
    }
}

STATIC void unpack_double_star(mp_obj_star_t *self, mp_arglist_t *arglist) {
    if (MP_OBJ_IS_TYPE(self->arg, &mp_type_dict)) {
        // dictionary

        mp_map_t *map = mp_obj_dict_get_map(self->arg);
        prepare_arglist(arglist, arglist->len + 2*map->used, arglist->len + 2*map->used);
        for (size_t i = 0; i < map->alloc; i++) {
            if (MP_MAP_SLOT_IS_FILLED(map, i)) {
                mp_obj_t key = map->table[i].key;
                if (!MP_OBJ_IS_TYPE(key, &mp_type_str)) {
                    mp_raise_TypError("");
                }
                key = mp_obj_str_intern(key);
                arglist->args[arglist->len++] = key;
                arglist->args[arglist->len++] = map->table[i].value;
            }
        }
    } else {
        // generic mapping
        // - call keys() to get an iterable of all keys in the mapping
       // - call __getitem__ for each key to get the corresponding value

       // get the keys iterable
       mp_obj_t dest[3];
       mp_load_method(self->arg, MP_QSTR_keys, dest);
       // TODO: raise TypeError if doesn't have keys() method
       mp_obj_t iterable = mp_getiter(mp_call_method_n_kw(0, 0, dest), NULL);

       mp_obj_t key;
       while ((key = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
           // expand size of args array if needed
           prepare_arglist(arglist, arglist->len + 1, arglist->alloc*2);

           // the key must be a qstr, so intern it if it's a string
           if (!MP_OBJ_IS_TYPE(key, &mp_type_str)) {
               mp_raise_TypError("");
           }
           key = mp_obj_str_intern(key);

           // get the value corresponding to the key
           mp_load_method(self->arg, MP_QSTR___getitem__, dest);
           dest[2] = key;
           mp_obj_t value = mp_call_method_n_kw(1, 0, dest);

           // store the key/value pair in the argument array
           arglist->args[arglist->len++] = key;
           arglist->args[arglist->len++] = value;
       }
    }
}

void mp_obj_star_unpack(mp_arglist_t *arglist, bool is_double) {
    // star expression should be the last element on the arglist
    mp_obj_t *top = arglist->args[arglist->len--];
    assert(MP_OBJ_IS_TYPE(top, &mp_type_star));
    mp_obj_star_t *self = (mp_obj_star_t*)top;

    if (is_double) {
        unpack_double_star(self, arglist);
    } else {
        unpack_single_star(self, arglist);
    }
}
#endif