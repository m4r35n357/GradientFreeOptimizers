#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "spiral.h"

config get_config (char **argv, bool single) {
    config conf = {
        .places = (int)strtol(argv[1], NULL, BASE),
        .fmt = (int)strtol(argv[2], NULL, BASE),
        .n = (int)strtol(argv[3], NULL, BASE),
        .m = (int)strtol(argv[4], NULL, BASE),
        .k_max = (int)strtol(argv[5], NULL, BASE),
        .delta = strtold(argv[6], NULL),
        .step_mode = single
    };
    CHECK(conf.places >= 3 && conf.places <= 36);
    CHECK(conf.fmt == 0 || conf.fmt == 1);
    CHECK(conf.n >= 1 && conf.n <= 100);
    CHECK(conf.m >= 1 && conf.m <= 10000);
    CHECK(conf.delta >= 0.0L && conf.delta <= 1.0L);
    CHECK(conf.k_max >= 1 && conf.k_max <= 100000);
    return conf;
}

void find_best (spiral *s, config c) {
    for (int i = 0; i < c.m; i++) {
        if (s->points[i]->f < s->best->f) {
            s->best = s->points[i];
        }
    }
}

point *get_point (spiral *s, real min_x, real max_x, model *m, config c) {
    point *p = malloc(sizeof (point));          CHECK(p);
    p->x = malloc((size_t)c.n * sizeof (real)); CHECK(p->x);
    for (int k = 0; k < c.n; k++) {
        p->x[k] = (max_x - min_x) * (real)rand() / (real)RAND_MAX + min_x;
    }
    cost(c.n, p, m);
    s->evaluations++;
    return p;
}

spiral *get_spiral (real min_x, real max_x, model *m, config c) {
    srand((unsigned int)time(NULL));
    spiral *s =  malloc(sizeof(spiral));
    s->k = s->k_star = s->evaluations = 0;
    s->points = malloc((size_t)c.m * sizeof (point *));    CHECK(s->points);
    for (int i = 0; i < c.m; i++) {
        s->points[i] = get_point(s, min_x, max_x, m, c);
    }
    s->update = get_point(s, min_x, max_x, m, c);
    s->best = s->points[0];
    find_best(s, c);
    s->centre = s->best;
    s->looping = false;
    return s;
}

bool soa (spiral *s, model *m, config c) {
    if (c.step_mode && s->looping) goto resume; else s->looping = true;
    while (s->k < c.k_max) {
        real r = (s->k >= s->k_star + 2.0L * c.n) ? powl(c.delta, 0.5L / c.n) : 1.0L;
        for (int i = 0; i < c.m; i++) {
            point *current = s->points[i];
            for (int k = 0; k < c.n; k++) {
                s->update->x[k] = s->centre->x[k] +
                    r * (k ? (current->x[k - 1] - s->centre->x[k - 1]) : - (current->x[c.n - 1] - s->centre->x[c.n - 1]));
            }
            for (int k = 0; k < c.n; k++) {
                current->x[k] = s->update->x[k];
            }
            cost(c.n, current, m);
            s->evaluations++;
        }
        find_best(s, c);
        if (s->best->f < s->centre->f) {
            s->centre = s->best;
            s->k_star = s->k + 1;
        }
        s->k++;
        printf(" %05d %06d  [ ", s->k, s->evaluations);
        for (int k = 0; k < c.n; k++) {
            printf(c.fmt ? "% .*Le " : "% .*Lf ", c.places, s->centre->x[k]);
        }
        printf(c.fmt ? "]  % .*Le\n" : "]  % .*Lf\n", c.places, s->centre->f);
        if (c.step_mode) return true;
        resume: ;
    }
    return s->looping = false;
}