
CFLAGS=-std=c99 -O3 -fno-math-errno -flto
WARNINGS=-Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes -Wconversion -Wredundant-decls -Wmissing-declarations
LIB_STD=-lm
LIB_GL=-lGLEW -lglut -lGLU -lGL

ifeq ($(CCC),gcc)  # fast option
  CC=/usr/bin/gcc
  CFLAGS += -s
  WARNINGS += -Wunsuffixed-float-constants
else ifeq ($(CCC),clang)  # fast option
  CC=/usr/bin/clang
else  # default for IDEs and git commits
  CC=/usr/bin/clang
  CFLAGS=-std=c99 -O0 -g
endif

%.o: %.c
	$(CC) $(CFLAGS) -MT $@ -MMD -MP -c -o $@ $< $(WARNINGS)

all: nm-std nm-gl

nm-%-std: %.o nelder_mead.o main.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIB_STD)

nm-std: nm-ackley-std nm-hartmann3-std nm-hartmann6-std nm-rosenbrock-std nm-dixon-price-std nm-himmelblau-std nm-sphere-std nm-st-std nm-rastrigin-std

nm-%-gl: %.o nelder_mead.o opengl.o simplex-gl.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIB_STD) $(LIB_GL)

nm-gl: nm-ackley-gl nm-hartmann3-gl nm-hartmann6-gl nm-rosenbrock-gl nm-dixon-price-gl nm-himmelblau-gl nm-sphere-gl nm-st-gl nm-rastrigin-gl

.PHONY: test test-6d test-4d test-3d test-2d ctags clean depclean

test: test-6d test-4d test-3d test-2d

test-6d: all
	@./run.sh -b nm-hartmann6-std -p 0.5,0.5,0.5,0.5,0.5,0.5 >/dev/null
	@./run.sh -b nm-sphere-std -p 1.0,1.0,1.0,-2.10,-3.04,4.50 >/dev/null

test-4d: all
	@./run.sh -b nm-ackley-std -p 3.0,-2.10,-3.04,4.50 >/dev/null
	@./run.sh -b nm-sphere-std -p 1.0,-2.10,-3.04,4.50 >/dev/null
	@./run.sh -b nm-dixon-price-std -p 1.0,1.0,1.0,1.0 >/dev/null

test-3d: all
	@./run.sh -b nm-ackley-std -p -2.10,-3.04,4.50 >/dev/null
	@./run.sh -b nm-sphere-std -p -2.10,-3.04,4.50 >/dev/null
	@./run.sh -b nm-st-std -p 0.0,0.0,0.0 >/dev/null
	@./run.sh -b nm-hartmann3-std -p 0.5,0.5,0.5 >/dev/null
	@./run.sh -b nm-dixon-price-std -p 1.0,1.0,1.0 >/dev/null
	@./run.sh -b nm-rosenbrock-std -p 1.0,0.0,-1.0 >/dev/null
	@./run.sh -b nm-rastrigin-std -p 0.0,0.0,0.0 >/dev/null

test-2d: all
	@./run.sh -b nm-rosenbrock-std -p 1.0,0.0 >/dev/null
	@./run.sh -b nm-himmelblau-std -p 3.0,3.0 >/dev/null
	@./run.sh -b nm-himmelblau-std -p 3.0,-3.0 >/dev/null
	@./run.sh -b nm-himmelblau-std -p -3.0,3.0 >/dev/null
	@./run.sh -b nm-himmelblau-std -p -3.0,-3.0 >/dev/null

ctags:
	@/usr/bin/ctags *.h *.c

clean:
	@rm -rf nm-* *.o

depclean: clean
	@rm -f *.d

-include *.d
