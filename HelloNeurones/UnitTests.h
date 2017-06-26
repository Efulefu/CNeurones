#pragma once

#include "HelloNeurones.h"
#include <stdlib.h>

struct Layer;
struct FuncAndDerivative;

void runWeightAssignementTest();
int testWeights(double **w, struct Layer *couche1, struct Layer *couche2);
double** randWeights(int inDim, int outDim);
double* randW(int dim);
void seedRand();