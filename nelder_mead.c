#include <stdio.h>
#include <stdlib.h>
#include "simplex.h"

optimset get_settings (char **argv, bool single) {
    optimset opt = {
        .places = (int)strtol(argv[1], NULL, BASE),
        .fmt = (int)strtol(argv[2], NULL, BASE),
        .tolerance = strtold(argv[3], NULL),
        .max_iterations = (int)strtol(argv[4], NULL, BASE),
        .size = strtold(argv[5], NULL),
        .adaptive = (int)strtol(argv[6], NULL, BASE),
        .step_mode = single
    };
    CHECK(opt.places >= 3 && opt.places <= 36);
    CHECK(opt.fmt == 0 || opt.fmt == 1);
    CHECK(opt.tolerance >= 1.0e-36L && opt.tolerance <= 1.0e-3L);
    CHECK(opt.max_iterations >= 1 && opt.max_iterations <= 100000);
    CHECK(opt.size >= 1.0e-12L && opt.size <= 1.0e3L);
    CHECK(opt.adaptive == 0 || opt.adaptive == 1);
    return opt;
}

/*
 * Initialize for Nelder-Mead
 */
simplex *get_nm_simplex (int n, real size, const point *start) {
    simplex *s = get_regular_simplex (n, size, start);
    s->reflect = get_point(n);
    s->centroid = get_point(n);
    s->trial = get_point(n);
    s->iterations = s->evaluations = 0;
    s->looping = false;
    return s;
}

/*
 * Nelder-Mead Optimizer
 */
bool nelder_mead (simplex *s, const model *m, const optimset *o) {
    real ALPHA = 1.0L;
    real GAMMA = o->adaptive ? 1.0L + 2.0L / s->n : 2.0L;
    real RHO = o->adaptive ? 0.75L - 0.5L / s->n : 0.5L;
    real SIGMA = o->adaptive ? 1.0L - 1.0L / s->n : 0.5L;
    point *best = s->p;
    point *worst = s->p + s->n;
    point *second_worst = worst - 1;
    if (o->step_mode && s->looping) goto resume; else s->looping = true;
    while (s->delta_x > o->tolerance || s->delta_f > o->tolerance) {
        CHECK(s->iterations <= o->max_iterations);
        int shrink = 0;
        project(s->reflect, s, m, ALPHA, worst, s->centroid);
        if (best->f <= s->reflect->f && s->reflect->f < second_worst->f) {
            printf("reflect       ");
            copy_point(s->n, s->reflect, worst);
        } else if (s->reflect->f < best->f) {
            project(s->trial, s, m, GAMMA, worst, s->centroid);
            if (s->trial->f < s->reflect->f) {
                printf("expand        ");
                copy_point(s->n, s->trial, worst);
            } else {
                printf("reflect       ");
                copy_point(s->n, s->reflect, worst);
            }
        } else if (s->reflect->f < worst->f) {
            project(s->trial, s, m, RHO, worst, s->centroid);
            if (s->trial->f < s->reflect->f) {
                printf("contract_out  ");
                copy_point(s->n, s->trial, worst);
            } else shrink = 1;
        } else {
            project(s->trial, s, m, - RHO, worst, s->centroid);
            if (s->trial->f < worst->f) {
                printf("contract_in   ");
                copy_point(s->n, s->trial, worst);
            } else shrink = 1;
        }
        if (shrink) {
            printf("shrink        ");
            for (int i = 1; i < s->n + 1; i++) {
                point *non_best = s->p + i;
                project(non_best, s, m, - SIGMA, non_best, best);
            }
        }
        sort(s);
        s->iterations++;
        printf(" %04d %04d  [ ", s->iterations, s->evaluations);
        for (int j = 0; j < s->n; j++) {
            printf(o->fmt ? "% .*Le " : "% .*Lf ", o->places, best->x[j]);
        }
        printf(o->fmt ? "]  % .*Le  % .*Le % .*Le\n" : "]  % .*Lf  % .*Lf % .*Lf\n",
                o->places, best->f, o->places, s->delta_x, o->places, s->delta_f);
        if (o->step_mode) return true;
        resume: ;
    }
    return s->looping = false;
}
