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
    bool            evmScriptMode           = false;                            // Whether EVM runs through EVM MEMORY
    register int    accumulator             = 0;                                // EVM ACCUMULATOR
    int             evmMemoryOffset         = 0;                                // EVM MEMORY OFFSET
    std::string     evmMemoryFilePath;                                          // Path to EVM MEMORY FILE
    std::string     evmBigCharFilePath;                                         // Path to EVM BIGCHAR FILE
    std::string     evmSimpleAssemblyScriptFilePath;                            // Path to SIMPLE ASSEMBLY SCRIPT FILE
    std::string     evmSimpleBasicScriptFilePath;                               // Path to SIMPLE BASIC SCRIPT FILE


    int*            evmMemory               = sc_memoryInit();                  // Initialize EVM MEMORY
    int*            evmFlag                 = sc_flagInit();                    // Initialize EVM FLAGS

    for (int i = 1; i < argc; ++i) {                                            // Handle launch parameters
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

        if (argv_param.compare("-sasf") == 0) {
            if (!argv_value.empty())
                evmSimpleAssemblyScriptFilePath = argv_value;
        }

        if (argv_param.compare("-sbsf") == 0) {
            if (!argv_value.empty())
                evmSimpleBasicScriptFilePath = argv_value;
        }

        if (argv_param.compare("-bcf") == 0) {
            if (!argv_value.empty())
                evmBigCharFilePath = argv_value;
        }

        if (argv_param.compare("-mf") == 0) {
            if (!argv_value.empty())
                evmMemoryFilePath = argv_value;
        }
    }

    bool evmRVO = false;                                                        // Whether value during last operation goes out of range
    bool evmRVE = false;                                                        // Whether value during last operation becomes even

    struct EVM_LABELS
    {
        std::string label_panelTitle_EVMMEMORY = "[EVM MEMORY]";
        std::string label_panelTitle_EVMSTATUS = "[EVM STATUS]";
        std::string label_panelTitle_EVMSELECTEDCELL = "[EVM MEMORY SELECTED CELL]";

        std::string label_error_evmMemoryOffsetException = "/!\\ EVM OFFSET is out of range!";
        std::string label_error_evmPathException = "/!\\ Empty paths or longer than 256 bytes is not allowed!";

        std::string label_warn_evmOperationException = "/!\\ EVM ALU operation result value is out of range!";
        std::string label_warn_evmSequenceException = "/i\\ Empty sequence! Aborting...";
        std::string label_warn_evmFileException = "/i\\ Can't open file! Aborting...";

        std::string label_notice_evmScriptMode1 = "EVM EXECUTING BINARY MEMORY DATA";
        std::string label_notice_evmScriptMode2 = "KEYHANDLING IGNORED";

        std::string label_EVMACCUMULATOR = "REGISTRY";
        std::string label_EVMOPERATIONCOUNTER = "OP. COUNTER";
        std::string label_EVMFLAGS = "FLAGS";

        std::string label_paktc = "Press any key to continue...";
    } EVM_LABELS;

    struct itimerval nval;                                                      // Create timer parameters structure

    nval.it_value.tv_sec = 1;
    nval.it_value.tv_usec = 0;
    nval.it_interval.tv_sec = 0;
    nval.it_interval.tv_usec = 0;

    std::ifstream scriptFile;
    FILE * scriptFileBinary;

    signal(SIGALRM, signalhandler);

    struct EVM_TERM
    {
        size_t      viewportWidth;
        size_t      viewportHeight;
        size_t      cursorPositionX;
        size_t      cursorPositionY;

        size_t      panel_EVMMEMORY_PositionX;
        size_t      panel_EVMMEMORY_PositionY;
        size_t      panel_EVMSTATUS_PositionX;
        size_t      panel_EVMSTATUS_PositionY;
        size_t      panel_EVMSELECTEDCELL_PositionX;
        size_t      panel_EVMSELECTEDCELL_PositionY;
        size_t      panel_EVMINPUT_PositionX;
        size_t      panel_EVMINPUT_PositionY;
        size_t      panel_EVMNOTICE_PositionX;
        size_t      panel_EVMNOTICE_PositionY;
    } EVM_TERM;

    while (1) {
        if (sc_flagGet(*evmFlag, enumFlag::Alpha))                              // Shutdown EVM on error
        {
            std::cout << EVM_LABELS.label_error_evmMemoryOffsetException << std::endl;
            return -1;
        }

        if (sc_flagGet(*evmFlag, enumFlag::Charlie))                            // Shutdown EVM on error
        {
            std::cout << EVM_LABELS.label_warn_evmOperationException << std::endl;
        }

        mt_getScreenSize(&EVM_TERM.viewportWidth, &EVM_TERM.viewportHeight);    // Get and store terminal viewport dimensions
        mt_clrscr();                                                            // Clear terminal

        EVM_TERM.cursorPositionX = 1;
        EVM_TERM.cursorPositionY = 1;

        EVM_TERM.panel_EVMMEMORY_PositionX = 1;
        EVM_TERM.panel_EVMMEMORY_PositionY = 1;

        EVM_TERM.panel_EVMSTATUS_PositionX = (int)ceil(EVM_TERM.viewportWidth * .5) - 1;
        EVM_TERM.panel_EVMSTATUS_PositionY = 1;

        EVM_TERM.panel_EVMSELECTEDCELL_PositionX = 1;
        EVM_TERM.panel_EVMSELECTEDCELL_PositionY = 1 + 2 + (int)ceil(100 / ((ceil(EVM_TERM.viewportWidth * .5) - 1) / 6));

        EVM_TERM.panel_EVMINPUT_PositionX = 1;
        EVM_TERM.panel_EVMINPUT_PositionY = EVM_TERM.panel_EVMSELECTEDCELL_PositionY + 11;

        EVM_TERM.panel_EVMNOTICE_PositionX = EVM_TERM.panel_EVMSTATUS_PositionX - std::max(EVM_LABELS.label_notice_evmScriptMode1.size(), EVM_LABELS.label_notice_evmScriptMode2.size()) - 2;
        EVM_TERM.panel_EVMNOTICE_PositionY = EVM_TERM.panel_EVMSELECTEDCELL_PositionY + 11;

        mt_setCurPos(
            EVM_TERM.panel_EVMMEMORY_PositionX,
            EVM_TERM.panel_EVMMEMORY_PositionY
            );

        std::cout << 
            std::setfill('=') << 
            EVM_LABELS.label_panelTitle_EVMMEMORY << 
            std::setw(EVM_TERM.panel_EVMSTATUS_PositionX - EVM_LABELS.label_panelTitle_EVMMEMORY.size()) << 
            "" << 
            std::setfill(' ');

        mt_setCurPos(
            EVM_TERM.panel_EVMSTATUS_PositionX,
            EVM_TERM.panel_EVMSTATUS_PositionY
            );

        std::cout << 
            std::setfill('=') << 
            EVM_LABELS.label_panelTitle_EVMSTATUS << 
            std::setw(EVM_TERM.viewportWidth - EVM_TERM.panel_EVMSTATUS_PositionX - EVM_LABELS.label_panelTitle_EVMSTATUS.size()) << 
            "" << 
            std::setfill(' ');

        mt_setCurPos(
            EVM_TERM.panel_EVMMEMORY_PositionX,
            EVM_TERM.panel_EVMMEMORY_PositionY + 1
            );

        for (int i = 0; i < 100; i++)
        {
            if (i == evmMemoryOffset)
            {
                mt_setCurFgColor(TerminalColorsEnums::RED, true);
                std::cout << "[" << std::setw(4) << std::right << std::hex << evmMemory[i] << std::dec << "]";
                std::cout << "\033[0m";
            }
            else
            {
                std::cout << "[" << std::setw(4) << std::right << std::hex << evmMemory[i] << std::dec << "]";
            }
            if ((i + 1) % (size_t)((ceil(EVM_TERM.viewportWidth * .5) - 1) / 6) == 0)
                std::cout << std::endl;
        }

        mt_setCurPos(
            EVM_TERM.panel_EVMSTATUS_PositionX,
            EVM_TERM.panel_EVMSTATUS_PositionY + 1
            );
        std::cout << EVM_LABELS.label_EVMACCUMULATOR << ": " << accumulator;
        mt_setCurPos(
            EVM_TERM.panel_EVMSTATUS_PositionX,
            EVM_TERM.panel_EVMSTATUS_PositionY + 2
            );
        std::cout << EVM_LABELS.label_EVMOPERATIONCOUNTER << ": " << opcounter;
        mt_setCurPos(
            EVM_TERM.panel_EVMSTATUS_PositionX,
            EVM_TERM.panel_EVMSTATUS_PositionY + 3
            );
        std::cout << EVM_LABELS.label_EVMFLAGS << ": A B C D E F G H";
        mt_setCurPos(
            EVM_TERM.panel_EVMSTATUS_PositionX,
            EVM_TERM.panel_EVMSTATUS_PositionY + 4
            );
        std::cout << std::setw(EVM_LABELS.label_EVMFLAGS.size() + 3) << 
            sc_flagGet(*evmFlag, RegistryStatusEnumFlags::Alpha) << " " <<
            sc_flagGet(*evmFlag, RegistryStatusEnumFlags::Bravo) << " " <<
            sc_flagGet(*evmFlag, RegistryStatusEnumFlags::Charlie) << " " <<
            sc_flagGet(*evmFlag, RegistryStatusEnumFlags::Delta) << " " <<
            sc_flagGet(*evmFlag, RegistryStatusEnumFlags::Echo) << " " <<
            sc_flagGet(*evmFlag, RegistryStatusEnumFlags::Foxtrot) << " " <<
            sc_flagGet(*evmFlag, RegistryStatusEnumFlags::Golf) << " " <<
            sc_flagGet(*evmFlag, RegistryStatusEnumFlags::Hotel) << " ";

        mt_setCurPos(
            EVM_TERM.panel_EVMSELECTEDCELL_PositionX,
            EVM_TERM.panel_EVMSELECTEDCELL_PositionY
            );
        std::cout << 
            std::setfill('=') << 
            std::setw(EVM_TERM.viewportWidth) << 
            std::left << EVM_LABELS.label_panelTitle_EVMSELECTEDCELL << 
            std::setfill(' ');

        std::string evmMemorySelected = std::to_string(evmMemory[evmMemoryOffset]);

        bc_printBigString(evmMemorySelected, EVM_TERM.panel_EVMSELECTEDCELL_PositionX, EVM_TERM.panel_EVMSELECTEDCELL_PositionY + 1);

        mt_setCurPos(
            EVM_TERM.panel_EVMSELECTEDCELL_PositionX,
            EVM_TERM.panel_EVMSELECTEDCELL_PositionY + 9
            );
        std::cout << std::setfill('=') << std::setw(EVM_TERM.viewportWidth) << "" << std::setfill(' ') << std::endl;

        std::cout <<
            std::left <<
            std::setw(20) <<
            "[-] Prev Cell" <<
            std::setw(20) <<
            "[+] Next Cell" <<
            std::setw(20) <<
            "[s] Save BSF" <<
            std::setw(20) <<
            "[l] Load BSF" <<
            std::setw(20) <<
            "[w] Write BC" <<
            std::setw(20) <<
            "[r] Read BC" <<
            std::setw(20) <<
            "[f5] Set Func" <<
            std::setw(20) <<
            "[f6] Run BSF" <<
            std::setw(20) <<
            "[f7] Compile SBSF" <<
            std::setw(20) <<
            "[f8] Compile SASF" <<
            std::setw(20) <<
            "[q] Reset EVM" <<
            std::setw(20) <<
            "[0] Exit EVM";

        if (evmScriptMode) {
            bc_printBox(
                EVM_TERM.panel_EVMNOTICE_PositionX + ceil(std::max(EVM_LABELS.label_notice_evmScriptMode1.size(), EVM_LABELS.label_notice_evmScriptMode2.size()) / 2),
                EVM_TERM.panel_EVMNOTICE_PositionY,
                std::max(EVM_LABELS.label_notice_evmScriptMode1.size(), EVM_LABELS.label_notice_evmScriptMode2.size()) + 4,
                5
                );
            std::cout << std::endl;                                             // Fix box printing

            mt_setCurPos(
                EVM_TERM.panel_EVMSTATUS_PositionX - ceil(EVM_LABELS.label_notice_evmScriptMode1.size() / 2),
                EVM_TERM.panel_EVMNOTICE_PositionY + 2
                );
            std::cout << EVM_LABELS.label_notice_evmScriptMode1;
            mt_setCurPos(
                EVM_TERM.panel_EVMSTATUS_PositionX - ceil(EVM_LABELS.label_notice_evmScriptMode2.size() / 2),
                EVM_TERM.panel_EVMNOTICE_PositionY + 3
                );
            std::cout << EVM_LABELS.label_notice_evmScriptMode2;
            std::cout << std::endl;

            pause();                                                            // Break on SIGALRM

            /* ALU HERE */
            evmMemoryOffset = opcounter;

            int cmd = 0;
            int val = 0;
            int bsequence = evmMemory[evmMemoryOffset];

            sc_commandDecode(bsequence, &cmd, &val);

            int ALU_RETURN = ALU(cmd, val, evmMemory, evmFlag, &accumulator, &evmMemoryOffset);

            switch (ALU_RETURN)
            {
            case 1:
                std::cout << EVM_LABELS.label_paktc;
                std::cin.get();
                break;

            case -1:
                sc_flagSet(evmFlag, enumFlag::Alpha, true);
                break;
            
            default:
                break;
            }

            if (evmMemoryOffset >= 99)
            {
                evmScriptMode = false;
                evmMemoryOffset = 0;
            }
            else
            {
                setitimer(ITIMER_REAL, &nval, NULL);
            }
        } else {
            mt_setCurPos(
                EVM_TERM.panel_EVMINPUT_PositionX,
                EVM_TERM.panel_EVMINPUT_PositionY + 9
                );
            std::cout << 
                "> ";
            
            int keyPressed;
            char * keyEscape = new char[6];
            rk_readkey(&keyPressed, keyEscape);

            switch (keyPressed)
            {
            case '0':
                {
                    std::cout << "Goodbye, cutie :3" << std::endl;
                    return 0;
                }
                break;

            case 'w':
                {
                    std::string path_in;
                    std::cout << "BCS file (Default: " << evmBigCharFilePath << "): ";
                    std::getline(std::cin, path_in);

                    if (!evmBigCharFilePath.empty())
                        path_in = evmBigCharFilePath;

                    if (evmBigCharFilePath.size() > 256 || evmBigCharFilePath.empty())
                    {
                        std::cout << EVM_LABELS.label_error_evmPathException << std::endl;
                        std::cout << EVM_LABELS.label_paktc;
                        std::cin.get();

                        continue;
                    }
                    bc_writeBigString(evmMemorySelected, evmBigCharFilePath);
                    std::cout << "Wrote Big Char String to file " << evmBigCharFilePath << std::endl;
                    std::cout << EVM_LABELS.label_paktc;
                    std::cin.get();
                }
                break;

            case 'r':
                {
                    std::string path_in;
                    std::cout << "BCS file (Default: " << evmBigCharFilePath << "): ";
                    std::getline(std::cin, path_in);

                    if (!evmBigCharFilePath.empty())
                        path_in = evmBigCharFilePath;

                    if (evmBigCharFilePath.size() > 256 || evmBigCharFilePath.empty())
                    {
                        std::cout << EVM_LABELS.label_error_evmPathException << std::endl;
                        std::cout << EVM_LABELS.label_paktc;
                        std::cin.get();

                        continue;
                    }
                    bc_readBigString(evmBigCharFilePath);
                    std::cout << "Read Big Char String from file " << evmBigCharFilePath << std::endl;
                    std::cout << EVM_LABELS.label_paktc;
                    std::cin.get();
                }
                break;

            case 'l':
                {
                    std::string path_in;
                    std::cout << "BSF file (Default: " << evmMemoryFilePath << "): ";
                    std::getline(std::cin, path_in);

                    if (!evmMemoryFilePath.empty())
                        path_in = evmMemoryFilePath;

                    if (evmMemoryFilePath.size() > 256 || evmMemoryFilePath.empty())
                    {
                        std::cout << EVM_LABELS.label_error_evmPathException << std::endl;
                        std::cout << EVM_LABELS.label_paktc;
                        std::cin.get();

                        continue;
                    }
                    sc_memoryLoad(evmMemoryFilePath, evmMemory);
                    std::cout << "Loaded Binary Script to EVM MEMORY from file " << evmBigCharFilePath << std::endl;
                    std::cout << EVM_LABELS.label_paktc;
                    std::cin.get();
                }
                break;

            case 's':
                {
                    std::string path_in;
                    std::cout << "BSF file (Default: " << evmMemoryFilePath << "): ";
                    std::getline(std::cin, path_in);

                    if (!evmMemoryFilePath.empty())
                        path_in = evmMemoryFilePath;

                    if (evmMemoryFilePath.size() > 256 || evmMemoryFilePath.empty())
                    {
                        std::cout << EVM_LABELS.label_error_evmPathException << std::endl;
                        std::cout << EVM_LABELS.label_paktc;
                        std::cin.get();

                        continue;
                    }
                    sc_memorySave(evmMemoryFilePath, evmMemory);
                    std::cout << "Saved EVM MEMORY as Binary Script to file " << evmBigCharFilePath << std::endl;
                    std::cout << EVM_LABELS.label_paktc;
                    std::cin.get();
                }
                break;

            case '-':
                {
                    if (evmMemoryOffset > 0)
                        evmMemoryOffset--;
                }
                break;

            case '+':
                {
                    if (evmMemoryOffset < 99)
                        evmMemoryOffset++;
                }
                break;

            case 'q':
                {
                    evmMemoryOffset = 0;
                    delete(evmMemory);
                    evmMemory = sc_memoryInit();
                    delete(evmFlag);
                    evmFlag = sc_flagInit();
                    opcounter = 0;
                    accumulator = 0;
                }
                break;
            
            default:
                break;
            }

            if (keyEscape[2] == '1' && keyEscape[3] == '5')                     // Key F5
            {
                std::string sequence;
                std::cout << "Enter sequence: ";
                std::getline(std::cin, sequence);

                if (sequence.empty())
                {
                    std::cout << EVM_LABELS.label_warn_evmSequenceException << std::endl;
                    std::cout << EVM_LABELS.label_paktc;
                    std::cin.get();
                    break;
                }

                int cmd = 0;
                int val = 0;
                std::stringstream ss;
                ss << sequence;

                std::string cmd_alias;
                ss >> cmd_alias;
                {
                    if (cmd_alias.compare("READ") == 0)
                        cmd = 10;
                    if (cmd_alias.compare("WRITE") == 0)
                        cmd = 11;

                    if (cmd_alias.compare("LOAD") == 0)
                        cmd = 20;
                    if (cmd_alias.compare("SAVE") == 0)
                        cmd = 21;
                        
                    if (cmd_alias.compare("ADD") == 0)
                        cmd = 30;
                    if (cmd_alias.compare("SUB") == 0)
                        cmd = 31;
                    if (cmd_alias.compare("MUL") == 0)
                        cmd = 32;
                    if (cmd_alias.compare("DIV") == 0)
                        cmd = 33;

                    if (cmd_alias.compare("JUMP") == 0)
                        cmd = 40;
                    if (cmd_alias.compare("JNEG") == 0)
                        cmd = 41;
                    if (cmd_alias.compare("JZ") == 0)
                        cmd = 42;
                    if (cmd_alias.compare("HALT") == 0)
                        cmd = 43;

                    if (cmd_alias.compare("NOT") == 0)
                        cmd = 50;
                    if (cmd_alias.compare("AND") == 0)
                        cmd = 51;
                    if (cmd_alias.compare("OR") == 0)
                        cmd = 52;
                    if (cmd_alias.compare("XOR") == 0)
                        cmd = 53;
                    if (cmd_alias.compare("JNS") == 0)
                        cmd = 54;
                    if (cmd_alias.compare("JC") == 0)
                        cmd = 56;
                    if (cmd_alias.compare("JNC") == 0)
                        cmd = 57;
                    if (cmd_alias.compare("JP") == 0)
                        cmd = 58;
                    if (cmd_alias.compare("JNP") == 0)
                        cmd = 59;

                    if (cmd_alias.compare("CHL") == 0)
                        cmd = 60;
                    if (cmd_alias.compare("SHR") == 0)
                        cmd = 61;
                    if (cmd_alias.compare("RCL") == 0)
                        cmd = 62;
                    if (cmd_alias.compare("RCR") == 0)
                        cmd = 63;
                    if (cmd_alias.compare("ADDC") == 0)
                        cmd = 65;
                    if (cmd_alias.compare("SUBC") == 0)
                        cmd = 66;
                    if (cmd_alias.compare("LOGLC") == 0)
                        cmd = 67;
                    if (cmd_alias.compare("LOGRC") == 0)
                        cmd = 68;
                    if (cmd_alias.compare("RCCL") == 0)
                        cmd = 69;
                    if (cmd_alias.compare("RCCR") == 0)
                        cmd = 70;

                    if (cmd_alias.compare("MOVA") == 0)
                        cmd = 71;
                    if (cmd_alias.compare("MOVR") == 0)
                        cmd = 72;
                    if (cmd_alias.compare("MOVCA") == 0)
                        cmd = 73;
                    if (cmd_alias.compare("MOVCR") == 0)
                        cmd = 74;

                    if (cmd_alias.compare("MADDC") == 0)
                        cmd = 75;
                    if (cmd_alias.compare("MSUBC") == 0)
                        cmd = 76;
                }
                ss >> val;

                int bsequence = 0;
                sc_commandEncode(cmd, val, &bsequence);
                evmMemory[evmMemoryOffset] = bsequence;
            }
            else if (keyEscape[2] == '1' && keyEscape[3] == '7')                // Key F6
            {
                setitimer(ITIMER_REAL, &nval, NULL);
                evmScriptMode = true;
                evmMemoryOffset = 0;
                opcounter = 0;
                continue;
            }
            else if (keyEscape[2] == '1' && keyEscape[3] == '8')                // Key F7
            {
                //COMPILE SIMPLEBASIC INTO SIMPLEASSEMBLY


            }
            else if (keyEscape[2] == '1' && keyEscape[3] == '9')                // Key F8
            {
                //COMPILE SIMPLEASSEMBLY INTO BINARYSCRIPTFILE

                std::string path_in;
                std::string path_out;

                std::cout << "SASF file (Default: " << evmSimpleAssemblyScriptFilePath << "): ";
                std::getline(std::cin, path_in);

                if (!evmSimpleAssemblyScriptFilePath.empty())
                    path_in = evmSimpleAssemblyScriptFilePath;

                if (path_in.size() > 256 || path_in.empty())
                {
                    std::cout << EVM_LABELS.label_error_evmPathException << std::endl;
                    std::cout << EVM_LABELS.label_paktc;
                    std::cin.get();

                    continue;
                }

                std::cout << "BSF file (Default: " << evmMemoryFilePath << "): ";
                std::getline(std::cin, path_out);

                if (!evmMemoryFilePath.empty())
                    path_out = evmMemoryFilePath;

                if (path_out.size() > 256 || path_out.empty())
                {
                    std::cout << EVM_LABELS.label_error_evmPathException << std::endl;
                    std::cout << EVM_LABELS.label_paktc;
                    std::cin.get();

                    continue;
                }

                std::ifstream file_in;
                file_in.open(path_in);
                size_t lineCount_file_in = 0;
                if (file_in.is_open())
                {
                    std::string tmpLine_file_in;

                    for(;std::getline(file_in, tmpLine_file_in);)
                    {
                        lineCount_file_in++;
                    }
                    file_in.close();
                    file_in.open(path_in);

                    int* buffer = new int[lineCount_file_in];

                    size_t bufferCount = 0;

                    for(;std::getline(file_in, tmpLine_file_in); bufferCount++)
                    {
                        int cmd = 0;
                        int val = 0;

                        std::stringstream ss;
                        ss << tmpLine_file_in;

                        std::string cmd_alias;
                        ss >> cmd_alias;
                        {
                            if (cmd_alias.compare("READ") == 0)
                                cmd = 10;
                            if (cmd_alias.compare("WRITE") == 0)
                                cmd = 11;

                            if (cmd_alias.compare("LOAD") == 0)
                                cmd = 20;
                            if (cmd_alias.compare("SAVE") == 0)
                                cmd = 21;
                                
                            if (cmd_alias.compare("ADD") == 0)
                                cmd = 30;
                            if (cmd_alias.compare("SUB") == 0)
                                cmd = 31;
                            if (cmd_alias.compare("MUL") == 0)
                                cmd = 32;
                            if (cmd_alias.compare("DIV") == 0)
                                cmd = 33;

                            if (cmd_alias.compare("JUMP") == 0)
                                cmd = 40;
                            if (cmd_alias.compare("JNEG") == 0)
                                cmd = 41;
                            if (cmd_alias.compare("JZ") == 0)
                                cmd = 42;
                            if (cmd_alias.compare("HALT") == 0)
                                cmd = 43;

                            if (cmd_alias.compare("NOT") == 0)
                                cmd = 50;
                            if (cmd_alias.compare("AND") == 0)
                                cmd = 51;
                            if (cmd_alias.compare("OR") == 0)
                                cmd = 52;
                            if (cmd_alias.compare("XOR") == 0)
                                cmd = 53;
                            if (cmd_alias.compare("JNS") == 0)
                                cmd = 54;
                            if (cmd_alias.compare("JC") == 0)
                                cmd = 56;
                            if (cmd_alias.compare("JNC") == 0)
                                cmd = 57;
                            if (cmd_alias.compare("JP") == 0)
                                cmd = 58;
                            if (cmd_alias.compare("JNP") == 0)
                                cmd = 59;

                            if (cmd_alias.compare("CHL") == 0)
                                cmd = 60;
                            if (cmd_alias.compare("SHR") == 0)
                                cmd = 61;
                            if (cmd_alias.compare("RCL") == 0)
                                cmd = 62;
                            if (cmd_alias.compare("RCR") == 0)
                                cmd = 63;
                            if (cmd_alias.compare("ADDC") == 0)
                                cmd = 65;
                            if (cmd_alias.compare("SUBC") == 0)
                                cmd = 66;
                            if (cmd_alias.compare("LOGLC") == 0)
                                cmd = 67;
                            if (cmd_alias.compare("LOGRC") == 0)
                                cmd = 68;
                            if (cmd_alias.compare("RCCL") == 0)
                                cmd = 69;
                            if (cmd_alias.compare("RCCR") == 0)
                                cmd = 70;

                            if (cmd_alias.compare("MOVA") == 0)
                                cmd = 71;
                            if (cmd_alias.compare("MOVR") == 0)
                                cmd = 72;
                            if (cmd_alias.compare("MOVCA") == 0)
                                cmd = 73;
                            if (cmd_alias.compare("MOVCR") == 0)
                                cmd = 74;

                            if (cmd_alias.compare("MADDC") == 0)
                                cmd = 75;
                            if (cmd_alias.compare("MSUBC") == 0)
                                cmd = 76;
                        }
                        ss >> val;

                        int bsequence = 0;
                        sc_commandEncode(cmd, val, &bsequence);
                        buffer[bufferCount] = bsequence;
                    }

                    // std::ofstream file_out;
                    // file_out.open(path_out);

                    // if (file_out.is_open())
                    // {
                    //     file_out.write(reinterpret_cast<const char *>(buffer), sizeof(int) * lineCount_file_in);
                    // }
                    // else
                    // {
                    //     std::cout << EVM_LABELS.label_warn_evmFileException << std::endl;
                    //     std::cout << EVM_LABELS.label_paktc;
                    //     std::cin.get();
                    //     continue;
                    // }

                    // delete[] buffer;
                    // file_out.close();

                    for (size_t i = 0; i < lineCount_file_in; i++)
                    {
                        std::cout << buffer[i] << std::endl;
                    }

                    FILE * file_out = fopen(path_out.c_str(), "w+b");

                    fwrite(buffer, sizeof(int), lineCount_file_in, file_out);

                    delete[] buffer;
                    fclose(file_out);
                }
                else
                {
                    std::cout << EVM_LABELS.label_warn_evmFileException << std::endl;
                    std::cout << EVM_LABELS.label_paktc;
                    std::cin.get();
                    continue;
                }

                file_in.close();

                std::cout << "Compiled Simple Assembly Script to Binary Script from file " << path_in << " to file " << path_out << std::endl;
                std::cout << EVM_LABELS.label_paktc;
                std::cin.get();
            }
        }
    }

    return 0;
}
