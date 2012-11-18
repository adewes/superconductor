#include"nr.h"

#define TINY 1.0e-10
#define NMAX 5000
#define GET_PSUM \
for (j=1;j<=ndim;j++) {\
for (sum=0.0,i=1;i<=mpts;i++) sum += p[i][j];\
	    psum[j]=sum;}
#define SWAP(a,b) {swap=(a);(a)=(b);(b)=swap;}
    void amoeba(double **p, double y[], int ndim, double ftol,
		double (*funk)(double []), int *nfunk)
{
    double amotry(double **p, double y[], double psum[], int ndim,
		double (*funk)(double []), int ihi, double fac);
    int i,ihi,ilo,inhi,j,mpts=ndim+1;
    double rtol,sum,swap,ysave,ytry,*psum;
    psum=vector(1,ndim);
    *nfunk=0;
    GET_PSUM
	    for (;;) {
	ilo=1;
	ihi = y[1]>y[2] ? (inhi=2,1) : (inhi=1,2);
	for (i=1;i<=mpts;i++) {
	    if (y[i] <= y[ilo]) ilo=i;
	    if (y[i] > y[ihi]) {
		inhi=ihi;
		ihi=i;
	    } else if (y[i] > y[inhi] && i != ihi) inhi=i;
	}
	rtol=2.0*fabs(y[ihi]-y[ilo])/(fabs(y[ihi])+fabs(y[ilo])+TINY);
	if (rtol < ftol) {
	    SWAP(y[1],y[ilo])
		    for (i=1;i<=ndim;i++) SWAP(p[1][i],p[ilo][i])
		    break;
	}
        if (*nfunk >= NMAX){ nrerror("NMAX exceeded");return;}
	*nfunk += 2;
	ytry=amotry(p,y,psum,ndim,funk,ihi,-1.0);
	if (ytry <= y[ilo])
	    ytry=amotry(p,y,psum,ndim,funk,ihi,2.0);
	else if (ytry >= y[inhi]) {
	    ysave=y[ihi];
	    ytry=amotry(p,y,psum,ndim,funk,ihi,0.5);
	    if (ytry >= ysave) {
		for (i=1;i<=mpts;i++) {
		    if (i != ilo) {
			for (j=1;j<=ndim;j++)
			    p[i][j]=psum[j]=0.5*(p[i][j]+p[ilo][j]);
			y[i]=(*funk)(psum);
		    }
		}
		*nfunk += ndim;
		GET_PSUM
	    }
	} else --(*nfunk);
	    }
	    free_vector(psum,1,ndim);
}
double amotry(double **p, double y[], double psum[], int ndim,
	     double (*funk)(double []), int ihi, double fac)
{
    int j;
    double fac1,fac2,ytry,*ptry;
    ptry=vector(1,ndim);
    fac1=(1.0-fac)/ndim;
    fac2=fac1-fac;
    for (j=1;j<=ndim;j++) ptry[j]=psum[j]*fac1-p[ihi][j]*fac2;
    ytry=(*funk)(ptry);
    if (ytry < y[ihi]) {
	y[ihi]=ytry;
	for (j=1;j<=ndim;j++) {
	    psum[j] += ptry[j]-p[ihi][j];
	    p[ihi][j]=ptry[j];
	}
    }
    free_vector(ptry,1,ndim);
    return ytry;
}


void lubksb(double **a, int n, int *indx, double b[])
{
    int i,ii=0,ip,j;
    double sum;

    for (i=1;i<=n;i++) {
        ip=indx[i];
        sum=b[ip];
        b[ip]=b[i];
        if (ii)
            for (j=ii;j<=i-1;j++) sum -= a[i][j]*b[j];
        else if (sum) ii=i;
        b[i]=sum;
    }
    for (i=n;i>=1;i--) {
        sum=b[i];
        for (j=i+1;j<=n;j++) sum -= a[i][j]*b[j];
        b[i]=sum/a[i][i];
    }
}



void tqli(double d[], double e[], int n, double **z)
{
    double pythag(double a, double b);
    int m,l,iter,i,k;
    double s,r,p,g,f,dd,c,b;

    for (i=2;i<=n;i++) e[i-1]=e[i];
    e[n]=0.0;
    for (l=1;l<=n;l++) {
        iter=0;
        do {
            for (m=l;m<=n-1;m++) {
                dd=fabs(d[m])+fabs(d[m+1]);
                if ((double)(fabs(e[m])+dd) == dd) break;
            }
            if (m != l) {
                if (iter++ == 30) nrerror("Too many iterations in tqli");
                g=(d[l+1]-d[l])/(2.0*e[l]);
                r=pythag(g,1.0);
                g=d[m]-d[l]+e[l]/(g+SIGN(r,g));
                s=c=1.0;
                p=0.0;
                for (i=m-1;i>=l;i--) {
                    f=s*e[i];
                    b=c*e[i];
                    e[i+1]=(r=pythag(f,g));
                    if (r == 0.0) {
                        d[i+1] -= p;
                        e[m]=0.0;
                        break;
                    }
                    s=f/r;
                    c=g/r;
                    g=d[i+1]-p;
                    r=(d[i]-g)*s+2.0*c*b;
                    d[i+1]=g+(p=s*r);
                    g=c*r-b;
					if (z!=NULL)
					{
						for (k=1;k<=n;k++) {
							f=z[k][i+1];
							z[k][i+1]=s*z[k][i]+c*f;
							z[k][i]=c*z[k][i]-s*f;
						}
					}
                }
                if (r == 0.0 && i >= l) continue;
                d[l] -= p;
                e[l]=g;
                e[m]=0.0;
            }
        } while (m != l);
    }
}

