// HelloNeurones.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <stdarg.h>
#include <math.h>
#include <time.h>
#include "HelloNeurones.h"


struct Neurone {
	double(*sum)(int, double*);
	double(*sigma)(double);
	int nbIn;
	Axone **e;
	int nbOut;
	Axone **s;
};

struct Axone {
	double w;
	Neurone *e;
	Neurone *s;
};

struct Couche {
	int depth;
	int dim;
	Neurone **n;
};

double randfrom(double min, double max)
{
	double range = (max - min);
	double div = RAND_MAX / range;
	return min + (rand() / div);
}

double** randWeights(int inDim, int outDim) {
	double **res = malloc(sizeof(double*) * inDim);
	for (int i = 0; i < inDim; i++) {
		res[i] = malloc(sizeof(double) * outDim);
		for (int j = 0; j < outDim; j++) {
			res[i][j] = randfrom(0, 1);
			printf("generated value %lf at coord %d : %d\n", res[i][j], i, j);
		}
	}
	return res;
}

int main()
{
	srand(time(NULL));
	double **test = randWeights(3, 4);
	Couche *first = makeLayer(0, 3, sum, sigma);
	Couche *second = makeLayer(1, 4, sum, sigma);
	connLayers(test, first, second);

	printf("Rand doubles[2][1] is %lf\n", test[2][1]);
	printf("First layer's 3rd neuron connects to 2nd layer 2nd neuron with weight: %lf\n", first->n[2]->s[1]->w);
	getchar();
    return 0;
}


double sum(int d, double *v) {
	int i;
	double res = 0;
	for (i = 0; i < d; i++) {
		res += v[i];
	}
	return res;
}

double sigma(double sum) {
	return 1 / (1 + exp(sum));
}

Axone*** connLayers(double **w, Couche* in, Couche* out) {
	Axone ***res = malloc(sizeof(Axone**) * in->dim);
	// TODO: sanity check w array sizes vs in and out dimensions
	for (int i = 0; i < in->dim; i++) {
		res[i] = malloc(sizeof(Axone*) * out->dim);
		for (int j = 0; j < out->dim; j++) {
			printf("Axone du in %d au out %d a le poids %lf\n", i, j, w[i][j]);
			res[i][j] = makeAxe(w[i][j], in->n[i], out->n[j]);
		}
	}
	return res;
}

Couche* makeLayer(int depth, int dim, double(*sum)(int, double*), double(*sigma)(double)) {
	Couche *c = malloc(sizeof(Couche));
	c->depth = depth;
	c->dim = dim;
	Neurone **n = malloc(sizeof(Neurone*) * dim);
	for (int i = 0; i < dim; i++) {
		n[i] = makeNeur(sum, sigma);
	}
	c->n = n;
	return c;
}

Axone* makeAxe(double w, Neurone* in, Neurone* out) {
	Axone *a = malloc(sizeof(Axone));
	a->w = w;
	a->e = in;
	a->s = out;
	if (in) {
		in->s = realloc(in->s, sizeof(Axone *) * ++(in->nbOut));
		in->s[in->nbOut - 1] = a;
	}
	if (out) {
		out->e = realloc(out->e, sizeof(Axone *) * ++(out->nbIn));
		out->e[out->nbIn - 1] = a;
	}
	return a;
}

Neurone* makeNeur(double(*sum)(int, double*), double(*sigma)(double)) {
	Neurone *n = malloc(sizeof(Neurone));
	n->sum = sum;
	n->sigma = sigma;
	n->e = NULL;
	n->nbIn = 0;
	n->s = NULL;
	n->nbOut = 0;
	return n;
}