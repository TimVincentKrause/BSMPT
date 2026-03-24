// Copyright (C) 2020  Philipp Basler, Margarete Mühlleitner and Jonas
// SPDX-FileCopyrightText: 2021 Philipp Basler, Margarete Mühlleitner and Jonas
// Müller
//
// SPDX-License-Identifier: GPL-3.0-or-later

/**
 * @file
 * Calculates the electroweak phase transition for a given Inputfile for a given
 * subset of lines in the file and adds it at the end of the line in the format
 * T_c v_c all single vevs. One parameter point per line.
 *
 */

#include <BSMPT/minimizer/Minimizer.h>
#include <BSMPT/models/ClassPotentialOrigin.h> // for Class_Potential_Origin
#include <BSMPT/models/IncludeAllModels.h>
#include <BSMPT/models/modeltests/ModelTestfunctions.h>
#include <BSMPT/utility/Logger.h>
#include <BSMPT/utility/parser.h>
#include <BSMPT/utility/utility.h>
#include <algorithm> // for copy
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>   // for unique_ptr
#include <stdlib.h> // for EXIT_FAILURE, atoi
#include <string>   // for getline, string
#include <utility>  // for pair
#include <vector>   // for vector
#include <BSMPT/ThermalFunctions/ThermalFunctions.h> //EU!!!
#include "Eigen/Eigenvalues" //EU!!!
#include <BSMPT/utility/NumericalDerivatives.h> //EU!!!

using namespace std;
using namespace BSMPT;
using namespace ThermalFunctions;

struct CLIOptions
{
  BSMPT::ModelID::ModelIDs Model{ModelID::ModelIDs::NotSet};
  int Line{};
  std::string InputFile, outputfile;
  double spectemp{0};
  bool UseGSL{Minimizer::UseGSLDefault};
  bool UseCMAES{Minimizer::UseLibCMAESDefault};
  bool UseNLopt{Minimizer::UseNLoptDefault};
  int WhichMinimizer{Minimizer::WhichMinimizerDefault};
  bool UseMultithreading{true};

  CLIOptions(const BSMPT::parser &argparser);
  bool good() const;
};

BSMPT::parser prepare_parser();

std::vector<std::string> convert_input(int argc, char *argv[]);

