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

void writeInt(char*, int*, int);
void writeConnectBitmask(char* buf, int* offset, struct Layer* target, struct Axon** conn, int nbAxons, enum Direction dir);
void writeDouble(char*, int*, double);
int sizeOfNetwork(struct Network* net);