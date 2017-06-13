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
	double result;
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
	/// retro propagation coefficient
	double eta;
};

// init functions
struct Network* initNet(int nbLayers, struct Layer **layers, double eta);
struct Layer* makeLayer(int dim, double(*sum)(int, double*), double(*sigma)(double));
struct Axon* makeAxe(double w, struct  Neuron* in, struct Neuron* out);
struct Neuron* makeNeur(double(*sum)(int, double*), double(*sigma)(double));

// propagation
void feedVector(int vSize, double *v, struct Network *net);

// retropropagation
double* errorHebb(double *expected, double *res);
double* errorWidrowHoff(double *expected, double *res);

// handling functions
void connLayers(double **w, struct Layer* in, struct Layer *out);
struct Axon*** connLayersAlloc(double **w, struct Layer* in, struct Layer *out);
struct Axon*** _connLayers(double **w, struct Layer* in, struct Layer* out, bool alloc);


// other functions

double sumVector(int, double*);
double sigma(double);
