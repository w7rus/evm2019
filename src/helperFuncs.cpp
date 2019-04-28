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
#include <termios.h>

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
uint32_t NONE[2]    = {0x00000000, 0x00000000};

int bc_bitGet(uint32_t i_value, int i_bit) {
    return ((i_value) >> (uint32_t(i_bit) - 1)) & 0x1;
}

void bc_printBigChar(uint32_t mask[2], int x, int y) {
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

void bc_printBigString(std::string str, int x, int y) {
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
                bc_printBigChar(NONE, x + 9 * i, y);
                break;
        }
    }
}

int bc_writeBigString(std::string str, char* filepath) {
    FILE* file = fopen(filepath, "w+b");
    if (file == NULL)
        return -1;

    unsigned int str_length = str.length();

    u_int32_t** data = new u_int32_t* [str_length];
    for (unsigned int i = 0; i < str_length; i++) {
        data[i] = new u_int32_t[2];
    }

    for (unsigned int i = 0; i < str_length; i++) {
        switch (str[i])
        {
            case '-':
                data[i] = MINUS;
                break;

            case '0':
                data[i] = ZERO;
                break;

            case '1':
                data[i] = ONE;
                break;

            case '2':
                data[i] = TWO;
                break;

            case '3':
                data[i] = THREE;
                break;

            case '4':
                data[i] = FOUR;
                break;

            case '5':
                data[i] = FIVE;
                break;

            case '6':
                data[i] = SIX;
                break;

            case '7':
                data[i] = SEVEN;
                break;

            case '8':
                data[i] = EIGHT;
                break;

            case '9':
                data[i] = NINE;
                break;
        
            default:
                data[i] = NONE;
                break;
        }
    }

    for (unsigned int i = 0; i < str_length; i++) {
        fwrite(data[i], sizeof(uint32_t) * 2, 1, file);
    }

    fclose(file);
    return 0;
}

int bc_readBigString(char* filepath) {
    FILE* file = fopen(filepath, "rb");
    if (file == NULL)
        return -1;

    unsigned int data_length = 0;

    fseek(file, 0, SEEK_END);
    data_length = ftell(file);
    rewind(file);

    data_length = data_length / (sizeof(u_int32_t) * 2);

    u_int32_t** data = new u_int32_t* [data_length];
    for (unsigned int i = 0; i < data_length; i++) {
        data[i] = new u_int32_t[2];
    }

    for (unsigned int i = 0; i < data_length; i++) {
        fread(data[i], sizeof(uint32_t) * 2, 1, file);
    }

    for (unsigned int i = 0; i < data_length; i++) {
        std::cout << std::hex << data[i][0] << " " << data[i][1] << std::dec << std::endl;
    }

    return 0;
}

void bc_printBox(int x, int y, int width, int height) {
    width -= 1;
    height -= 1;

    mt_setCurPos(x, y);
    std::cout << "\u250C";

    mt_setCurPos(x + width, y);
    std::cout << "\u2510";

    mt_setCurPos(x, y + height);
    std::cout << "\u2514";

    mt_setCurPos(x + width, y + height);
    std::cout << "\u2518";

    for (int i = 0; i < width - 1; i++) {
        mt_setCurPos(x + i + 1, y);
        std::cout << "\u2500";
        mt_setCurPos(x + i + 1, y + height);
        std::cout << "\u2500";
    }

    for (int i = 0; i < height - 1; i++) {
        mt_setCurPos(x, y + i + 1);
        std::cout << "\u2502";
        mt_setCurPos(x + width, y + i + 1);
        std::cout << "\u2502";
    }
}

/*
*
*   Key system
*
*/

static struct termios t;

void rk_bufMode(bool value) {
    if (value) {
        tcgetattr(STDIN_FILENO, &t);
        t.c_lflag &= ~ICANON;
        t.c_lflag &= ~ECHO;
        tcsetattr(STDIN_FILENO, TCSANOW, &t);
    } else {
        tcgetattr(STDIN_FILENO, &t);
        t.c_lflag |= ICANON;
        t.c_lflag |= ECHO;
        tcsetattr(STDIN_FILENO, TCSANOW, &t);
    }
}

