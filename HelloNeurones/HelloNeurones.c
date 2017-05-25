// HelloNeurones.cpp : Defines the entry point for the console application.
//

#include "HelloNeurones.h"
#include "UnitTests.h"




int main()
{
	runTest();
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

Axon*** connLayers(double **w, Layer* in, Layer* out) {
	Axon ***res = malloc(sizeof(Axon**) * in->dim);
	// TODO: sanity check w array sizes vs in and out dimensions
	for (int i = 0; i < in->dim; i++) {
		res[i] = malloc(sizeof(Axon*) * out->dim);
		for (int j = 0; j < out->dim; j++) {
			res[i][j] = makeAxe(w[i][j], in->n[i], out->n[j]);
		}
	}
	return res;
}

Layer* makeLayer(int dim, double(*sum)(int, double*), double(*sigma)(double)) {
	Layer *c = malloc(sizeof(Layer));
	c->dim = dim;
	Neuron **n = malloc(sizeof(Neuron*) * dim);
	for (int i = 0; i < dim; i++) {
		n[i] = makeNeur(sum, sigma);
	}
	c->n = n;
	return c;
}

Axon* makeAxe(double w, Neuron* in, Neuron* out) {
	Axon *a = malloc(sizeof(Axon));
	a->w = w;
	a->e = in;
	a->s = out;
	if (in) {
		in->s = realloc(in->s, sizeof(Axon *) * ++(in->nbOut));
		in->s[in->nbOut - 1] = a;
	}
	if (out) {
		out->e = realloc(out->e, sizeof(Axon *) * ++(out->nbIn));
		out->e[out->nbIn - 1] = a;
	}
	return a;
}

Neuron* makeNeur(double(*sum)(int, double*), double(*sigma)(double)) {
	Neuron *n = malloc(sizeof(Neuron));
	n->sum = sum;
	n->sigma = sigma;
	n->e = NULL;
	n->nbIn = 0;
	n->s = NULL;
	n->nbOut = 0;
	return n;
}

Network* makeNet(int nbLayers, Layer **layers) {
	Network *n = malloc(sizeof(Network));
	n->layers = layers;
	n->nbLayers = nbLayers;
	return n;
}

double* feedVector(int vSize, double *v, Network *net) {
	for (int i = 0; i < net->nbLayers; i++) {
		int lSize = net->layers[i]->dim;
		double *layerRes = malloc(sizeof(double) * lSize);
		//pass values through this layer
		for (int j = 0; j < lSize; j++) {
			Neuron *n = net->layers[i]->n[j];
			double sum = n->sum(vSize, v);
			layerRes[j] = n->sigma(sum);
		}
		//var replacement makes it recursive?
		v = layerRes;
	}
	return v;
}