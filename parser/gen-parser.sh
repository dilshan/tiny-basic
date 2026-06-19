#!/bin/bash

bison -d tinybasic.y
flex tinybasic.l

mv lex.yy.c ../src/lex.yy.c
mv tinybasic.tab.c ../src/tinybasic.tab.c
mv tinybasic.tab.h ../include/tinybasic.tab.h