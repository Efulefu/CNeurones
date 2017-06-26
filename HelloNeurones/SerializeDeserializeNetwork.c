#include "SerializeDeserializeNetwork.h"

void saveNetwork(struct Network* net) {
	char* buffer = malloc(sizeof(char) * sizeOfNetwork(net));
	int offset = 0;

	buffer[offset++] = Version;
	int nbLayers = net->nbLayers;
	writeInt(buffer, &offset, nbLayers);
	writeDouble(buffer, &offset, net->eta);
	// layers for loop
	for (int i = 0; i < nbLayers; i++) {
		buffer[offset++] = LAYER;
		struct Layer* l = net->layers[i];
		int nbNeur = l->dim;
		writeInt(buffer, &offset, nbNeur);
		// neurons for loop
		for (int j = 0; j < nbNeur; j++) {
			buffer[offset++] = NEURON;
			struct Neuron* n = l->n[j];
			writeDouble(buffer, &offset, n->result);
			serializeActivation(buffer, &offset, &(n->activation));
			writeInt(buffer, &offset, n->nbIn);
			// write pointer comparison logic to make the connection bitmask
			// write weights array here in the for loop:
			int k = 0;
			for (; k < n->nbIn; k++) {
				writeDouble(buffer, &offset, n->inputs[k]->w);
			}
			writeInt(buffer, &offset, n->nbOut);
			// reuse pointer comparison logic here
			for (k = 0; k < n->nbOut; k++) {
				writeDouble(buffer, &offset, n->outputs[k]->w);
			}
		}
	}
	// write to file here!
	// add an EOF code?
}

void serializeActivation(char* buf, int* offset, struct FuncAndDerivative* f) {
	if (f == &Sigmoid) {
		buf[*offset++] = SIGMOID;
	}
	else if (f == &SoftPlus) {
		buf[*offset++] = SOFTPLUS;
	}
	else if (f == &LeakyRelu) {
		buf[*offset++] = LEAKYRELU;
	}
	else {
		printf("Woops! Error in serializing activation func!\n");
	}
}

void writeInt(char* buf, int* offset, int n) {
	buf[*offset++] = (n >> 24) & 0xff;
	buf[*offset++] = (n >> 16) & 0xff;
	buf[*offset++] = (n >> 8) & 0xff;
	buf[*offset++] = n & 0xff;
}

void writeDouble(char* buf, int* offset, double d) {
	memcpy(buf[*offset], &d, sizeof(double));
	*offset += sizeof(double);
}

int sizeOfNetwork(struct Network* net) {
	// header info 
	// version in 8 bits, nbLayers 32 bits, eta 64 bits
	int netHeaderSize = 13;
	int neurHeaderSize = 11;
	int layerSize = 5;

	int res = netHeaderSize;

	int nbLayers = net->nbLayers;

	for (int i = 0; i < nbLayers; i++) {
		res += layerSize;
		struct Layer* l = net->layers[i];
		int nbNeur = l->dim;
		for (int j = 0; j < nbNeur; j++) {
			struct Neuron* n = l->n[j];
			res += neurHeaderSize;
			int nbIn = n->nbIn;
			int nbOut = n->nbOut;
			// calculate space for in and out bitmasks
			res += sizeOfBitmask(nbIn);
			res += sizeOfBitmask(nbOut);
			res += nbIn * sizeof(double);
			res += nbOut * sizeof(double);
		}
	}
	return res;
}

int sizeOfBitmask(int n) {
	int roundUp = n % 8;
	int res = roundUp ? n / 8 + 1 : n / 8;
	return res;
}