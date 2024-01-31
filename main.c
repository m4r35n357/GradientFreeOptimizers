#include <stdio.h>
#include <stdlib.h>
#include "nelder_mead.h"

int main(int argc, char **argv) {
    PRINT_ARGS(argc, argv);
    CHECK(argc >= 8);
    const int n = argc - 7;

    // optimizer settings
    optimset o = get_settings(argv, false);

    // model parameters
    model *m = model_init();

    // set initial point from command arguments
    point *start = get_point(n);
    for (int j = 0; j < n; j++) {
        start->x[j] = strtod(argv[j + 7], NULL);
    }

    // print starting point
    fprintf(stderr, "%s       Initial  ", GRY);
    cost(n, start, m);
    print_point(n, start, o.places, o.fmt);
    // default simplex . . .
    simplex *s1 = get_simplex(n, o.size, start);
    fprintf(stderr, o.fmt ? "      %sDiameter %s% .*Le\n" : "      %sDiameter%s    % .*Lf\n",
            GRY, NRM, o.places, distance(s1->n, s1->p, s1->p + s1->n));
    for (int i = 0; i < s1->n + 1; i++) {  // initial cost at simplex vertices
        cost(s1->n, s1->p + i, m);
        s1->evaluations++;
        fprintf(stderr, "%s                  ", GRY);
        for (int j = 0; j < n; j++) {
            fprintf(stderr, o.fmt ? "% .*Le " : "% .*Lf ", o.places, s1->p[i].x[j]);
        }
        fprintf(stderr, "%s\n", NRM);
    }
    sort(s1);

    // begin optimization
    point *solution1 = get_point(n);
    nelder_mead(s1, solution1, m, &o);
    // print solution
    fprintf(stderr, "  %s1%s ", GRY, NRM);
    fprintf(stderr, " %4d %4d  ", s1->iterations, s1->evaluations);
    print_point(n, solution1, o.places, o.fmt);

    if (n > 1) {
        // . . . and its "dual"
        simplex *s2 = get_simplex(n, o.size, start);
        for (int i = 0; i < s2->n + 1; i++) {  // form "dual" by projecting vertices through the centre
            project(s2->p + i, s2, m, 1.0L, s2->p + i, start);
            fprintf(stderr, "%s                  ", GRY);
            for (int j = 0; j < n; j++) {
                fprintf(stderr, o.fmt ? "% .*Le " : "% .*Lf ", o.places, s2->p[i].x[j]);
            }
            fprintf(stderr, "%s\n", NRM);
        }
        sort(s2);

        // begin optimization
        point *solution2 = get_point(n);
        nelder_mead(s2, solution2, m, &o);
        // print solution
        fprintf(stderr, "  %s2%s ", GRY, NRM);
        fprintf(stderr, " %4d %4d  ", s2->iterations, s2->evaluations);
        print_point(n, solution2, o.places, o.fmt);
    }

    return 0;
}
