#ifndef HELPERFUNCS_H
#define HELPERFUNCS_H

#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <new>
#include <typeinfo>
#include <iostream>
#include <string>
#include <sstream>

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

namespace TerminalColorsEnums {
    enum termClr {
        BLACK   = 30,
        RED     = 31,
        GREEN   = 32,
        YELLOW  = 33,
        BLUE    = 34,
        MAGENTA = 35,
        CYAN    = 36,
        WHITE   = 37
    };
}

using namespace RegistryStatusEnumFlags;
using namespace TerminalColorsEnums;

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
    if (b_state == true) {
        std::cout << "ISTRUE";
        *ptr_i_flag = *ptr_i_flag | (1 << (int(enumFlag_mask01) - 1));
    } else {
        std::cout << "ISFALSE";
        *ptr_i_flag = *ptr_i_flag & (~(1 << (int(enumFlag_mask01) - 1)));
    }
    return 0;
}

int sc_flagGet(int i_flag, enumFlag enumFlag_mask01, bool* ptr_b_state) {
    if (typeid(enumFlag_mask01) != typeid(enumFlag))
        return -1;
    *ptr_b_state = ((i_flag) >> (int(enumFlag_mask01) - 1)) & 0x1;
    return 0;
}
int sc_flagGet(int i_flag, enumFlag enumFlag_mask01) {
    if (typeid(enumFlag_mask01) != typeid(enumFlag))
        return -1;
    return ((i_flag) >> (int(enumFlag_mask01) - 1)) & 0x1;
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
        
        if ((*ptr_i_command == 10 || *ptr_i_command == 11 || *ptr_i_command == 20 || *ptr_i_command == 21 || (*ptr_i_command > 29 && *ptr_i_command < 34) || (*ptr_i_command > 39 && *ptr_i_command < 44) || (*ptr_i_command > 50 && *ptr_i_command < 77))) {
            return 0;
        }
        return -1;
    } else {
        return -1;
    }
}

/*
*
*   Visual System
*
*/

#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

// template<typename T>
// std::string toString(const T& value)
// {
//     std::ostringstream oss;
//     oss << value;
//     return oss.str();
// }

void mt_getScreenSize(int* width, int* height) {
    struct winsize size;
    ioctl(STDOUT_FILENO,TIOCGWINSZ,&size);
    *width = size.ws_col - 1;
    *height = size.ws_row - 1;
}

void mt_setCurPos(int x, int y) {
    std::string esc1 = "\033[";
    std::string esc2 = std::to_string(y);
    std::string esc3 = ";";
    std::string esc4 = std::to_string(x);
    std::string esc5 = "H";
    std::cout << esc1 + esc2 + esc3 + esc4 + esc5;
}

void mt_setCurFgColor(termClr color, bool bright) {
    std::string esc1 = "\033[";
    std::string esc2 = std::to_string(color);
    std::string esc4;
    if (bright) {
        esc4 = ";1";
    } else {
        esc4 = "";
    }
    std::string esc5 = "m";
    std::cout << esc1 + esc2 + esc4 + esc5;
}

void mt_setCurBgColor(termClr color, bool bright) {
    std::string esc1 = "\033[";
    std::string esc2 = std::to_string(color + 10);
    std::string esc4;
    if (bright) {
        esc4 = ";1";
    } else {
        esc4 = "";
    }
    std::string esc5 = "m";
    std::cout << esc1 + esc2 + esc4 + esc5;
}

void mt_clrscr() {
    std::cout<<"\033[2J";
}

uint32_t ZERO[2]    = {0xc3c3ffff, 0xffffc3c3};
uint32_t ONE[2]     = {0x03030303, 0x03030303};
uint32_t TWO[2]     = {0xffc0ffff, 0xffff03ff};
uint32_t THREE[2]   = {0xff03ffff, 0xffff03ff};
uint32_t FOUR[2]    = {0xff030303, 0xc3c3c3ff};
uint32_t FIVE[2]    = {0xff03ffff, 0xffffc0ff};
uint32_t SIX[2]     = {0xffc3ffff, 0xffffc0ff};
uint32_t SEVEN[2]   = {0x03030303, 0xffff0303};
uint32_t EIGHT[2]   = {0xffc3ffff, 0xffffc3ff};
uint32_t NINE[2]    = {0xff03ffff, 0xffffc3ff};

uint32_t PLUS[2]    = {0xff181818, 0x181818ff};
uint32_t MINUS[2]   = {0xff000000, 0x000000ff};

int bc_bitGet(uint32_t i_value, int i_bit) {
    return ((i_value) >> (uint32_t(i_bit) - 1)) & 0x1;
}

void bc_printBigChar (uint32_t mask[2], int x, int y) {
    mt_setCurPos(x, y);

    int lineCount = 0;

    for (int i = 1; i > -1; i--) {
        for (int j = 31; j > -1; j--) {
            if (bc_bitGet(mask[i], j + 1)) {
                std::cout << "\u2588";
            } else {
                std::cout << ' ';
            }

            if (j % 8 == 0) {
                lineCount++;
                mt_setCurPos(x, y + lineCount);
            }
        }
    }

    return;
}

void bc_printBigString (std::string str, int x, int y) {
    for (unsigned int i = 0; i < str.length(); i++) {
        switch (str[i])
        {
            case '-':
                bc_printBigChar(MINUS, x + 9 * i, y);
                break;

            case '0':
                bc_printBigChar(ZERO, x + 9 * i, y);
                break;

            case '1':
                bc_printBigChar(ONE, x + 9 * i, y);
                break;

            case '2':
                bc_printBigChar(TWO, x + 9 * i, y);
                break;

            case '3':
                bc_printBigChar(THREE, x + 9 * i, y);
                break;

            case '4':
                bc_printBigChar(FOUR, x + 9 * i, y);
                break;

            case '5':
                bc_printBigChar(FIVE, x + 9 * i, y);
                break;

            case '6':
                bc_printBigChar(SIX, x + 9 * i, y);
                break;

            case '7':
                bc_printBigChar(SEVEN, x + 9 * i, y);
                break;

            case '8':
                bc_printBigChar(EIGHT, x + 9 * i, y);
                break;

            case '9':
                bc_printBigChar(NINE, x + 9 * i, y);
                break;
        
            default:
                break;
        }
    }
}

#endif