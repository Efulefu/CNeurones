#pragma once


#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <stdarg.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

#include "UnitTests.h"
#include "Utils.h"

// structure declarations and definitions

struct Neuron {
	double(*sum)(int, double*);
	double(*sigma)(double);
	int nbIn;
	struct Axon **e;
	int nbOut;
	struct Axon **s;
};

struct Axon {
	double w;
	struct Neuron *e;
	struct Neuron *s;
};

struct Layer {
	int dim;
	struct Neuron **n;
};

struct Network {
	int nbLayers;
	struct Layer **layers;
};

// init functions
struct Network* initNet(int nbLayers, struct Layer **layers);
double* feedVector(int vSize, double *v, struct Network *net);
struct Layer* makeLayer(int dim, double(*sum)(int, double*), double(*sigma)(double));
struct Axon* makeAxe(double w, struct  Neuron* in, struct  Neuron* out);
struct Neuron* makeNeur(double(*sum)(int, double*), double(*sigma)(double));

// handling functions
void connLayers(double **w, struct Layer* in, struct Layer *out);
struct Axon*** connLayersAlloc(double **w, struct Layer* in, struct Layer *out);
struct Axon*** _connLayers(double **w, struct Layer* in, struct Layer* out, bool alloc);


// other functions

double sum(int, double*);
double sigma(double);
