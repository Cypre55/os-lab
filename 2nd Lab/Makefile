
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