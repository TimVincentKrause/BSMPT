#pragma once
#include <vector>
#include <algorithm>
#include <cmath>


using Mat3Ddouble = std::vector< std::vector< std::vector <double>>>;
using Mat2Ddouble = std::vector< std::vector<double> >;

struct Base_interp
{
    int n, mm, jsav, cor, dj;
    const double *xx, *yy;
    
    Base_interp(const std::vector<double> &x, const double *y, int m)
    : n(x.size()), mm(m), jsav(0), cor(0), xx(&x[0]), yy(y) {
        dj = std::min(1,(int)std::pow((double)n,0.25));
    }
    
    Base_interp() {}
    
    double interp(double x) {
        int jlo = cor ? hunt(x) : locate(x);
        return rawinterp(jlo,x);
    }
    
    int locate(const double x);
    int hunt(const double x);
    
    double virtual rawinterp(int jlo, double x) = 0;
    
};


struct Poly_interp : Base_interp
{
    double dy;
    
    Poly_interp(const std::vector<double> &xv, const std::vector<double> &yv, int m)
    : Base_interp(xv,&yv[0],m), dy(0.) {}
    
    Poly_interp() {}

    void load(const std::vector<double> &xv, const std::vector<double> &yv, int m) {
        n = xv.size();
        mm = m;
        jsav = 0;
        cor = 0;
        xx = &xv[0];
        yy = &yv[0];
        dy = 0.;
        dj = std::min(1,(int)std::pow((double)n,0.25));
    }
    
    double rawinterp(int jl, double x);
};





struct Linear_interp : Base_interp
{
    Linear_interp(const std::vector<double> &xv, const std::vector<double> &yv)
    : Base_interp(xv,&yv[0],2)  {}
    
    Linear_interp() {}
    
    double rawinterp(int j, double x) {
        if (xx[j]==xx[j+1]) return yy[j];
        else return yy[j] + ((x-xx[j])/(xx[j+1]-xx[j]))*(yy[j+1]-yy[j]);
    }
};


struct Trilin_interp {
    int m,n,r;
    Mat3Ddouble f;
    Linear_interp x1terp, x2terp, x3terp;
    
    Trilin_interp(const std::vector<double> &x1v, const std::vector<double> &x2v, const std::vector<double> &x3v, const Mat3Ddouble &ym)
    : m(x1v.size()), n(x2v.size()), r(x3v.size()), f(ym),
    x1terp(x1v,x1v), x2terp(x2v,x2v), x3terp(x3v,x3v) {}
    
    Trilin_interp() {}
    
    double interp(double x1p, double x2p, double x3p);
};


struct Bilin_interp {
    int m, n;
    Mat2Ddouble f;
    Linear_interp x1terp, x2terp;
    
    Bilin_interp(const std::vector<double> &x1v, const std::vector<double> &x2v, const Mat2Ddouble &ym)
    : m(x1v.size()), n(x2v.size()), f(ym),
    x1terp(x1v,x1v), x2terp(x2v,x2v) {}
    
    Bilin_interp() {}
    
    double interp(double x1p, double x2p);
};

struct Poly2D_interp {
    int m,n,mm,nn;
    Mat2Ddouble y;
    std::vector<double> yv;
    Poly_interp x1terp, x2terp;
    Poly2D_interp(const std::vector<double> &x1v, const std::vector<double> &x2v, Mat2Ddouble &ym, int mp, int np) : m(x1v.size()), n(x2v.size()), mm(mp), nn(np), y(ym), yv(m), x1terp(x1v,yv,mm), x2terp(x2v,x2v,nn) {}
    
    Poly2D_interp() {}

    double interp(double x1p, double x2p);
};

struct Poly3D_interp {

