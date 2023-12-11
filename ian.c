#include <math.h>

#include "ian.h"

#define SQUARE(x) ((x) * (x))

//-----------------------------------------------------------------------------
// Ackley function
// - n is the dimension of the data
// - point is the location where the function will be evaluated
// - arg contains the parameters of the function
// More details on the function at http://www.sfu.ca/%7Essurjano/ackley.html
//-----------------------------------------------------------------------------

void ian_fun(int n, point_t *point, const void *arg) {
  // cast the void pointer to what we expect to find
  const ian_param_t *params = (const ian_param_t *)arg;

  // cost function computation for arguments of exp
  double sum_squares = 0;
  for (int i = 0; i < n; i++) {
    sum_squares += SQUARE(point->x[i]);
  }

  // final result
  point->fx = sqrt(sum_squares / n);
}
