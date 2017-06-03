
#include "Utils.h"

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

void printWeights(int size, struct Axon **axes) {
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

void printWBetweenLayers(double **w, int firstDim, int sndDim) {
	for (int i = 0; i < firstDim; i++) {
		printf("!!! Printing w[%d] after random generation\n", i);
		printVector(sndDim, w[i]);
	}
}

void trace(char *funcName, int vSize, double *v) {
	printf("!!! From %s() we print an array of random doubles:\n", funcName);
	printVector(vSize, v);
}

void printNetwork(struct Network *n) {
	struct Neuron *neur;

	for (int i = 0; i < n->nbLayers; i++) {
		printf("struct Layer %d\n", i);
		printf("---------------------------------------------------\n");
		struct Layer *layer = n->layers[i];
		int nbNeur = layer->dim;
		for (int j = 0; j < nbNeur; j++) {
			printf("struct Neuron %d\n", j);
			neur = layer->n[j];
			int nbIn = neur->nbIn;
			int nbOut = neur->nbOut;
			printf("Inputs: %d      Outputs: %d\n", nbIn, nbOut);
			printf("---------------------------------------------------\n");
			printf("In struct Axon struct address: %p\n", neur->e);
			printf("Input weights:\n");
			printWeights(nbIn, neur->e);
			printf("---------------------------------------------------\n");
			printf("Out struct Axon struct address: %p\n", neur->s);
			printf("Output weights:\n");
			printWeights(nbOut, neur->s);
		}
	}
}