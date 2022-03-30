run1: demo1
	@./demo1

run2: demo2
	@./demo2

runt: t
	@./t

demo1: demo1.c memlab.o
	@gcc $^ -o $@

demo2: demo2.c memlab.o
	@gcc $^ -o $@

t: t.c memlab.o
	@gcc $^ -o $@

memlab.o: memlab.c memlab.h
	@gcc -c $< -o $@