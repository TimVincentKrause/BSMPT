#include "BSMPT/ThermalFunctions/thermal_table.hpp"
#include <fstream>
#include <stdexcept>

namespace BSMPT {
namespace ThermalFunctions {

ThermalFunctionTables::ThermalFunctionTables::ThermalFunctionTables()
{
    std::string path;
    path = std::string(BSMPT_TABLES_PATH) + "/potential/JB.dat";
    load_table(path, xJB, yJB, interp_JB);
    path = std::string(BSMPT_TABLES_PATH) + "/potential/DJB.dat";
    load_table(path, xDJB, yDJB, interp_DJB);
    path = std::string(BSMPT_TABLES_PATH) + "/potential/JF.dat";
    load_table(path, xJF, yJF, interp_JF);
    path = std::string(BSMPT_TABLES_PATH) + "/potential/DJF.dat";
    load_table(path, xDJF, yDJF, interp_DJF);
    
    xJB_min = xJB.front();
    xJB_max = xJB.back();
    xJF_min = xJF.front();
    xJF_max = xJF.back();
}

void ThermalFunctionTables::load_table(const std::string& path,
                                  std::vector<double>& x,
                                  std::vector<double>& y,
                                  Poly_interp& interp)
{
    std::ifstream file(path);

    if (!file)
        throw std::runtime_error("Cannot open " + path);

    double xi, yi;

    while (file >> xi >> yi)
    {
        x.push_back(xi);
        y.push_back(yi);
    }

    interp = Poly_interp(x, y, 4);
}

double ThermalFunctionTables::JB(double x) const
{
    if (x > xJB_max)
        return 0.0;

    if (x < xJB_min)
        return interp_JB.interp(xJB_min);

    return interp_JB.interp(x);
}

double ThermalFunctionTables::DJB(double x) const
{
    if (x > xJB_max)
        return 0.0;

    if (x < xJB_min)
        return interp_DJB.interp(xJB_min);

    return interp_DJB.interp(x);
}

double ThermalFunctionTables::JF(double x) const
{
    if (x > xJF_max)
        return 0.0;

    if (x < xJF_min)
        return interp_JF.interp(xJF_min);

    return interp_JF.interp(x);
}

double ThermalFunctionTables::DJF(double x) const
{
    if (x > xJF_max)
        return 0.0;

    if (x < xJF_min)
        return interp_DJF.interp(xJF_min);

    return interp_DJF.interp(x);
}

} // namespace ThermalFunctions
} // namespace BSMPT