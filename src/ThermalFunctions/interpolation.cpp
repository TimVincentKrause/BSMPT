
#include "BSMPT/ThermalFunctions/interpolation.hpp"



int Base_interp::locate(const double x)
{
    int ju,jm,jl;
    if (n < 2 || mm < 2 || mm > n) throw("locate size error");
    bool ascnd=(xx[n-1] >= xx[0]);
    jl=0;
    ju=n-1;
    while (ju-jl > 1) {
        jm = (ju+jl) >> 1;
        if (x >= xx[jm] == ascnd)
            jl=jm;
        else
            ju=jm;
    }
    cor = fabs(jl-jsav) > dj ? 0 : 1;
    jsav = jl;
    return std::max(0,std::min(n-mm,jl-((mm-2)>>1)));
}


int Base_interp::hunt(const double x)
{
    int jl=jsav, jm, ju, inc=1;
    if (n < 2 || mm < 2 || mm > n) throw("hunt size error");
    bool ascnd=(xx[n-1] >= xx[0]);
    if (jl < 0 || jl > n-1) {
        jl=0;
        ju=n-1;
    } else {
        if (x >= xx[jl] == ascnd) {
            for (;;) {
                ju = jl + inc;
                if (ju >= n-1) { ju = n-1; break;}
                else if (x < xx[ju] == ascnd) break;
                else {
                    jl = ju;
                    inc += inc;
                }
            }
        } else {
            ju = jl;
            for (;;) {
                jl = jl - inc;
                if (jl <= 0) { jl = 0; break;}
                else if (x >= xx[jl] == ascnd) break;
                else {
                    ju = jl;
                    inc += inc;
                }
            }
        }
    }
    while (ju-jl > 1) {
        jm = (ju+jl) >> 1;
        if (x >= xx[jm] == ascnd)
            jl=jm;
        else
            ju=jm;
    }
    cor = fabs(jl-jsav) > dj ? 0 : 1;
    jsav = jl;
    return std::max(0,std::min(n-mm,jl-((mm-2)>>1)));
}


double Poly_interp::rawinterp(int jl, double x)
{
    int i,m,ns=0;
    double y,den,dif,dift,ho,hp,w;
    const double *xa = &xx[jl], *ya = &yy[jl];
    std::vector<double> c(mm),d(mm);
    dif=fabs(x-xa[0]);
    for (i=0;i<mm;i++) {
        if ((dift=fabs(x-xa[i])) < dif) {
            ns=i;
            dif=dift;
        }
        c[i]=ya[i];
        d[i]=ya[i];
    }
    y=ya[ns--];
    for (m=1;m<mm;m++) {
        for (i=0;i<mm-m;i++) {
            ho=xa[i]-x;
            hp=xa[i+m]-x;
            w=c[i+1]-d[i];
            if ((den=ho-hp) == 0.0) throw("Poly_interp error");
            den=w/den;
            d[i]=hp*den;
            c[i]=ho*den;
        }
        y += (dy=(2*(ns+1) < (mm-m) ? c[ns+1] : d[ns--]));
    }
    return y;
}



double Trilin_interp::interp(double x1p, double x2p, double x3p) {
    int i,j,k;
    double ff, x, y, z;
    i = x1terp.cor ? x1terp.hunt(x1p) : x1terp.locate(x1p);
    j = x2terp.cor ? x2terp.hunt(x2p) : x2terp.locate(x2p);
    k = x3terp.cor ? x3terp.hunt(x3p) : x3terp.locate(x3p);
    
    x = (x1p-x1terp.xx[i])/(x1terp.xx[i+1]-x1terp.xx[i]);
    y = (x2p-x2terp.xx[j])/(x2terp.xx[j+1]-x2terp.xx[j]);
    z = (x3p-x3terp.xx[k])/(x3terp.xx[k+1]-x3terp.xx[k]);
    
    double f00 = f[i][j][k]*(1. - x) + f[i+1][j][k] * x;
    double f01 = f[i][j][k+1]*(1. - x) + f[i+1][j][k+1] * x;
    double f10 = f[i][j+1][k]*(1. - x) + f[i+1][j+1][k] * x;
    double f11 = f[i][j+1][k+1]*(1. - x) + f[i+1][j+1][k+1] * x;
    
    double f0 = f00*(1. - y) + f10*y;
    double f1 = f01*(1. - y) + f11*y;
    
    ff = f0 * (1. - z) + f1*z;
    
    return ff;
}


double Bilin_interp::interp(double x1p, double x2p) {
    int i,j;
    double ff, x, y;
    i = x1terp.cor ? x1terp.hunt(x1p) : x1terp.locate(x1p);
    j = x2terp.cor ? x2terp.hunt(x2p) : x2terp.locate(x2p);
    
    x = (x1p-x1terp.xx[i])/(x1terp.xx[i+1]-x1terp.xx[i]);
    y = (x2p-x2terp.xx[j])/(x2terp.xx[j+1]-x2terp.xx[j]);
    
    double f0 = f[i][j]*(1. - x) + f[i+1][j] * x;
    double f1 = f[i][j+1]*(1. - x) + f[i+1][j+1] * x;
    
    ff = f0 * (1. - y) + f1*y;
    
    return ff;
}