void rk_readkey(int * key, char * escape) {
    rk_bufMode(true);
    *key = std::getchar();
    if (*key == 126) {
        read(0, escape, 6);
    }
    rk_bufMode(false);
}

/*
*
*   Script system
*
*/

int ALU(std::string str_sequence, int * evmMemory, int * evmMemoryOffset, int * accumulator, bool * resValOverflow, bool * resValEven) {
    if(str_sequence.compare(0,2,"10") == 0) { //READ
        int offset = std::stoi(str_sequence.substr(3, 2));
        std::cout << "[0x" << std::hex << offset << ']' << std::dec << evmMemory[offset] << std::endl;
        std::cin.get();
    }
    if(str_sequence.compare(0,2,"11") == 0) { //WRITE
        int offset = std::stoi(str_sequence.substr(3, 2));
        int value = 0;
        std::cout << "Enter value: ";
        std::cin >> value;
        evmMemory[offset] = value;
    }

    if(str_sequence.compare(0,2,"20") == 0) { //LOAD
        int offset = std::stoi(str_sequence.substr(3, 2));
        *accumulator = evmMemory[offset];
    }
    if(str_sequence.compare(0,2,"21") == 0) { //SAVE
        int offset = std::stoi(str_sequence.substr(3, 2));
        evmMemory[offset] = *accumulator;
    }

    if(str_sequence.compare(0,2,"30") == 0) { //ADD
        int offset = std::stoi(str_sequence.substr(3, 2));
        *accumulator = *accumulator + evmMemory[offset];
        if (*accumulator < 9999 || *accumulator > -9999) {
            *resValOverflow = false;
        } else {
            *resValOverflow = true;
        }

        if (*accumulator % 2 == 0) {
            *resValEven = true;
        } else {
            *resValEven = false;
        }
    }
    if(str_sequence.compare(0,2,"31") == 0) { //SUB
        int offset = std::stoi(str_sequence.substr(3, 2));
        *accumulator = *accumulator - evmMemory[offset];
        if (*accumulator < 9999 || *accumulator > -9999) {
            *resValOverflow = false;
        } else {
            *resValOverflow = true;
        }

        if (*accumulator % 2 == 0) {
            *resValEven = true;
        } else {
            *resValEven = false;
        }
    }
    if(str_sequence.compare(0,2,"32") == 0) { //MUL
        int offset = std::stoi(str_sequence.substr(3, 2));
        *accumulator = *accumulator * evmMemory[offset];
        if (*accumulator < 9999 || *accumulator > -9999) {
            *resValOverflow = false;
        } else {
            *resValOverflow = true;
        }

        if (*accumulator % 2 == 0) {
            *resValEven = true;
        } else {
            *resValEven = false;
        }
    }
    if(str_sequence.compare(0,2,"33") == 0) { //DIVIDE
        int offset = std::stoi(str_sequence.substr(3, 2));
        *accumulator = *accumulator / evmMemory[offset];
        if (*accumulator < 9999 || *accumulator > -9999) {
            *resValOverflow = false;
        } else {
            *resValOverflow = true;
        }

        if (*accumulator % 2 == 0) {
            *resValEven = true;
        } else {
            *resValEven = false;
        }
    }

    if(str_sequence.compare(0,2,"40") == 0) { //JUMP
        *evmMemoryOffset = std::stoi(str_sequence.substr(3, 2));
    }
    if(str_sequence.compare(0,2,"41") == 0) { //JNEG
        if (*accumulator < 0)
            *evmMemoryOffset = std::stoi(str_sequence.substr(3, 2));
    }
    if(str_sequence.compare(0,2,"42") == 0) { //JZ
        if (*accumulator == 0)
            *evmMemoryOffset = std::stoi(str_sequence.substr(3, 2));
    }
    if(str_sequence.compare(0,2,"43") == 0) { //HALT
        return -1;
    }

    if(str_sequence.compare(0,2,"50") == 0) { //NOT
        int offset = std::stoi(str_sequence.substr(3, 2));
        evmMemory[offset] = ~(*accumulator);
    }
    if(str_sequence.compare(0,2,"51") == 0) { //AND
        int offset = std::stoi(str_sequence.substr(3, 2));
        *accumulator &= evmMemory[offset];
        if (*accumulator < 9999 || *accumulator > -9999) {
            *resValOverflow = false;
        } else {
            *resValOverflow = true;
        }

        if (*accumulator % 2 == 0) {
            *resValEven = true;
        } else {
            *resValEven = false;
        }
    }
    if(str_sequence.compare(0,2,"52") == 0) { //OR
        int offset = std::stoi(str_sequence.substr(3, 2));
        *accumulator |= evmMemory[offset];
        if (*accumulator < 9999 || *accumulator > -9999) {
            *resValOverflow = false;
        } else {
            *resValOverflow = true;
        }

        if (*accumulator % 2 == 0) {
            *resValEven = true;
        } else {
            *resValEven = false;
        }
    }
    if(str_sequence.compare(0,2,"53") == 0) { //XOR
        int offset = std::stoi(str_sequence.substr(3, 2));
        *accumulator ^= evmMemory[offset];
        if (*accumulator < 9999 || *accumulator > -9999) {
            *resValOverflow = false;
        } else {
            *resValOverflow = true;
        }

        if (*accumulator % 2 == 0) {
            *resValEven = true;
        } else {
            *resValEven = false;
        }
    }
    if(str_sequence.compare(0,2,"54") == 0) { //JNS
        if (*accumulator > 0)
            *evmMemoryOffset = std::stoi(str_sequence.substr(3, 2));
    }
    if(str_sequence.compare(0,2,"56") == 0) { //JC
        if (*resValOverflow)
            *evmMemoryOffset = std::stoi(str_sequence.substr(3, 2));
    }
    if(str_sequence.compare(0,2,"57") == 0) { //JNC
        if (!(*resValOverflow))
            *evmMemoryOffset = std::stoi(str_sequence.substr(3, 2));
    }
    if(str_sequence.compare(0,2,"58") == 0) { //JP
        if (*resValEven)
            *evmMemoryOffset = std::stoi(str_sequence.substr(3, 2));
    }
    if(str_sequence.compare(0,2,"59") == 0) { //JNP
        if (!(*resValEven))
            *evmMemoryOffset = std::stoi(str_sequence.substr(3, 2));
    }

    if(str_sequence.compare(0,2,"60") == 0) { //CHL
        int offset = std::stoi(str_sequence.substr(3, 2));
        *accumulator = evmMemory[offset] << 1;
        if (*accumulator < 9999 || *accumulator > -9999) {
            *resValOverflow = false;
        } else {
            *resValOverflow = true;
        }

        if (*accumulator % 2 == 0) {
            *resValEven = true;
        } else {
            *resValEven = false;
        }
    }
    if(str_sequence.compare(0,2,"61") == 0) { //SHR
        int offset = std::stoi(str_sequence.substr(3, 2));
        *accumulator = evmMemory[offset] >> 1;
        if (*accumulator < 9999 || *accumulator > -9999) {
            *resValOverflow = false;
        } else {
            *resValOverflow = true;
        }

        if (*accumulator % 2 == 0) {
            *resValEven = true;
        } else {
            *resValEven = false;
        }
    }
    if(str_sequence.compare(0,2,"62") == 0) { //RCL
        int offset = std::stoi(str_sequence.substr(3, 2));
        *accumulator = (evmMemory[offset] << 1) | (evmMemory[offset] >> (32 - 1));
        if (*accumulator < 9999 || *accumulator > -9999) {
            *resValOverflow = false;
        } else {
            *resValOverflow = true;
        }

        if (*accumulator % 2 == 0) {
            *resValEven = true;
        } else {
            *resValEven = false;
        }
    }
    if(str_sequence.compare(0,2,"63") == 0) { //RCR
        int offset = std::stoi(str_sequence.substr(3, 2));
        *accumulator = (evmMemory[offset] >> 1) | (evmMemory[offset] << (32 - 1));
        if (*accumulator < 9999 || *accumulator > -9999) {
            *resValOverflow = false;
        } else {
            *resValOverflow = true;
        }

        if (*accumulator % 2 == 0) {
            *resValEven = true;
        } else {
            *resValEven = false;
        }
    }
    if(str_sequence.compare(0,2,"65") == 0) { //ADDC
        int offset = std::stoi(str_sequence.substr(3, 2));
        *accumulator = evmMemory[offset] + evmMemory[*accumulator];
        if (*accumulator < 9999 || *accumulator > -9999) {
            *resValOverflow = false;
        } else {
            *resValOverflow = true;
        }

        if (*accumulator % 2 == 0) {
            *resValEven = true;
        } else {
            *resValEven = false;
        }
    }
    if(str_sequence.compare(0,2,"66") == 0) { //SUBC
        int offset = std::stoi(str_sequence.substr(3, 2));
        *accumulator = evmMemory[offset] - evmMemory[*accumulator];
        if (*accumulator < 9999 || *accumulator > -9999) {
            *resValOverflow = false;
        } else {
            *resValOverflow = true;
        }

        if (*accumulator % 2 == 0) {
            *resValEven = true;
        } else {
            *resValEven = false;
        }
    }
    if(str_sequence.compare(0,2,"67") == 0) { //LOGLC
        int offset = std::stoi(str_sequence.substr(3, 2));
        *accumulator = evmMemory[offset] << *accumulator;
        if (*accumulator < 9999 || *accumulator > -9999) {
            *resValOverflow = false;
        } else {
            *resValOverflow = true;
        }

        if (*accumulator % 2 == 0) {
            *resValEven = true;
        } else {
            *resValEven = false;
        }
    }
    if(str_sequence.compare(0,2,"68") == 0) { //LOGRC
        int offset = std::stoi(str_sequence.substr(3, 2));
        *accumulator = evmMemory[offset] >> *accumulator;
        if (*accumulator < 9999 || *accumulator > -9999) {
            *resValOverflow = false;
        } else {
            *resValOverflow = true;
        }

        if (*accumulator % 2 == 0) {
            *resValEven = true;
        } else {
            *resValEven = false;
        }
    }
    if(str_sequence.compare(0,2,"69") == 0) { //RCCL
        int offset = std::stoi(str_sequence.substr(3, 2));
        *accumulator = (evmMemory[offset] << *accumulator) | (evmMemory[offset] >> (32 - *accumulator));
        if (*accumulator < 9999 || *accumulator > -9999) {
            *resValOverflow = false;
        } else {
            *resValOverflow = true;
        }

        if (*accumulator % 2 == 0) {
            *resValEven = true;
        } else {
            *resValEven = false;
        }
    }
    if(str_sequence.compare(0,2,"70") == 0) { //RCCR
        int offset = std::stoi(str_sequence.substr(3, 2));
        *accumulator = (evmMemory[offset] >> *accumulator) | (evmMemory[offset] << (32 - *accumulator));
        if (*accumulator < 9999 || *accumulator > -9999) {
            *resValOverflow = false;
        } else {
            *resValOverflow = true;
        }

        if (*accumulator % 2 == 0) {
            *resValEven = true;
        } else {
            *resValEven = false;
        }
    }

    if(str_sequence.compare(0,2,"71") == 0) { //MOVA
        int offset = std::stoi(str_sequence.substr(3, 2));
        evmMemory[*accumulator] = evmMemory[offset];
    }
    if(str_sequence.compare(0,2,"72") == 0) { //MOVR
        int offset = std::stoi(str_sequence.substr(3, 2));
        evmMemory[offset] = evmMemory[*accumulator];
    }
    if(str_sequence.compare(0,2,"73") == 0) { //MOVCA
        int offset = std::stoi(str_sequence.substr(3, 2));
        evmMemory[evmMemory[*accumulator]] = evmMemory[offset];
    }
    if(str_sequence.compare(0,2,"74") == 0) { //MOVCR
        int offset = std::stoi(str_sequence.substr(3, 2));
        evmMemory[evmMemory[*accumulator]] = evmMemory[offset];
    }

    if(str_sequence.compare(0,2,"75") == 0) { //MADDC
        int offset = std::stoi(str_sequence.substr(3, 2));
        *accumulator = evmMemory[offset] + evmMemory[evmMemory[*accumulator]];
        if (*accumulator < 9999 || *accumulator > -9999) {
            *resValOverflow = false;
        } else {
            *resValOverflow = true;
        }

        if (*accumulator % 2 == 0) {
            *resValEven = true;
        } else {
            *resValEven = false;
        }
    }
    if(str_sequence.compare(0,2,"76") == 0) { //MSUBC
        int offset = std::stoi(str_sequence.substr(3, 2));
        *accumulator = evmMemory[offset] - evmMemory[evmMemory[*accumulator]];
        if (*accumulator < 9999 || *accumulator > -9999) {
            *resValOverflow = false;
        } else {
            *resValOverflow = true;
        }

        if (*accumulator % 2 == 0) {
            *resValEven = true;
        } else {
            *resValEven = false;
        }
    }

    return 0;
}

