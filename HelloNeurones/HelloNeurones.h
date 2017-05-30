#pragma once


#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <stdarg.h>
#include <math.h>
#include <time.h>

// structure declarations and definitions

typedef struct Neuron Neuron;
typedef struct Axon Axon;
typedef struct Layer Layer;
typedef struct Network Network;

struct Neuron {
	double(*sum)(int, double*);
	double(*sigma)(double);
	int nbIn;
	Axon **e;
	int nbOut;
	Axon **s;
};

struct Axon {
	double w;
	Neuron *e;
	Neuron *s;
};

struct Layer {
	int dim;
	Neuron **n;
};

struct Network {
	int nbLayers;
	Layer **layers;
};

// init functions
Layer* makeLayer(int dim, double(*sum)(int, double*), double(*sigma)(double));
Axon* makeAxe(double w, Neuron* in, Neuron* out);
Neuron* makeNeur(double(*sum)(int, double*), double(*sigma)(double));

// handling functions
Axon*** connLayers(double **w, Layer* in, Layer* out);


// other functions

double sum(int, double*);
double sigma(double);