#include <cstdlib>
#include <cstdio>
#include <new>
#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>
#include "helperFuncs.h"

using namespace RegistryStatusEnumFlags;
using namespace TerminalColorsEnums;

int main(int argc, char const *argv[]) {
    register int accumulator = 0;
    int evmMemoryOffset = 0;
    int evmFlagOffset = 1;
    int* evmMemory = sc_memoryInit();
    char evmMemoryDataPath[256] = "./content/db/memoryData.dat";
    int* evmFlag = sc_flagInit();

    std::string panelTitle_MEMORY("MEMORY");
    std::string panelTitle_STATUS("STATUS");

    while (1) {
        int* conWidth = new int;
        int* conHeight = new int;
        mt_getScreenSize(conWidth, conHeight);
        int* curPosX = new int;
        int* curPosY = new int;
        mt_getCurPos(curPosX, curPosY);

        int* panelPosX_MEMORY = new int;
        int* panelPosY_MEMORY = new int;
        int* panelPosX_STATUS = new int;
        int* panelPosY_STATUS = new int;

        mt_clrscr();

        mt_getCurPos(panelPosX_MEMORY, panelPosY_MEMORY);
        std::cout << std::setfill('=') << "[" << panelTitle_MEMORY << "]" << std::setw(*conWidth * .5 - 2 - panelTitle_MEMORY.size()) << "" << std::setfill(' ');

        mt_getCurPos(panelPosX_STATUS, panelPosY_STATUS);
        std::cout << std::setfill('=') << "[" << panelTitle_STATUS << "]" << std::setw(*conWidth * .5 - 2 - panelTitle_MEMORY.size()) << "" << std::setfill(' ');

        mt_setCurPos(*panelPosX_MEMORY, *panelPosY_MEMORY + 1);
        for (int _counter = 0; _counter < 100; _counter++) {
            if (_counter == evmMemoryOffset) {
                mt_setCurFgColor(TerminalColorsEnums::RED, true);
                std::cout << "[" << evmMemory[_counter] << "]";
                std::cout << "\033[0m";
            } else {
                std::cout << "[" << evmMemory[_counter] << "]";
            }
            if ((_counter + 1) % (int)(*conWidth * .5 / 3) == 0)
                std::cout << std::endl;
        }

        mt_setCurPos(*panelPosX_STATUS, *panelPosY_STATUS + 1);
        std::cout << "ACCUMULATOR: " << accumulator;

        mt_setCurPos(*panelPosX_STATUS, *panelPosY_STATUS + 2);
        std::cout << "FLAGS: A B C D E F G H";
        mt_setCurPos(*panelPosX_STATUS, *panelPosY_STATUS + 3);
        std::cout << "       "  << sc_flagGet(evmFlag, RegistryStatusEnumFlags::Alpha) << " "
                                << sc_flagGet(evmFlag, RegistryStatusEnumFlags::Bravo) << " "
                                << sc_flagGet(evmFlag, RegistryStatusEnumFlags::Charlie) << " "
                                << sc_flagGet(evmFlag, RegistryStatusEnumFlags::Delta) << " "
                                << sc_flagGet(evmFlag, RegistryStatusEnumFlags::Echo) << " "
                                << sc_flagGet(evmFlag, RegistryStatusEnumFlags::Foxtrot) << " "
                                << sc_flagGet(evmFlag, RegistryStatusEnumFlags::Golf) << " "
                                << sc_flagGet(evmFlag, RegistryStatusEnumFlags::Hotel) << " ";

        mt_setCurPos(*panelPosX_MEMORY, *panelPosY_MEMORY + 1 + ceil(100 / (*conWidth * .5 / 3)));
        std::cout << std::setfill('=') << std::setw(*conWidth) << "" << std::setfill(' ') << std::endl;
        std::cout << std::left << std::setw(20) << "[W] Offset++" << std::setw(20) << "[S] Offset--" << std::setw(20) << "[A] Action 3" << std::setw(20) << "[D] Action 4" << std::setw(20) << "[E] Action 5" << std::setw(20) << "[0] Exit" << std::endl;
        char keyPressed = std::cin.get();
        if (keyPressed == '0') break;
        if (keyPressed == 'a') {
            if (evmFlagOffset > 1) {
                evmFlagOffset--;
            }
        }
        if (keyPressed == 'w') {
            if (evmMemoryOffset < 99) {
                evmMemoryOffset++;
            }
        }
        if (keyPressed == 'd') {
            if (evmFlagOffset < 7) {
                evmFlagOffset++;
            }

        }
        if (keyPressed == 's') {
            if (evmMemoryOffset > 0) {
                evmMemoryOffset--;
            }
        }
        if (keyPressed == 'e') {
            if (sc_flagGet(evmFlag, static_cast<enumFlag>(evmFlagOffset))) {
                sc_flagSet(evmFlag, static_cast<enumFlag>(evmFlagOffset), false);
            } else {
                sc_flagSet(evmFlag, static_cast<enumFlag>(evmFlagOffset), true);
            }
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
