// Hellostruct Neurones.cpp : Defines the entry point for the console application.
//

#include "HelloNeurones.h"


// globals
bool verbose = false;

int main(int argc, char* argv[])
{
	// CLI options
	for (int i = 0; i < argc; i++) {
		if (strcmp("-log", argv[i]) == 0) {
			FILE *stream;
			errno_t err;
			if ((err = freopen_s(&stream, "neuron_log.txt", "w+", stdout)) != 0) exit(-1);
		}
		if (strcmp("-v", argv[i]) == 0) {
			verbose = true;
		}
	}


	seedRand();
	int layerSize = 3;
	int nbLayers = 2;
	struct Layer **layers = malloc(sizeof(struct Layer*) * nbLayers);
	for (int i = 0; i < nbLayers; i++) {
		layers[i] = makeLayer(layerSize, sum, sigma);
	}

	struct Network *n = (struct Network *) initNet(nbLayers, layers);

	if (verbose) printNetwork(n);

	double in[] = { 0.5 };
	double *res = feedVector(1, &in[0], n);


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

void connLayers(double **w, struct Layer* in, struct Layer* out) {
	_connLayers(w, in, out, false);
}

struct Axon*** connLayersAlloc(double **w, struct Layer* in, struct Layer* out) {
	return _connLayers(w, in, out, true);
}
struct Axon*** _connLayers(double **w, struct Layer* in, struct Layer* out, bool alloc) {
	struct Axon ***res = NULL;
	if (alloc) res = malloc(sizeof(struct Axon**) * in->dim);
	// TODO: sanity check w array sizes vs in and out dimensions
	for (int i = 0; i < in->dim; i++) {
		if (alloc) res[i] = malloc(sizeof(struct Axon*) * out->dim);
		for (int j = 0; j < out->dim; j++) {
			struct Axon *conn = makeAxe(w[i][j], in->n[i], out->n[j]);
			if (alloc) res[i][j] = conn;
		}
	}
	return res;
}

struct Layer* makeLayer(int dim, double(*sum)(int, double*), double(*sigma)(double)) {
	struct Layer *c = malloc(sizeof(struct Layer));
	c->dim = dim;
	struct Neuron **n = malloc(sizeof(struct Neuron*) * dim);
	for (int i = 0; i < dim; i++) {
		n[i] = makeNeur(sum, sigma);
	}
	c->n = n;
	return c;
}

struct Axon* makeAxe(double w, struct Neuron* in, struct Neuron* out) {
	struct Axon *a = malloc(sizeof(struct Axon));
	a->w = w;
	a->e = in;
	a->s = out;
	if (in != NULL) {
		in->s = realloc(in->s, sizeof(struct Axon *) * ++(in->nbOut));
		in->s[in->nbOut - 1] = a;
	}
	if (out != NULL) {
		out->e = realloc(out->e, sizeof(struct Axon *) * ++(out->nbIn));
		out->e[out->nbIn - 1] = a;
	}
	return a;
}

struct Neuron* makeNeur(double(*sum)(int, double*), double(*sigma)(double)) {
	struct Neuron *n = malloc(sizeof(struct Neuron));
	n->sum = sum;
	n->sigma = sigma;
	n->e = NULL;
	n->nbIn = 0;
	n->s = NULL;
	n->nbOut = 0;
	return n;
}


struct Network* initNet(int nbLayers, struct Layer **layers) {
	struct Network *n = malloc(sizeof(struct Network));
	n->layers = layers;
	n->nbLayers = nbLayers;
	//sanity check
	if (nbLayers < 1) {
		printf("Cannot make network with less than 1 layers\n");
		return NULL;
	}
	//connect layers
	int firstDim;
	int secondDim;
	int i;
	for (i = 1; i < nbLayers; i++) {
		struct Layer *prev = layers[i - 1];
		struct Layer *next = layers[i];
		firstDim = prev->dim;
		secondDim = next->dim;
		double **w = randWeights(firstDim, secondDim);
		printWBetweenLayers(w, firstDim, secondDim);
		connLayers(w, prev, next);
		free(w);
	}
	//init first and last layers inputs and outputs here
	int dim = layers[0]->dim;
	struct Neuron *neur;
	for (i = 0; i < dim; i++) {
		neur = layers[0]->n[i];
		neur->e = malloc(sizeof(struct Axon *));
		neur->e[0] = makeAxe(1.0, NULL, neur);
	}
	int last = nbLayers - 1;
	dim = layers[last]->dim;
	for (i = 0; i < dim; i++) {
		neur = layers[last]->n[i];
		neur->s = malloc(sizeof(struct Axon *));
		neur->s[0] = makeAxe(1.0, neur, NULL);
	}

	return n;
}

double* feedVector(int vSize, double *v, struct Network *net) {
	int lSize = net->layers[0]->dim;
	double *layerRes = malloc(sizeof(double) * lSize);
	for (int i = 0; i < net->nbLayers; i++) {
		if (lSize < net->layers[i]->dim) {
			lSize = net->layers[i]->dim;
			layerRes = realloc(layerRes, sizeof(double) * lSize);
		}
		//pass values through this layer
		for (int j = 0; j < lSize; j++) {
			struct Neuron *n = net->layers[i]->n[j];
			//multiply by weights first here!!!

			//then sum
			double sum = n->sum(vSize, v);
			//activation function
			layerRes[j] = n->sigma(sum);
		}
		if (verbose) {
			printf("---------------------------------------------------\n");
			printf("At layer %d the vector values are:\n", i);
			printVector(lSize, layerRes);
		}
		//var replacement makes it recursive?
		v = layerRes;
	}
	if (verbose) {
		printf("Printing results vector\n\n");
		printVector(lSize, v);
	}
	return v;
}