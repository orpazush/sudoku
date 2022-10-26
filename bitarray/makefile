CPP = g++
INC = ../../include
FILES = $(wildcard *.cpp)
OBJ = $(patsubst %.cpp, %.o, $(FILES))
TEST := $(wildcard *_test.cpp)
OUT = $(patsubst %_test.cpp, %_test.out, $(TEST))
HEADER = $(patsubst %_test.cpp, %.hpp, $(TEST))
LIB = ../../lib
EXTRA =-g
CPPFLAGS =-std=c++98 -pedantic-errors -Wall -Wextra $(EXTRA) -I$(INC)

all: $(OUT)

$(OUT): $(OBJ)
	$(CPP) -o $@ $(CPPFLAGS) $(FILES)

%.o: %.cpp $(HEADER)

.PHONY : clean
clean :
	rm *.out *.o
