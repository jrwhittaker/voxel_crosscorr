
#include "crosscorr.h"

float voxel_corr(float *vin, int nxyz, float *lin, int nt)
{

  float vin_mean=0, vin_std, lin_mean=0, lin_std, prod_mean=0;

  for (int i=0; i<nt; i++)
    vin_mean += *(vin + i*nxyz);
  vin_mean /= nt;

  for (int i=0; i<nt; i++)
    lin_mean += *(lin + i);
  lin_mean /= nt;

  for (int i=0; i<nt; i++)
    prod_mean += *(vin + i*nxyz) * *(lin + i);
  prod_mean /= nt;

  float vin_squares[nt], vin_squares_mean=0;
  for (int i=0; i<nt; i++)
    vin_squares[i]=pow(*(vin + i*nxyz),2);
  for (int i=0; i<nt; i++)
    vin_squares_mean += *(vin_squares + i);
  vin_squares_mean /= nt;
  vin_std=sqrt(vin_squares_mean-pow(vin_mean,2));

  float lin_squares[nt], lin_squares_mean=0;
  for (int i=0; i<nt; i++)
    lin_squares[i]=pow(*(lin + i),2);
  for (int i=0; i<nt; i++)
    lin_squares_mean += *(lin_squares + i);
  lin_squares_mean /= nt;
  lin_std=sqrt(lin_squares_mean-pow(lin_mean,2));

  float cov, rho;
  cov = prod_mean - (vin_mean * lin_mean);
  rho = cov / (vin_std * lin_std);

  return rho;

}

/* poly fit function - https://rosettacode.org/wiki/Polynomial_regression#C */

float polynomialfit(int obs, int degree, float *dy, float *store)
{

  gsl_multifit_linear_workspace *ws;
  gsl_matrix *cov, *X;
  gsl_vector *y, *c;
  double chisq;

  double x[obs], stepSize=2.0/((double) obs - 1);
  int i, j;

  for (i=0; i<obs; i++)
    x[i]=-1+i*stepSize;

  X = gsl_matrix_alloc(obs, (degree+1));
  y = gsl_vector_alloc(obs);
  c = gsl_vector_alloc((degree+1));
  cov = gsl_matrix_alloc((degree+1),(degree+1));

  /* Legendre polynomials */
  for (i=0; i<obs; i++){
    for (j=0; j<(degree+1); j++){
      if (j == 0)
	{
	  gsl_matrix_set(X,i,j,1.0);
	}
      else if (j == 1)
	{
	  gsl_matrix_set(X,i,j,x[i]);
	}
      else
	{
	  gsl_matrix_set(X,i,j,((2*j-1)*x[i]*gsl_matrix_get(X,i,j-1)-(j-1)*gsl_matrix_get(X,i,j-2))/j);
	}
    }
    gsl_vector_set(y,i,(double) dy[i]);
  }

  /* raw polynomials */
  /*  for (i=0; i<obs; i++){
    for (j=0; j<(degree+1); j++){
      gsl_matrix_set(X,i,j,pow((double) dx[i],j));
    }
    gsl_vector_set(y,i,(double) dy[i]);
    }*/

  ws = gsl_multifit_linear_alloc(obs,(degree+1));
  gsl_multifit_linear(X,y,c,cov,&chisq,ws);

  /* store coefficients */
  for (i=0; i<(degree+1); i++)
    {
      store[i] = (float) gsl_vector_get(c,i);
    }

  gsl_multifit_linear_free(ws);
  gsl_matrix_free(X);
  gsl_matrix_free(cov);
  gsl_vector_free(y);
  gsl_vector_free(c);

  //return true;
  return (float) chisq;

}

bool polynomialeval(int obs, int degree, float *coeffs, float *polyout)
{

  gsl_matrix *X;
  float x[obs], stepSize=2.0 /((double) obs - 1);
  int i, j;

  X = gsl_matrix_alloc(obs,degree+1);

  for(i=0; i<obs; i++)
    x[i]=-1+i*stepSize;

  for (i=0; i<obs; i++){
    for (j=0; j<(degree+1); j++){
      if (j == 0)
	{
	  gsl_matrix_set(X,i,j,1.0);
	}
      else if (j == 1)
	{
	  gsl_matrix_set(X,i,j,x[i]);
	}
      else
	{
	  gsl_matrix_set(X,i,j,((2*j-1)*x[i]*gsl_matrix_get(X,i,j-1)-(j-1)*gsl_matrix_get(X,i,j-2))/j);
	}
    }
  }

  double tmp=0.0;
  for (i=0; i<obs; i++){
    for (j=0; j<(degree+1); j++){
      tmp += gsl_matrix_get(X,i,j) * (double) coeffs[j];
    }
    polyout[i] = (float) tmp;
    tmp=0.0;
  }

  gsl_matrix_free(X);
  return true;
 }

float calc_rsqr(float *y, float rss, int n)
{

  float ymean=0.0, tss=0.0, rsqr=0.0;
  for (int i=0; i<n; i++)
    ymean += *(y + i) / n;
  for (int i=0; i<n; i++)
    tss += sqr(*(y + i) - ymean);

  rsqr = 1.0 - (rss / tss);
  return rsqr;
} 

int maxabs(int n, float *y)
{

  int maxidx=0;
  for (int i=0; i<n; i++)
    {
      if (fabsf(*(y+i)) > fabsf(*(y+maxidx)))
	maxidx = i;
    }

  return maxidx;
}

int cmax(int n, float *y)
{

  int maxidx=0;
  for (int i=0; i<n; i++)
    {
      if (*(y+i) > *(y+maxidx))
	maxidx=i;
    }

  return maxidx;
}

int cmin(int n, float *y)
{

  int minidx=0;
  for (int i=0; i<n; i++)
    {
      if (*(y+i) < *(y+minidx))
	minidx=i;
    }

  return minidx;
}
