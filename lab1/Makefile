cc = gcc
prom = njush
source = main.c shell.c
dep = shell.h
obj = main.o shell.o

$(prom):$(obj)
	$(cc) -o $(prom) $(obj)

%.o: %.c $(deps)
	$(cc) -c $< -o $@

play: $(prom)
	./$(prom)

clean:
	rm -rf $(obj) $(prom) test

debug:
	gcc -o test $(source) -g

