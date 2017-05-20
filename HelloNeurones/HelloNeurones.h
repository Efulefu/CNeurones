#pragma once

typedef struct Neurone Neurone;
typedef struct Axone Axone;
typedef struct Couche Couche;

double sum(int, double*);
double sigma(double);

// init functions
Couche* makeLayer(int depth, int dim, double(*sum)(int, double*), double(*sigma)(double));
Axone* makeAxe(double w, Neurone* in, Neurone* out);
Neurone* makeNeur(double(*sum)(int, double*), double(*sigma)(double));

// handling functions
Axone*** connLayers(double **w, Couche* in, Couche* out);
