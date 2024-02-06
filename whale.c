#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <time.h>
#include "whale.h"

options get_options (char **argv, bool single) {
    options opt = {
        .places = (int)strtol(argv[1], NULL, BASE),
        .fmt = (int)strtol(argv[2], NULL, BASE),
        .dim = (int)strtol(argv[3], NULL, BASE),
        .whales = (int)strtol(argv[4], NULL, BASE),
        .iterations = (int)strtol(argv[5], NULL, BASE),
        .step_mode = single
    };
    CHECK(opt.places >= 3 && opt.places <= 36);
    CHECK(opt.fmt == 0 || opt.fmt == 1);
    CHECK(opt.dim >= 1 && opt.dim <= 100);
    CHECK(opt.whales >= 1 && opt.whales <= 10000);
    CHECK(opt.iterations >= 1 && opt.iterations <= 10000);
    return opt;
}

int rand_int (int n) {
    return (int)((real)rand() / ((real)RAND_MAX + 1) * n);
}

real rand_real () {
    return (real)rand() / (real)RAND_MAX;
}

whale *get_whale (int dim, real min_x, real max_x, model *m) {
    whale *w = malloc(sizeof(whale));
    w->x = malloc((size_t)dim * sizeof(real));
    for (int j = 0; j < dim; j++) {
        w->x[j] = (max_x - min_x) * rand_real() + min_x;
    }
    cost(dim, w, m);
    return w;
}

population *get_population (real min_x, real max_x, model *m, options o) {
    srand((unsigned int)time(NULL));
    population *p =  malloc(sizeof(population));
    p->iterations = p->evaluations = 0;
    p->whales = malloc((size_t)o.whales * sizeof(whale *));
    for (int i = 0; i < o.whales; i++) {
        p->whales[i] = get_whale(o.dim, min_x, max_x, m);
        p->evaluations++;
    }
    p->prey = p->whales[0];
    for (int i = 1; i < o.whales; i++) {
        if (p->whales[i]->f < p->prey->f) {
            p->prey = p->whales[i];
        }
    }
    p->looping = false;
    return p;
}

bool woa (population *p, real min_x, real max_x, model *m, options o) {
    real TWO_PI = 2.0L * acosl(-1.0L);
    if (o.step_mode && p->looping) goto resume; else p->looping = true;
    while (p->iterations < o.iterations) {
        real a = 2.0L * (1.0L - (real)p->iterations / (real)o.iterations);
        for (int i = 0; i < o.whales; i++) {
            whale *current = p->whales[i];
            real A = a * (2.0L * rand_real() - 1.0L);
            real C = 2.0L * rand_real();
            real b = 1.0L;
            real l = 2.0L * rand_real() - 1.0L;
            if (rand_real() < 0.5L) {
                if (fabsl(A) < 1.0L) { // "encircling" update (1)
                    for (int j = 0; j < o.dim; j++) {
                        current->x[j] = p->prey->x[j] - A * fabsl(C * p->prey->x[j] - current->x[j]);
                    }
                } else {  // "searching/random" update (9)
                    int r = rand_int(o.whales);
                    while (r == i) r = rand_int(o.whales);
                    whale *random = p->whales[r];
                    for (int j = 0; j < o.dim; j++) {
                        current->x[j] = random->x[j] - A * fabsl(C * random->x[j] - current->x[j]);
                    }
                }
            } else {  // "spiral" update (7)
                for (int j = 0; j < o.dim; j++) {
                    current->x[j] = fabsl(p->prey->x[j] - current->x[j]) * expl(b * l) * cosl(TWO_PI * l) + p->prey->x[j];
                }
            }
            for (int j = 0; j < o.dim; j++) {
                if (current->x[j] > max_x || current->x[j] < min_x) {
                	current->x[j] = (max_x - min_x) * rand_real() + min_x;
                }
            }
            cost(o.dim, current, m);
            p->evaluations++;
            if (current->f < p->prey->f) p->prey = current;
        }
        p->iterations++;
        printf(" %05d %06d  [ ", p->iterations, p->evaluations);
        for (int j = 0; j < o.dim; j++) {
            printf(o.fmt ? "% .*Le " : "% .*Lf ", o.places, p->prey->x[j]);
        }
        printf(o.fmt ? "]  % .*Le\n" : "]  % .*Lf\n", o.places, p->prey->f);
        if (o.step_mode) return true;
        resume: ;
    }
    return p->looping = false;
}