void mnewt(int ntrial, double x[], int n, double tolx, double tolf,void (*usrfun)(double [],int n,double *fvec,double **fjac))
{
#define FREERETURN {free_matrix(fjac,1,n,1,n);free_vector(fvec,1,n);free_ivector(indx,1,n);free_vector(p,1,n);return;}
    void lubksb(double **a, int n, int *indx, double b[]);
    void ludcmp(double **a, int n, int *indx, double *d);
    int k,i,*indx;
    double errx,errf,d,*fvec,**fjac,*p;

    indx=ivector(1,n);
    p=vector(1,n);
    fvec=vector(1,n);
    fjac=matrix(1,n,1,n);
    for (k=1;k<=ntrial;k++) {
        usrfun(x,n,fvec,fjac);
        errf=0.0;
        for (i=1;i<=n;i++) errf += fabs(fvec[i]);
	if (errf <= tolf){FREERETURN;}
                    for (i=1;i<=n;i++) p[i] = -fvec[i];
        ludcmp(fjac,n,indx,&d);
        lubksb(fjac,n,indx,p);
        errx=0.0;
        for (i=1;i<=n;i++) {
            errx += fabs(p[i]);
            x[i] += p[i];
        }
	if (errx <= tolx){FREERETURN;}
    }
    FREERETURN;
}

#define TINY 1.0e-10
void ludcmp(double **a, int n, int *indx, double *d)
{
    int i,imax,j,k;
    double big,dum,sum,temp;
    double *vv;

    vv=vector(1,n);
    *d=1.0;
    for (i=1;i<=n;i++) {
        big=0.0;
        for (j=1;j<=n;j++)
            if ((temp=fabs(a[i][j])) > big) big=temp;
        if (big == 0.0) nrerror("Singular matrix in routine ludcmp");
        vv[i]=1.0/big;
    }
    for (j=1;j<=n;j++) {
        for (i=1;i<j;i++) {
            sum=a[i][j];
            for (k=1;k<i;k++) sum -= a[i][k]*a[k][j];
            a[i][j]=sum;
        }
        big=0.0;
        for (i=j;i<=n;i++) {
            sum=a[i][j];
            for (k=1;k<j;k++)
                sum -= a[i][k]*a[k][j];
            a[i][j]=sum;
            if ( (dum=vv[i]*fabs(sum)) >= big) {
                big=dum;
                imax=i;
            }
        }
        if (j != imax) {
            for (k=1;k<=n;k++) {
                dum=a[imax][k];
                a[imax][k]=a[j][k];
                a[j][k]=dum;
            }
            *d = -(*d);
            vv[imax]=vv[j];
        }
        indx[j]=imax;
        if (a[j][j] == 0.0)a[j][j]=TINY;//Pivot is zero...
        if (j != n) {
            dum=1.0/(a[j][j]);
            for (i=j+1;i<=n;i++) a[i][j] *= dum;
        }
    }
    free_vector(vv,1,n);
}



#define SWAP(g,h) {y=(g);(g)=(h);(h)=y;}

			 void elmhes(double **a, int n)
{
	int m,j,i;
	double y,x;

	for (m=2;m<n;m++) {
		x=0.0;
		i=m;
		for (j=m;j<=n;j++) {
			if (fabs(a[j][m-1]) > fabs(x)) {
				x=a[j][m-1];
				i=j;
			}
		}
		if (i != m) {
			for (j=m-1;j<=n;j++) SWAP(a[i][j],a[m][j])
						for (j=1;j<=n;j++) SWAP(a[j][i],a[j][m])
		}
		if (x) {
			for (i=m+1;i<=n;i++) {
				if ((y=a[i][m-1]) != 0.0) {
					y /= x;
					a[i][m-1]=y;
					for (j=m;j<=n;j++)
						a[i][j] -= y*a[m][j];
					for (j=1;j<=n;j++)
						a[j][m] += y*a[j][i];
				}
			}
		}
	}
}
#undef SWAP



#define NRANSI

