#ifndef HELPERFUNCS_H
#define HELPERFUNCS_H

#include <cstdlib>
#include <cstdio>
#include <fstream>
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

//Initialize memory for EVM
int* sc_memoryInit();

//Set value to a EVM memory by index
int sc_memorySet(int* ptr_arr_i_array01, int i_address, int i_value);

//Overwrite value in passed pointer with value stored in EVM memory by index
int sc_memoryGet(int* arr_i_array01, int i_address, int* i_value);
//Return value stored in EVM memory by index
int sc_memoryGet(int* arr_i_array01, int i_address);

//Save EVM memory data to a file by path
int sc_memorySave(char* arr_c_path, int* arr_i_array01);
//Load EVM memory data from file by path
int sc_memoryLoad(char* arr_c_path, int* ptr_arr_i_array01);

/*
*
*   Flag Functions
*
*/

//Initialize flag for EVM
int* sc_flagInit();

//Set state to a EVM flag
int sc_flagSet(int* ptr_i_flag, enumFlag enumFlag_mask01, bool b_state);

//Overwrite state in passed pointer with stored state in EVM flag by RegistryStatusEnumFlags enum
int sc_flagGet(int* i_flag, enumFlag enumFlag_mask01, bool* ptr_b_state);
//Return state stored in EVM flag by RegistryStatusEnumFlags enum
int sc_flagGet(int* i_flag, enumFlag enumFlag_mask01);

/*
*
*   Command System
*
*/

int sc_commandEncode(int i_command, int i_operand, int* ptr_i_value);

int sc_commandDecode(int value, int* command, int* operand);

#endif