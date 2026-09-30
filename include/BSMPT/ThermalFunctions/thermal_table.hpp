#pragma once
#include <vector>
#include <string>
#include "BSMPT/ThermalFunctions/interpolation.hpp"

namespace BSMPT {
namespace ThermalFunctions {

class ThermalFunctionTables {
public:
    ThermalFunctionTables();

    double JB(double x) const;
    double DJB(double x) const;
    double JF(double x) const;
    double DJF(double x) const;

private:
    void load_table(const std::string& path,
                    std::vector<double>& x,
                    std::vector<double>& y,
                    Poly_interp& interp);

    std::vector<double> xJB, yJB;
    std::vector<double> xDJB, yDJB;
    std::vector<double> xJF, yJF;
    std::vector<double> xDJF, yDJF;

    double xJB_min, xJB_max;
    double xJF_min, xJF_max;
    
    mutable Poly_interp interp_JB;
    mutable Poly_interp interp_DJB;
    mutable Poly_interp interp_JF;
    mutable Poly_interp interp_DJF;
};

} // namespace ThermalFunctions
} // namespace BSMPT