void hqr(double **a, int n, double wr[], double wi[])
{
	int nn,m,l,k,j,its,i,mmin;
	double z,y,x,w,v,u,t,s,r,q,p,anorm;

	anorm=0.0;
	for (i=1;i<=n;i++)
		for (j=IMAX(i-1,1);j<=n;j++)
			anorm += fabs(a[i][j]);
	nn=n;
	t=0.0;
	while (nn >= 1) {
		its=0;
		do {
			for (l=nn;l>=2;l--) {
				s=fabs(a[l-1][l-1])+fabs(a[l][l]);
				if (s == 0.0) s=anorm;
				if ((double)(fabs(a[l][l-1]) + s) == s) {
					a[l][l-1]=0.0;
					break;
				}
			}
			x=a[nn][nn];
			if (l == nn) {
				wr[nn]=x+t;
				wi[nn--]=0.0;
			} else {
				y=a[nn-1][nn-1];
				w=a[nn][nn-1]*a[nn-1][nn];
				if (l == (nn-1)) {
					p=0.5*(y-x);
					q=p*p+w;
					z=sqrt(fabs(q));
					x += t;
					if (q >= 0.0) {
						z=p+SIGN(z,p);
						wr[nn-1]=wr[nn]=x+z;
						if (z) wr[nn]=x-w/z;
						wi[nn-1]=wi[nn]=0.0;
					} else {
						wr[nn-1]=wr[nn]=x+p;
						wi[nn-1]= -(wi[nn]=z);
					}
					nn -= 2;
				} else {
					if (its == 30) nrerror("Too many iterations in hqr");
					if (its == 10 || its == 20) {
						t += x;
						for (i=1;i<=nn;i++) a[i][i] -= x;
						s=fabs(a[nn][nn-1])+fabs(a[nn-1][nn-2]);
						y=x=0.75*s;
						w = -0.4375*s*s;
					}
					++its;
					for (m=(nn-2);m>=l;m--) {
						z=a[m][m];
						r=x-z;
						s=y-z;
						p=(r*s-w)/a[m+1][m]+a[m][m+1];
						q=a[m+1][m+1]-z-r-s;
						r=a[m+2][m+1];
						s=fabs(p)+fabs(q)+fabs(r);
						p /= s;
						q /= s;
						r /= s;
						if (m == l) break;
						u=fabs(a[m][m-1])*(fabs(q)+fabs(r));
						v=fabs(p)*(fabs(a[m-1][m-1])+fabs(z)+fabs(a[m+1][m+1]));
						if ((double)(u+v) == v) break;
					}
					for (i=m+2;i<=nn;i++) {
						a[i][i-2]=0.0;
						if (i != (m+2)) a[i][i-3]=0.0;
					}
					for (k=m;k<=nn-1;k++) {
						if (k != m) {
							p=a[k][k-1];
							q=a[k+1][k-1];
							r=0.0;
							if (k != (nn-1)) r=a[k+2][k-1];
							if ((x=fabs(p)+fabs(q)+fabs(r)) != 0.0) {
								p /= x;
								q /= x;
								r /= x;
							}
						}
						if ((s=SIGN(sqrt(p*p+q*q+r*r),p)) != 0.0) {
							if (k == m) {
								if (l != m)
									a[k][k-1] = -a[k][k-1];
							} else
								a[k][k-1] = -s*x;
								p += s;
								x=p/s;
								y=q/s;
								z=r/s;
								q /= p;
								r /= p;
								for (j=k;j<=nn;j++) {
									p=a[k][j]+q*a[k+1][j];
									if (k != (nn-1)) {
										p += r*a[k+2][j];
										a[k+2][j] -= p*z;
									}
									a[k+1][j] -= p*y;
									a[k][j] -= p*x;
								}
								mmin = nn<k+3 ? nn : k+3;
								for (i=l;i<=mmin;i++) {
									p=x*a[i][k]+y*a[i][k+1];
									if (k != (nn-1)) {
										p += z*a[i][k+2];
										a[i][k+2] -= p*r;
									}
									a[i][k+1] -= p*q;
									a[i][k] -= p;
								}
						}
					}
				}
			}
		} while (l < nn-1);
	}
}
#undef NRANSI


#define ROTATE(a,i,j,k,l) g=a[i][j];h=a[k][l];a[i][j]=g-s*(h+g*tau);\
	a[k][l]=h+s*(g-h*tau);

void jacobi(double **a, int n, double d[], double **v, int *nrot)
{
	int j,iq,ip,i;
	double tresh,theta,tau,t,sm,s,h,g,c,*b,*z;

	b=vector(1,n);
	z=vector(1,n);
	for (ip=1;ip<=n;ip++) {
		for (iq=1;iq<=n;iq++) v[ip][iq]=0.0;
		v[ip][ip]=1.0;
	}
	for (ip=1;ip<=n;ip++) {
		b[ip]=d[ip]=a[ip][ip];
		z[ip]=0.0;
	}
	*nrot=0;
	for (i=1;i<=50;i++) {
		sm=0.0;
		for (ip=1;ip<=n-1;ip++) {
			for (iq=ip+1;iq<=n;iq++)
				sm += fabs(a[ip][iq]);
		}
		if (sm == 0.0) {
			free_vector(z,1,n);
			free_vector(b,1,n);
			return;
		}
		if (i < 4)
			tresh=0.2*sm/(n*n);
		else
			tresh=0.0;
		for (ip=1;ip<=n-1;ip++) {
			for (iq=ip+1;iq<=n;iq++) {
				g=100.0*fabs(a[ip][iq]);
				if (i > 4 && (double)(fabs(d[ip])+g) == (double)fabs(d[ip])
					&& (double)(fabs(d[iq])+g) == (double)fabs(d[iq]))
					a[ip][iq]=0.0;
				else if (fabs(a[ip][iq]) > tresh) {
					h=d[iq]-d[ip];
					if ((double)(fabs(h)+g) == (double)fabs(h))
						t=(a[ip][iq])/h;
					else {
						theta=0.5*h/(a[ip][iq]);
						t=1.0/(fabs(theta)+sqrt(1.0+theta*theta));
						if (theta < 0.0) t = -t;
					}
					c=1.0/sqrt(1+t*t);
					s=t*c;
					tau=s/(1.0+c);
					h=t*a[ip][iq];
					z[ip] -= h;
					z[iq] += h;
					d[ip] -= h;
					d[iq] += h;
					a[ip][iq]=0.0;
					for (j=1;j<=ip-1;j++) {
						ROTATE(a,j,ip,j,iq)
					}
					for (j=ip+1;j<=iq-1;j++) {
						ROTATE(a,ip,j,j,iq)
					}
					for (j=iq+1;j<=n;j++) {
						ROTATE(a,ip,j,iq,j)
					}
					for (j=1;j<=n;j++) {
						ROTATE(v,j,ip,j,iq)
					}
					++(*nrot);
				}
			}
		}
		for (ip=1;ip<=n;ip++) {
			b[ip] += z[ip];
			d[ip]=b[ip];
			z[ip]=0.0;
		}
	}
	nrerror("Too many iterations in routine jacobi");
}
#undef ROTATE
#undef NRANSI


