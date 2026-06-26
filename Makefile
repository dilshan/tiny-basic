## This is a PlatformIO project. To run this project use PlatoformIO commands or IDE extensions. 
## The Makefile is provided to build a debug version of the project on a host machine without Arduino hardware.

TARGET := tinybasic

CXX := g++
FLEX := flex
BISON := bison

BUILD_DIR := build
GEN_DIR := $(BUILD_DIR)/generated

SRC_DIR := src
INC_DIR := include
PARSER_DIR := parser

CXXFLAGS := \
    -std=c++17 \
    -Wall \
    -Wextra \
    -g \
    -DDEBUG \
    -I$(INC_DIR) \
    -I$(SRC_DIR) \
    -MMD -MP

LIBS := -lfl

CPP_SRCS := \
    $(SRC_DIR)/main.cpp \
    $(SRC_DIR)/platform.c \
    $(SRC_DIR)/platform_io.cpp \
    $(SRC_DIR)/tinybasic.tab.c \
    $(SRC_DIR)/lex.yy.c

OBJS := $(patsubst $(SRC_DIR)/%, $(BUILD_DIR)/%, $(CPP_SRCS))
OBJS := $(OBJS:.cpp=.o)
OBJS := $(OBJS:.c=.o)

DEPS := $(OBJS:.o=.d)

all: parser $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LIBS)

parser: \
    $(SRC_DIR)/tinybasic.tab.c \
    $(INC_DIR)/tinybasic.tab.h \
    $(SRC_DIR)/lex.yy.c

$(GEN_DIR):
	mkdir -p $(GEN_DIR)

$(GEN_DIR)/tinybasic.tab.c $(GEN_DIR)/tinybasic.tab.h: \
    $(PARSER_DIR)/tinybasic.y | $(GEN_DIR)
	$(BISON) -d -o $(GEN_DIR)/tinybasic.tab.c $<

$(SRC_DIR)/tinybasic.tab.c: $(GEN_DIR)/tinybasic.tab.c
	cp $< $@

$(INC_DIR)/tinybasic.tab.h: $(GEN_DIR)/tinybasic.tab.h
	cp $< $@

$(GEN_DIR)/lex.yy.c: \
    $(PARSER_DIR)/tinybasic.l \
    $(INC_DIR)/tinybasic.tab.h | $(GEN_DIR)
	$(FLEX) -o $@ $(PARSER_DIR)/tinybasic.l

$(SRC_DIR)/lex.yy.c: $(GEN_DIR)/lex.yy.c
	cp $< $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -x c++ -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)
	rm -f $(TARGET)

distclean: clean
	rm -f $(SRC_DIR)/tinybasic.tab.c
	rm -f $(SRC_DIR)/lex.yy.c
	rm -f $(INC_DIR)/tinybasic.tab.h

rebuild: clean all

-include $(DEPS)

.PHONY: all clean distclean rebuild parser