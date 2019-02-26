#include <cstdlib>
#include <cstdio>
#include <new>
#include <iostream>
#include <iomanip>
#include "helperFuncs.h"

int main(int argc, char const *argv[]) {
    int* evmMemory = sc_memoryInit();
    char evmMemoryDataPath[256] = "./content/db/memoryData.dat";

    try
    {
        std::cout << "Printing evmMemory" << std::endl;
        for (int _counter = 0; _counter < 100; _counter++)
            std::cout << "[" << evmMemory[_counter] << "]";
        std::cout << std::endl;

        std::cout << "Editing evmMemory" << std::endl;
        sc_memorySet(evmMemory, 50, 1023);

        std::cout << "Printing evmMemory" << std::endl;
        for (int _counter = 0; _counter < 100; _counter++)
            std::cout << "[" << evmMemory[_counter] << "]";
        std::cout << std::endl;

        std::cout << "Saving evmMemory" << std::endl;
        sc_memorySave(evmMemoryDataPath, evmMemory);

        std::cout << "Editing evmMemory" << std::endl;
        sc_memorySet(evmMemory, 50, 0);

        std::cout << "Printing evmMemory" << std::endl;
        for (int _counter = 0; _counter < 100; _counter++)
            std::cout << "[" << evmMemory[_counter] << "]";
        std::cout << std::endl;

        std::cout << "Overriding evmMemory" << std::endl;
        sc_memoryLoad(evmMemoryDataPath, evmMemory);

        std::cout << "Printing evmMemory" << std::endl;
        for (int _counter = 0; _counter < 100; _counter++)
            std::cout << "[" << evmMemory[_counter] << "]";

        std::cout << std::endl << std::endl;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    using namespace RegistryStatusEnumFlags;

    int* evmFlag = sc_flagInit();

    std::cout << "Printing evmFlag::Alpha" << std::endl;
    std::cout << "(" << sc_flagGet(evmFlag, RegistryStatusEnumFlags::Alpha) << ")" << std::endl;

    std::cout << "Editing evmFlag::Alpha" << std::endl;
    sc_flagSet(evmFlag, RegistryStatusEnumFlags::Alpha, true);

    std::cout << "Printing evmFlag::Alpha" << std::endl;
    std::cout << "(" << sc_flagGet(evmFlag, RegistryStatusEnumFlags::Alpha) << ")" << std::endl;

    std::cout << std::endl;

    int* value = new (std::nothrow) int;

    std::cout << "Encoding Value" << std::endl;
    sc_commandEncode(10, 13, value);

    std::cout << *value << std::endl;

    int* command = new (std::nothrow) int;
    int* operand = new (std::nothrow) int;

    std::cout << "Decoding Value" << std::endl;
    sc_commandDecode(*value, command, operand);
    std::cout << "Command: " << *command << std::endl;
    std::cout << "Operand: " << *operand << std::endl;

    return 0;
}