#define RADIX 2.0

void balanc(double **a, int n)
{
	int last,j,i;
	double s,r,g,f,c,sqrdx;

	sqrdx=RADIX*RADIX;
	last=0;
	while (last == 0) {
		last=1;
		for (i=1;i<=n;i++) {
			r=c=0.0;
			for (j=1;j<=n;j++)
				if (j != i) {
				c += fabs(a[j][i]);
				r += fabs(a[i][j]);
				}
				if (c && r) {
					g=r/RADIX;
					f=1.0;
					s=c+r;
					while (c<g) {
						f *= RADIX;
						c *= sqrdx;
					}
					g=r*RADIX;
					while (c>g) {
						f /= RADIX;
						c /= sqrdx;
					}
					if ((c+r)/f < 0.95*s) {
						last=0;
						g=1.0/f;
						for (j=1;j<=n;j++) a[i][j] *= g;
						for (j=1;j<=n;j++) a[j][i] *= f;
					}
				}
		}
	}
}
#undef RADIX


double pythag(double a, double b)
{
    double absa,absb;
    absa=fabs(a);
    absb=fabs(b);
    if (absa > absb) return absa*sqrt(1.0+SQR(absb/absa));
    else return (absb == 0.0 ? 0.0 : absb*sqrt(1.0+SQR(absa/absb)));
}

void tred2(double **a, int n, double d[], double e[])
{
    int l,k,j,i;
    double scale,hh,h,g,f;

    for (i=n;i>=2;i--) {
        l=i-1;
        h=scale=0.0;
        if (l > 1) {
            for (k=1;k<=l;k++)
                scale += fabs(a[i][k]);
            if (scale == 0.0)
                e[i]=a[i][l];
            else {
                for (k=1;k<=l;k++) {
                    a[i][k] /= scale;
                    h += a[i][k]*a[i][k];
                }
                f=a[i][l];
                g=(f >= 0.0 ? -sqrt(h) : sqrt(h));
                e[i]=scale*g;
                h -= f*g;
                a[i][l]=f-g;
                f=0.0;
                for (j=1;j<=l;j++) {
                    a[j][i]=a[i][j]/h;
                    g=0.0;
                    for (k=1;k<=j;k++)
                        g += a[j][k]*a[i][k];
                    for (k=j+1;k<=l;k++)
                        g += a[k][j]*a[i][k];
                    e[j]=g/h;
                    f += e[j]*a[i][j];
                }
                hh=f/(h+h);
                for (j=1;j<=l;j++) {
                    f=a[i][j];
                    e[j]=g=e[j]-hh*f;
                    for (k=1;k<=j;k++)
                        a[j][k] -= (f*e[k]+g*a[i][k]);
                }
            }
        } else
            e[i]=a[i][l];
            d[i]=h;
    }
    d[1]=0.0;
    e[1]=0.0;
	/* Contents of this loop can be omitted if eigenvectors not
    wanted except for statement d[i]=a[i][i]; */
    for (i=1;i<=n;i++) {
        l=i-1;
        if (d[i]) {
            for (j=1;j<=l;j++) {
                g=0.0;
                for (k=1;k<=l;k++)
                    g += a[i][k]*a[k][j];
                for (k=1;k<=l;k++)
                    a[k][j] -= g*a[k][i];
            }
        }
        d[i]=a[i][i];
        a[i][i]=1.0;
        for (j=1;j<=l;j++) a[j][i]=a[i][j]=0.0;
    }
}



#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr

