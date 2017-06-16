#include "UnitTests.h"
#include "HelloNeurones.h"
#include "Utils.h"

double randfrom(double min, double max)
{
	double range = (max - min);
	double div = RAND_MAX / range;
	return min + (rand() / div);
}

void seedRand() {
	srand((unsigned int)time(NULL));
}

double* randW(int dim) {
	double *res = malloc(sizeof(double) * dim);
	for (int i = 0; i < dim; i++) {
		res[i] = randfrom(0, 1);
	}
	return res;
}

double** randWeights(int inDim, int outDim) {
	double **res = malloc(sizeof(double*) * inDim);
	for (int i = 0; i < inDim; i++) {
		res[i] = randW(outDim);
	}
	return res;
}

int testWeights(double **w, struct Layer *coucheFrom, struct Layer *coucheTo) {
	for (int i = 0; i < coucheFrom->dim; i++) {
		for (int j = 0; j < coucheTo->dim; j++) {
			if (w[i][j] != coucheFrom->n[i]->outputs[j]->w) {
				return -1;
			}
			if (w[i][j] != coucheTo->n[j]->inputs[i]->w) {
				return 1;
			}
		}
	}
	return 0;
}

//make the run dynamic
void runTest(/*int nb, int *dims*/) {
	srand((unsigned int)time(NULL));
	//for (int i = 0; i < nb; i++) {	}
	double **test = randWeights(3, 4);
	struct Layer *first = makeLayer(3, sumVector, sigmoid);
	struct Layer *second = makeLayer(4, sumVector, sigmoid);
	connLayers(test, first, second);

	int testRes = testWeights(test, first, second);
	switch (testRes)
	{
	case 1:
		printf("Incoherent entry\n");
	case -1:
		printf("Incoherent out\n");
	default:
		printf("Test passed\n");
		break;
	}
}