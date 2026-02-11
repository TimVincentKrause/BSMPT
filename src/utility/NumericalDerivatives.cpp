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
    xp[i] += 4 * eps; //EU!!!
    val -= V(xp) / 12;

    xp = phi;
    xp[i] += 2 * eps;
    val += 4/3 * V(xp);

    val -= 5/2 * V(phi);

    xp = phi;
    xp[i] -= 2 * eps;
    val += 4/3 * V(xp);

    xp = phi;
    xp[i] -= 4 * eps;
    val -= V(xp) / 12;

    result[i][i] = val / (4 * eps * eps);

    // https://en.wikipedia.org/wiki/Finite_difference
    for (size_t j = i + 1; j < phi.size(); j++)
    {
      double r = 0;

      // Coefficients are 1/12, -2/3, 2/3, -1/12 for -2h, -h, +h, +2h

      xp = phi; // F(x+2h, y+2h)
      xp[i] += 2*eps;
      xp[j] += 2*eps;
      r += V(xp) /12. /12.;

      xp = phi; // F(x+2h, y+h)
      xp[i] += 2*eps;
      xp[j] += eps;
      r += -V(xp) /12. * 2/3.;

      xp = phi; // F(x+2h, y-h)
      xp[i] += 2*eps;
      xp[j] -= eps;
      r += V(xp) /12. * 2/3.;

      xp = phi; // F(x+2h, y-2h)
      xp[i] += 2*eps;
      xp[j] -= 2*eps;
      r += -V(xp) /12. /12.;
    


      xp = phi; // F(x+h, y+2h)
      xp[i] += eps;
      xp[j] += 2*eps;
      r += -V(xp) * 2/3. /12.;

      xp = phi; // F(x+h, y+h)
      xp[i] += eps;
      xp[j] += eps;
      r += V(xp) * 2/3. * 2/3.;

      xp = phi; // F(x+h, y-h)
      xp[i] += eps;
      xp[j] -= eps;
      r += -V(xp) * 2/3. * 2/3.;

      xp = phi; // F(x+h, y-2h)
      xp[i] += eps;
      xp[j] -= 2*eps;
      r += V(xp) * 2/3. /12.;



      xp = phi; // F(x-h, y+2h)
      xp[i] -= eps;
      xp[j] += 2*eps;
      r += V(xp) * 2/3. /12.;

      xp = phi; // F(x-h, y+h)
      xp[i] -= eps;
      xp[j] += eps;
      r += -V(xp) * 2/3. * 2/3.;

      xp = phi; // F(x-h, y-h)
      xp[i] -= eps;
      xp[j] -= eps;
      r += V(xp) * 2/3. * 2/3.;

      xp = phi; // F(x-h, y-2h)
      xp[i] -= eps;
      xp[j] -= 2*eps;
      r += -V(xp) * 2/3. /12.;



      xp = phi; // F(x-2h, y+2h)
      xp[i] -= 2*eps;
      xp[j] += 2*eps;
      r += -V(xp) /12. /12.;

      xp = phi; // F(x-2h, y+h)
      xp[i] -= 2*eps;
      xp[j] += eps;
      r += V(xp) /12. * 2/3.;

      xp = phi; // F(x-2h, y-h)
      xp[i] -= 2*eps;
      xp[j] -= eps;
      r += -V(xp) /12. * 2/3.;

      xp = phi; // F(x-2h, y-2h)
      xp[i] -= 2*eps;
      xp[j] -= 2*eps;
      r += V(xp) /12. /12.;

      result[i][j] = r / (eps * eps);
      result[j][i] = r / (eps * eps);

    }
  }

  return result;
}
} // namespace BSMPT