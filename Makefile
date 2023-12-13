
CC=/usr/bin/gcc
CFLAGS=-std=c99 -O3 -fno-math-errno -flto -s
WARNINGS=-Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes -Wconversion -Wredundant-decls -Wmissing-declarations

%.o: %.c
	$(CC) $(CFLAGS) -MT $@ -MMD -MP -c -o $@ $< $(WARNINGS)

all: nm-ackley nm-sphere nm-rosenbrock nm-himmelblau

nm-%: %.o nelder_mead.o main.o
	$(CC) $(CFLAGS) -o $@ $^ -lm

.PHONY: test ctags clean depclean

test: all
	@./nm-ackley 0 -2.10 -3.04 4.50 
	@./nm-rosenbrock 0  1.0 0.0
	@./nm-rosenbrock 0 -1.0 0.0
	@./nm-rosenbrock 0 -1.0 1.0
	@./nm-himmelblau 0  3.0  3.0
	@./nm-himmelblau 0  3.0 -3.0
	@./nm-himmelblau 0 -3.0  3.0
	@./nm-himmelblau 0 -3.0 -3.0

ctags:
	@/usr/bin/ctags *.h *.c

clean:
	@rm -rf nm-* *.o

depclean: clean
	@rm -f *.d

-include *.d