void four1(double data[], unsigned long nn, int isign)
{
	unsigned long n,mmax,m,j,istep,i;
	double wtemp,wr,wpr,wpi,wi,theta;
	double tempr,tempi;

	n=nn << 1;
	j=1;
	for (i=1;i<n;i+=2) {
		if (j > i) {
			SWAP(data[j],data[i]);
			SWAP(data[j+1],data[i+1]);
		}
		m=nn;
		while (m >= 2 && j > m) {
			j -= m;
			m >>= 1;
		}
		j += m;
	}
	mmax=2;
	while (n > mmax) {
		istep=mmax << 1;
		theta=isign*(6.28318530717959/mmax);
		wtemp=sin(0.5*theta);
		wpr = -2.0*wtemp*wtemp;
		wpi=sin(theta);
		wr=1.0;
		wi=0.0;
		for (m=1;m<mmax;m+=2) {
			for (i=m;i<=n;i+=istep) {
				j=i+mmax;
				tempr=wr*data[j]-wi*data[j+1];
				tempi=wr*data[j+1]+wi*data[j];
				data[j]=data[i]-tempr;
				data[j+1]=data[i+1]-tempi;
				data[i] += tempr;
				data[i+1] += tempi;
			}
			wr=(wtemp=wr)*wpr-wi*wpi+wr;
			wi=wi*wpr+wtemp*wpi+wi;
		}
		mmax=istep;
	}
}
#undef SWAP

void realft(double data[], unsigned long n, int isign)
{
	void four1(double data[], unsigned long nn, int isign);
	unsigned long i,i1,i2,i3,i4,np3;
	double c1=0.5,c2,h1r,h1i,h2r,h2i;
	double wr,wi,wpr,wpi,wtemp,theta;

	theta=3.141592653589793/(double) (n>>1);
	if (isign == 1) {
		c2 = -0.5;
		four1(data,n>>1,1);
	} else {
		c2=0.5;
		theta = -theta;
	}
	wtemp=sin(0.5*theta);
	wpr = -2.0*wtemp*wtemp;
	wpi=sin(theta);
	wr=1.0+wpr;
	wi=wpi;
	np3=n+3;
	for (i=2;i<=(n>>2);i++) {
		i4=1+(i3=np3-(i2=1+(i1=i+i-1)));
		h1r=c1*(data[i1]+data[i3]);
		h1i=c1*(data[i2]-data[i4]);
		h2r = -c2*(data[i2]+data[i4]);
		h2i=c2*(data[i1]-data[i3]);
		data[i1]=h1r+wr*h2r-wi*h2i;
		data[i2]=h1i+wr*h2i+wi*h2r;
		data[i3]=h1r-wr*h2r+wi*h2i;
		data[i4] = -h1i+wr*h2i+wi*h2r;
		wr=(wtemp=wr)*wpr-wi*wpi+wr;
		wi=wi*wpr+wtemp*wpi+wi;
	}
	if (isign == 1) {
		data[1] = (h1r=data[1])+data[2];
		data[2] = h1r-data[2];
	} else {
		data[1]=c1*((h1r=data[1])+data[2]);
		data[2]=c1*(h1r-data[2]);
		four1(data,n>>1,-1);
	}
}



void asolve(unsigned long n, double b[], double x[], int itrnsp,double sa[],double *inv_vec,unsigned long *inv_ind)
{
	dsprsax(inv_vec,inv_ind,b,x,n);
}


double snrm(unsigned long n, double sx[], int itol)
{
	unsigned long i,isamax;
	double ans;

	if (itol <= 3) {
		ans = 0.0;
		for (i=1;i<=n;i++) ans += sx[i]*sx[i];
		return sqrt(ans);
	} else {
		isamax=1;
		for (i=1;i<=n;i++) {
			if (fabs(sx[i]) > fabs(sx[isamax])) isamax=i;
		}
		return fabs(sx[isamax]);
	}
}

void dsprsax(double sa[], unsigned long ija[], double x[], double b[], unsigned long n)
{
	unsigned long i,k;
//	if (ija[1] != n+2) nrerror("dsprsax: mismatched vector and matrix");
	for (i=1;i<=n;i++) {
		b[i]=sa[i]*x[i];
		for (k=ija[i];k<=ija[i+1]-1;k++) b[i] += sa[k]*x[ija[k]];
	}
}


void dsprstx(double sa[], unsigned long ija[], double x[], double b[], unsigned long n)
{
	unsigned long i,j,k;
	if (ija[1] != n+2) nrerror("mismatched vector and matrix in dsprstx");
	for (i=1;i<=n;i++) b[i]=sa[i]*x[i];
	for (i=1;i<=n;i++) {
		for (k=ija[i];k<=ija[i+1]-1;k++) {
			j=ija[k];
			b[j] += sa[k]*x[i];
		}
	}
}



void atimes(unsigned long n, double x[], double r[], int itrnsp,double sa[],unsigned long ija[])
{
	if (itrnsp) dsprstx(sa,ija,x,r,n);
	else dsprsax(sa,ija,x,r,n);
}



#define NRANSI
#define SWAP(a,b) {temp=(a);(a)=(b);(b)=temp;}

