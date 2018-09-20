cc = gcc
prom = njush
source = main.c shell.c
dep = shell.h
obj = main.o shell.o

$(prom):$(obj)
	$(cc) -o $(prom) $(obj) -Wall

%.o: %.c $(deps)
	$(cc) -c $< -o $@ -Wall

play: $(prom) $(obj) $(source) $(dep)
	./$(prom)

clean:
	rm -rf $(obj) $(prom) test

cleanall:
	rm -rf $(obj) $(prom) test hello hello2 sleep

debug:$(obj)
	gcc -o test $(source) -g

hello:hello.c hello2.c sleep.c
	gcc -o hello hello.c -Wall
	gcc -o hello2 hello2.c -Wall
	gcc -o sleep sleep.c -Wall
