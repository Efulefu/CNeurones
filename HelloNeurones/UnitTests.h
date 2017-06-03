#pragma once

#include "HelloNeurones.h"
#include <stdlib.h>

struct Layer;

// if weights 
int testWeights(double **w, struct Layer *couche1, struct Layer *couche2);
double** randWeights(int inDim, int outDim);
double* randW(int dim);
void seedRand();