int main(int argc, char *argv[])
try
{
  const auto SMConstants = GetSMConstants();
  auto argparser         = prepare_parser();
  argparser.add_input(convert_input(argc, argv));
  const CLIOptions args(argparser);
  if (not args.good())
  {
    return EXIT_FAILURE;
  }

  int linecounter = 1, filecounter = 1;
  std::ifstream infile(args.InputFile);
  if (!infile.good())
  {
    Logger::Write(LoggingLevel::Default, "Input file not found ");
    return EXIT_FAILURE;
  }

  Logger::Write(LoggingLevel::ProgDetailed, "Found file");

  std::string linestr;
  std::unique_ptr<Class_Potential_Origin> modelPointer =
      ModelID::FChoose(args.Model, SMConstants);

  Logger::Write(LoggingLevel::ProgDetailed, "Created modelpointer ");

  while (getline(infile, linestr))
  {
    if (linecounter > args.Line) break;

    if (linecounter == 1)
    {

      modelPointer->setUseIndexCol(linestr);
    }
    if (linecounter == args.Line and linecounter != 1)
    {
      Logger::Write(LoggingLevel::ProgDetailed, "Found line");

      std::string outfilename =
          args.outputfile + "_" + std::to_string(filecounter) + ".tsv";
      Logger::Write(LoggingLevel::ProgDetailed,
                    "Creating outfile with name " + outfilename);
      std::ofstream outfile(outfilename);
      if (!outfile.good())
      {
        Logger::Write(LoggingLevel::Default,
                      "Can not create file " + outfilename);
        return EXIT_FAILURE;
      }

      std::vector<std::string> Legend;
      // std::size_t NHiggs = modelPointer->get_NHiggs();
      // std::size_t NGauge = modelPointer->get_NGauge();
      // std::size_t NQuarks = modelPointer->get_NQuarks();
      // std::size_t NLepton = modelPointer->get_NLepton();

      Legend.push_back("ysq");
      Legend.push_back("Jbd");
      Legend.push_back("Jbd1");
      Legend.push_back("Jbd2");
      Legend.push_back("Jbd3");

      outfile << Legend << std::endl;

      modelPointer->initModel(linestr);
      modelPointer->write();

      // construct Temperature and Vev vonfiguration for 5415;
      double Tmax = args.spectemp;

      // for (double T = 0; T < Tmax; T++)
      // {

      //   int Tint = std::round(T);

      //   std::vector<double> vevlist = {246.219650833599,246.2195858910263,246.2191083257477,246.2190663719373,246.2188931741589,246.2186421001112,246.2185427168938,246.2186210733491,246.2189291612548,246.219515110149,246.2204181887464,246.2216655516637,246.2232701800374,246.2252298243666,246.2275268135768,246.2301279506954,246.2329845284746,246.2360322186214,246.2391909460319,246.2423646801424,246.2454413691011,246.248293001049,246.2507759587542,246.2527315741302,246.2539869283655,246.2543558446333,246.253640127219,246.2512632743035,246.247289808434,246.2415252168055,246.2336070448441,246.2231849567339,246.2101756947443,246.1943033588616,246.1752898785407,246.152855315957,246.1267186318195,246.096624454402,246.0623523198485,246.023637178235,245.9802116615747,245.9316459748547,245.8776125364726,245.8180855548182,245.7528147770934,245.6815551537045,245.6040669272426,245.5201155351087,245.4294716331192,245.3319112057026,245.2272153422897,245.1151702627332,244.995567275182,244.8682026437937,244.7328775686867,244.5893981301129,244.4375750720817,244.2772239321746,244.1081647655654,243.930222200813,243.7432254149455,243.5470078905755,243.3414076631283,243.1262669877508,242.901432622225,242.6667556434277,242.422091513486,242.1673001954867,241.9022461190789,241.6267981992333,241.3408305898682,241.0442217902641,240.7368555382239,240.4186209493359,240.0894124414294,239.7491303967404,239.3976812318665,239.0349779271922,238.6609403334251,238.2754955544555,237.8785786876671,237.4701332889453,237.0501118110014,236.6184767049072,236.175200785999,235.7202683126519,235.2536757585719,234.7754329505275,234.2855640126045,233.7841086815403,233.2711233809363,232.7466827633736,232.2108811223804,231.6638339141383,231.10567946501,230.5365808055542,229.9567275273969,229.3663376786027,228.7656600131747,228.1549761599928,227.5346027300733,226.904893803446,226.2662433336062,225.6190874635222,224.9639069338087,224.3012296333585,223.5952738661156,222.8486845788791,222.0865267013941,221.3085805932105,220.5146206348926,219.7044145014231,218.8777227942227,218.0342988836494,217.1738879622499,216.2962269284037,215.4010438384241,214.4880570790482,213.5569752654099,212.6074963087519,211.6393069191221,210.6520821060734,209.6454841302557,208.6191622601887,207.5727514889432,206.5058720446324,205.4181285149513,204.3091085573419,203.1783823278086,202.0255010547869,200.8499960585872,199.6513774052141,198.4291324877606,197.1827247120495,195.9115916508574,194.6151436129241,193.2927614789858,191.943794624776,190.5675589826093,189.1633344913301,187.7303619434022,186.267840515151,184.7749245490895,183.2507193881005,181.6942782818773,180.1045969590118,178.4806096288252,176.8211830374969,175.125110646686,173.3911052268415,171.6177921764559,169.8036997902683,167.9472507281108,166.0467497563765,164.1003716697938,162.1061470179457,160.0619450264394,157.9654551611422,155.8141641560523,153.6053309424022,151.3359557045238,149.0027451281847,146.6020656058855,144.1299068378495,141.5539671766995,138.8534595016647,136.0876630669459,133.2379966682787,130.2936132413515,127.2444548633579,124.0799216759225,120.7881764594623,117.3556200275909,113.7662769716745,110.0010375942661,106.0366015782608,101.8440009770515,97.38635883228785,92.6154238345834,87.46589005870786,81.84540281872538,75.61549405344623,68.55084537061835,60.23532822572897,49.70691289426178,32.94355539016712,23.19042487428218,2.893205738239051e-09,2.893205738239051e-09,2.893205738239051e-09,2.893205738239051e-09,2.893205738239051e-09,2.893205738239051e-09,2.893205738239051e-09,2.893205738239051e-09,2.893205738239051e-09,2.893205738239051e-09,2.893205738239051e-09,2.893205738239051e-09,2.893205738239051e-09,2.893205738239051e-09};
      //   std::vector<double> vev = {0,0,0,0,0};
      //   vev[1] = vevlist[Tint];
      //   outfile << T;
      //   outfile << sep << vev[1];

      //   double JbmodelPointer-> JbosonInterpolated(20000,2);
      //   std::cout double

      //   outfile << std::endl;
      //   break
      // }

      for (double ysq=-100; ysq < 100; ysq++)
      {
        double Jb,Jb1,Jb2,Jb3;
        if (ysq <= 0)
        {
            Jb =  JbosonInterpolated(ysq,0);
            Jb1 =  JbosonInterpolated(ysq,1);
            Jb2 =  JbosonInterpolated(ysq,2);
            Jb3 =  JbosonInterpolated(ysq,3);
        }
        else
        {
            Jb =  JbosonNumericalIntegration(ysq,0);
            Jb1 =  JbosonNumericalIntegration(ysq,1);
            Jb2 =  JbosonNumericalIntegration(ysq,2);
            Jb3 =  JbosonNumericalIntegration(ysq,3);
        }

        // calculate the Hessian of the potential
        std::vector<std::vector<std::vector<double>>> ThirdNum;
        double eps = 0.1;

        // Define effective potential
        std::function<double(std::vector<double>)> func;
        func = [&](std::vector<double> effvev)
        {
          return std::pow(effvev.at(0)+effvev.at(1)+effvev.at(2),3);
        };

        std::vector<double> pt = {0,0,0};
        ThirdNum = ThirdTensorDerivativeNumerical(pt,func, eps);
        std::cout << "ThirdTensorDerivative" << std::endl;
        std::cout << ThirdNum << std::endl;


        outfile << ysq << sep;
        outfile << Jb << sep;
        outfile << Jb1 << sep;
        outfile << Jb2 << sep;
        outfile << Jb3;
        outfile << std::endl;
      }

      filecounter++;
      outfile.close();
      // std::vector<double> dummy;
      // modelPointer->Debugging(dummy, dummy);
      // ModelTests::CheckImplementation(*modelPointer, args.WhichMinimizer);
    }
    linecounter++;
    if (infile.eof()) break;
  }
  return EXIT_SUCCESS;
}
catch (int)
{
  return EXIT_SUCCESS;
}
catch (exception &e)
{
  Logger::Write(LoggingLevel::Default, e.what());
  return EXIT_FAILURE;
}

