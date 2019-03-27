#include <cstdlib>
#include <cstdio>
#include <new>
#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>
#include <limits.h>
#include "helperFuncs.h"

using namespace RegistryStatusEnumFlags;
using namespace TerminalColorsEnums;

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

void bc_printBigChar (uint32_t mask[2], int x, int y, termClr colorFG, termClr colorBG) {
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

int main(int argc, char const *argv[]) {
    register int accumulator = 0;
    int evmMemoryOffset = 0;
    int* evmMemory = sc_memoryInit();
    evmMemory[2] = 1488;
    evmMemory[3] = -228;
    char evmMemoryDataPath[NAME_MAX] = "./content/db/memoryData.dat";
    int* evmFlag = sc_flagInit();

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

        if (bc_bitGet(evmMemory[evmMemoryOffset], 31)) {
            bc_printBigChar(MINUS, *panelPosX_SELECTED, *panelPosY_SELECTED + 1, termClr::GREEN, termClr::BLACK);
            evmMemorySelected.erase(evmMemorySelected.begin());
        } else {
            bc_printBigChar(PLUS, *panelPosX_SELECTED, *panelPosY_SELECTED + 1, termClr::GREEN, termClr::BLACK);
        }

        for (unsigned int i = 0; i < evmMemorySelected.length(); i++) {
            if (evmMemorySelected[i] == '0') {
                bc_printBigChar(ZERO, *panelPosX_SELECTED + 9 * (i + 1), *panelPosY_SELECTED + 1, termClr::GREEN, termClr::BLACK);
            }
            if (evmMemorySelected[i] == '1') {
                bc_printBigChar(ONE, *panelPosX_SELECTED+ 9 * (i + 1), *panelPosY_SELECTED + 1, termClr::GREEN, termClr::BLACK);
            }
            if (evmMemorySelected[i] == '2') {
                bc_printBigChar(TWO, *panelPosX_SELECTED+ 9 * (i + 1), *panelPosY_SELECTED + 1, termClr::GREEN, termClr::BLACK);
            }
            if (evmMemorySelected[i] == '3') {
                bc_printBigChar(THREE, *panelPosX_SELECTED+ 9 * (i + 1), *panelPosY_SELECTED + 1, termClr::GREEN, termClr::BLACK);
            }
            if (evmMemorySelected[i] == '4') {
                bc_printBigChar(FOUR, *panelPosX_SELECTED+ 9 * (i + 1), *panelPosY_SELECTED + 1, termClr::GREEN, termClr::BLACK);
            }
            if (evmMemorySelected[i] == '5') {
                bc_printBigChar(FIVE, *panelPosX_SELECTED+ 9 * (i + 1), *panelPosY_SELECTED + 1, termClr::GREEN, termClr::BLACK);
            }
            if (evmMemorySelected[i] == '6') {
                bc_printBigChar(SIX, *panelPosX_SELECTED+ 9 * (i + 1), *panelPosY_SELECTED + 1, termClr::GREEN, termClr::BLACK);
            }
            if (evmMemorySelected[i] == '7') {
                bc_printBigChar(SEVEN, *panelPosX_SELECTED+ 9 * (i + 1), *panelPosY_SELECTED + 1, termClr::GREEN, termClr::BLACK);
            }
            if (evmMemorySelected[i] == '8') {
                bc_printBigChar(EIGHT, *panelPosX_SELECTED+ 9 * (i + 1), *panelPosY_SELECTED + 1, termClr::GREEN, termClr::BLACK);
            }
            if (evmMemorySelected[i] == '9') {
                bc_printBigChar(NINE, *panelPosX_SELECTED+ 9 * (i + 1), *panelPosY_SELECTED + 1, termClr::GREEN, termClr::BLACK);
            }
        }

        mt_setCurPos(*panelPosX_SELECTED, *panelPosY_SELECTED + 9);
        std::cout << std::setfill('=') << std::setw(*conWidth) << "" << std::setfill(' ') << std::endl;

        std::cout << std::left  << std::setw(20) << "[W] Offset++"
                                << std::setw(20) << "[S] Offset--"
                                << std::setw(20) << "[A] Unassigned"
                                << std::setw(20) << "[D] Unassigned"
                                << std::setw(20) << "[E] Unassigned"
                                << std::setw(20) << "[0] Exit" << std::endl;
        
        char keyPressed = std::cin.get();
        if (keyPressed == '0') break;
        if (keyPressed == 'a') {
            
        }
        if (keyPressed == 'w') {
            if (evmMemoryOffset < 99) {
                evmMemoryOffset++;
            }
        }
        if (keyPressed == 'd') {
            

        }
        if (keyPressed == 's') {
            if (evmMemoryOffset > 0) {
                evmMemoryOffset--;
            }
        }
        if (keyPressed == 'e') {
            
        }
    }

    // try
    // {
    //     std::cout << "Printing evmMemory" << std::endl;
    //     for (int _counter = 0; _counter < 100; _counter++)
    //         std::cout << "[" << evmMemory[_counter] << "]";
    //     std::cout << std::endl;

    //     std::cout << "Editing evmMemory" << std::endl;
    //     sc_memorySet(evmMemory, 50, 1023);

    //     std::cout << "Printing evmMemory" << std::endl;
    //     for (int _counter = 0; _counter < 100; _counter++)
    //         std::cout << "[" << evmMemory[_counter] << "]";
    //     std::cout << std::endl;

    //     std::cout << "Saving evmMemory" << std::endl;
    //     sc_memorySave(evmMemoryDataPath, evmMemory);

    //     std::cout << "Editing evmMemory" << std::endl;
    //     sc_memorySet(evmMemory, 50, 0);

    //     std::cout << "Printing evmMemory" << std::endl;
    //     for (int _counter = 0; _counter < 100; _counter++)
    //         std::cout << "[" << evmMemory[_counter] << "]";
    //     std::cout << std::endl;

    //     std::cout << "Overriding evmMemory" << std::endl;
    //     sc_memoryLoad(evmMemoryDataPath, evmMemory);

    //     std::cout << "Printing evmMemory" << std::endl;
    //     for (int _counter = 0; _counter < 100; _counter++)
    //         std::cout << "[" << evmMemory[_counter] << "]";

    //     std::cout << std::endl << std::endl;
    // }
    // catch(const std::exception& e)
    // {
    //     std::cerr << e.what() << '\n';
    // }

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
