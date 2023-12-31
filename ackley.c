
#include <stdlib.h>
#include <math.h>
#include "model.h"

struct Model { real a, b, c; };

model *get_parameters () {
    model *m = malloc(sizeof (model));
    m->a = 20.0L;
    m->b = 0.2L;
    m->c = 2.0L * acosl(-1.0L);
    return m;
}

//-----------------------------------------------------------------------------
// Ackley function
// - n is the dimension of the data
// - point is the location where the function will be evaluated
// - arg contains the parameters of the function
// More details on the function at http://www.sfu.ca/%7Essurjano/ackley.html
//-----------------------------------------------------------------------------
void cost (int n, point *p, const model *m) {
    // cost function computation for arguments of exp
    real sum_sqr = 0.0L;
    real sum_cos = 0.0L;
    for (int i = 0; i < n; i++) {
        sum_sqr += SQR(p->x[i]);
        sum_cos += cosl(m->c * p->x[i]);
    }
    // final result
    p->f = -m->a * expl(-m->b * sqrtl(sum_sqr / n)) - expl(sum_cos / n) + m->a + expl(1.0L);
}