double Poly2D_interp::interp(double x1p, double x2p) {
    int i,j,k;
    i = x1terp.cor ? x1terp.hunt(x1p) : x1terp.locate(x1p);
    j = x2terp.cor ? x2terp.hunt(x2p) : x2terp.locate(x2p);
    for (k=i;k<i+mm;k++) {
        x2terp.yy = &y[k][0];
        yv[k] = x2terp.rawinterp(j,x2p);
    }
    return x1terp.rawinterp(i,x1p);
}

double Poly3D_interp::interp(double x1p, double x2p, double x3p) {
        int i, j, k, l, s;
        i = x1terp.cor ? x1terp.hunt(x1p) : x1terp.locate(x1p);
        j = x2terp.cor ? x2terp.hunt(x2p) : x2terp.locate(x2p);
        k = x3terp.cor ? x3terp.hunt(x3p) : x3terp.locate(x3p);

        for(l = i; l < i + mm; l++)
        {
            //interpolazione 2D per ogni superficie
            for(s = j; s < j + nn; s++)
            {
                x3terp.yy = &y[l][s][0];
                yv2[s] = x3terp.rawinterp(k, x3p);
            }
            //update di yv1 ussando il valore interpolato 2D
            yv1[l] = x2terp.rawinterp(j, x2p);
        }
        return x1terp.rawinterp(i,x1p);
    }


void Symmeig::tred2()
{
    int l,k,j,i;
    double scale,hh,h,g,f;
    for (i=n-1;i>0;i--) {
        l=i-1;
        h=scale=0.0;
        if (l > 0) {
            for (k=0;k<i;k++)
                scale += fabs(z[i][k]);
            if (scale == 0.0)
                e[i]=z[i][l];
            else {
                for (k=0;k<i;k++) {
                    z[i][k] /= scale;
                    h += z[i][k]*z[i][k];
                }
                f=z[i][l];
                g=(f >= 0.0 ? -sqrt(h) : sqrt(h));
                e[i]=scale*g;
                h -= f*g;
                z[i][l]=f-g;
                f=0.0;
                for (j=0;j<i;j++) {
                    if (yesvecs)
                        z[j][i]=z[i][j]/h;
                    g=0.0;
                    for (k=0;k<j+1;k++)
                        g += z[j][k]*z[i][k];
                    for (k=j+1;k<i;k++)
                        g += z[k][j]*z[i][k];
                    e[j]=g/h;
                    f += e[j]*z[i][j];
                }
                hh=f/(h+h);
                for (j=0;j<i;j++) {
                    f=z[i][j];
                    e[j]=g=e[j]-hh*f;
                    for (k=0;k<j+1;k++)
                        z[j][k] -= (f*e[k]+g*z[i][k]);
                }
            }
        } else
            e[i]=z[i][l];
        d[i]=h;
    }
    if (yesvecs) d[0]=0.0;
    e[0]=0.0;
    for (i=0;i<n;i++) {
        if (yesvecs) {
            if (d[i] != 0.0) {
                for (j=0;j<i;j++) {
                    g=0.0;
                    for (k=0;k<i;k++)
                        g += z[i][k]*z[k][j];
                    for (k=0;k<i;k++)
                        z[k][j] -= g*z[k][i];
                }
            }
            d[i]=z[i][i];
            z[i][i]=1.0;
            for (j=0;j<i;j++) z[j][i]=z[i][j]=0.0;
        } else {
            d[i]=z[i][i];
        }
    }
}

void Symmeig::tqli()
{
    int m,l,iter,i,k;
    double s,r,p,g,f,dd,c,b;
    const double EPS=std::numeric_limits<double>::epsilon();
    for (i=1;i<n;i++) e[i-1]=e[i];
    e[n-1]=0.0;
    for (l=0;l<n;l++) {
        iter=0;
        do {
            for (m=l;m<n-1;m++) {
                dd=fabs(d[m])+fabs(d[m+1]);
                if (fabs(e[m]) <= EPS*dd) break;
            }
            if (m != l) {
                if (iter++ == 30) throw("Too many iterations in tqli");
                g=(d[l+1]-d[l])/(2.0*e[l]);
                r=pythag(g,1.0);
                g=d[m]-d[l]+e[l]/(g+std::copysign(r,g));
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
                    if (yesvecs) {
                        for (k=0;k<n;k++) {
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


double Symmeig::pythag(const double a, const double b) {
    double absa=fabs(a), absb=fabs(b);
    return (absa > absb ? absa*sqrt(1.0+pow(absb/absa,2)) :
            (absb == 0.0 ? 0.0 : absb*sqrt(1.0+pow(absa/absb,2))));
}

void Symmeig::eigsrt(std::vector<double> &d, Mat2Ddouble *v)
{
    int k;
    int n=d.size();
    for (int i=0;i<n-1;i++) {
        double p=d[k=i];
        for (int j=i;j<n;j++)
            if (d[j] >= p) p=d[k=j];
        if (k != i) {
            d[k]=d[i];
            d[i]=p;
            if (v != NULL)
                for (int j=0;j<n;j++) {
                    p=(*v)[j][i];
                    (*v)[j][i]=(*v)[j][k];
                    (*v)[j][k]=p;
                }
        }
    }
}
