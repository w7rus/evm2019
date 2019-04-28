#include <cstdlib>
#include <cstdio>
#include <new>
#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>
#include <limits.h>
#include <algorithm>
#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>
#include <cstring>
#include <fstream>
#include "helperFuncs.h"

using namespace RegistryStatusEnumFlags;
using namespace TerminalColorsEnums;

int opcounter = 0;

void signalhandler(int signum) {
    opcounter++;
}

int main(int argc, char const *argv[]) {
    bool evmScriptMode = false;

    for (int i = 0; i < argc; ++i) {
        if (strcmp(argv[i], "-runscript") == 0) {
            evmScriptMode = true;
        }
    }

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

    struct itimerval nval, oval;

    nval.it_value.tv_sec = 2;
    nval.it_value.tv_usec = 0;
    nval.it_interval.tv_sec = 0;
    nval.it_interval.tv_usec = 0;

    std::ifstream scriptFile;

    if (evmScriptMode) {
        scriptFile.open("./content/script.txt");
        signal(SIGALRM, signalhandler);
        setitimer(ITIMER_REAL, &nval, NULL);
    }

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
        std::cout << "OP. COUNTER: " << opcounter;
        mt_setCurPos(*panelPosX_STATUS, *panelPosY_STATUS + 3);
        std::cout << "FLAGS: A B C D E F G H";
        mt_setCurPos(*panelPosX_STATUS, *panelPosY_STATUS + 4);
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

        if (evmScriptMode) {
            bc_printBox(*panelPosX_STATUS - 30,*panelPosY_SELECTED + 16,60,5);
            std::cout << std::endl;
            mt_setCurPos(*panelPosX_STATUS - 18, *panelPosY_SELECTED + 17);
            std::cout << "EVM RUNNING IN SCRIPT EXECUTION MODE";
            mt_setCurPos(*panelPosX_STATUS - 10, *panelPosY_SELECTED + 18);
            std::cout << std::endl;
            pause();

            std::string scriptFileLine;
            std::getline(scriptFile, scriptFileLine);
            if (scriptFileLine.empty()) {
                mt_setCurPos(0, *panelPosY_SELECTED + 21);
                std::cout << "End of script file! Press any key to continue..." << std::endl;
                std::cin.get();
                evmScriptMode = false;
            } else {
                ALU(scriptFileLine, evmMemory, &evmMemoryOffset, &accumulator, &resValOverflow, &resValEven);
            }
            setitimer(ITIMER_REAL, &nval, NULL);
        } else {
            std::cout << std::endl << "> ";
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
                    ALU(str_sequence, evmMemory, &evmMemoryOffset, &accumulator, &resValOverflow, &resValEven);
                }
            }

            if (keyEscape[2] == '1' && keyEscape[3] == '7') {
                std::cout << "Key [F6]" << std::endl;
                std::cin.get();
            }
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
