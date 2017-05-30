// HelloNeurones.cpp : Defines the entry point for the console application.
//

#include "HelloNeurones.h"
#include "UnitTests.h"

int main()
{
	seedRand();
	//runTest();
	int layerSize = 3;
	int nbLayers = 2;
	Layer **layers = malloc(sizeof(Layer*) * nbLayers);
	for (int i = 0; i < nbLayers; i++) {
		layers[i] = makeLayer(layerSize, sum, sigma);
	}

	Network *n = (Network *) initNet(nbLayers, layers);

	double in[] = { 0.5 };
	double *res = feedVector(1, &in[0], n);

	for (int i = 0; i < n->nbLayers; i++) {
		printf("Layer %d\n", i);
		printf("---------------------------------------------------\n");
		Layer *layer = n->layers[i];
		int nbNeur = layer->dim;
		for (int j = 0; j < nbNeur; j++) {
			printf("Neuron %d\n", j);
			Neuron *neur = layer->n[j];
			int nbIn = neur->nbIn;
			int nbOut = neur->nbOut;
			printf("Inputs: %d      Outputs: %d\n", nbIn, nbOut);
			printf("---------------------------------------------------\n");
			printf("Input weights:\n");
			int k;
			for (k = 0; k < nbIn; k++) {
				printWeights(nbIn, neur->e);
			}
			printf("---------------------------------------------------\n");
			printf("Output weights:\n");
			for (k = 0; k < nbOut; k++) {
				printWeights(nbOut, neur->s);
			}
		}
	}
	
	printf("Printing results vector\n\n");
	printVector(layerSize, res);


	getchar();
    return 0;
}

void printVector(int size, double *v) {
	//printf("This vector of size %d contains:\n", size);
	printf("[ ");
	int last = size - 1;
	for (int i = 0; i < size; i++) {
		if (i == last) {
			printf("%f ]\n", v[i]);
		}
		else {
			printf("%f, ", v[i]);
		}
	}
}

void printWeights(int size, Axon **axes) {
	printf("[ ");
	int last = size - 1;
	for (int i = 0; i < size; i++) {
		if (i == last) {
			printf("%f ]\n", axes[i]->w);
		}
		else {
			printf("%f, ", axes[i]->w);
		}
	}
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

Network* initNet(int nbLayers, Layer **layers) {
	Network *n = malloc(sizeof(Network));
	n->layers = layers;
	n->nbLayers = nbLayers;
	//sanity check
	if (nbLayers < 2) {
		printf("Cannot make network with less than 2 layers\n");
		return NULL;
	}
	//connect layers
	int firstDim;
	int secondDim;
	int i;
	for (i = 1; i < nbLayers; i++) {
		firstDim = layers[i-1]->dim;
		secondDim = layers[i]->dim;
		double **w = randWeights(firstDim, secondDim);
		connLayers(w, layers[i-1], layers[i]);
	}
	//init first and last layers inputs and outputs here
	int dim = layers[0]->dim;
	Neuron *neur;
	for (i = 0; i < dim; i++) {
		neur = layers[0]->n[i];
		neur->nbIn = 1;
		neur->e = makeAxe(1.0, NULL, neur);
	}
	int last = nbLayers - 1;
	dim = layers[last]->dim;
	for (i = 0; i < dim; i++) {
		neur = layers[last]->n[i];
		neur->nbIn = 1;
		neur->e = makeAxe(1.0, neur, NULL);
	}

	return n;
}

double* feedVector(int vSize, double *v, Network *net) {
	for (int i = 0; i < net->nbLayers; i++) {
		int lSize = net->layers[i]->dim;
		double *layerRes = malloc(sizeof(double) * lSize);
		//pass values through this layer
		for (int j = 0; j < lSize; j++) {
			Neuron *n = net->layers[i]->n[j];
			//multiply by weights first here!!! with sanity check for null entries and null outputs

			//then sum
			double sum = n->sum(vSize, v);
			//activation function
			layerRes[j] = n->sigma(sum);
		}
		printf("---------------------------------------------------\n");
		printf("At layer %d the vector values are:\n", i);
		printVector(lSize, layerRes);
		//var replacement makes it recursive?
		v = layerRes;
	}
	return v;
}