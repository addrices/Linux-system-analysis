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

cleanall:
	rm -rf $(obj) $(prom) test hello hello2 g

debug:$(obj)
	gcc -o test $(source) -g

hello:hello.c hello2.c
	gcc -o hello hello.c
	gcc -o hello2 hello2.c
