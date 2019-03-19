G++ = g++
G++FLAGS = -c -Wall -Werror -Wno-unused-variable
VPATH = src:build
PATH_IN = ./src/
PATH_BUILD = ./build/
PATH_OUT = ./bin/
evm2019_main = main
evm2019_helperFuncs = helperFuncs
#evm2019_conMenu = conMenu

all: app clean

app: $(evm2019_main).o $(evm2019_helperFuncs).o
	$(G++) $(PATH_BUILD)$(evm2019_main).o $(PATH_BUILD)$(evm2019_helperFuncs).o -o $(PATH_OUT)evm2019_run

$(evm2019_main).o: $(evm2019_main).cpp
	$(G++) $(G++FLAGS) $(PATH_IN)$(evm2019_main).cpp -o $(PATH_BUILD)$(evm2019_main).o

$(evm2019_helperFuncs).o: $(evm2019_helperFuncs).cpp
	$(G++) $(G++FLAGS) $(PATH_IN)$(evm2019_helperFuncs).cpp -o $(PATH_BUILD)$(evm2019_helperFuncs).o

ifeq ($(OS),Windows_NT)
clean:
	del /S *.o
else
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
clean:
	find . -name \*.o -type f -exec rm -f {} \;
endif
endif