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

		if (strcmp("-cfg", argv[i]) == 0) {
			// Guard clause
			if (i + 1 >= argc) {
				printf("Use for '-cfg' option: -cfg <filename>\n");
				exit(-1);
			}
			
			// Parse the given file here
		}

		if (strcmp("-learn", argv[i]) == 0) {
			// Guard clause
			if (i + 1 >= argc) {
				printf("Use for '-learn' option: -learn <filename>\n");
				exit(-1);
			}

		}
	}


	seedRand();
	int layerSize = 3;
	int nbLayers = 5;
	double eta = 0.5;

	struct Layer **layers = malloc(sizeof(struct Layer*) * nbLayers);
	for (int i = 0; i < nbLayers; i++) {
		layers[i] = makeLayer(layerSize, sumVector, sigmoidIntegral);
	}

	struct Network *n = (struct Network *) initNet(nbLayers, layers, eta);

	if (verbose) printNetwork(n);

	double in[] = { 0.5, 0.2, 0.8 };
	feedVector(3, &in[0], n);


	getchar();
    return 0;
}

double sumVector(int d, double *v) {
	double res = 0;
	for (int i = 0; i < d; i++) {
		res += v[i];
	}
	return res;
}



double sigmoid(double sum) {
	return 1 / (1 + exp(-sum));
}

double sigmoidIntegral(double val) {
	return log(1 + exp(val));
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

struct Layer* makeLayer(int dim, double(*sum)(int, double*), double(*activation)(double)) {
	struct Layer *c = malloc(sizeof(struct Layer));
	c->dim = dim;
	struct Neuron **n = malloc(sizeof(struct Neuron*) * dim);
	for (int i = 0; i < dim; i++) {
		n[i] = makeNeur(sum, activation);
	}
	c->n = n;
	return c;
}

struct Axon* makeAxe(double w, struct Neuron* in, struct Neuron* out) {
	struct Axon *a = malloc(sizeof(struct Axon));
	a->w = w;
	a->in = in;
	a->out = out;
	if (in) {
		in->outputs = realloc(in->outputs, sizeof(struct Axon *) * ++(in->nbOut));
		in->outputs[in->nbOut - 1] = a;
	}
	if (out) {
		out->inputs = realloc(out->inputs, sizeof(struct Axon *) * ++(out->nbIn));
		out->inputs[out->nbIn - 1] = a;
	}
	return a;
}

struct Neuron* makeNeur(double(*sum)(int, double*), double(*sigma)(double)) {
	struct Neuron *n = malloc(sizeof(struct Neuron));
	n->sum = sum;
	n->activation = sigma;
	n->inputs = NULL;
	n->nbIn = 0;
	n->outputs = NULL;
	n->nbOut = 0;
	n->result = 0;
	return n;
}


struct Network* initNet(int nbLayers, struct Layer **layers, double eta) {
	struct Network *n = malloc(sizeof(struct Network));
	n->layers = layers;
	n->nbLayers = nbLayers;
	n->eta = eta;
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
		neur->inputs = malloc(sizeof(struct Axon *));
		neur->inputs[0] = makeAxe(1.0, NULL, neur);
	}
	int last = nbLayers - 1;
	dim = layers[last]->dim;
	for (i = 0; i < dim; i++) {
		neur = layers[last]->n[i];
		neur->outputs = malloc(sizeof(struct Axon *));
		neur->outputs[0] = makeAxe(1.0, neur, NULL);
	}

	return n;
}

double* errorWidrowHoff(double *expected, double *res, int dim, double eta) {
	int i = 0;
	for (; i < dim; i++) {
		res[i] = (expected[i] - res[i]) * eta;
	}
	return res;
}

