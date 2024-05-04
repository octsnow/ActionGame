SRC_FILES = $(wildcard *.cpp)
INCLUDE_FILES = /I C:\glut\freeglut-3.2.1\install\include
LIB_FILES = 	$(wildcard C:\glut\freeglut-3.2.1\install\lib\*.lib) $(wildcard C:\glut\freeglut-3.2.1\install_d\lib\*.lib)
GAMESRC_FILES = $(wildcard OctGame/*.cpp)

all: main

main:
	cl /EHsc /std:c++20 $(SRC_FILES) $(GAMESRC_FILES) $(LIB_FILES) $(INCLUDE_FILES) /Fe:main

run: main
	main

clean:
	rm *.obj *.exe
