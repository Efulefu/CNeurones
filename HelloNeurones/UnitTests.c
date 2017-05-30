#include <stdlib.h>
#include "UnitTests.h"

double randfrom(double min, double max)
{
	double range = (max - min);
	double div = RAND_MAX / range;
	return min + (rand() / div);
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
		res[i] = malloc(sizeof(double) * outDim);
		for (int j = 0; j < outDim; j++) {
			res[i][j] = randfrom(0, 1);
		}
	}
	return res;
}

int testWeights(double **w, Layer *coucheFrom, Layer *coucheTo) {
	for (int i = 0; i < coucheFrom->dim; i++) {
		for (int j = 0; j < coucheTo->dim; j++) {
			if (w[i][j] != coucheFrom->n[i]->s[j]->w) {
				return -1;
			}
			if (w[i][j] != coucheTo->n[j]->e[i]->w) {
				return 1;
			}
		}
	}
	return 0;
}

//make the run dynamic
void runTest(/*int nb, int *dims*/) {
	srand(time(NULL));
	//for (int i = 0; i < nb; i++) {	}
	double **test = randWeights(3, 4);
	Layer *first = makeLayer(3, sum, sigma);
	Layer *second = makeLayer(4, sum, sigma);
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