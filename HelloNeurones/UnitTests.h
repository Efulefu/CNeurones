#pragma once

#include "HelloNeurones.h"

// if weights 
int testWeights(double **w, Layer *couche1, Layer *couche2);
double** randWeights(int inDim, int outDim);
double* randW(int dim);
void seedRand();