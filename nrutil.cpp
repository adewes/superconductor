/* nrutil.c - supplied by the Numerical Recipes folks;
   minor modifications made by David MacKay.
   Copyright remains with the Numerical Recipes authors */

#include <stdlib.h>
#include <stdio.h>
#include "nrutil.h"
#define NR_END 1

void nrerror(const char error_text[])
{
	fprintf(stderr,"Numerical Recipes run-time error...\n");
	fprintf(stderr,"%s\n",error_text);
	fprintf(stderr,"...now exiting to system...\n");
//	exit(0);
	return;
}

void myerror(const char error_text[])
{
	fprintf(stderr,"%s\n",error_text);
}

double *vector(int nl,int nh)
{
	double *v;
	v=(double *)malloc((unsigned) (nh-nl+1)*sizeof(double));
	if (!v) nrerror("allocation failure in vector()");
	return v-nl;
}

unsigned long *lvector(int nl,int nh)
/* allocate an unsigned long vector with subscript range v[nl..nh] */
{
	unsigned long int *v;
	v=(unsigned long *)malloc((nh-nl+1)*(sizeof(unsigned long int)));
	if (!v) nrerror("allocation failure in vector()");
	return v-nl;
}


int *ivector(int nl,int nh)
{
	int *v;

	v=(int *)malloc((unsigned) (nh-nl+1)*sizeof(int));
	if (!v) nrerror("allocation failure in ivector()");
	return v-nl;
}

unsigned char *cvector(int nl,int nh)
{
	unsigned char *v;

	v=(unsigned char *)malloc((unsigned) (nh-nl+1)*sizeof(unsigned char));
	if (!v) nrerror("allocation failure in ivector()");
	return v-nl;
}

double *dvector(int nl,int nh)
{
	double *v;

	v=(double *)malloc((unsigned) (nh-nl+1)*sizeof(double));
	if (!v) nrerror("allocation failure in dvector()");
	return v-nl;
}



double **matrix(int nrl,int nrh,int ncl,int nch)
{
	int i;
	double **m;

	m=(double **) malloc((unsigned) (nrh-nrl+1)*sizeof(double*));
	if (!m) nrerror("allocation failure 1 in matrix()");
	m -= nrl;

	for(i=nrl;i<=nrh;i++) {
		m[i]=(double *) malloc((unsigned) (nch-ncl+1)*sizeof(double));
		if (!m[i]) nrerror("allocation failure 2 in matrix()");
		m[i] -= ncl;
	}
	return m;
}

double **dmatrix(int nrl,int nrh,int ncl,int nch)
{
	int i;
	double **m;

	m=(double **) malloc((unsigned) (nrh-nrl+1)*sizeof(double*));
	if (!m) nrerror("allocation failure 1 in dmatrix()");
	m -= nrl;

	for(i=nrl;i<=nrh;i++) {
		m[i]=(double *) malloc((unsigned) (nch-ncl+1)*sizeof(double));
		if (!m[i]) nrerror("allocation failure 2 in dmatrix()");
		m[i] -= ncl;
	}
	return m;
}

int **imatrix(int nrl,int nrh,int ncl,int nch)
{
  int i,**m;
  
  m=(int **)malloc((unsigned) (nrh-nrl+1)*sizeof(int*));
  if (!m) nrerror("allocation failure 1 in imatrix()");
  m -= nrl;
  
  for(i=nrl;i<=nrh;i++) {
    m[i]=(int *)malloc((unsigned) (nch-ncl+1)*sizeof(int));
    if (!m[i]) nrerror("allocation failure 2 in imatrix()");
    m[i] -= ncl;
  }
  return m;
}

unsigned char **cmatrix(int nrl,int nrh,int ncl,int nch)
{
  int i;
  unsigned char **m;
  
  m=(unsigned char **)malloc((unsigned) (nrh-nrl+1)*sizeof(unsigned char*));
  if (!m) nrerror("allocation failure 1 in cmatrix()");
  m -= nrl;
  
  for(i=nrl;i<=nrh;i++) {
    m[i]=(unsigned char *)malloc((unsigned) (nch-ncl+1)*sizeof(unsigned char));
    if (!m[i]) nrerror("allocation failure 2 in cmatrix()");
    m[i] -= ncl;
  }
  return m;
}

double **submatrix(double **a,
		  int oldrl,int oldrh,int oldcl,int oldch,
		  int newrl,int newcl)
{
	int i,j;
	double **m;

	m=(double **) malloc((unsigned) (oldrh-oldrl+1)*sizeof(double*));
	if (!m) nrerror("allocation failure in submatrix()");
	m -= newrl;

	for(i=oldrl,j=newrl;i<=oldrh;i++,j++) m[j]=a[i]+oldcl-newcl;

	return m;
}

void free_vector(double *v,int nl,int nh)
{
	free((void*) (v+nl));
}

void free_cvector(unsigned char *v,int nl,int nh)
{
	free((char*) (v+nl));
}

void free_ivector(int *v,int nl,int nh)
{
	free((char*) (v+nl));
}

void free_lvector(long *v,int nl,int nh)
{
	free((char*) (v+nl));
}


void free_dvector(double *v,int nl,int nh)
{
	free((char*) (v+nl));
}


void free_matrix(double **m,int nrl,int nrh,int ncl,int nch)
{
	int i;

	for(i=nrh;i>=nrl;i--) free((void *) (m[i]+ncl));
	free((void *) (m+nrl));
}

void free_dmatrix(double **m,int nrl,int nrh,int ncl,int nch)
{
	int i;

	for(i=nrh;i>=nrl;i--) free((char*) (m[i]+ncl));
	free((char*) (m+nrl));
}

void free_cmatrix(unsigned char **m,int nrl,int nrh,int ncl,int nch)
{
	int i;

	for(i=nrh;i>=nrl;i--) free((char*) (m[i]+ncl));
	free((char*) (m+nrl));
}

void free_imatrix(int **m,int nrl,int nrh,int ncl,int nch)
{
	int i;

	for(i=nrh;i>=nrl;i--) free((char*) (m[i]+ncl));
	free((char*) (m+nrl));
}



void free_submatrix(double **b,int nrl,int nrh,int ncl,int nch)
{
	free((char*) (b+nrl));
}



double **convert_matrix(double *a,int nrl,int nrh,int ncl,int nch)
{
	int i,j,nrow,ncol;
	double **m;

	nrow=nrh-nrl+1;
	ncol=nch-ncl+1;
	m = (double **) malloc((unsigned) (nrow)*sizeof(double*));
	if (!m) nrerror("allocation failure in convert_matrix()");
	m -= nrl;
	for(i=0,j=nrl;i<=nrow-1;i++,j++) m[j]=a+ncol*i-ncl;
	return m;
}



void free_convert_matrix(double **b,int nrl,int nrh,int ncl,int nch)
{
	free((char*) (b+nrl));
}
