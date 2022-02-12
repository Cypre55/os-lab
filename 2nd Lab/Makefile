
BUILD_DIR ?= ./.out

temp: temp.cpp
	@g++ -o $@ $^

run: temp
	@./temp

siba: siba.c
	@gcc -o $@ $^

sib: siba
	@./siba

clean:
	@rm -rf siba temp


MKDIR_P ?= mkdir -p

t: history.cpp.o t.cpp
	g++ $^ -o $@
	./t

history.cpp.o: history.cpp history.hpp
	$(CXX) -c $< -o $@