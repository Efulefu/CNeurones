#include <stdlib.h>
#include "UnitTests.h"

double randfrom(double min, double max)
{
	double range = (max - min);
	double div = RAND_MAX / range;
	return min + (rand() / div);
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

void runTest() {
	srand(time(NULL));
	double **test = randWeights(3, 4);
	Layer *first = makeLayer(0, 3, sum, sigma);
	Layer *second = makeLayer(1, 4, sum, sigma);
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