/*
*
*   Disk management system
*
*/

struct tCHS {
    unsigned int addr:20;
};

struct tLARGE {
    unsigned int addr:24;
};

struct tIDECHS {
    unsigned int addr:28;
};

struct tLBA {
    unsigned int addr:32;
};

int g_chs2idechs(tCHS, tIDECHS *) {
    return 0;
}
int g_chs2large(tCHS, tLARGE *) {
    return 0;
}
int g_chs2lba(tCHS, tLBA *) {
    return 0;
}

int g_idechs2chs(tIDECHS, tCHS *) {
    return 0;
}
int g_idechs2large(tIDECHS, tLARGE *) {
    return 0;
}
int g_idechs2lba(tIDECHS, tLBA *) {
    return 0;
}

int g_large2chs(tLARGE, tCHS *) {
    return 0;
}
int g_large2idechs(tLARGE, tIDECHS *) {
    return 0;
}
int g_large2lba(tLARGE, tLBA *) {
    return 0;
}

int g_lba2chs(tLBA, tCHS *) {
    return 0;
}
int g_lba2idechs(tLBA, tIDECHS *) {
    return 0;
}
int g_lba2large(tLBA, tLARGE *) {
    return 0;
}



int a_chs2idechs(tIDECHS geometry, tCHS, tIDECHS *) {
    return 0;
}
int a_chs2large(tLARGE geometry, tCHS, tLARGE *) {
    return 0;
}
int a_chs2lba(tLBA geometry, tCHS, tLBA *) {
    return 0;
}

int a_idechs2chs(tCHS geometry, tIDECHS, tCHS *) {
    return 0;
}
int a_idechs2large(tLARGE geometry, tIDECHS, tLARGE *) {
    return 0;
}
int a_idechs2lba(tLBA geometry, tIDECHS, tLBA *) {
    return 0;
}

int a_large2chs(tCHS geometry, tLARGE, tCHS *) {
    return 0;
}
int a_large2idechs(tIDECHS geometry, tLARGE, tIDECHS *) {
    return 0;
}
int a_large2lba(tLBA geometry, tLARGE, tLBA *) {
    return 0;
}

int a_lba2chs(tCHS geometry, tLBA, tCHS *) {
    return 0;
}
int a_lba2idechs(tIDECHS geometry, tLBA, tIDECHS *) {
    return 0;
}
int a_lba2large(tLARGE geometry, tLBA, tLARGE *) {
    return 0;
}

#endif