void gaussj(double **a, int n, double **b, int m)
{
	int *indxc,*indxr,*ipiv;
	int i,icol,irow,j,k,l,ll;
	double big,dum,pivinv,temp;

	indxc=ivector(1,n);
	indxr=ivector(1,n);
	ipiv=ivector(1,n);
	for (j=1;j<=n;j++) ipiv[j]=0;
	for (i=1;i<=n;i++) {
		big=0.0;
		for (j=1;j<=n;j++)
			if (ipiv[j] != 1)
				for (k=1;k<=n;k++) {
					if (ipiv[k] == 0) {
						if (fabs(a[j][k]) >= big) {
							big=fabs(a[j][k]);
							irow=j;
							icol=k;
						}
					}
				}
		++(ipiv[icol]);
		if (irow != icol) {
			for (l=1;l<=n;l++) SWAP(a[irow][l],a[icol][l])
			for (l=1;l<=m;l++) SWAP(b[irow][l],b[icol][l])
		}
		indxr[i]=irow;
		indxc[i]=icol;
		if (a[icol][icol] == 0.0) nrerror("gaussj: Singular Matrix");
		pivinv=1.0/a[icol][icol];
		a[icol][icol]=1.0;
		for (l=1;l<=n;l++) a[icol][l] *= pivinv;
		for (l=1;l<=m;l++) b[icol][l] *= pivinv;
		for (ll=1;ll<=n;ll++)
			if (ll != icol) {
				dum=a[ll][icol];
				a[ll][icol]=0.0;
				for (l=1;l<=n;l++) a[ll][l] -= a[icol][l]*dum;
				for (l=1;l<=m;l++) b[ll][l] -= b[icol][l]*dum;
			}
	}
	for (l=n;l>=1;l--) {
		if (indxr[l] != indxc[l])
			for (k=1;k<=n;k++)
				SWAP(a[k][indxr[l]],a[k][indxc[l]]);
	}
	free_ivector(ipiv,1,n);
	free_ivector(indxr,1,n);
	free_ivector(indxc,1,n);
}
#undef SWAP
#undef NRANSI


#define EPS 1.0e-14

//This reduces a matrix to Gaussian normal form, or as close to this as possible (if A is singular).
void gauss_elimination(double **a,int n)
{
	int i,k,l,m;
	double *row;
	double max;
	int swaps;
	double mult=0.0;
	int n_max=0;
	//Three steps, carried out for each row: 
	//1: We find the pivot element in each row. We swap the columns, such that the pivot element is at the right place.
	//2: We divide the row by the pivot element.
	//3: We substract a multiple of the row from all other rows, such that all elements in the pivot columns become zero (besides the pivot element itself)
	swaps=0;
	for(i=1;i<=n;i++)
		{
START:
			max=fabs(a[i][i]);
			n_max=i;
			for (k=i+1;k<=n;k++)
			{
				if (fabs(a[k][i])>max)
				{
					max=fabs(a[k][i]);
					n_max=k;
				}
			}
//			cout << "column:" << i << "\n";
			//pivot element is at a[i][n_max]
			if (a[n_max][i]==0.0)
			{
//				cout << "No non-zero elements found. Returning...\n";
				i++;
				if (i<=n)
					goto START;
				else
					return;
			}
			if (n_max!=i)
			{
//				cout << "Exchaning " << i << " for " << n_max << "\n";
				//We exchange k & i.
				row=a[i];
				a[i]=a[n_max];
				a[n_max]=row;
			}
//			cout << "Pivot element:" << a[i][i] << "\n";
			//If the row contains only zeros, we swap it with one of the last rows and start again.
			//pivoting is done. Now we divide the pivot row by the pivot element and subtract this row from the other ones.
			
//			cout << "Multiplying...\n";
			double mult=a[i][i];
			for(k=i;k<=n;k++)
				a[i][k]/=mult;
			for(l=1;l<=n;l++)
				if (l!=i)
				{
					mult=a[l][i];
					for(m=i;m<=n;m++)
						a[l][m]-=a[i][m]*mult;
//						a[l][m]=a[l][m]-a[i][m]*a[l][i];
				}
			//Now all elements but the pivot element in the pivot column are zero. We can go on with the next column, until we reach the end of the matrix.
		}
}

//This reduces a matrix to Gaussian normal form, or as close to this as possible (if A is singular).
void full_gauss_elimination(double **a,double **b,int n)
{
	int i,k,l,m;
	double max;
	int swaps;
	double temp;
	double mult=0.0;
	int n_max=0;
	//Three steps, carried out for each row: 
	//1: We find the pivot element in each row. We swap the columns, such that the pivot element is at the right place.
	//2: We divide the row by the pivot element.
	//3: We substract a multiple of the row from all other rows, such that all elements in the pivot columns become zero (besides the pivot element itself)
	swaps=0;
	for(i=1;i<=n;i++)
		{
			max=fabs(a[i][i]);
			n_max=i;
			for (k=i+1;k<=n;k++)
			{
				if (fabs(a[i][k])>max)
				{
					max=fabs(a[i][k]);
					n_max=k;
				}
			}
//			cout << "column:" << i << "\n";
			//pivot element is at a[i][n_max]
			if (a[i][n_max]==0.0)
			{
				cout << "Err: Singular matrix!\n";
				return;
			}
			if (n_max!=i)
			{
//				cout << "Exchaning " << i << " for " << n_max << "\n";
				//We exchange k & i.
				for(k=i;k<=n;k++)
				{
					temp=a[i][k];
					a[i][k]=a[i][n_max];
					a[i][n_max]=temp;
				}
			}
//			cout << "Pivot element:" << a[i][i] << "\n";
			//If the row contains only zeros, we swap it with one of the last rows and start again.
			//pivoting is done. Now we divide the pivot row by the pivot element and subtract this row from the other ones.
			
//			cout << "Multiplying...\n";
			double mult=a[i][i];
			for(k=i;k<=n;k++)
				a[i][k]/=mult;
			for(l=1;l<=n;l++)
				if (l!=i)
				{
					mult=a[l][i];
					for(m=i;m<=n;m++)
						a[l][m]-=a[i][m]*mult;
//						a[l][m]=a[l][m]-a[i][m]*a[l][i];
				}
			//Now all elements but the pivot element in the pivot column are zero. We can go on with the next column, until we reach the end of the matrix.
		}
}


