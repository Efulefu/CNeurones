#pragma once

#include "HelloNeurones.h"

/*
	Let's define constants here
*/

const char Version = 0x01;

enum Direction {
	BACKWARDS,
	FORWARDS
};

enum Activation {
	SIGMOID = 0x00,
	SOFTPLUS = 0x01,
	LEAKYRELU = 0x02
};

enum Objects {
	LAYER = 0x00,
	NEURON = 0x01
};

enum Sum {
	SUM
};

void saveNetwork(const char* filename, struct Network* net);
void writeInt(char*, int*, const int);
void writeConnectBitmask(char* buf, int* offset, const struct Layer* target, const struct Axon** conn, const int nbAxons, const enum Direction dir);
void writeDouble(char*, int*, const double);
int sizeOfNetwork(const struct Network* net);
int sizeOfBitmask(const int n);
void writeBufferToFile(const char* fname, int buffc, char* buffer);