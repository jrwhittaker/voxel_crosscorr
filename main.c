
#include "nifti2_io.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "argparse.h"
#include "crosscorr.h"

/* ------------------------
   -------- MAIN ----------
   ----------------------- */

int main(int argc, char *argv[])
{

  nifti_image *nim_input=NULL, *nim_mask=NULL;
  char *nin=NULL, *nmask=NULL, *flag=NULL, *spref=NULL;
  float limits[2];
  char pname[256]={'\0'};
  int idx;
  int max=1;

  /* get arguments */
  if (!get_arg(argc, argv, "-input", 6, &nin))
    printf("\n++ -input is %s ++\n",nin);
  if (!get_arg(argc, argv, "-prefix", 7, &spref))
    printf("\n++ -prefix is %s ++\n",spref);
  if (!get_arg(argc, argv, "-mask", 5, &nmask))
    printf("\n++ -mask is %s ++\n",nmask);
  if (!get_arg(argc, argv, "-lagfile", 8, &flag))
    printf("\n++ -lagfile is %s ++\n",flag);
  //if (!get_arg_array_float(argc, argv, "-limits", 7, &limits))
  //  printf("\n++ -limits are %5.2f and %5.2f ++\n", &limits, limits); 

  strcpy(pname,spref);;

  printf("\nload data\n");
  nim_input = nifti_image_read(nin,1);
  nim_mask = nifti_image_read(nmask,1);

  if (nim_input->datatype != 16 || nim_mask->datatype != 2)
    ERREX("Input data needs to be FLOAT and mask data needs to be byte");

  /* get dimensions of input */
  int nt = nim_input->nt;
  int nx = nim_input->nx;
  int ny = nim_input->ny;
  int nz = nim_input->nz;
  int nxy = nim_input->nx * nim_input->ny;
  int nxyz = nim_input->nx * nim_input->ny * nim_input->nz;

  /* get access to the data */
  float *nim_input_data = nim_input->data;
  unsigned char *nim_mask_data = nim_mask->data;

  FILE *lagfile;
  lagfile=fopen(flag,"r");
  if (lagfile == NULL)
    printf("\n**Could not open file %s\n",flag),exit(1);


  /* calculate the number of rows and columns in text file */
  int c, temp=0, numCols=0, numRows=0;
  do {
    c = getc(lagfile);
    if ((temp != 2) && (c == ' ' || c == '\t' || c == '\n'))
      numCols++;
    if (c == '\n')
      {
	temp = 2;
	numRows++;
      }
      } while (c != EOF);

  rewind(lagfile);

  printf("\nColumns = %d and Rows = %d\n",numCols,numRows);

  float mat[numRows][numCols];
  for (int i=0; i<numRows; i++){
    for (int j=0; j<numCols; j++){
      fscanf(lagfile,"%f",&mat[i][j]);
    }
  }

  fclose(lagfile);

  /* create lag vector */
  /* float lagvec[numCols], lagStep;
  lagStep = (*(limits + 1) - *limits) / ((float) numCols - 1.0);
  for (int i=0; i<numCols; i++)
    lagvec[i] = *limits+i*lagStep; */
  int lagidx[numCols];
  for (int i=0; i<numCols; i++)
    lagidx[i] = i + 1;

  /* allocated memory for output data */
  nifti_image *nim_out_poly = nifti_copy_nim_info(nim_input);
  nim_out_poly->data = calloc((nxyz*numCols),sizeof(float));
  float *nim_out_poly_data = nim_out_poly->data;
  nim_out_poly->nt = numCols;
  nim_out_poly->dim[4] = numCols;
  nim_out_poly->nvox = (nxyz * numCols);

  nifti_image *nim_out_cc = nifti_copy_nim_info(nim_input);
  nim_out_cc->data = calloc((nxyz),sizeof(float));
  float *nim_out_cc_data = nim_out_cc->data;
  nim_out_cc->nt = 1;
  nim_out_cc->dim[4] = 1;
  nim_out_cc->nvox = nxyz;

  nifti_image *nim_out_lag = nifti_copy_nim_info(nim_out_cc);
  nim_out_lag->data = calloc(nxyz,sizeof(float));
  float *nim_out_lag_data = nim_out_lag->data;

  float vec[nt], cc[numCols], r2, rss, coeffs[10], polyout[numCols];
  int polydeg, maxlag;

  printf("\nCalculating voxelwise cross-correlation...\n");
  int count=0;

  for (int islice=0; islice<nz; islice++){
    for (int iy=0; iy<nx; iy++){
      for (int ix=0; ix<ny; ix++){

	idx = nxy*islice + nx*ix + iy;

	if (count % 50000 == 0)
	  printf("\t...voxel %d of %d\n",count,nxyz);

	if ( *(nim_mask_data + idx) > 0 )
	  {
	    for (int m=0; m<numCols; m++)
	      {
		for (int n=0; n<nt; n++)
		  vec[n] = *(*(mat+n)+m);

		cc[m] = voxel_corr((nim_input_data + idx),nxyz,vec,nt);
		//*(nim_out_cc_data + idx + m*nxyz) = cc[m]
	      }
	    printf("%6.3f\n %6.3f %6.3f %6.3f",cc[0],cc[5],cc[10],cc[20]);

	    polydeg=0;
	    for (int i=0; i<10; i++)
	      coeffs[i]=0.0;
	    do {
	      rss=polynomialfit(numCols,polydeg,cc,coeffs);
	      polynomialeval(numCols,polydeg,coeffs,polyout);
	      r2 = calc_rsqr(cc,rss,numCols);
	      polydeg++;
	    } while (r2 < 0.95 && polydeg < 10);

	    if (max == 0 )
	      {
		maxlag = maxabs(numCols,polyout);
	      }
	    else if (max == 1)
	      {
		maxlag = cmax(numCols,polyout);
	      }
	    else
	      {
		maxlag = cmin(numCols,polyout);
	      }
	    
   	    //printf("\n%d %6.3f\n",maxlag,cc[maxlag]);
	    *(nim_out_lag_data + idx) = lagidx[maxlag]; //lagvec[maxlag];
	    *(nim_out_cc_data + idx) = cc[maxlag];

	    for (int m=0; m<numCols; m++)
	      *(nim_out_poly_data + idx + m*nxyz) = polyout[m];

	  }
	else
	  {
	    //for (int m=0; m<numCols; m++)
	      //*(nim_out_cc_data + idx + m*nxyz) = 0.0;
	    for (int m=0; m<numCols; m++)
	      *(nim_out_poly_data + idx + m*nxyz) = 0.0;
	    *(nim_out_lag_data + idx) = 0.0;
	    *(nim_out_cc_data + idx) = 0.0;
	  }

	count++;
      }
    }
  }

  printf("\nhere now\n");

  size_t prefixLength = strlen(pname);
  char fout_cc[prefixLength+10];
  sprintf(fout_cc,"%s.cc.nii.gz",pname);

  char fout_poly[prefixLength+12];
  sprintf(fout_poly,"%s.poly.nii.gz",pname);

  char fout_lag[prefixLength+11];
  sprintf(fout_lag,"%s.lag.nii.gz",pname);

  if (nifti_set_filenames(nim_out_cc,fout_cc,1,1)) return 1;
  nifti_image_write(nim_out_cc);
  if (nifti_set_filenames(nim_out_poly,fout_poly,1,1)) return 1;
  nifti_image_write(nim_out_poly);
  if (nifti_set_filenames(nim_out_lag,fout_lag,1,1)) return 1;
  nifti_image_write(nim_out_lag);

}





















