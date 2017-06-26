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

struct FuncAndDerivative {
	double(*func)(double);
	double(*derivative)(double);
};

struct Neuron {
	double(*sum)(int, double*);
	struct FuncAndDerivative activation;
	int nbIn;
	struct Axon **inputs;
	int nbOut;
	struct Axon **outputs;
	double result;
};

struct Axon {
	double w;
	struct Neuron *in;
	struct Neuron *out;
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
	/// error propagation callback
	void(*error)(double *expected, struct Layer* lastLayer, double eta);
};

// init functions
struct Network* initNet(int nbLayers, struct Layer **layers, double eta);
struct Layer* makeLayer(int dim, double(*sum)(int, double*), struct FuncAndDerivative);
struct Axon* makeAxe(double w, struct  Neuron* in, struct Neuron* out);
struct Neuron* makeNeur(double(*sum)(int, double*), struct FuncAndDerivative);

// propagation
void feedVector(int vSize, double *v, struct Network *net);

// retropropagation
void simpleRetroPropagate(struct Network* net, double* expected);
//double* errorHebb(double *expected, double *res, int dim, double eta);
void errorWidrowHoff(double *expected, struct Layer* lastLayer, double eta);

// handling functions
void connLayers(double **w, struct Layer* in, struct Layer *out);

// other functions

double sumVector(int, double*);
double sigmoid(double);
double softplus(double);
double derivatedSigmoid(double);
double leakyRelu(double);
double leakyReluDerivated(double);

const struct FuncAndDerivative Sigmoid;
const struct FuncAndDerivative SoftPlus;
const struct FuncAndDerivative LeakyRelu;