void simpleRetroPropagate(struct Network *net, double* expected) {
	// now we have an array of eta * ( t(j) - o(j) )
	int errorVectorSize = -1;
	double *neuronErrVector = NULL;
	int i = net->nbLayers - 1;
	// Initial calculations for retropropagation
	struct Layer* layer = net->layers[i--];		// i-- because we will need to step over this layer in the loop
	int nbNeur = layer->dim;
	double **layerErrVector = malloc(sizeof(double *) * nbNeur);
	for (int j = nbNeur-1; j > -1; j--) {
		struct Neuron* n = layer->n[j];
		int nbAxons = n->nbIn;

		// handle dynamic allocation
		if (neuronErrVector == NULL) {
			errorVectorSize = nbAxons;
			neuronErrVector = malloc(sizeof(double) * nbAxons);
		}
		if (nbAxons > errorVectorSize) {
			errorVectorSize = nbAxons;
			neuronErrVector = realloc(neuronErrVector, sizeof(double) * nbAxons);
		}

		int k;
		for (k = 0; k < nbAxons; k++) {
			neuronErrVector[k] = sigmoidIntegral(n->result);
		}

		layerErrVector[j] = net->error(expected, neuronErrVector, nbAxons, net->eta);

		for (k = 0; k < nbAxons; k++) {
			double w = n->inputs[k]->w;
			n->inputs[k]->w = w - neuronErrVector[k];
		}
	}

	// we now our initialized error vectors?
	// TODO: iterate the errorvalues back through the axons multiplied by the result of applying
	// the derivated transfer function to the preceding neuron's result

	for (; i > -1; i--) {
		layer = net->layers[i];
		nbNeur = layer->dim;
		for (int j = nbNeur-1; j > -1; j--) {
			struct Neuron* n = layer->n[j];
			int nbAxons = n->nbIn;

			// handle dynamic allocation
			if (neuronErrVector == NULL) {
				errorVectorSize = nbAxons;
				neuronErrVector = malloc(sizeof(double) * nbAxons);
			}
			if (nbAxons > errorVectorSize) {
				errorVectorSize = nbAxons;
				neuronErrVector = realloc(neuronErrVector, sizeof(double) * nbAxons);
			}
			int k;
			for (k = 0; k < nbAxons; k++) {
				neuronErrVector[k] = sigmoidIntegral(n->result);
			}

			neuronErrVector = net->error(expected, neuronErrVector, nbAxons, net->eta);

			for (k = 0; k < nbAxons; k++) {
				double w = n->inputs[k]->w;
				n->inputs[k]->w = w - neuronErrVector[k];
			}
		}
	}
}

void feedVector(int vSize, double *v, struct Network *net) {
	int nbNeurIn = net->layers[0]->dim;
	// sanity check
	if (vSize != nbNeurIn) {
		printf("Initial vector needs to be size of entry layer!\n");
		return;
	}

	// let's do one layer here
	int i = 0, j = 0, k = 0;
	for (; i < nbNeurIn; i++) {
		double value = v[i];
		// keep in mind first layer neurons have just one entry, hence e[0]
		// and reason why we can immediately put the value back in layerRes
		net->layers[0]->n[i]->result = net->layers[0]->n[i]->activation(value * net->layers[0]->n[i]->inputs[0]->w);
	}
	// this was all just to understand the logic, because first layer weights should always be 1.0 right?

	// init stack values:
	int nbNeurOut, nbEntryAxons;

	// initialize results storage array
	int prevLayerSize = net->layers[0]->dim;
	double *tempVector = malloc(sizeof(double) * prevLayerSize);
	struct Layer *nextLayer;

	// init at 1 because first layer already entered
	for (i = 1; i < net->nbLayers; i++) {
		nextLayer = net->layers[i];
		nbNeurOut = nextLayer->dim;

		//pass values through this layer's neurons
		for (k = 0; k < nbNeurOut; k++) {
			struct Neuron *target = nextLayer->n[k];
			nbEntryAxons = target->nbIn;

			// make sure tempVector is large enough for the results
			if (nbEntryAxons > prevLayerSize) {
				tempVector = realloc(tempVector, sizeof(double) * nbEntryAxons);
			}

			for (j = 0; j < nbEntryAxons; j++) {
				struct Axon *axe = target->inputs[j];
				double weight = axe->w;
				double prevRes = axe->in->result;
				tempVector[j] = (weight * prevRes);
			}
			//then sum
			double sum = target->sum(nbEntryAxons, tempVector);
			//activation function
			target->result = target->activation(sum);
		}
		if (verbose) {
			printf("---------------------------------------------------\n");
			printf("At layer %d the vector values are:\n", i);
			printNeuronResults(nextLayer->dim, nextLayer->n);
		}
	}
	if (verbose) {
		printf("Reached end of network\n");
	}
	free(tempVector);
}