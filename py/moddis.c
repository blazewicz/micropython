/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Krzysztof Blazewicz
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

#include "py/bc.h"
#include "py/compile.h"
#include "py/obj.h"


#define printf(...) mp_printf(&mp_plat_print, __VA_ARGS__)


#if MICROPY_PY_DIS


// typedef struct _mp_raw_code_t {
//     mp_uint_t kind : 3; // of type mp_raw_code_kind_t
//     mp_uint_t scope_flags : 7;
//     mp_uint_t n_pos_args : 11;
//     const void *fun_data;
//     const mp_uint_t *const_table;
//     #if MICROPY_PERSISTENT_CODE_SAVE
//     size_t fun_data_len;
//     uint16_t n_obj;
//     uint16_t n_raw_code;
//     #if MICROPY_EMIT_NATIVE || MICROPY_EMIT_INLINE_ASM
//     uint16_t prelude_offset;
//     uint16_t n_qstr;
//     mp_qstr_link_entry_t *qstr_link;
//     #endif
//     #endif
//     #if MICROPY_EMIT_NATIVE || MICROPY_EMIT_INLINE_ASM
//     mp_uint_t type_sig; // for viper, compressed as 2-bit types; ret is MSB, then arg0, arg1, etc
//     #endif
// } mp_raw_code_t;


mp_raw_code_t *mp_compile_to_raw_code(mp_parse_tree_t *parse_tree, qstr source_file, uint emit_opt, bool is_repl);

STATIC mp_obj_t mp_dis_dis(mp_obj_t x_arg) {
    size_t str_len;
    const char *str = mp_obj_str_get_data(x_arg, &str_len);

    mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_string_gt_, str, str_len, 0);
    mp_parse_input_kind_t parse_input_kind = MP_PARSE_EVAL_INPUT;
    mp_parse_tree_t parse_tree = mp_parse(lex, parse_input_kind);
    mp_raw_code_t *rc = mp_compile_to_raw_code(&parse_tree, lex->source_name, MP_EMIT_OPT_NONE, false);
    // mp_bytecode_print(rc, rc->fun_data, 20 /* rc->fun_data_len */, rc->const_table);

    const byte *ip = rc->fun_data;
    const byte *mp_showbc_code_start = ip;

    // get bytecode parameters
    /*mp_uint_t n_state =*/ mp_decode_uint(&ip);
    /*mp_uint_t n_exc_stack =*/ mp_decode_uint(&ip);
    /*mp_uint_t scope_flags =*/ ip++;
    /*mp_uint_t n_pos_args =*/ ip++;
    /*mp_uint_t n_kwonly_args =*/ ip++;
    /*mp_uint_t n_def_pos_args =*/ ip++;

    const byte *code_info = ip;
    mp_uint_t code_info_size = mp_decode_uint(&code_info);
    ip += code_info_size;

    // size_t len = 30;
    size_t len = rc->fun_data_len;

    {
        uint local_num;
        while ((local_num = *ip++) != 255) {
            printf("(INIT_CELL %u)\n", local_num);
        }
        len -= ip - mp_showbc_code_start;
    }

    mp_bytecode_print2(ip, len, rc->const_table);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(mp_dis_dis_obj, mp_dis_dis);


STATIC const mp_rom_map_elem_t mp_module_dis_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_dis) },
    { MP_ROM_QSTR(MP_QSTR_dis), MP_ROM_PTR(&mp_dis_dis_obj) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_dis_globals, mp_module_dis_globals_table);

const mp_obj_module_t mp_module_dis = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_dis_globals,
};


#endif // MICROPY_PY_SYS
