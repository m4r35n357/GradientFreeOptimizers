#include <stdio.h>
#include <stdlib.h>
#include "whale.h"

double cost (double *position, int dim) {
    double value = 0.0;
    for (int j = 0; j < dim - 1; j++) {
        double xi = position[j];
        value += (1.0 - xi) * (1.0 - xi) + 100.0 * (position[j + 1] - xi * xi) * (position[j + 1] - xi * xi);
    }
    return value;
}

int main (int argc, char *argv[]) {
    int dim, num_whales, max_iter;
    if (argc == 1) {
        dim = 3;
        num_whales = 50;
        max_iter = 100;
    } else {
        dim = atoi(argv[1]);
        num_whales = atoi(argv[2]);
        max_iter = atoi(argv[3]);
    }

    double *best_x = woa(max_iter, num_whales, dim, -10.0, 10.0);
    printf("Best solution found:  ");
    for (int k = 0; k < dim; k++) {
        printf("% .6f ", best_x[k]);
    }
    printf("  value = % .6f\n\n", cost(best_x, dim));

    return 0;
}
