#include <cstdlib>
#include <cstdio>
#include <new>
#include <iostream>
#include <iomanip>
#include "helperFuncs.h"

int main(int argc, char const *argv[]) {
    int* evmMemory = sc_memoryInit();
    char evmMemoryDataPath[256] = "./content/db/memoryData.dat";

    for (int _counter = 0; _counter < 100; _counter++)
        std::cout << "[" << evmMemory[_counter] << "]";

    std::cout << std::endl;

    sc_memorySet(evmMemory, 50, 1023);

    for (int _counter = 0; _counter < 100; _counter++)
        std::cout << "[" << evmMemory[_counter] << "]";

    std::cout << std::endl;

    sc_memorySave(evmMemoryDataPath, evmMemory);

    sc_memorySet(evmMemory, 50, 0);

    for (int _counter = 0; _counter < 100; _counter++)
        std::cout << "[" << evmMemory[_counter] << "]";

    std::cout << std::endl;

    sc_memoryLoad(evmMemoryDataPath, evmMemory);

    for (int _counter = 0; _counter < 100; _counter++)
        std::cout << "[" << evmMemory[_counter] << "]";

    std::cout << std::endl;

    std::cout << std::endl;

    using namespace RegistryStatusEnumFlags;

    char* evmFlag = sc_flagInit();

    std::cout << "(" << sc_flagGet(evmFlag, RegistryStatusEnumFlags::Alpha) << ")" << std::endl;

    sc_flagSet(evmFlag, RegistryStatusEnumFlags::Alpha, true);

    std::cout << "(" << sc_flagGet(evmFlag, RegistryStatusEnumFlags::Alpha) << ")" << std::endl;

    return 0;
}
