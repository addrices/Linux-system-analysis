cc = gcc
prom = njush
source = main.c
dep = shell.h
obj = main.o

$(prom):$(obj)
	$(cc) -o $(prom) $(obj)

%.o: %.c $(deps)
	$(cc) -c $< -o $@

play: $(prom)
	./$(prom)

clean:
	rm -rf $(obj) $(prom)

