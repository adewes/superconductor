/* nrutil.h - supplied by the Numerical Recipes folks Press et al ;
   minor modifications made by David MacKay.
   Copyright remains with the Numerical Recipes authors */

/* #include 	"dbmalloc/malloc.h" */

/* #include "smartall.h" This piece of shit didn't work for me */
/*
   These are the NR routines (modified by me) that are in the 
   file nrutil.c

   Other NR routines are specified in mynr.h
   See also macopt.h 
*/
#include<iostream>

using namespace std;

double *vector(int,int);
double **matrix(int,int,int,int);
double **convert_matrix(double *,int,int,int,int);
double *dvector(int,int);
double **dmatrix(int,int,int,int);
int *ivector(int,int);
int **imatrix(int,int,int,int);
unsigned char *cvector(int,int);
unsigned char **cmatrix(int,int,int,int);
double **submatrix(double **,int,int,int,int,int,int);
void free_vector(double *,int,int);
void free_dvector(double *,int,int);
void free_cvector(unsigned char *,int,int);
void free_cmatrix(unsigned char **,int,int,int,int);
void free_ivector(int *,int,int);
void free_lvector(unsigned long *v,int nl,int nh);
void free_matrix(double**,int,int,int,int);
void free_dmatrix(double **, int,int,int,int);
void free_imatrix(int **,int,int,int,int);
void free_submatrix(double**,int,int,int,int);
void free_convert_matrix(double**,int,int,int,int);
void nrerror(const char error_text[]);
unsigned long *lvector(int nl, int nh);

#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))

static int imaxarg1,imaxarg2;
#define IMAX(a,b) (imaxarg1=(a),imaxarg2=(b),(imaxarg1) > (imaxarg2) ?\
        (imaxarg1) : (imaxarg2))


static double dsqrarg;
#define SQR(a) ((sqrarg=(a)) == 0.0 ? 0.0 : sqrarg*sqrarg)

static double sqrarg;
#define SQR(a) ((sqrarg=(a)) == 0.0 ? 0.0 : sqrarg*sqrarg)
