CC = g++
INC = ./bitarray/
# LDIR = ../design_patterns/lib
FILES = $(wildcard *.cpp) ./bitarray/bitarray.cpp
EXTRA =-g
CFLAGS =-std=c++98 -pedantic-errors -Wall -Wextra $(EXTRA) -I$(INC)
# LINKERFLAGS=-lboost_system -pthread -lboost_thread -lboost_chrono

all:
	$(CC) $(CFLAGS) $(FILES) -o play
	# $(CC) $(CFLAGS) $(FILES) $(LINKERFLAGS) -o $(OUT)


debug:
	echo $(FILES)
	echo
	echo $(OBJ)
	echo
	echo $(OUT)

.PHONY : clean
clean :
	rm *.out *.o
