#include <cstdlib>
#include <cstdio>
#include <new>
#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>
#include <limits.h>
#include <algorithm>
#include "helperFuncs.h"

using namespace RegistryStatusEnumFlags;
using namespace TerminalColorsEnums;

int main(int argc, char const *argv[]) {
    register int accumulator = 0;
    int evmMemoryOffset = 0;
    int* evmMemory = sc_memoryInit();
    char evmMemoryDataPath[NAME_MAX] = "./content/db/memoryData.dat";
    char evmBigCharDataPath[NAME_MAX] = "./content/bigchars.txt";
    int* evmFlag = sc_flagInit();

    bool resValOverflow = false;
    bool resValEven = false;


    std::string panelTitle_MEMORY("[MEMORY]");
    std::string panelTitle_STATUS("[STATUS]");
    std::string panelTitle_SELECTED("[SELECTED VALUE]");

    while (1) {
        int* conWidth = new int;
        int* conHeight = new int;
        mt_getScreenSize(conWidth, conHeight);

        int* curPosX = new int(1);
        int* curPosY = new int(1);

        int* panelPosX_MEMORY = new int(1);
        int* panelPosY_MEMORY = new int(1);
        int* panelPosX_STATUS = new int(*conWidth * .5 + 1);
        int* panelPosY_STATUS = new int(1);
        int* panelPosX_SELECTED = new int(1);
        int* panelPosY_SELECTED = new int(*panelPosY_MEMORY + 2 + ceil(100 / (*conWidth * .5 / 6)));

        mt_clrscr();

        mt_setCurPos(*panelPosX_MEMORY, *panelPosY_MEMORY);
        std::cout << std::setfill('=') << panelTitle_MEMORY << std::setw(*conWidth * .5 - panelTitle_MEMORY.size()) << "" << std::setfill(' ');

        mt_setCurPos(*panelPosX_STATUS, *panelPosY_STATUS);
        std::cout << std::setfill('=') << panelTitle_STATUS << std::setw(*conWidth * .5 - panelTitle_MEMORY.size()) << "" << std::setfill(' ');

        mt_setCurPos(*panelPosX_MEMORY, *panelPosY_MEMORY + 1);
        for (int _counter = 0; _counter < 100; _counter++) {
            if (_counter == evmMemoryOffset) {
                mt_setCurFgColor(TerminalColorsEnums::RED, true);
                std::cout << "[" << std::setw(4) << std::right << evmMemory[_counter] << "]";
                std::cout << "\033[0m";
            } else {
                std::cout << "[" << std::setw(4) << std::right << evmMemory[_counter] << "]";
            }
            if ((_counter + 1) % (int)(*conWidth * .5 / 6) == 0)
                std::cout << std::endl;
        }

        mt_setCurPos(*panelPosX_STATUS, *panelPosY_STATUS + 1);
        std::cout << "ACCUMULATOR: " << accumulator;

        mt_setCurPos(*panelPosX_STATUS, *panelPosY_STATUS + 2);
        std::cout << "FLAGS: A B C D E F G H";
        mt_setCurPos(*panelPosX_STATUS, *panelPosY_STATUS + 3);
        std::cout << "       "  << sc_flagGet(*evmFlag, RegistryStatusEnumFlags::Alpha) << " "
                                << sc_flagGet(*evmFlag, RegistryStatusEnumFlags::Bravo) << " "
                                << sc_flagGet(*evmFlag, RegistryStatusEnumFlags::Charlie) << " "
                                << sc_flagGet(*evmFlag, RegistryStatusEnumFlags::Delta) << " "
                                << sc_flagGet(*evmFlag, RegistryStatusEnumFlags::Echo) << " "
                                << sc_flagGet(*evmFlag, RegistryStatusEnumFlags::Foxtrot) << " "
                                << sc_flagGet(*evmFlag, RegistryStatusEnumFlags::Golf) << " "
                                << sc_flagGet(*evmFlag, RegistryStatusEnumFlags::Hotel) << " ";

        mt_setCurPos(*panelPosX_SELECTED, *panelPosY_SELECTED);
        std::cout << std::setfill('=') << std::setw(*conWidth) << std::left << panelTitle_SELECTED << std::setfill(' ') << std::endl;

        std::string evmMemorySelected = std::to_string(evmMemory[evmMemoryOffset]);

        bc_printBigString(evmMemorySelected, *panelPosX_SELECTED, *panelPosY_SELECTED + 1);

        mt_setCurPos(*panelPosX_SELECTED, *panelPosY_SELECTED + 9);
        std::cout << std::setfill('=') << std::setw(*conWidth) << "" << std::setfill(' ') << std::endl;

        std::cout << std::left  << std::setw(20) << "[-] Prev Memory"
                                << std::setw(20) << "[+] Next Memory"
                                << std::setw(20) << "[s] Save Memory"
                                << std::setw(20) << "[l] Load Memory"
                                << std::setw(20) << "[w] Write BC"
                                << std::setw(20) << "[r] Read BC"
                                << std::setw(20) << "[f5] Call Func"
                                << std::setw(20) << "[q] Reset EVM"
                                << std::setw(20) << "[0] Exit EVM" << std::endl;

        std::cout << std::endl << "> ";

        // bc_printBox(*panelPosX_STATUS,*panelPosY_STATUS + 5,15,9);
        
        int keyPressed;
        char * keyEscape = new char[6];
        rk_readkey(&keyPressed, keyEscape);

        if (keyPressed == '0') break;

        if (keyPressed == 'w') {
            bc_writeBigString(evmMemorySelected, evmBigCharDataPath);
        }
        if (keyPressed == 'r') {
            bc_readBigString(evmBigCharDataPath);
            std::cin.get();
        }
        if (keyPressed == 'l') {
            sc_memoryLoad(evmMemoryDataPath, evmMemory);
        }
        if (keyPressed == 's') {
            sc_memorySave(evmMemoryDataPath, evmMemory);
        }
        if (keyPressed == '-') {
            if (evmMemoryOffset > 0) {
                evmMemoryOffset--;
            }
        }
        if (keyPressed == '+') {
            if (evmMemoryOffset < 99) {
                evmMemoryOffset++;
            }
        }

        if (keyPressed == 'q') {
            evmMemoryOffset = 0;
            delete(evmMemory);
            evmMemory = sc_memoryInit();
            delete(evmFlag);
            evmFlag = sc_flagInit();
            accumulator = 0;
        }

        if (keyEscape[2] == '1' && keyEscape[3] == '5') {
            std::string str_sequence;
            std::cout << "Enter sequence: ";
            std::getline(std::cin, str_sequence);
            if (str_sequence.empty()) {
                std::cout << "Unresolved sequence!" << std::endl;
            } else {
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
                    if (value < -9999 || value > 9999) {
                        std::cout << "Unresolved exception!" << std::endl;
                        break;
                    }
                    evmMemory[offset] = value;
                }

                if(str_sequence.compare(0,2,"20") == 0) { //LOAD
                    int offset = std::stoi(str_sequence.substr(3, 2));
                    accumulator = evmMemory[offset];
                }
                if(str_sequence.compare(0,2,"21") == 0) { //SAVE
                    int offset = std::stoi(str_sequence.substr(3, 2));
                    evmMemory[offset] = accumulator;
                }

                if(str_sequence.compare(0,2,"30") == 0) { //ADD
                    int offset = std::stoi(str_sequence.substr(3, 2));
                    accumulator = accumulator + evmMemory[offset];
                    if (accumulator < 9999 || accumulator > -9999) {
                        resValOverflow = false;
                    } else {
                        resValOverflow = true;
                    }

                    if (accumulator % 2 == 0) {
                        resValEven = true;
                    } else {
                        resValEven = false;
                    }
                }
                if(str_sequence.compare(0,2,"31") == 0) { //SUB
                    int offset = std::stoi(str_sequence.substr(3, 2));
                    accumulator = accumulator - evmMemory[offset];
                    if (accumulator < 9999 || accumulator > -9999) {
                        resValOverflow = false;
                    } else {
                        resValOverflow = true;
                    }

                    if (accumulator % 2 == 0) {
                        resValEven = true;
                    } else {
                        resValEven = false;
                    }
                }
                if(str_sequence.compare(0,2,"32") == 0) { //MUL
                    int offset = std::stoi(str_sequence.substr(3, 2));
                    accumulator = accumulator * evmMemory[offset];
                    if (accumulator < 9999 || accumulator > -9999) {
                        resValOverflow = false;
                    } else {
                        resValOverflow = true;
                    }

                    if (accumulator % 2 == 0) {
                        resValEven = true;
                    } else {
                        resValEven = false;
                    }
                }
                if(str_sequence.compare(0,2,"33") == 0) { //DIVIDE
                    int offset = std::stoi(str_sequence.substr(3, 2));
                    accumulator = accumulator / evmMemory[offset];
                    if (accumulator < 9999 || accumulator > -9999) {
                        resValOverflow = false;
                    } else {
                        resValOverflow = true;
                    }

                    if (accumulator % 2 == 0) {
                        resValEven = true;
                    } else {
                        resValEven = false;
                    }
                }

                if(str_sequence.compare(0,2,"40") == 0) { //JUMP
                    evmMemoryOffset = std::stoi(str_sequence.substr(3, 2));
                }
                if(str_sequence.compare(0,2,"41") == 0) { //JNEG
                    if (accumulator < 0)
                        evmMemoryOffset = std::stoi(str_sequence.substr(3, 2));
                }
                if(str_sequence.compare(0,2,"42") == 0) { //JZ
                    if (accumulator == 0)
                        evmMemoryOffset = std::stoi(str_sequence.substr(3, 2));
                }
                if(str_sequence.compare(0,2,"43") == 0) { //HALT
                    return 0;
                }

                if(str_sequence.compare(0,2,"50") == 0) { //NOT
                    int offset = std::stoi(str_sequence.substr(3, 2));
                    evmMemory[offset] = ~accumulator;
                }
                if(str_sequence.compare(0,2,"51") == 0) { //AND
                    int offset = std::stoi(str_sequence.substr(3, 2));
                    accumulator &= evmMemory[offset];
                    if (accumulator < 9999 || accumulator > -9999) {
                        resValOverflow = false;
                    } else {
                        resValOverflow = true;
                    }

                    if (accumulator % 2 == 0) {
                        resValEven = true;
                    } else {
                        resValEven = false;
                    }
                }
                if(str_sequence.compare(0,2,"52") == 0) { //OR
                    int offset = std::stoi(str_sequence.substr(3, 2));
                    accumulator |= evmMemory[offset];
                    if (accumulator < 9999 || accumulator > -9999) {
                        resValOverflow = false;
                    } else {
                        resValOverflow = true;
                    }

                    if (accumulator % 2 == 0) {
                        resValEven = true;
                    } else {
                        resValEven = false;
                    }
                }
                if(str_sequence.compare(0,2,"53") == 0) { //XOR
                    int offset = std::stoi(str_sequence.substr(3, 2));
                    accumulator ^= evmMemory[offset];
                    if (accumulator < 9999 || accumulator > -9999) {
                        resValOverflow = false;
                    } else {
                        resValOverflow = true;
                    }

                    if (accumulator % 2 == 0) {
                        resValEven = true;
                    } else {
                        resValEven = false;
                    }
                }
                if(str_sequence.compare(0,2,"54") == 0) { //JNS
                    if (accumulator > 0)
                        evmMemoryOffset = std::stoi(str_sequence.substr(3, 2));
                }
                if(str_sequence.compare(0,2,"56") == 0) { //JC
                    if (resValOverflow)
                        evmMemoryOffset = std::stoi(str_sequence.substr(3, 2));
                }
                if(str_sequence.compare(0,2,"57") == 0) { //JNC
                    if (!resValOverflow)
                        evmMemoryOffset = std::stoi(str_sequence.substr(3, 2));
                }
                if(str_sequence.compare(0,2,"58") == 0) { //JP
                    if (resValEven)
                        evmMemoryOffset = std::stoi(str_sequence.substr(3, 2));
                }
                if(str_sequence.compare(0,2,"59") == 0) { //JNP
                    if (!resValEven)
                        evmMemoryOffset = std::stoi(str_sequence.substr(3, 2));
                }

                if(str_sequence.compare(0,2,"60") == 0) { //CHL
                    int offset = std::stoi(str_sequence.substr(3, 2));
                    accumulator = evmMemory[offset] << 1;
                    if (accumulator < 9999 || accumulator > -9999) {
                        resValOverflow = false;
                    } else {
                        resValOverflow = true;
                    }

                    if (accumulator % 2 == 0) {
                        resValEven = true;
                    } else {
                        resValEven = false;
                    }
                }
                if(str_sequence.compare(0,2,"61") == 0) { //SHR
                    int offset = std::stoi(str_sequence.substr(3, 2));
                    accumulator = evmMemory[offset] >> 1;
                    if (accumulator < 9999 || accumulator > -9999) {
                        resValOverflow = false;
                    } else {
                        resValOverflow = true;
                    }

                    if (accumulator % 2 == 0) {
                        resValEven = true;
                    } else {
                        resValEven = false;
                    }
                }
                if(str_sequence.compare(0,2,"62") == 0) { //RCL
                    int offset = std::stoi(str_sequence.substr(3, 2));
                    accumulator = (evmMemory[offset] << 1) | (evmMemory[offset] >> (32 - 1));
                    if (accumulator < 9999 || accumulator > -9999) {
                        resValOverflow = false;
                    } else {
                        resValOverflow = true;
                    }

                    if (accumulator % 2 == 0) {
                        resValEven = true;
                    } else {
                        resValEven = false;
                    }
                }
                if(str_sequence.compare(0,2,"63") == 0) { //RCR
                    int offset = std::stoi(str_sequence.substr(3, 2));
                    accumulator = (evmMemory[offset] >> 1) | (evmMemory[offset] << (32 - 1));
                    if (accumulator < 9999 || accumulator > -9999) {
                        resValOverflow = false;
                    } else {
                        resValOverflow = true;
                    }

                    if (accumulator % 2 == 0) {
                        resValEven = true;
                    } else {
                        resValEven = false;
                    }
                }
                if(str_sequence.compare(0,2,"65") == 0) { //ADDC
                    int offset = std::stoi(str_sequence.substr(3, 2));
                    accumulator = evmMemory[offset] + evmMemory[accumulator];
                    if (accumulator < 9999 || accumulator > -9999) {
                        resValOverflow = false;
                    } else {
                        resValOverflow = true;
                    }

                    if (accumulator % 2 == 0) {
                        resValEven = true;
                    } else {
                        resValEven = false;
                    }
                }
                if(str_sequence.compare(0,2,"66") == 0) { //SUBC
                    int offset = std::stoi(str_sequence.substr(3, 2));
                    accumulator = evmMemory[offset] - evmMemory[accumulator];
                    if (accumulator < 9999 || accumulator > -9999) {
                        resValOverflow = false;
                    } else {
                        resValOverflow = true;
                    }

                    if (accumulator % 2 == 0) {
                        resValEven = true;
                    } else {
                        resValEven = false;
                    }
                }
                if(str_sequence.compare(0,2,"67") == 0) { //LOGLC
                    int offset = std::stoi(str_sequence.substr(3, 2));
                    accumulator = evmMemory[offset] << accumulator;
                    if (accumulator < 9999 || accumulator > -9999) {
                        resValOverflow = false;
                    } else {
                        resValOverflow = true;
                    }

                    if (accumulator % 2 == 0) {
                        resValEven = true;
                    } else {
                        resValEven = false;
                    }
                }
                if(str_sequence.compare(0,2,"68") == 0) { //LOGRC
                    int offset = std::stoi(str_sequence.substr(3, 2));
                    accumulator = evmMemory[offset] >> accumulator;
                    if (accumulator < 9999 || accumulator > -9999) {
                        resValOverflow = false;
                    } else {
                        resValOverflow = true;
                    }

                    if (accumulator % 2 == 0) {
                        resValEven = true;
                    } else {
                        resValEven = false;
                    }
                }
                if(str_sequence.compare(0,2,"69") == 0) { //RCCL
                    int offset = std::stoi(str_sequence.substr(3, 2));
                    accumulator = (evmMemory[offset] << accumulator) | (evmMemory[offset] >> (32 - accumulator));
                    if (accumulator < 9999 || accumulator > -9999) {
                        resValOverflow = false;
                    } else {
                        resValOverflow = true;
                    }

                    if (accumulator % 2 == 0) {
                        resValEven = true;
                    } else {
                        resValEven = false;
                    }
                }
                if(str_sequence.compare(0,2,"70") == 0) { //RCCR
                    int offset = std::stoi(str_sequence.substr(3, 2));
                    accumulator = (evmMemory[offset] >> accumulator) | (evmMemory[offset] << (32 - accumulator));
                    if (accumulator < 9999 || accumulator > -9999) {
                        resValOverflow = false;
                    } else {
                        resValOverflow = true;
                    }

                    if (accumulator % 2 == 0) {
                        resValEven = true;
                    } else {
                        resValEven = false;
                    }
                }

                if(str_sequence.compare(0,2,"71") == 0) { //MOVA
                    int offset = std::stoi(str_sequence.substr(3, 2));
                    evmMemory[accumulator] = evmMemory[offset];
                }
                if(str_sequence.compare(0,2,"72") == 0) { //MOVR
                    int offset = std::stoi(str_sequence.substr(3, 2));
                    evmMemory[offset] = evmMemory[accumulator];
                }
                if(str_sequence.compare(0,2,"73") == 0) { //MOVCA
                    int offset = std::stoi(str_sequence.substr(3, 2));
                    evmMemory[evmMemory[accumulator]] = evmMemory[offset];
                }
                if(str_sequence.compare(0,2,"74") == 0) { //MOVCR
                    int offset = std::stoi(str_sequence.substr(3, 2));
                    evmMemory[evmMemory[accumulator]] = evmMemory[offset];
                }

                if(str_sequence.compare(0,2,"75") == 0) { //MADDC
                    int offset = std::stoi(str_sequence.substr(3, 2));
                    accumulator = evmMemory[offset] + evmMemory[evmMemory[accumulator]];
                    if (accumulator < 9999 || accumulator > -9999) {
                        resValOverflow = false;
                    } else {
                        resValOverflow = true;
                    }

                    if (accumulator % 2 == 0) {
                        resValEven = true;
                    } else {
                        resValEven = false;
                    }
                }
                if(str_sequence.compare(0,2,"76") == 0) { //MSUBC
                    int offset = std::stoi(str_sequence.substr(3, 2));
                    accumulator = evmMemory[offset] - evmMemory[evmMemory[accumulator]];
                    if (accumulator < 9999 || accumulator > -9999) {
                        resValOverflow = false;
                    } else {
                        resValOverflow = true;
                    }

                    if (accumulator % 2 == 0) {
                        resValEven = true;
                    } else {
                        resValEven = false;
                    }
                }
            }
        }

        if (keyEscape[2] == '1' && keyEscape[3] == '7') {
            std::cout << "Key [F6]" << std::endl;
            std::cin.get();
        }
    }

    // int* evmFlag = sc_flagInit();

    // std::cout << "Printing evmFlag::Alpha" << std::endl;
    // std::cout << "(" << sc_flagGet(evmFlag, RegistryStatusEnumFlags::Alpha) << ")" << std::endl;

    // std::cout << "Editing evmFlag::Alpha" << std::endl;
    // sc_flagSet(evmFlag, RegistryStatusEnumFlags::Alpha, true);

    // std::cout << "Printing evmFlag::Alpha" << std::endl;
    // std::cout << "(" << sc_flagGet(evmFlag, RegistryStatusEnumFlags::Alpha) << ")" << std::endl;

    // std::cout << std::endl;

    // int* value = new (std::nothrow) int;
    // *value = 0;

    // std::cout << "Encoding Value" << std::endl;
    // sc_commandEncode(10, 127, value);

    // std::cout << *value << std::endl;

    // int* command = new (std::nothrow) int;
    // *command = 0;
    // int* operand = new (std::nothrow) int;
    // *operand = 0;

    // std::cout << "Decoding Value" << std::endl;
    // sc_commandDecode(*value, command, operand);
    // std::cout << "Command: " << *command << std::endl;
    // std::cout << "Operand: " << *operand << std::endl;

    // return 0;
}