    int m, n, mm, nn, ll;
    Mat3Ddouble y;
    std::vector<double> yv1, yv2;
    Poly_interp x1terp, x2terp, x3terp;
    Poly3D_interp(const std::vector<double> &x1v, const std::vector<double> &x2v, const std::vector<double> &x3v, Mat3Ddouble &ym, int mp, int np, int lp) : m(x1v.size()), n(x2v.size()), mm(mp), nn(np), ll(lp), y(ym), yv1(m), yv2(n), x1terp(x1v,yv1,mm), x2terp(x2v,yv2,nn), x3terp(x3v,x3v,ll) {}

    Poly3D_interp() {}

    double interp(double x1p, double x2p, double x3p);
};

struct Symmeig {
    int n;
    Mat2Ddouble z;
    std::vector<double> d,e;
    bool yesvecs;
    
    Symmeig(Mat2Ddouble &a, bool yesvec=true) : n(a.size()), z(a), d(n),
    e(n), yesvecs(yesvec)
    {
        tred2();
        tqli();
        sort();
    }

    void sort() {
        if (yesvecs)
            eigsrt(d,&z);
        // else
            //eigsrt(d);
    }
    void tred2();
    void tqli();
    double pythag(const double a, const double b);
    void eigsrt(std::vector<double> &d, Mat2Ddouble *v);
};




struct Jacobi {
    const int n;
    Mat2Ddouble a,v;
    std::vector<double> d;
    int nrot;
    const double EPS;
    
    Jacobi(Mat2Ddouble &aa) : n(aa.size()), a(aa), d(n), nrot(0),
    EPS(std::numeric_limits<double>::epsilon())
    {
        v.resize(n,std::vector<double>(n));
        
        int i,j,ip,iq;
        double tresh,theta,tau,t,sm,s,h,g,c;
        std::vector<double> b(n),z(n);
        for (ip=0;ip<n;ip++) {
            for (iq=0;iq<n;iq++) v[ip][iq]=0.0;
            v[ip][ip]=1.0;
        }
        for (ip=0;ip<n;ip++) {
            b[ip]=d[ip]=a[ip][ip];
            z[ip]=0.0;
        }
        for (i=1;i<=50;i++) {
            sm=0.0;
            for (ip=0;ip<n-1;ip++) {
                for (iq=ip+1;iq<n;iq++)
                    sm += fabs(a[ip][iq]);
            }
            if (sm == 0.0) {
                eigsrt(d,&v);
                return;
            }
            if (i < 4)
                tresh=0.2*sm/(n*n);
            else
                tresh=0.0;
            for (ip=0;ip<n-1;ip++) {
                for (iq=ip+1;iq<n;iq++) {
                    g=100.0*fabs(a[ip][iq]);
                    if (i > 4 && g <= EPS*fabs(d[ip]) && g <= EPS*fabs(d[iq]))
                        a[ip][iq]=0.0;
                    else if (fabs(a[ip][iq]) > tresh) {
                        h=d[iq]-d[ip];
                        if (g <= EPS*fabs(h))
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
                        for (j=0;j<ip;j++)
                            rot(a,s,tau,j,ip,j,iq);
                        for (j=ip+1;j<iq;j++)
                            rot(a,s,tau,ip,j,j,iq);
                        for (j=iq+1;j<n;j++)
                            rot(a,s,tau,ip,j,iq,j);
                        for (j=0;j<n;j++)
                            rot(v,s,tau,j,ip,j,iq);
                        ++nrot;
                    }
                }
            }
            for (ip=0;ip<n;ip++) {
                b[ip] += z[ip];
                d[ip]=b[ip];
                z[ip]=0.0;
            }
        }
        throw("Too many iterations in routine jacobi");
    }
    inline void rot(Mat2Ddouble &a, const double s, const double tau, const int i,
                    const int j, const int k, const int l)
    {
        double g=a[i][j];
        double h=a[k][l];
        a[i][j]=g-s*(h+g*tau);
        a[k][l]=h+s*(g-h*tau);
    }
    
    void eigsrt(std::vector<double> &d, Mat2Ddouble *v)
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
    
};


