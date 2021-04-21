OBJS=app.o list.o window.o grid.o button.o split.o screen.o

CXXFLAGS=-Wall -Werror -Wextra -Wnon-virtual-dtor -Wshadow -pedantic-errors -std=c++20

all : main

.PHONY : clean
clean :
	rm -- *.o
	rm main

debug : CXXFLAGS+=-g -fsanitize=leak -fno-omit-frame-pointer
debug : main

$(OBJS) : %.o : src/%.cpp makefile
	$(CXX) $(CXXFLAGS) -c -o $@ $<

main : src/main.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^
