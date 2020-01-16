CXXDEBUG=-g -fsanitize=leak -fno-omit-frame-pointer
CXXFLAGS=-Wall -Werror -Wextra -Wshadow -pedantic-errors -std=c++17

all : main

obj/app.o : src/app.cpp
	$(CXX) $(CXXFLAGS) $(CXXDEBUG) -c -o $@ $^

obj/list.o : src/list.cpp
	$(CXX) $(CXXFLAGS) $(CXXDEBUG) -c -o $@ $^

obj/window.o : src/window.cpp
	$(CXX) $(CXXFLAGS) $(CXXDEBUG) -c -o $@ $^

obj/grid.o : src/grid.cpp
	$(CXX) $(CXXFLAGS) $(CXXDEBUG) -c -o $@ $^

obj/button.o : src/button.cpp
	$(CXX) $(CXXFLAGS) $(CXXDEBUG) -c -o $@ $^

obj/split.o : src/split.cpp
	$(CXX) $(CXXFLAGS) $(CXXDEBUG) -c -o $@ $^

obj/screen.o : src/screen.cpp
	$(CXX) $(CXXFLAGS) $(CXXDEBUG) -c -o $@ $^

main : src/main.cpp obj/app.o obj/list.o obj/window.o obj/grid.o obj/button.o obj/split.o obj/screen.o
	$(CXX) $(CXXFLAGS) $(CXXDEBUG) -o $@ $^
