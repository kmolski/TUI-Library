SRCS=$(wildcard src/*.cppm)
OBJS=$(SRCS:src/%.cppm=obj/%.o)
PCMS=$(OBJS:obj/%.o=obj/libtui_%.pcm)

CXXFLAGS=-Wall -Werror -Wextra -Wnon-virtual-dtor -Wshadow -pedantic-errors \
         -fmodules -fprebuilt-module-path=obj/ -stdlib=libc++ -std=c++20
CXX=clang++

all : main

.PHONY : clean
clean :
	rm $(PCMS)
	rm $(OBJS)
	rm main

obj/ :
	mkdir obj

debug : CXXFLAGS+=-g -fsanitize=leak -fno-omit-frame-pointer
debug : main

obj/libtui_app.pcm : src/app.cppm obj/ obj/libtui_base.pcm obj/libtui_screen.pcm obj/libtui_window.pcm
	$(CXX) $(CXXFLAGS) --precompile -o $@ $<

obj/libtui_base.pcm : src/base.cppm obj/
	$(CXX) $(CXXFLAGS) --precompile -o $@ $<

obj/libtui_button.pcm : src/button.cppm obj/ obj/libtui_base.pcm
	$(CXX) $(CXXFLAGS) --precompile -o $@ $<

obj/libtui_grid.pcm : src/grid.cppm obj/ obj/libtui_base.pcm
	$(CXX) $(CXXFLAGS) --precompile -o $@ $<

obj/libtui_list.pcm : src/list.cppm obj/ obj/libtui_base.pcm
	$(CXX) $(CXXFLAGS) --precompile -o $@ $<

obj/libtui_screen.pcm : src/screen.cppm obj/ obj/libtui_base.pcm obj/libtui_window.pcm
	$(CXX) $(CXXFLAGS) --precompile -o $@ $<

obj/libtui_split.pcm : src/split.cppm obj/ obj/libtui_base.pcm
	$(CXX) $(CXXFLAGS) --precompile -o $@ $<

obj/libtui_window.pcm : src/window.cppm obj/ obj/libtui_base.pcm
	$(CXX) $(CXXFLAGS) --precompile -o $@ $<

$(OBJS) : obj/%.o : src/%.cppm $(PCMS)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

main : $(OBJS) src/main.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^