void linbcg_single(unsigned long n, double b[], double x[], int itol, double tol,
				   int itmax, int *iter, double *err,double sa[],unsigned long ija[],double *inv_vec,unsigned long *inv_ind)
{
	unsigned long j;
	
	static double *p=dvector(1,n);
	static double *r=dvector(1,n);
	static double *z=dvector(1,n);
	static double *za=dvector(1,n);
	double my_err=1.0;
	double lambda=0.0;
	double zx,zz;
	//dsprsax is the main bottleneck!!! make matrix multiplication faster, i.e.: make A and thus A^{-1} more sparse or accelerate matrix multiplication!
	int cnt=0;
	while(my_err>1e-4 && cnt<100)
	{
		dsprsax(sa,ija,x,r,n);
		
		for (j=1;j<=n;j++) {
			r[j]=b[j]-r[j];
		}
//		for(j=1;j<=n;j++)
//			z[j]=r[j]/(sa[j]!=0.0?sa[j]:1e-10);
		dsprsax(inv_vec,inv_ind,r,z,n);
		zx=0.0;
		zz=0.0;
		my_err=0.0;

		dsprsax(sa,ija,z,za,n);

		for(j=1;j<=n;j++)
		{
			zx+=za[j]*r[j];
			zz+=za[j]*za[j];
			my_err+=fabs(r[j]);
		}
		my_err/=(double)n;
		lambda=zx/zz;
//		cout << "zz:" << zz << ", zx:" << zx << "\n";
//		cout << "Lambda:" << lambda << "\n";
		for (j=1;j<=n;j++) 
			x[j] += lambda*z[j];
		cnt++;
//		cout << "Iterations:" << cnt << ", error:" << my_err << "\n";
	}
}

void linbcg(unsigned long n, double b[], double x[], int itol, double tol,
	int itmax, int *iter, double *err,double sa[],unsigned long ija[],double *inv_vec,unsigned long *inv_ind)
{
	unsigned long j;
	double ak,akden,bk,bkden,bknum,bnrm,dxnrm,xnrm,zm1nrm,znrm;
	double *p,*pp,*r,*rr,*z,*zz;

	p=dvector(1,n);
	pp=dvector(1,n);
	r=dvector(1,n);
	rr=dvector(1,n);
	z=dvector(1,n);
	zz=dvector(1,n);

	*iter=0;
	atimes(n,x,r,0,sa,ija);
	for (j=1;j<=n;j++) {
		r[j]=b[j]-r[j];
		rr[j]=r[j];
	}
	if (itol == 1) {
		bnrm=snrm(n,b,itol);
		asolve(n,r,z,0,sa,inv_vec,inv_ind);
	}
	else if (itol == 2) {
		asolve(n,b,z,0,sa,inv_vec,inv_ind);
		bnrm=snrm(n,z,itol);
		asolve(n,r,z,0,sa,inv_vec,inv_ind);
	}
	else if (itol == 3 || itol == 4) {
		asolve(n,b,z,0,sa,inv_vec,inv_ind);
		bnrm=snrm(n,z,itol);
		asolve(n,r,z,0,sa,inv_vec,inv_ind);
		znrm=snrm(n,z,itol);
	} else nrerror("illegal itol in linbcg");
	while (*iter <= itmax) {
		++(*iter);
		asolve(n,rr,zz,1,sa,inv_vec,inv_ind);
		for (bknum=0.0,j=1;j<=n;j++) bknum += z[j]*rr[j];
		if (*iter == 1) {
			for (j=1;j<=n;j++) {
				p[j]=z[j];
				pp[j]=zz[j];
			}
		}
		else {
			bk=bknum/bkden;
			for (j=1;j<=n;j++) {
				p[j]=bk*p[j]+z[j];
				pp[j]=bk*pp[j]+zz[j];
			}
		}
		bkden=bknum;
		atimes(n,p,z,0,sa,ija);
		for (akden=0.0,j=1;j<=n;j++) akden += z[j]*pp[j];
		ak=bknum/akden;
		atimes(n,pp,zz,1,sa,ija);
		for (j=1;j<=n;j++) {
			x[j] += ak*p[j];
			r[j] -= ak*z[j];
			rr[j] -= ak*zz[j];
		}
		asolve(n,r,z,0,sa,inv_vec,inv_ind);
		if (itol == 1)
			*err=snrm(n,r,itol)/bnrm;
 		else if (itol == 2)
			*err=snrm(n,z,itol)/bnrm;
		else if (itol == 3 || itol == 4) {
			zm1nrm=znrm;
			znrm=snrm(n,z,itol);
			if (fabs(zm1nrm-znrm) > EPS*znrm) {
				dxnrm=fabs(ak)*snrm(n,p,itol);
				*err=znrm/fabs(zm1nrm-znrm)*dxnrm;
			} else {
				*err=znrm/bnrm;
				continue;
			}
			xnrm=snrm(n,x,itol);
			if (*err <= 0.5*xnrm) *err /= xnrm;
			else {
				*err=znrm/bnrm;
				continue;
			}
		}
	if (*err <= tol) break;
	}

	free_dvector(p,1,n);
	free_dvector(pp,1,n);
	free_dvector(r,1,n);
	free_dvector(rr,1,n);
	free_dvector(z,1,n);
	free_dvector(zz,1,n);
}



