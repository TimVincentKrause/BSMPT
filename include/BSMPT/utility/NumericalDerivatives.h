#pragma once
#include <functional>
#include <vector>
#include <limits>
#include <cmath>

namespace BSMPT
{
/**
 * @brief Numerical method to calculate the
 * gradient of a function f using finite differences method.
 *
 * This method is used while BSMPT is not able to
 * calculate the potential derivative analytically. We used the 4th order
 * method
 *
 * \f$\frac{\partial f}{\partial \phi_i} = \frac{1}{12
 * \epsilon}\left(-f(\dots ,\vec{\phi}_i + 2  \epsilon ) + 8 f(\dots
 * ,\vec{\phi}_i + \epsilon )- 8 f(\dots ,\vec{\phi}_i - \epsilon ) +
 * f(\dots ,\vec{\phi}_i - 2  \epsilon )\right)\f$
 *
 * where \f$ \epsilon \f$ is a small step.
 *
 * @param phi Where we want to calculate the gradient
 * @param f function
 * @param eps Size of finite differences step
 * @return std::vector<double> The \f$ dim \times 1 \f$ gradient of V taken at
 * phi
 */
std::vector<double>
NablaNumerical(const std::vector<double> &phi,
               const std::function<double(std::vector<double>)> &f,
               const double &eps);

double
FiniteDifference2D(const std::vector<double> &phi,
               const std::function<double(std::vector<double>)> &f,
               const double &eps, const std::size_t &i, const std::size_t &j);

double
FiniteDifference2D(const std::vector<double> &phi,
               const std::function<double(std::vector<double>)> &f,
               const double &eps, const std::size_t &i);

double
FiniteDifference2Do4(const std::vector<double> &phi,
               const std::function<double(std::vector<double>)> &f,
               const double &eps, const std::size_t &i, const std::size_t &j);

double
FiniteDifference2Do4(const std::vector<double> &phi,
               const std::function<double(std::vector<double>)> &f,
               const double &eps, const std::size_t &i);

std::vector<double>
NablaRidders(const std::vector<double> &phi,
               const std::function<double(std::vector<double>)> &f,
               const std::vector<double> &eps, std::vector<double> &err);

std::vector<double>
NablaRidders(const std::vector<double> &phi,
               const std::function<double(std::vector<double>)> &f,
               const double &eps);

std::vector<double>
NablaRidderso6(const std::vector<double> &phi,
               const std::function<double(std::vector<double>)> &f,
               const double &eps);

std::vector<std::vector<double>>
HessianNablaRidders(const std::vector<double> &phi,
                 const std::function<double(std::vector<double>)> &V,
                 const std::vector<double> &eps, std::vector<std::vector<double>> &err);

std::vector<std::vector<double>>
HessianNablaRidderso4(const std::vector<double> &phi,
                 const std::function<double(std::vector<double>)> &V,
                 const std::vector<double> &eps, std::vector<std::vector<double>> &err);

std::vector<std::vector<double>>
HessianNablaRidderso4(const std::vector<double> &phi,
                 const std::function<double(std::vector<double>)> &V,
                 const double &eps);
/*
std::vector<double>
NablaRidders(const std::vector<double> &phi,
               const std::function<double(std::vector<double>)> &f,
               const double &eps);

std::vector<std::vector<double>>
HessianNablaRidders(const std::vector<double> &phi,
                 const std::function<double(std::vector<double>)> &V,
                 const double &eps);
*/
/**
 * @brief Numerical method to calculate the potential's (or other functions's)
 * hessian matrix using finite differences method.
 *
 * \f$\frac{\partial^2 V}{\partial \phi_i \phi_j} = \frac{1}{4
 * \epsilon^2}\left(V(\dots, \vec{\phi}_i + \epsilon , \vec{\phi}_j +
 * \epsilon) - V(\dots, \vec{\phi}_i - \epsilon , \vec{\phi}_j +
 * \epsilon) - V(\dots, \vec{\phi}_i + \epsilon , \vec{\phi}_j -
 * \epsilon) + V(\dots, \vec{\phi}_i - \epsilon , \vec{\phi}_j -
 * \epsilon) \right)\f$
 *
 * where \f$ \epsilon \f$ is a small step.
 *
 * @param phi Where we want to calculate the Hessian matrix
 * @param V Potential (or other function)
 * @param eps Size of finite differences step
 * @return std::vector<std::vector<double>> The \f$ dim \times \dim \f$
 *  hessian matrix of V taken at phi
 */
std::vector<std::vector<double>>
HessianNumerical(const std::vector<double> &phi,
                 const std::function<double(std::vector<double>)> &V,
                 double eps);


std::vector<std::vector<double>>
HessianNablaNumerical(const std::vector<double> &phi,
                 const std::function<double(std::vector<double>)> &V,
                 double eps);
} // namespace BSMPT
