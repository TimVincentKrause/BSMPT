#include <BSMPT/utility/NumericalDerivatives.h>

namespace BSMPT
{

std::vector<double>
NablaNumerical(const std::vector<double> &phi,
               const std::function<double(std::vector<double>)> &f,
               const double &eps)
{
  std::vector<double> result(phi.size());

  for (size_t i = 0; i < phi.size(); i++)
  {
    std::vector<double> lp2 = phi;
    lp2[i] += 2 * eps;
    std::vector<double> lp1 = phi;
    lp1[i] += eps;
    std::vector<double> lm1 = phi;
    lm1[i] -= eps;
    std::vector<double> lm2 = phi;
    lm2[i] -= 2 * eps;
    result[i] = (-f(lp2) + 8 * f(lp1) - 8 * f(lm1) + f(lm2)) / (12 * eps);
  }
  return result;
}

double
FiniteDifference(const std::vector<double> &phi,
               const std::function<double(std::vector<double>)> &f,
               const double &eps, const std::size_t &i)
{
    std::vector<double> lp2 = phi;
    lp2[i] += 2 * eps;
    std::vector<double> lp1 = phi;
    lp1[i] += eps;
    std::vector<double> lm1 = phi;
    lm1[i] -= eps;
    std::vector<double> lm2 = phi;
    lm2[i] -= 2 * eps;
    return (-f(lp2) + 8 * f(lp1) - 8 * f(lm1) + f(lm2)) / (12 * eps);
}

double
FiniteDifferenceo6(const std::vector<double> &phi,
               const std::function<double(std::vector<double>)> &f,
               const double &eps, const std::size_t &i)
{
    std::vector<double> lp3 = phi;
    lp3[i] += 3 * eps;
    std::vector<double> lp2 = phi;
    lp2[i] += 2 * eps;
    std::vector<double> lp1 = phi;
    lp1[i] += eps;
    std::vector<double> lm1 = phi;
    lm1[i] -= eps;
    std::vector<double> lm2 = phi;
    lm2[i] -= 2 * eps;
    std::vector<double> lm3 = phi;
    lm3[i] -= 3 * eps;
    return (f(lp3) - 9*f(lp2) + 45*f(lp1) - 45*f(lm1) + 9*f(lm2) - f(lm3)) / (60 * eps);
}


double
FiniteDifference2D(const std::vector<double> &phi,
               const std::function<double(std::vector<double>)> &f,
               const double &eps, const std::size_t &i, const std::size_t &j)
{

    double r = 0;

    auto xp = phi; // F(x+h, y+h)
    xp[i] += eps;
    xp[j] += eps;
    r += f(xp);

    xp = phi; //-F(x+h, y-h)
    xp[i] += eps;
    xp[j] -= eps;
    r -= f(xp);

    xp = phi; //-F(x-h, y+h)
    xp[i] -= eps;
    xp[j] += eps;
    r -= f(xp);

    xp = phi; // F(x-h, y-h)
    xp[i] -= eps;
    xp[j] -= eps;
    r += f(xp);

    return r;
}


double
FiniteDifference2D(const std::vector<double> &phi,
               const std::function<double(std::vector<double>)> &f,
               const double &eps, const std::size_t &i)
{

    double val = 0;
    auto xp    = phi;
    xp[i] += 2 * eps;
    val += f(xp);

    val -= 2 * f(phi);

    xp = phi;
    xp[i] -= 2 * eps;
    val += f(xp);

    return val / (4 * eps * eps);
}

double
FiniteDifference2Do4(const std::vector<double> &phi,
               const std::function<double(std::vector<double>)> &f,
               const double &eps, const std::size_t &i, const std::size_t &j)
{

    double r = 0;

    // V(x + 2eps, ...)
    auto xp = phi;
    xp[i] += 2 * eps;
    xp[j] += 2 * eps;
    r += f(xp);

    xp = phi;
    xp[i] += 2 * eps;
    xp[j] += eps;
    r -= 8 * f(xp);

    xp = phi;
    xp[i] += 2 * eps;
    xp[j] -= eps;
    r += 8 * f(xp);

    xp = phi;
    xp[i] += 2 * eps;
    xp[j] -= 2 * eps;
    r -= f(xp);

    // V(x + eps, ...)
    xp = phi;
    xp[i] += eps;
    xp[j] += 2 * eps;
    r -= 8 * f(xp);

    xp = phi;
    xp[i] += eps;
    xp[j] += eps;
    r += 64 * f(xp);

    xp = phi;
    xp[i] += eps;
    xp[j] -= eps;
    r -= 64 * f(xp);

    xp = phi;
    xp[i] += eps;
    xp[j] -= 2 * eps;
    r += 8 * f(xp);

    // V(x - eps, ...)
    xp = phi;
    xp[i] -= eps;
    xp[j] += 2 * eps;
    r += 8 * f(xp);

    xp = phi;
    xp[i] -= eps;
    xp[j] += eps;
    r -= 64 * f(xp);

    xp = phi;
    xp[i] -= eps;
    xp[j] -= eps;
    r += 64 * f(xp);

    xp = phi;
    xp[i] -= eps;
    xp[j] -= 2 * eps;
    r -= 8 * f(xp);

    // V(x - 2eps, ...)
    xp = phi;
    xp[i] -= 2 * eps;
    xp[j] += 2 * eps;
    r -= f(xp);

    xp = phi;
    xp[i] -= 2 * eps;
    xp[j] += eps;
    r += 8 * f(xp);

    xp = phi;
    xp[i] -= 2 * eps;
    xp[j] -= eps;
    r -= 8 * f(xp);

    xp = phi;
    xp[i] -= 2 * eps;
    xp[j] -= 2 * eps;
    r += f(xp);

    return r / (144 * eps * eps);
}

double
FiniteDifference2Do4(const std::vector<double> &phi,
               const std::function<double(std::vector<double>)> &f,
               const double &eps, const std::size_t &i)
{

    double val = 0;
    auto xp = phi;
    xp[i] += 4 * eps;
    val += f(xp);

    xp = phi;
    xp[i] += 3 * eps;
    val -= 16 * f(xp);

    xp = phi;
    xp[i] += 2 * eps;
    val += 64 * f(xp);

    xp = phi;
    xp[i] += eps;
    val += 16 * f(xp);

    val -= 130 * f(phi);

    xp = phi;
    xp[i] -= eps;
    val += 16 * f(xp);

    xp = phi;
    xp[i] -= 2 * eps;
    val += 64 * f(xp);

    xp = phi;
    xp[i] -= 3 * eps;
    val -= 16 * f(xp);

    xp = phi;
    xp[i] -= 4 * eps;
    val += f(xp);

    return val / (144 * eps * eps);
}

std::vector<double>
NablaRidders(const std::vector<double> &phi,
               const std::function<double(std::vector<double>)> &f,
               const std::vector<double> &h, std::vector<double> &err)
{
  std::vector<double> result(phi.size());

  std::size_t ntab=20;
  double con=1.4, con2 = con*con;
  const double big = std::numeric_limits<double>::max();
  const double safe = 2.;
  std::size_t i,j,k;
  double errt,fac,hh;
  std::vector<std::vector<double>> a(ntab,
                                          std::vector<double>(ntab));
  //if (h == 0.) throw("h must be nonzero in NablaRidders.");
  for (i = 0; i < phi.size(); i++)
  {
    hh=h.at(i);

    a[0][0] = FiniteDifference(phi,f,hh,i);
    err.at(i) = big;
    for (j = 1; j < ntab; j++)
    {
        hh /= con;
        a[0][j] =FiniteDifference(phi,f,hh,i);
        fac=con2;
        for (k=1; k<=j; k++)
        {
            a[k][j]=(a[k-1][j]*fac-a[k-1][j-1]) /(fac-1.0);
                fac=con2*fac;
                errt=std::max(std::abs(a[k][j]-a[k-1][j]),std::abs(a[k][j]-a[k-1][j-1]));
                if ( errt <= err.at(i))
                {
                    if (errt <= err.at(i)) {err.at(i)=errt;result.at(i)=a[k][j];}
                }
        }
        if (std::abs(a[j][j]-a[j-1][j-1]) >= safe*err.at(i)) break;
    }
  }
  return result;
}


std::vector<double>
NablaRidders(const std::vector<double> &phi,
               const std::function<double(std::vector<double>)> &f,
               const double &h)
{
  std::vector<double> result(phi.size());
  std::vector<double> err(phi.size());

  std::size_t ntab=12;
  double con=1.4, con2 = con*con;
  const double big = std::numeric_limits<double>::max();
  const double safe = 2.;
  std::size_t i,j,k;
  double errt,fac,hh;
  std::vector<std::vector<double>> a(ntab,
                                          std::vector<double>(ntab));
  //if (h == 0.) throw("h must be nonzero in NablaRidders.");
  for (i = 0; i < phi.size(); i++)
  {
    hh=h;

    a[0][0] = FiniteDifference(phi,f,hh,i);
    err.at(i) = big;
    for (j = 1; j < ntab; j++)
    {
        hh /= con;
        a[0][j] =FiniteDifference(phi,f,hh,i);
        fac=con2;
        for (k=1; k<=j; k++)
        {
            a[k][j]=(a[k-1][j]*fac-a[k-1][j-1]) /(fac-1.0);
                fac=con2*fac;
                errt=std::max(std::abs(a[k][j]-a[k-1][j]),std::abs(a[k][j]-a[k-1][j-1]));
                if ( errt <= err.at(i))
                {
                    if (errt <= err.at(i)) {err.at(i)=errt;result.at(i)=a[k][j];}
                }
        }
        if (std::abs(a[j][j]-a[j-1][j-1]) >= safe*err.at(i)) break;
    }
  }
  return result;
}

std::vector<double>
NablaRidderso6(const std::vector<double> &phi,
               const std::function<double(std::vector<double>)> &f,
               const double &h)
{
  std::vector<double> result(phi.size());
  std::vector<double> err(phi.size());

  std::size_t ntab=12;
  double con=1.4, con2 = con*con;
  const double big = std::numeric_limits<double>::max();
  const double safe = 2.;
  std::size_t i,j,k;
  double errt,fac,hh;
  std::vector<std::vector<double>> a(ntab,
                                          std::vector<double>(ntab));
  //if (h == 0.) throw("h must be nonzero in NablaRidders.");
  for (i = 0; i < phi.size(); i++)
  {
    hh=h;

    a[0][0] = FiniteDifferenceo6(phi,f,hh,i);
    err.at(i) = big;
    for (j = 1; j < ntab; j++)
    {
        hh /= con;
        a[0][j] =FiniteDifferenceo6(phi,f,hh,i);
        fac=con2;
        for (k=1; k<=j; k++)
        {
            a[k][j]=(a[k-1][j]*fac-a[k-1][j-1]) /(fac-1.0);
                fac=con2*fac;
                errt=std::max(std::abs(a[k][j]-a[k-1][j]),std::abs(a[k][j]-a[k-1][j-1]));
                if ( errt <= err.at(i))
                {
                    if (errt <= err.at(i)) {err.at(i)=errt;result.at(i)=a[k][j];}
                }
        }
        if (std::abs(a[j][j]-a[j-1][j-1]) >= safe*err.at(i)) break;
    }
  }
  return result;
}






std::vector<std::vector<double>>
HessianNablaRidders(const std::vector<double> &phi,
               const std::function<double(std::vector<double>)> &f,
               const std::vector<double> &h, std::vector<std::vector<double>> &err)
{
    std::vector<std::vector<double>> result(phi.size(),
                                            std::vector<double>(phi.size()));

  std::size_t ntab=10;
  double con=1.4, con2 = con*con;
  const double big = std::numeric_limits<double>::max();
  const double safe = 2.;
  std::size_t i,j,k,l;
  double errt,fac,hh;
  std::vector<std::vector<double>> a(ntab,
                                          std::vector<double>(ntab));
  //if (h == 0.) throw("h must be nonzero in NablaRidders.");

  for (l = 0; l < phi.size(); l++)
  {
    for (i = l; i < phi.size(); i++)
    {
        if (l==i)
        {hh=h.at(l);}
        else if ((l == 6) & (i == 8) || ((l == 8) & (i == 6)))
        {hh=h.at(8);}
        else if ((l == 7) & (i == 8) || ((l == 8) & (i == 7)))
        {hh=h.at(8);}
        else
        {hh=h.at(0);}

        if (l ==i)
        {a[0][0] = FiniteDifference2D(phi,f,hh,i);}
        else
        {a[0][0] = FiniteDifference2D(phi,f,hh,i,l);}
        err.at(l).at(i) = big;
        for (j = 1; j < ntab; j++)
        {
            hh /= con;
            if(l==i)
            {a[0][j] = FiniteDifference2D(phi,f,hh,i);}
            else
            {a[0][j] = FiniteDifference2D(phi,f,hh,i,l);}
            fac=con2;
            for (k=1; k<=j; k++)
            {
                a[k][j]=(a[k-1][j]*fac-a[k-1][j-1]) /(fac-1.0);
                    fac=con2*fac;
                    errt=std::max(std::abs(a[k][j]-a[k-1][j]),std::abs(a[k][j]-a[k-1][j-1]));
                    if ( errt <= err.at(l).at(i))
                    {
                        if (errt <= err.at(l).at(i)) {err.at(l).at(i)=errt;result.at(l).at(i)=a[k][j];}
                    }
            }
            if (std::abs(a[j][j]-a[j-1][j-1]) >= safe*err.at(l).at(i)) break;
        }
        if (i != l)
        {result.at(i).at(l) = result.at(l).at(i);err.at(i).at(l) = err.at(l).at(i);}

    }
  }
  return result;
}


std::vector<std::vector<double>>
HessianNablaRidderso4(const std::vector<double> &phi,
               const std::function<double(std::vector<double>)> &f,
               const std::vector<double> &h, std::vector<std::vector<double>> &err)
{
    std::vector<std::vector<double>> result(phi.size(),
                                            std::vector<double>(phi.size()));

  std::size_t ntab=12;
  double con=1.4, con2 = con*con;
  const double big = std::numeric_limits<double>::max();
  const double safe = 2.;
  std::size_t i,j,k,l;
  double errt,fac,hh;
  std::vector<std::vector<double>> a(ntab,
                                          std::vector<double>(ntab));
  //if (h == 0.) throw("h must be nonzero in NablaRidders.");

  for (l = 0; l < phi.size(); l++)
  {
    for (i = l; i < phi.size(); i++)
    {
        if (l==i)
        {hh=h.at(l);}
        else if ((l == 6) & (i == 8) || ((l == 8) & (i == 6)))
        {hh=h.at(8);}
        else if ((l == 7) & (i == 8) || ((l == 8) & (i == 7)))
        {hh=h.at(8);}
        else
        {hh=h.at(0);}

        if (l ==i)
        {a[0][0] = FiniteDifference2Do4(phi,f,hh,i,i);}
        else
        {a[0][0] = FiniteDifference2Do4(phi,f,hh,i,l);}
        err.at(l).at(i) = big;
        for (j = 1; j < ntab; j++)
        {
            hh /= con;
            if(l==i)
            {a[0][j] = FiniteDifference2Do4(phi,f,hh,i);}
            else
            {a[0][j] = FiniteDifference2Do4(phi,f,hh,i,l);}
            fac=con2;
            for (k=1; k<=j; k++)
            {
                a[k][j]=(a[k-1][j]*fac-a[k-1][j-1]) /(fac-1.0);
                    fac=con2*fac;
                    errt=std::max(std::abs(a[k][j]-a[k-1][j]),std::abs(a[k][j]-a[k-1][j-1]));
                    if ( errt <= err.at(l).at(i))
                    {
                        if (errt <= err.at(l).at(i)) {err.at(l).at(i)=errt;result.at(l).at(i)=a[k][j];}
                    }
            }
            if (std::abs(a[j][j]-a[j-1][j-1]) >= safe*err.at(l).at(i)) break;
        }
        if (i != l)
        {result.at(i).at(l) = result.at(l).at(i);err.at(i).at(l) = err.at(l).at(i);}

    }
  }
  return result;
}


std::vector<std::vector<double>>
HessianNablaRidderso4(const std::vector<double> &phi,
               const std::function<double(std::vector<double>)> &f,
               const double &h)
{
    std::vector<std::vector<double>> result(phi.size(),
                                            std::vector<double>(phi.size()));
    std::vector<std::vector<double>> err(phi.size(),
                                            std::vector<double>(phi.size()));

  std::size_t ntab=12;
  double con=1.4, con2 = con*con;
  const double big = std::numeric_limits<double>::max();
  const double safe = 2.;
  std::size_t i,j,k,l;
  double errt,fac,hh;
  std::vector<std::vector<double>> a(ntab,
                                          std::vector<double>(ntab));
  //if (h == 0.) throw("h must be nonzero in NablaRidders.");

  for (l = 0; l < phi.size(); l++)
  {
    for (i = l; i < phi.size(); i++)
    {
        hh=h;

        if (l ==i)
        {a[0][0] = FiniteDifference2Do4(phi,f,hh,i,i);}
        else
        {a[0][0] = FiniteDifference2Do4(phi,f,hh,i,l);}
        err.at(l).at(i) = big;
        for (j = 1; j < ntab; j++)
        {
            hh /= con;
            if(l==i)
            {a[0][j] = FiniteDifference2Do4(phi,f,hh,i);}
            else
            {a[0][j] = FiniteDifference2Do4(phi,f,hh,i,l);}
            fac=con2;
            for (k=1; k<=j; k++)
            {
                a[k][j]=(a[k-1][j]*fac-a[k-1][j-1]) /(fac-1.0);
                    fac=con2*fac;
                    errt=std::max(std::abs(a[k][j]-a[k-1][j]),std::abs(a[k][j]-a[k-1][j-1]));
                    if ( errt <= err.at(l).at(i))
                    {
                        if (errt <= err.at(l).at(i)) {err.at(l).at(i)=errt;result.at(l).at(i)=a[k][j];}
                    }
            }
            if (std::abs(a[j][j]-a[j-1][j-1]) >= safe*err.at(l).at(i)) break;
        }
        if (i != l)
        {result.at(i).at(l) = result.at(l).at(i);err.at(i).at(l) = err.at(l).at(i);}

    }
  }
  return result;
}

/*

std::vector<double>
NablaRidders(const std::vector<double> &phi,
               const std::function<double(std::vector<double>)> &f,
               const double &h)
{
  std::vector<double> result(phi.size());

  std::size_t ntab=10;
  double con=1.4, con2 = con*con;
  const double big = std::numeric_limits<double>::max();
  const double safe = 2.;
  std::size_t i,j,k;
  double errt,err,fac,hh;
  std::vector<std::vector<double>> a(ntab,
                                          std::vector<double>(ntab));
  if (h == 0.) throw("h must be nonzero in NablaRidders.");
  for (i = 0; i < phi.size(); i++)
  {
    hh=h;

    a[0][0] = FiniteDifference(phi,f,hh,i);
    err = big;
    for (j = 1; j < ntab; j++)
    {
        hh /= con;
        a[0][j] =FiniteDifference(phi,f,hh,i);
        fac=con2;
        for (k=1; k<=j; k++)
        {
            a[k][j]=(a[k-1][j]*fac-a[k-1][j-1]) /(fac-1.0);
                fac=con2*fac;
                errt=std::max(std::abs(a[k][j]-a[k-1][j]),std::abs(a[k][j]-a[k-1][j-1]));
                if ( errt <= err)
                {
                    if (errt <= err) {err=errt;result.at(i)=a[k][j];}
                }
        }
        if (std::abs(a[j][j]-a[j-1][j-1]) >= safe*err) break;
    }
  }
  return result;
}



std::vector<std::vector<double>>
HessianNablaRidders(const std::vector<double> &phi,
                 const std::function<double(std::vector<double>)> &V,
                 const double &eps, std::vector<double> &err)
{
    std::vector<std::vector<double>> result(phi.size(),
                                            std::vector<double>(phi.size()));
    for (std::size_t i = 0; i < phi.size();i++)
    {
        // dVeff/dw_i as a function
        std::function<double(std::vector<double>)> gradV_ati;
        gradV_ati = [&](std::vector<double> phi_in)
            {
                double error_in;
                return NablaRidders(phi_in,V,eps,error_in).at(i);
            };

        for (std::size_t j = i; j < phi.size();j++)
        {
            result.at(i).at(j) = NablaRidders(phi,gradV_ati, eps,err.at(i)).at(j);
            if (i!=j)
            {
                result.at(j).at(i) = result.at(i).at(j);
            }
        }

    }

    return result;

}

std::vector<std::vector<double>>
HessianNablaRidders(const std::vector<double> &phi,
                 const std::function<double(std::vector<double>)> &V,
                 const double &eps)
{
    std::vector<std::vector<double>> result(phi.size(),
                                            std::vector<double>(phi.size()));
    for (std::size_t i = 0; i < phi.size();i++)
    {
        // dVeff/dw_i as a function
        std::function<double(std::vector<double>)> gradV_ati;
        gradV_ati = [&](std::vector<double> phi_in)
            {return NablaRidders(phi_in,V,eps).at(i);};

        for (std::size_t j = i; j < phi.size();j++)
        {
            result.at(i).at(j) = NablaRidders(phi,gradV_ati, eps).at(j);
            if (i!=j)
            {
                result.at(j).at(i) = result.at(i).at(j);
            }
        }

    }

    return result;

}

 */


std::vector<std::vector<double>>
HessianNablaNumerical(const std::vector<double> &phi,
                 const std::function<double(std::vector<double>)> &V,
                 double eps)
{
    std::vector<std::vector<double>> result(phi.size(),
                                            std::vector<double>(phi.size()));
    for (std::size_t i = 0; i < phi.size();i++)
    {
        // dVeff/dw_i as a function
        std::function<double(std::vector<double>)> gradV_ati;
        gradV_ati = [&](std::vector<double> phi_in)
            {return NablaNumerical(phi_in,V,eps).at(i);};

        for (std::size_t j = 0; j < phi.size();j++)
            result.at(i).at(j) = NablaNumerical(phi,gradV_ati, eps).at(j);
    }

    return result;

}

std::vector<std::vector<double>>
HessianNumerical(const std::vector<double> &phi,
                 const std::function<double(std::vector<double>)> &V,
                 double eps)
{
  std::vector<std::vector<double>> result(phi.size(),
                                          std::vector<double>(phi.size()));
  for (size_t i = 0; i < phi.size(); i++)
  {
    double val = 0;
    auto xp    = phi;
    xp[i] += 2 * eps;
    val += V(xp);

    val -= 2 * V(phi);

    xp = phi;
    xp[i] -= 2 * eps;
    val += V(xp);

    result[i][i] = val / (4 * eps * eps);

    // https://en.wikipedia.org/wiki/Finite_difference
    for (size_t j = i + 1; j < phi.size(); j++)
    {
      double r = 0;

      xp = phi; // F(x+h, y+h)
      xp[i] += eps;
      xp[j] += eps;
      r += V(xp);

      xp = phi; //-F(x+h, y-h)
      xp[i] += eps;
      xp[j] -= eps;
      r -= V(xp);

      xp = phi; //-F(x-h, y+h)
      xp[i] -= eps;
      xp[j] += eps;
      r -= V(xp);

      xp = phi; // F(x-h, y-h)
      xp[i] -= eps;
      xp[j] -= eps;
      r += V(xp);

      result[i][j] = r / (4 * eps * eps);
      result[j][i] = r / (4 * eps * eps);
    }
  }

  return result;
}
} // namespace BSMPT