bool CLIOptions::good() const
{
  if (UseGSL and not Minimizer::UseGSLDefault)
  {
    throw std::runtime_error(
        "You set --useGSL=true but GSL was not found during compilation.");
  }
  if (UseCMAES and not Minimizer::UseLibCMAESDefault)
  {
    throw std::runtime_error(
        "You set --useCMAES=true but CMAES was not found during compilation.");
  }
  if (UseNLopt and not Minimizer::UseNLoptDefault)
  {
    throw std::runtime_error(
        "You set --useNLopt=true but NLopt was not found during compilation.");
  }
  if (WhichMinimizer == 0)
  {
    throw std::runtime_error(
        "You disabled all minimizers. You need at least one.");
  }
  if (Model == ModelID::ModelIDs::NotSet)
  {
    Logger::Write(
        LoggingLevel::Default,
        "Your Model parameter does not match with the implemented Models.");
    ShowInputError();
    return false;
  }
  if (Line < 1)
  {
    Logger::Write(LoggingLevel::Default, "Start line counting with 1");
    return false;
  }
  return true;
}

CLIOptions::CLIOptions(const BSMPT::parser &argparser)
{
  argparser.check_required_parameters();
  Model     = BSMPT::ModelID::getModel(argparser.get_value("model"));
  InputFile = argparser.get_value("input");
  outputfile = argparser.get_value("output");
  Line      = argparser.get_value<int>("line");
  spectemp  = argparser.get_value<int>("spectemp");

  try
  {
    spectemp = argparser.get_value<double>("spectemp");
  }
  catch (BSMPT::parserException &)
  {
  }

  try
  {
    UseGSL = argparser.get_value<bool>("useGSL");
  }
  catch (BSMPT::parserException &)
  {
  }

  try
  {
    UseCMAES = argparser.get_value<bool>("useCMAES");
  }
  catch (BSMPT::parserException &)
  {
  }

  try
  {
    UseNLopt = argparser.get_value<bool>("useNLopt");
  }
  catch (BSMPT::parserException &)
  {
  }

  try
  {
    UseMultithreading = argparser.get_value<bool>("useMultithreading");
  }
  catch (BSMPT::parserException &)
  {
  }

  WhichMinimizer = Minimizer::CalcWhichMinimizer(UseGSL, UseCMAES, UseNLopt);
}

BSMPT::parser prepare_parser()
{
  BSMPT::parser argparser;
  argparser.add_argument("model", "The model you want to investigate.", true);
  argparser.add_argument("input", "The input file in tsv format.", true);
  argparser.add_argument("output", "output file (in tsv format)", true);
  argparser.add_argument("spectemp", "specific temperature [GeV]", "0", false);
  argparser.add_argument(
      "line",
      "The line in the input file with the parameter point used to "
      "check the model.",
      true);

  std::stringstream ss;
  ss << "Test performs a serious of tests on the given model. "
        "Intended for testing new models."
     << std::endl
     << "It is called either by " << std::endl
     << "./Test model input Line" << std::endl
     << "or with the following arguments" << std::endl;
  argparser.set_help_header(ss.str());

  argparser.enable_minimizer_options();

  return argparser;
}

std::vector<std::string> convert_input(int argc, char *argv[])
{
  std::vector<std::string> arguments;
  if (argc == 1) return arguments;
  auto first_arg = std::string(argv[1]);

  bool UsePrefix =
      StringStartsWith(first_arg, "--") or StringStartsWith(first_arg, "-");

  if (UsePrefix)
  {
    for (int i{1}; i < argc; ++i)
    {
      arguments.emplace_back(argv[i]);
    }
  }
  else
  {
    if (argc >= 2)
    {
      arguments.emplace_back("--model=" + std::string(argv[1]));
    }
    if (argc >= 3)
    {
      arguments.emplace_back("--input=" + std::string(argv[2]));
    }
    if (argc >= 4)
    {
      arguments.emplace_back("--output=" + std::string(argv[2]));
    }
    if (argc >= 5)
    {
      arguments.emplace_back("--line=" + std::string(argv[3]));
    }
  }
  return arguments;
}
