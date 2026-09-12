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


double
Deriv2Numerical(const std::vector<double> &phi,
               const std::function<double(std::vector<double>)> &f,
               const double &eps,
               const size_t &i,
               const size_t &j)
{
  double result = 0;

  std::vector<double> l_jp2_ip2 = phi;
  l_jp2_ip2[j] += 2 * eps;
  l_jp2_ip2[i] += 2 * eps;
  std::vector<double> l_jp2_ip1 = phi;
  l_jp2_ip1[j] += 2 * eps;
  l_jp2_ip1[i] += eps;
  std::vector<double> l_jp2_im1 = phi;
  l_jp2_im1[j] += 2 * eps;
  l_jp2_im1[i] -= eps;
  std::vector<double> l_jp2_im2 = phi;
  l_jp2_im2[j] += 2 * eps;
  l_jp2_im2[i] -= 2 * eps;


  std::vector<double> l_jp1_ip2 = phi;
  l_jp1_ip2[j] += eps;
  l_jp1_ip2[i] += 2 * eps;
  std::vector<double> l_jp1_ip1 = phi;
  l_jp1_ip1[j] += eps;
  l_jp1_ip1[i] += eps;
  std::vector<double> l_jp1_im1 = phi;
  l_jp1_im1[j] += eps;
  l_jp1_im1[i] -= eps;
  std::vector<double> l_jp1_im2 = phi;
  l_jp1_im2[j] += eps;
  l_jp1_im2[i] -= 2 * eps;


  std::vector<double> l_jm1_ip2 = phi;
  l_jm1_ip2[j] -= eps;
  l_jm1_ip2[i] += 2 * eps;
  std::vector<double> l_jm1_ip1 = phi;
  l_jm1_ip1[j] -= eps;
  l_jm1_ip1[i] += eps;
  std::vector<double> l_jm1_im1 = phi;
  l_jm1_im1[j] -= eps;
  l_jm1_im1[i] -= eps;
  std::vector<double> l_jm1_im2 = phi;
  l_jm1_im2[j] -= eps;
  l_jm1_im2[i] -= 2 * eps;


  std::vector<double> l_jm2_ip2 = phi;
  l_jm2_ip2[j] -= 2 * eps;
  l_jm2_ip2[i] += 2 * eps;
  std::vector<double> l_jm2_ip1 = phi;
  l_jm2_ip1[j] -= 2 * eps;
  l_jm2_ip1[i] += eps;
  std::vector<double> l_jm2_im1 = phi;
  l_jm2_im1[j] -= 2 * eps;
  l_jm2_im1[i] -= eps;
  std::vector<double> l_jm2_im2 = phi;
  l_jm2_im2[j] -= 2 * eps;
  l_jm2_im2[i] -= 2 * eps;

  result -=     (-f(l_jp2_ip2) + 8 * f(l_jp2_ip1) - 8 * f(l_jp2_im1) + f(l_jp2_im2));
  result += 8 * (-f(l_jp1_ip2) + 8 * f(l_jp1_ip1) - 8 * f(l_jp1_im1) + f(l_jp1_im2));
  result -= 8 * (-f(l_jm1_ip2) + 8 * f(l_jm1_ip1) - 8 * f(l_jm1_im1) + f(l_jm1_im2));
  result +=     (-f(l_jm2_ip2) + 8 * f(l_jm2_ip1) - 8 * f(l_jm2_im1) + f(l_jm2_im2));


  return result / (12 * eps) / (12 * eps);
}

} // namespace BSMPT


