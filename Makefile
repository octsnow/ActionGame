SRC_FILES = $(wildcard *.cpp)
INCLUDE_FILES = /I C:\glut\freeglut-3.2.1\install\include
LIB_FILES = 	$(wildcard C:\glut\freeglut-3.2.1\install\lib\*.lib) $(wildcard C:\glut\freeglut-3.2.1\install_d\lib\*.lib)
GAMESRC_FILES = $(wildcard OctGame/*.cpp)
OBJ_FILES = $(SRC_FILES:%.cpp=%.obj) $(GAMESRC_FILES:%.cpp=%.obj)

all: allcpp

.SUFFIXES: .cpp .obj

main: $(OBJ_FILES)
	cl /EHsc $(subst OctGame/,,$^) $(LIB_FILES) $(INCLUDE_FILES) /Fe:main

allcpp:
	cl /EHsc /std:c++20 /DOCT_DEBUG $(SRC_FILES) $(GAMESRC_FILES) $(LIB_FILES) $(INCLUDE_FILES) /Fe:main

.cpp.obj:
	cl /c /EHsc /std:c++20 /DOCT_DEBUG $< /Fo$(@F)

run: main
	main

clean:
	rm *.obj *.exe
