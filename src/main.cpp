#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <limits.h>
#include <algorithm>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>
#include <cstring>
#include <cmath>
#include "helperFuncs.h"

using namespace RegistryStatusEnumFlags;
using namespace TerminalColorsEnums;

int opcounter = 0;

void signalhandler(int signum) {
    opcounter++;
}

int main(int argc, char const *argv[]) {
    bool evmScriptMode = false;
    int evmScriptType = 0;
    register int accumulator = 0;
    int evmMemoryOffset = 0;
    int* evmMemory = sc_memoryInit();
    std::string evmMemoryDataPath = "./content/db/memoryData.dat";
    std::string evmBigCharDataPath = "./content/db/bcData.dat";
    std::string evmRawScriptFilePath = "./content/script.rsf";
    std::string evmBinaryScriptFilePath = "./content/script.bsf";
    int* evmFlag = sc_flagInit();

    for (int i = 1; i < argc; ++i) {
        std::string _argv(argv[i], strlen(argv[i]));

        std::size_t argv_split = _argv.find('=');

        std::string argv_param;
        std::string argv_value;

        std::cout << argv_split << std::endl;

        if (argv_split == std::string::npos) {
            argv_param = _argv;
        } else {
            argv_param = _argv.substr (0, argv_split);
            argv_value = _argv.substr (++argv_split);
        }

        if (argv_param.compare("-rsf") == 0) {
            evmScriptMode = true;
            evmScriptType = 0;
            if (!argv_value.empty())
                evmRawScriptFilePath = argv_value;
        }

        if (argv_param.compare("-bsf") == 0) {
            evmScriptMode = true;
            evmScriptType = 1;
            if (!argv_value.empty())
                evmBinaryScriptFilePath = argv_value;
        }

        if (argv_param.compare("-bcf") == 0) {
            if (!argv_value.empty())
                evmBigCharDataPath = argv_value;
        }

        if (argv_param.compare("-mf") == 0) {
            if (!argv_value.empty())
                evmMemoryDataPath = argv_value;
        }
    }

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
    FILE * scriptFileBinary;

    if (evmScriptMode) {
        if (!evmScriptType) {
            scriptFile.open(evmRawScriptFilePath);
        } else {
            scriptFileBinary = fopen(evmBinaryScriptFilePath.c_str(), "rb");
        }
        
        signal(SIGALRM, signalhandler);
        setitimer(ITIMER_REAL, &nval, NULL);
    }

    while (1) {
        if (sc_flagGet(*evmFlag, enumFlag::Alpha)) {
            return 0;
        }

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
            mt_setCurPos(*panelPosX_STATUS - 14, *panelPosY_SELECTED + 18);
            std::cout << "USERINPUT IS NOT REGISTERED";
            std::cout << std::endl;
            pause();

            if (!evmScriptType) {
                std::string scriptFileLine;
                std::getline(scriptFile, scriptFileLine);
                if (scriptFileLine.empty()) {
                    mt_setCurPos(0, *panelPosY_SELECTED + 21);
                    std::cout << "End of script file! Press any key to continue..." << std::endl;
                    std::cin.get();
                    evmScriptMode = false;
                    opcounter--;
                    continue;
                }

                int alures = ALU(scriptFileLine, evmMemory, &evmMemoryOffset, &accumulator, &resValOverflow, &resValEven, evmScriptMode);

                if (alures == -1) {
                    evmScriptMode = false;
                    opcounter--;
                } else if (alures == 1) {
                    sc_flagSet(evmFlag, enumFlag::Alpha, true);
                }

                if (resValEven) {
                    sc_flagSet(evmFlag, enumFlag::Bravo, true);
                } else {
                    sc_flagSet(evmFlag, enumFlag::Bravo, false);
                }

                if (resValOverflow) {
                    sc_flagSet(evmFlag, enumFlag::Charlie, true);
                } else {
                    sc_flagSet(evmFlag, enumFlag::Charlie, false);
                }
            } else {
                int value = 0;
                fread(&value, sizeof(int), 1, scriptFileBinary);

                int command = 0;
                int offset = 0;

                sc_commandDecode(value, &command, &offset);
                int alures = ALUB(command, offset, evmMemory, &evmMemoryOffset, &accumulator, &resValOverflow, &resValEven, evmScriptMode);

                if (alures == -1) {
                    evmScriptMode = false;
                    opcounter--;
                } else if (alures == 1) {
                    sc_flagSet(evmFlag, enumFlag::Alpha, true);
                }

                if (resValEven) {
                    sc_flagSet(evmFlag, enumFlag::Bravo, true);
                } else {
                    sc_flagSet(evmFlag, enumFlag::Bravo, false);
                }

                if (resValOverflow) {
                    sc_flagSet(evmFlag, enumFlag::Charlie, true);
                } else {
                    sc_flagSet(evmFlag, enumFlag::Charlie, false);
                }

                if (feof(scriptFileBinary)) {
                    mt_setCurPos(0, *panelPosY_SELECTED + 21);
                    std::cout << "End of script file! Press any key to continue..." << std::endl;
                    std::cin.get();
                    evmScriptMode = false;
                    opcounter--;
                    continue;
                }
            }
            setitimer(ITIMER_REAL, &nval, NULL);
        } else {
            std::cout << std::endl << "> ";
            int keyPressed;
            char * keyEscape = new char[6];
            rk_readkey(&keyPressed, keyEscape);

            if (keyPressed == '0') break;

            if (keyPressed == 'w') {
                bc_writeBigString(evmMemorySelected, evmBigCharDataPath.c_str());
            }
            if (keyPressed == 'r') {
                bc_readBigString(evmBigCharDataPath.c_str());
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
                    int alures = ALU(str_sequence, evmMemory, &evmMemoryOffset, &accumulator, &resValOverflow, &resValEven, evmScriptMode);
                    
                    if (alures == -1) {
                        evmScriptMode = false;
                    } else if (alures == 1) {
                        sc_flagSet(evmFlag, enumFlag::Alpha, true);
                    }

                    if (resValEven) {
                        sc_flagSet(evmFlag, enumFlag::Bravo, true);
                    } else {
                        sc_flagSet(evmFlag, enumFlag::Bravo, false);
                    }

                    if (resValOverflow) {
                        sc_flagSet(evmFlag, enumFlag::Charlie, true);
                    } else {
                        sc_flagSet(evmFlag, enumFlag::Charlie, false);
                    }
                }
            }

            if (keyEscape[2] == '1' && keyEscape[3] == '7') {
                std::string str_filepath_in;
                std::cout << "Enter path to RSF: ";
                std::getline(std::cin, str_filepath_in);

                std::string str_filepath_out;
                std::cout << "Enter path to BSF: ";
                std::getline(std::cin, str_filepath_out);

                if (str_filepath_in.empty() || str_filepath_out.empty()) {
                    std::cout << "Unresolved path!" << std::endl;
                } else {
                    std::ifstream file_in;
                    file_in.open(str_filepath_in);

                    FILE * file_out = fopen(str_filepath_out.c_str(), "w+b");

                    std::string str_fileline_in;

                    for(;;)
                    {
                        std::getline(file_in, str_fileline_in);

                        if (str_fileline_in.empty())
                        {
                            std::cout << "Done (RSF -> BSF)!" << std::endl;
                            break;
                        } else {

                            int command = 0;
                            int offset = 0;

                            std::stringstream ss;
                            ss << str_fileline_in;

                            std::string commandAlias;

                            ss >> commandAlias;

                            {
                                if (commandAlias.compare("READ") == 0)
                                    command = 10;
                                if (commandAlias.compare("WRITE") == 0)
                                    command = 11;

                                if (commandAlias.compare("LOAD") == 0)
                                    command = 20;
                                if (commandAlias.compare("SAVE") == 0)
                                    command = 21;
                                    
                                if (commandAlias.compare("ADD") == 0)
                                    command = 30;
                                if (commandAlias.compare("SUB") == 0)
                                    command = 31;
                                if (commandAlias.compare("MUL") == 0)
                                    command = 32;
                                if (commandAlias.compare("DIV") == 0)
                                    command = 33;

                                if (commandAlias.compare("JUMP") == 0)
                                    command = 40;
                                if (commandAlias.compare("JNEG") == 0)
                                    command = 41;
                                if (commandAlias.compare("JZ") == 0)
                                    command = 42;
                                if (commandAlias.compare("HALT") == 0)
                                    command = 43;

                                if (commandAlias.compare("NOT") == 0)
                                    command = 50;
                                if (commandAlias.compare("AND") == 0)
                                    command = 51;
                                if (commandAlias.compare("OR") == 0)
                                    command = 52;
                                if (commandAlias.compare("XOR") == 0)
                                    command = 53;
                                if (commandAlias.compare("JNS") == 0)
                                    command = 54;
                                if (commandAlias.compare("JC") == 0)
                                    command = 56;
                                if (commandAlias.compare("JNC") == 0)
                                    command = 57;
                                if (commandAlias.compare("JP") == 0)
                                    command = 58;
                                if (commandAlias.compare("JNP") == 0)
                                    command = 59;

                                if (commandAlias.compare("CHL") == 0)
                                    command = 60;
                                if (commandAlias.compare("SHR") == 0)
                                    command = 61;
                                if (commandAlias.compare("RCL") == 0)
                                    command = 62;
                                if (commandAlias.compare("RCR") == 0)
                                    command = 63;
                                if (commandAlias.compare("ADDC") == 0)
                                    command = 65;
                                if (commandAlias.compare("SUBC") == 0)
                                    command = 66;
                                if (commandAlias.compare("LOGLC") == 0)
                                    command = 67;
                                if (commandAlias.compare("LOGRC") == 0)
                                    command = 68;
                                if (commandAlias.compare("RCCL") == 0)
                                    command = 69;
                                if (commandAlias.compare("RCCR") == 0)
                                    command = 70;

                                if (commandAlias.compare("MOVA") == 0)
                                    command = 71;
                                if (commandAlias.compare("MOVR") == 0)
                                    command = 72;
                                if (commandAlias.compare("MOVCA") == 0)
                                    command = 73;
                                if (commandAlias.compare("MOVCR") == 0)
                                    command = 74;

                                if (commandAlias.compare("MADDC") == 0)
                                    command = 75;
                                if (commandAlias.compare("MSUBC") == 0)
                                    command = 76;
                            }

                            ss >> offset;

                            int value = 0;
                            sc_commandEncode(command, offset, &value);

                            fwrite(&value, sizeof(int), 1, file_out);

                            std::cout << str_fileline_in << std::endl;
                            std::cout << command << std::endl;
                            std::cout << offset << std::endl;
                            std::cout << value << std::endl;
                            std::cin.get();
                            std::cin.get();
                        }
                    }

                    fflush(file_out);
                    fclose(file_out);
                    file_in.close();
                }
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

    return 0;
}
