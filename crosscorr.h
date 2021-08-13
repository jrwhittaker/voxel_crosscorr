
#include <math.h>
#include <stdbool.h>
#include <gsl/gsl_multifit.h>

#define sqr(x) ((x)*(x))

float voxel_corr(float *vin, int nxyz, float *lin, int nt);
float polynomialfit(int obs, int degree, float *dy, float *store);
bool polynomialeval(int obs, int degree, float *coeffs, float *polyout);
float calc_rsqr(float *y, float rss, int n);
int maxabs(int n, float *y);
int cmax(int n, float *y);
int cmin(int n, float *y);
