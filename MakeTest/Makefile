#Makefile
.SUFFIXES : .c 0.
OBJECTS = main.o print.o input.o
SRCS = $(OBJECTS:.o=.c)

CFLAGS = -g
TARGET = main
CC = gcc

#$@ 현재 목표 파일의 이름
$(TARGET): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS)

clean :
	$(RM) $(OBJECTS) $(TARGET) core

#$< 현재 목표파일 (Target) 보다 더 최근에 생신되 파이명으로, 
#첫번째 종속물의 이름
.c .o :
	$(CC) $(CFLAGS) -c $< -o $@
	
main.o : make.c	
print.o : print.c
intput.o : input.c
