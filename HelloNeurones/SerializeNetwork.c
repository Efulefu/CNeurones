#include "SerializeNetwork.h"

void saveNetwork(const char* filename, struct Network* net) {
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

			if (i > 0) // inputs
				writeConnectBitmask(buffer, &offset, net->layers[i - 1], n->inputs, n->nbIn, BACKWARDS);

			// write weights array here in the for loop:
			int k = 0;
			for (; k < n->nbIn; k++) {
				writeDouble(buffer, &offset, n->inputs[k]->w);
			}
			writeInt(buffer, &offset, n->nbOut);

			if (i < nbLayers - 1) // outputs
				writeConnectBitmask(buffer, &offset, net->layers[i + 1], n->outputs, n->nbOut, FORWARDS);

			// reuse pointer comparison logic here
			for (k = 0; k < n->nbOut; k++) {
				writeDouble(buffer, &offset, n->outputs[k]->w);
			}
		}
	}

	if (offset > sizeOfNetwork) {
		// TODO: Error utils!
		printf("Serialization write buffer length got exceeded! Wtf?\n");
		printf("The expected necessary buffer size was %d!\n", sizeOfNetwork);
		printf("But we wrote into the buffer up to offset: %d\n", offset);
		free(buffer);
		return;
	}

	writeBufferToFile(filename, offset, buffer);
	free(buffer);
}

void writeBufferToFile(const char* fname, const int buffc, const char* buffer) {
	FILE* saveFile = fopen(fname, "wb+");
	size_t written = fwrite(buffer, sizeof(char), (size_t)buffc, saveFile);
	if (written != buffc)
	{
		// TODO: handle error
	}
	int err = fclose(saveFile);
	if (err != 0)
	{
		// TODO: handle error
	}
}

void writeConnectBitmask(char* buf, int* offset, const struct Layer* targetLayer, const struct Axon** connections, const int nbAxons, const enum Direction dir) {
	int nbTargets = targetLayer->dim;
	int bitOffset = 0;
	buf[*offset] = 0;
	for (int j = 0; j < nbAxons; j++) {
		struct Axon* conn = connections[j];
		for (int i = 0; i < nbTargets; i++) {
			struct Neuron* targetNeur = targetLayer->n[i];
			
			if ((dir == BACKWARDS && conn->in == targetLayer->n[i]) || (dir == FORWARDS && conn->out == targetLayer->n[i]))
			{
				// flip a bit here
				buf[*offset] += 1 << bitOffset++;
			}
			else
			{
				// leave a 0 here
				bitOffset++;
			}

			if (bitOffset % 8 == 0) {
				bitOffset = 0;
				buf[++*offset] = 0;
			}
			
		}
	}
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

void writeInt(char* buf, int* offset, const int n) {
	buf[*offset++] = (n >> 24) & 0xff;
	buf[*offset++] = (n >> 16) & 0xff;
	buf[*offset++] = (n >> 8) & 0xff;
	buf[*offset++] = n & 0xff;
}

void writeDouble(char* buf, int* offset, const double d) {
	memcpy(&buf[*offset], &d, sizeof(double));
	*offset += sizeof(double);
}

int sizeOfNetwork(const struct Network* net) {
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
			if (i > 0) res += sizeOfBitmask(net->layers[i - 1]->dim);

			if (i < nbLayers - 1) res += sizeOfBitmask(net->layers[i + 1]->dim);

			res += nbIn * sizeof(double);
			res += nbOut * sizeof(double);
		}
	}
	return res;
}

int sizeOfBitmask(const int n) {
	int roundUp = n % 8;
	int res = roundUp ? n / 8 + 1 : n / 8;
	return res;
}