#undef EPS


void sprsin(double **a,int **map,int n, double thresh, unsigned long nmax, double sa[],
	unsigned long ija[])
{
	int i,j;
	unsigned long k;

	for (j=1;j<=n;j++) {sa[j]=a[j][j];if (map!=NULL)map[j][j]=j;}
	ija[1]=n+2;
	k=n+1;
	for (i=1;i<=n;i++) {
		for (j=1;j<=n;j++) {
			if (fabs(a[i][j]) > thresh && i != j) {
				if (++k > nmax) nrerror("sprsin: nmax too small");
				if (map!=NULL)
					map[i][j]=k;
				sa[k]=a[i][j];
				ija[k]=j;
			}
		}
		ija[i+1]=k+1;
	}
}



#define IM1 2147483563
#define IM2 2147483399
#define AM (1.0/IM1)
#define IMM1 (IM1-1)
#define IA1 40014
#define IA2 40692
#define IQ1 53668
#define IQ2 52774
#define IR1 12211
#define IR2 3791
#define NTAB 32
#define NDIV (1+IMM1/NTAB)
#define EPS 1.2e-7
#define RNMX (1.0-EPS)
double ran2(long *idum)
{
    int j;
    long k;
    static long idum2=123456789;
    static long iy=0;
    static long iv[NTAB];
    double temp;
    if (*idum <= 0) { 
	if (-(*idum) < 1) *idum=1; 
	else *idum = -(*idum);
	idum2=(*idum);
	for (j=NTAB+7;j>=0;j--) {
	    k=(*idum)/IQ1;
	    *idum=IA1*(*idum-k*IQ1)-k*IR1;
	    if (*idum < 0) *idum += IM1;
	    if (j < NTAB) iv[j] = *idum;
	}
	iy=iv[0];
    }
    k=(*idum)/IQ1; 
    *idum=IA1*(*idum-k*IQ1)-k*IR1; 
    k=idum2/IQ2;
    idum2=IA2*(idum2-k*IQ2)-k*IR2; 
    if (idum2 < 0) idum2 += IM2;
    j=iy/NDIV; 
    iy=iv[j]-idum2; 
    iv[j] = *idum; 
    if (iy < 1) iy += IMM1;
    if ((temp=AM*iy) > RNMX) return RNMX; 
    else return temp;
}
    
#define IA 16807
#define IM 2147483647
#define IQ 127773
#define IR 2836
#define NTAB 32
#define EPS 1.2e-7
#define RNMX (1.0-EPS)

double ran1(long *idum)
{
    int j;
    long k;
    static long iy=0;
    static long iv[NTAB];
    double temp;
    if (*idum <= 0 || !iy) { 
        if (-(*idum) < 1) *idum=1; 
        else *idum = -(*idum);
        for (j=NTAB+7;j>=0;j--) { 
            k=(*idum)/IQ;
            *idum=IA*(*idum-k*IQ)-IR*k;
            if (*idum < 0) *idum += IM;
            if (j < NTAB) iv[j] = *idum;
        }
        iy=iv[0];
    }
    k=(*idum)/IQ; 
    *idum=IA*(*idum-k*IQ)-IR*k;
    if (*idum < 0) *idum += IM; 
    j=iy/NDIV;
    iy=iv[j];
    iv[j] = *idum; 
    if ((temp=AM*iy) > RNMX) return RNMX;
    else return temp;
}

double expdev(long *idum)
{
    double ran2(long *idum);
    double dum;
    do
        dum=ran2(idum);
    while (dum == 0.0);
    return -log(dum);
}

double gasdev(long *idum)
{
    double ran2(long *idum);
    static int iset=0;
    static double gset;
    double fac,rsq,v1,v2;
    if (*idum < 0) iset=0;
    if (iset == 0) { 
        do {
            v1=2.0*ran1(idum)-1.0;
            v2=2.0*ran1(idum)-1.0;
            rsq=v1*v1+v2*v2; 
        } while (rsq >= 1.0 || rsq == 0.0); 
        fac=sqrt(-2.0*log(rsq)/rsq);	
        gset=v1*fac;
        iset=1; 
        return v2*fac;
    } else {
        iset=0; 
        return gset;
    }
}