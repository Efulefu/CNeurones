#pragma once

#include "HelloNeurones.h"
#include "UnitTests.h"

struct Axon;
struct Network;

void printVector(int vSize, double *v);
void printWeights(int size, struct Axon **axes);
void printNetwork(struct Network *n);
void printWBetweenLayers(double **w, int firstDim, int sndDim);
void trace(char *funcName, int vSize, double *v);
