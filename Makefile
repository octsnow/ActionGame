SRC_FILES = $(wildcard *.cpp)
GAMESRC_FILES = $(wildcard OctGame/*.cpp)

all: main

main:
	cl /EHsc $(SRC_FILES) $(GAMESRC_FILES) /Fe:main
