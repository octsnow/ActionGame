encode = $(subst $() ,*,$(1))
decode = $(subst *,$() ,$(1))
quotes = $(addprefix ',$(addsuffix ',$(1)))
includes = $(addprefix -I,$(1))
libs = $(addprefix -LIBPATH:,$(1))

RM = cmd.exe /C del

SRC_DIR = src
OBJ_DIR = obj

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.obj,$(SRCS))
INC_DIR = includes ..\OctGame\includes C:\glut\freeglut-3.2.1\install\include $(call encode,C:\Program Files (x86)\OpenAL 1.1 SDK\include)
LIB_DIR = ..\..\OctBinary\lib ..\..\Wav\lib ..\OctGame\lib C:\glut\freeglut-3.2.1\install\lib C:\glut\freeglut-3.2.1\install_d\lib $(call encode,C:\Program Files (x86)\OpenAL 1.1 SDK\libs\Win64)

INC_DIR := $(call decode,$(call includes,$(call quotes,$(INC_DIR))))
LIB_DIR := $(call decode,$(call libs,$(call quotes,$(LIB_DIR))))

TARGET = out/main.exe
COMPILEOPTIONS = -EHsc -std:c++20 -c
DEBUGFLAGS = -DOCT_DEBUG
CXXFLAGS := $(COMPILEOPTIONS)

.SUFFIXES:.obj .cpp
.PHONY: debug release

all: debug

debug: CXXFLAGS = $(COMPILEOPTIONS) $(DEBUGFLAGS)
debug: $(TARGET)

release: CXXFLAGS = $(COMPILEOPTIONS)
release: $(TARGET)

$(TARGET): $(OBJS)
	LINK $(OBJS) $(LIB_DIR) -OUT:$(TARGET)

$(OBJ_DIR)/%.obj: $(SRC_DIR)/%.cpp
	cl $(CXXFLAGS) $< -Fo$(OBJ_DIR)\$(@F) $(INC_DIR)

run:
	$(TARGET)

clean:
	-$(RM) $(subst /,\,$(OBJS)) $(subst /,\,$(TARGET))
