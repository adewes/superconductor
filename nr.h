#include"nrutil.h"
#include<math.h>
#include<stdlib.h>
#include<iostream>
#include<stdio.h>
#define NR 1
double ran1(long *idum);
double expdev(long *idum);
double gasdev(long *idum);
void realft(double data[], unsigned long n, int isign);
void amoeba(double **p, double y[], int ndim, double ftol,double (*funk)(double []), int *nfunk);
void tqli(double d[], double e[], int n, double **z);
void tred2(double **a, int n, double d[], double e[]);
double pythag(double a, double b);
void lubksb(double **a, int n, int *indx, double b[]);
void ludcmp(double **a, int n, int *indx, double *d);
void mnewt(int ntrial, double x[], int n, double tolx, double tolf,void (*usrfun)(double [],int n,double *fvec,double **fjac));
void lubksb(double **a, int n, int *indx, double b[]);
void elmhes(double **a, int n);
void hqr(double **a, int n, double wr[], double wi[]);
void balanc(double **a, int n);
void jacobi(double **a, int n, double d[], double **v, int *nrot);
void sprsin(double **a,int **map, int n, double thresh, unsigned long nmax, double sa[],
	unsigned long ija[]);
void linbcg(unsigned long n, double b[], double x[], int itol, double tol,
	int itmax, int *iter, double *err,double sa[],unsigned long ija[],double *inv_vec,unsigned long *inv_ind);
void linbcg_single(unsigned long n, double b[], double x[], int itol, double tol,
	int itmax, int *iter, double *err,double sa[],unsigned long ija[],double *inv_vec,unsigned long *inv_ind);
void atimes(unsigned long n, double x[], double r[], int itrnsp,double sa[],unsigned long ija[]);
double snrm(unsigned long n, double sx[], int itol);
void asolve(unsigned long n, double b[], double x[], int itrnsp,double sa[],double *inv_vec,unsigned long *inv_ind);
void dsprsax(double sa[], unsigned long ija[], double x[], double b[], unsigned long n);
void dsprstx(double sa[], unsigned long ija[], double x[], double b[], unsigned long n);
void gauss_elimination(double **a,int n);
void full_gauss_elimination(double **a,double **b,int n);
void gaussj(double **a, int n, double **b, int m);


#define PI 3.14159265
