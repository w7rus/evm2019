#ifndef HELPERFUNCS_H
#define HELPERFUNCS_H

#include <cstdlib>
#include <cstdio>
#include <new>
#include <typeinfo>

namespace RegistryStatusEnumFlags {
    enum enumFlag {
        Alpha   = 1,
        Bravo   = 2,
        Charlie = 3,
        Delta   = 4,
        Echo    = 5,
        Foxtrot = 6,
        Golf    = 7,
        Hotel   = 8,
        India   = 9,
        Juliett = 10,
        Kilo    = 11,
        Lima    = 12,
        Mike    = 13,
        November= 14,
        Oscar   = 15,
        Papa    = 16
    };
}

using namespace RegistryStatusEnumFlags;

/*
*
*   Memory Functions
*
*/

int* sc_memoryInit() {
    int* ptr_arr_i_array01 = new (std::nothrow) int[100];
    for (int _counter = 0; _counter < 100; _counter++)
        ptr_arr_i_array01[_counter] = 0;
    return ptr_arr_i_array01;
}

int sc_memorySet(int* ptr_arr_i_array01, int i_address, int i_value) {
    if (i_address <= 0 && i_address > 100)
        return -1;
    ptr_arr_i_array01[i_address] = i_value;
    return 0;
}

int sc_memoryGet(int* arr_i_array01, int i_address, int* i_value) {
    if (i_address <= 0 && i_address > 100)
        return -1;
    *i_value = arr_i_array01[i_address];
    return 0;
}
int sc_memoryGet(int* arr_i_array01, int i_address) {
    if (i_address <= 0 && i_address > 100)
        return -1;
    return arr_i_array01[i_address];
}

int sc_memorySave(char* arr_c_path, int* arr_i_array01) {
    FILE* ptr_file_data01 = fopen(arr_c_path, "w+b");
    if (ptr_file_data01 == NULL)
        return -1;
    fwrite(arr_i_array01, sizeof(int), 100, ptr_file_data01);
    fclose(ptr_file_data01);
    return 0;
}

int sc_memoryLoad(char* arr_c_path, int* ptr_arr_i_array01) {
    FILE* ptr_file_data01 = fopen(arr_c_path, "rb");
    if (ptr_file_data01 == NULL)
        return -1;
    fread(ptr_arr_i_array01, sizeof(int), 100, ptr_file_data01);
    fclose(ptr_file_data01);
    return 0;
}

/*
*
*   Flag Functions
*
*/

int* sc_flagInit() {
    int* ptr_i_flag = new (std::nothrow) int;
    *ptr_i_flag = 0;
    return ptr_i_flag;
}

int sc_flagSet(int* ptr_i_flag, enumFlag enumFlag_mask01, bool b_state) {
    if (typeid(enumFlag_mask01) != typeid(enumFlag))
        return -1;
    if (b_state) {
        *ptr_i_flag = *ptr_i_flag | (1 << (int(enumFlag_mask01) - 1));
    } else {
        *ptr_i_flag = *ptr_i_flag | (1 << (~(int(enumFlag_mask01) - 1)));
    }
    return 0;
}

int sc_flagGet(int* i_flag, enumFlag enumFlag_mask01, bool* ptr_b_state) {
    if (typeid(enumFlag_mask01) != typeid(enumFlag))
        return -1;
    *ptr_b_state = ((*i_flag) >> (int(enumFlag_mask01) - 1)) & 0x1;
    return 0;
}
int sc_flagGet(int* i_flag, enumFlag enumFlag_mask01) {
    if (typeid(enumFlag_mask01) != typeid(enumFlag))
        return -1;
    return ((*i_flag) >> (int(enumFlag_mask01) - 1)) & 0x1;
}

/*
*
*   Command System
*
*/

int sc_commandEncode(int i_command, int i_operand, int* ptr_i_value) {
    if ((i_command == 10 || i_command == 11 || i_command == 20 || i_command == 21 || (i_command > 29 && i_command < 34) || (i_command > 39 && i_command < 44) || (i_command > 50 && i_command < 77))) {
        *ptr_i_value = (i_command << 7) | i_operand;
        return 0;
    }
    return -1;
}

int sc_commandDecode(int i_value, int* ptr_i_command, int* ptr_i_operand) {
    if (!((i_value >> 14) & 0x1)) {
        *ptr_i_command = (i_value & 0x3F80) >> 7;
        *ptr_i_operand = i_value & 0x7F;
        return 0;
    } else {
        return -1;
    }
}

#endif