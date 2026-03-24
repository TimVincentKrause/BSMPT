// Copyright (C) 2024 Lisa Biermann, Margarete Mühlleitner, Rui Santos, João
// Viana SPDX-FileCopyrightText: 2021 Philipp Basler, Margarete Mühlleitner and
// Jonas Müller
//
// SPDX-License-Identifier: GPL-3.0-or-later

/**
 * @file
 * This program traces all minima in a temperature range
 *
 */

#include <BSMPT/minimizer/Minimizer.h>
#include <BSMPT/minimum_tracer/minimum_tracer.h> // MinimumTracer
#include <BSMPT/models/ClassPotentialOrigin.h>   // for Class_Potential_Origin
#include <BSMPT/models/IncludeAllModels.h>
#include <BSMPT/transition_tracer/transition_tracer.h> // TransitionTracer
#include <BSMPT/utility/Logger.h>
#include <BSMPT/utility/parser.h>
#include <BSMPT/utility/utility.h>
#include <algorithm> // for copy, max
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory> // for shared_ptr, __shared_...
#include <random>
#include <stdlib.h> // for atoi, EXIT_FAILURE
#include <string>   // for string, operator<<
#include <utility>  // for pair
#include <vector>   // for vector
#include <BSMPT/utility/NumericalDerivatives.h> //EU!!!
#include "Eigen/Eigenvalues" //EU!!!

using namespace std;
using namespace BSMPT;

struct CLIOptions
{
  BSMPT::ModelID::ModelIDs Model{ModelID::ModelIDs::NotSet};
  int line{0}, npoints{100};
  std::string inputfile, outputfile;
  double templow{0}, temphigh{300};
  bool UseGSL{Minimizer::UseGSLDefault};
  bool UseCMAES{Minimizer::UseLibCMAESDefault};
  bool UseNLopt{Minimizer::UseNLoptDefault};
  int WhichMinimizer{Minimizer::WhichMinimizerDefault};
  bool UseMultithreading{false};
  MultiStepPTMode UseMultiStepPTMode{MultiStepPTMode::Default};
  int CheckEWSymmetryRestoration{1};
  int num_check_pts{10};

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

  std::ifstream infile(args.inputfile);
  if (!infile.good())
  {
    Logger::Write(LoggingLevel::Default,
                  "Input file " + args.inputfile + " not found ");
    return EXIT_FAILURE;
  }

  Logger::Write(LoggingLevel::ProgDetailed, "Found file");

  std::shared_ptr<BSMPT::Class_Potential_Origin> modelPointer =
      ModelID::FChoose(args.Model, SMConstants);

  Logger::Write(LoggingLevel::ProgDetailed, "Created modelpointer ");

  std::string linestr, linestr_store;
  int linecounter = 1, filecounter = 1;

  std::size_t nPar, nParCT;
  nPar   = modelPointer->get_nPar();
  nParCT = modelPointer->get_nParCT();
  std::vector<double> par(nPar);
  std::vector<double> parCT(nParCT);
  std::size_t NHiggs = modelPointer->get_NHiggs();

  while (getline(infile, linestr))
  {
    if (linecounter == 1) linestr_store = linestr;

    if (linecounter > args.line) break;

    if (linecounter == args.line)
    {
      Logger::Write(LoggingLevel::ProgDetailed,
                    "Currently at line " + std::to_string(linecounter));

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

      modelPointer->setUseIndexCol(linestr_store);

      std::pair<std::vector<double>, std::vector<double>> parameters =
          modelPointer->initModel(linestr);

      modelPointer->write();

      auto start = std::chrono::high_resolution_clock::now();

      std::shared_ptr<MinimumTracer> MinTracer(new MinimumTracer(
          modelPointer, args.WhichMinimizer, args.UseMultithreading));

      // NLO stability check
      bool nlostable = modelPointer->CheckNLOVEV(
          MinTracer->ConvertToVEVDim(MinTracer->GetGlobalMinimum(0)));
      StatusNLOStability status_nlostable =
          MinTracer->GetStatusNLOVEV(nlostable);
      Logger::Write(LoggingLevel::ProgDetailed,
                    "Status of NLO stability check is: " +
                        StatusNLOStabilityToString.at(status_nlostable));

      // EWSR check
      double EWSymmetryRestoration_status = 0;
      StatusEWSR status_ewsr              = StatusEWSR::Off;

      if (args.CheckEWSymmetryRestoration > 0)
      {
        EWSymmetryRestoration_status =
            MinTracer->IsThereEWSymmetryRestoration();
        status_ewsr = MinTracer->GetStatusEWSR(EWSymmetryRestoration_status);
      }
      else
      {
        Logger::Write(LoggingLevel::ProgDetailed,
                      "Check for EW symmetry restoration is disabled.\n");
      }

      // // phase tracking
      // Logger::Write(
      //     LoggingLevel::ProgDetailed,
      //     "Track phases in between T_low = " + std::to_string(args.templow) +
      //         " and T_high = " + std::to_string(args.temphigh));

      // //bool do_only_tracing = true;

      // Vacuum vac(args.templow,
      //            args.temphigh,
      //            MinTracer,
      //            modelPointer,
      //            args.UseMultiStepPTMode,
      //            args.num_check_pts);

      // Logger::Write(LoggingLevel::ProgDetailed,
      //               "Found and traced " +
      //                   std::to_string(vac.PhasesList.size()) +
      //                   " minima with status = " +
      //                   StatusTracingToString.at(vac.status_vacuum) +
      //                   ".\n-------------------------------");

      // if ((vac.PhasesList.size() != 2) and (vac.PhasesList.size() != 1))
      //   throw std::runtime_error(
      //       "This code is not ready for handling anything other than "
      //       "two phases!\n");


      // prepare legend
      std::vector<std::string> LegendMinima;
      LegendMinima.push_back("status_nlo_stability");
      LegendMinima.push_back("status_ewsr");
      //LegendMinima.push_back("status_tracing");

      LegendMinima.push_back("Temp");

      for (std::size_t j = 0; j < modelPointer->get_nVEV(); j++)
        LegendMinima.push_back(modelPointer->addLegendVEV().at(j));

      // NLO Legend
      for (auto x : modelPointer->addLegendCT())
        LegendMinima.push_back(x + "_T");
      for (auto x : modelPointer->addLegendTripleCouplings())
        LegendMinima.push_back(x);

      LegendMinima.push_back("runtime");
      outfile << linestr_store << sep << modelPointer->addLegendCT() << sep
              << LegendMinima << std::endl;

      //std::size_t length = 0;
      // for (std::size_t j = 0; j < vac.PhasesList.size(); j++)
      // {
      //   if (length == 0)
      //   {
      //     length = vac.PhasesList.at(j).MinimumPhaseVector.size();
      //   }
      //   else if (length < vac.PhasesList.at(j).MinimumPhaseVector.size())
      //   {
      //     length = vac.PhasesList.at(j).MinimumPhaseVector.size();
      //   }
      // }

      // if (length == 0)
      // {
      //   outfile << std::setprecision(16);
      //   outfile << linestr;
      //   outfile << sep << parameters.second;
      //   outfile << sep << status_nlostable;
      //   outfile << sep << status_ewsr;
      //   outfile << sep << vac.status_vacuum;
      //   auto time = std::chrono::duration_cast<std::chrono::milliseconds>(
      //                   std::chrono::high_resolution_clock::now() - start)
      //                   .count() /
      //               1000.;

      //   outfile << sep << time;
      //   outfile << std::endl;
      // }

      std::vector<double> T_list;

      for (int n = 0; n < args.npoints; n++)
        T_list.push_back(args.templow +
                         n * (args.temphigh - args.templow) / args.npoints);

      // std::cout << "tcrit=" << vac.CoexPhasesList.at(0).crit_temp << std::endl;
      // if (vac.PhasesList.size() == 2)
      // {
      //   T_list.insert(std::lower_bound(T_list.begin(),
      //                                 T_list.end(),
      //                                 vac.CoexPhasesList.at(0).crit_temp),
      //                 vac.CoexPhasesList.at(0).crit_temp);
      // }

      // for every temperature:

      std::pair<std::vector<double>, std::vector<double>> tripleHiggsparameters =
          modelPointer->initModel(linestr);
      par   = tripleHiggsparameters.first;
      parCT = tripleHiggsparameters.second;

      std::vector<double> vevlist = {246.219650833599,246.2195858910263,246.2191083257477,246.2190663719373,246.2188931741589,246.2186421001112,246.2185427168938,246.2186210733491,246.2189291612548,246.219515110149,246.2204181887464,246.2216655516637,246.2232701800374,246.2252298243666,246.2275268135768,246.2301279506954,246.2329845284746,246.2360322186214,246.2391909460319,246.2423646801424,246.2454413691011,246.248293001049,246.2507759587542,246.2527315741302,246.2539869283655,246.2543558446333,246.253640127219,246.2512632743035,246.247289808434,246.2415252168055,246.2336070448441,246.2231849567339,246.2101756947443,246.1943033588616,246.1752898785407,246.152855315957,246.1267186318195,246.096624454402,246.0623523198485,246.023637178235,245.9802116615747,245.9316459748547,245.8776125364726,245.8180855548182,245.7528147770934,245.6815551537045,245.6040669272426,245.5201155351087,245.4294716331192,245.3319112057026,245.2272153422897,245.1151702627332,244.995567275182,244.8682026437937,244.7328775686867,244.5893981301129,244.4375750720817,244.2772239321746,244.1081647655654,243.930222200813,243.7432254149455,243.5470078905755,243.3414076631283,243.1262669877508,242.901432622225,242.6667556434277,242.422091513486,242.1673001954867,241.9022461190789,241.6267981992333,241.3408305898682,241.0442217902641,240.7368555382239,240.4186209493359,240.0894124414294,239.7491303967404,239.3976812318665,239.0349779271922,238.6609403334251,238.2754955544555,237.8785786876671,237.4701332889453,237.0501118110014,236.6184767049072,236.175200785999,235.7202683126519,235.2536757585719,234.7754329505275,234.2855640126045,233.7841086815403,233.2711233809363,232.7466827633736,232.2108811223804,231.6638339141383,231.10567946501,230.5365808055542,229.9567275273969,229.3663376786027,228.7656600131747,228.1549761599928,227.5346027300733,226.904893803446,226.2662433336062,225.6190874635222,224.9639069338087,224.3012296333585,223.5952738661156,222.8486845788791,222.0865267013941,221.3085805932105,220.5146206348926,219.7044145014231,218.8777227942227,218.0342988836494,217.1738879622499,216.2962269284037,215.4010438384241,214.4880570790482,213.5569752654099,212.6074963087519,211.6393069191221,210.6520821060734,209.6454841302557,208.6191622601887,207.5727514889432,206.5058720446324,205.4181285149513,204.3091085573419,203.1783823278086,202.0255010547869,200.8499960585872,199.6513774052141,198.4291324877606,197.1827247120495,195.9115916508574,194.6151436129241,193.2927614789858,191.943794624776,190.5675589826093,189.1633344913301,187.7303619434022,186.267840515151,184.7749245490895,183.2507193881005,181.6942782818773,180.1045969590118,178.4806096288252,176.8211830374969,175.125110646686,173.3911052268415,171.6177921764559,169.8036997902683,167.9472507281108,166.0467497563765,164.1003716697938,162.1061470179457,160.0619450264394,157.9654551611422,155.8141641560523,153.6053309424022,151.3359557045238,149.0027451281847,146.6020656058855,144.1299068378495,141.5539671766995,138.8534595016647,136.0876630669459,133.2379966682787,130.2936132413515,127.2444548633579,124.0799216759225,120.7881764594623,117.3556200275909,113.7662769716745,110.0010375942661,106.0366015782608,101.8440009770515,97.38635883228785,92.6154238345834,87.46589005870786,81.84540281872538,75.61549405344623,68.55084537061835,60.23532822572897,49.70691289426178,32.94355539016712,23.19042487428218,2.893205738239051e-09,2.893205738239051e-09,2.893205738239051e-09,2.893205738239051e-09,2.893205738239051e-09,2.893205738239051e-09,2.893205738239051e-09,2.893205738239051e-09,2.893205738239051e-09,2.893205738239051e-09,2.893205738239051e-09,2.893205738239051e-09,2.893205738239051e-09,2.893205738239051e-09};
      std::vector<double> vev = {0,0,0,0,0};
      size_t Tcount = -1;
      for (const auto T : T_list)
      {
        Tcount += 1;
        std::cout << "Tcount: " << Tcount << std::endl;


        //if (T == 0){continue;}
        outfile << std::setprecision(16);
        outfile << linestr;
        outfile << sep << parameters.second;
        outfile << sep << status_nlostable;
        outfile << sep << status_ewsr;
        //outfile << sep << vac.status_vacuum;
        outfile << sep << T;
        vev.at(1) = vevlist.at(Tcount);




        // if (vac.PhasesList.size() == 2)
        // {
        //   if (T <= vac.CoexPhasesList.at(0).crit_temp)
        //   {
        //     vev = vac.PhasesList.at(1).Get(T).point;
        //   }
        //   else
        //   {
        //     vev = vac.PhasesList.at(0).Get(T).point;
        //   }
        // }
        // else
        // {
        //   vev = vac.PhasesList.at(0).Get(T).point;
        // }

        std::cout<<"T = "<<T<<std::endl;
        std::cout<<"v ="<<vev<<std::endl;
        // std::vector<double> MassSquaredGauge = modelPointer->addLegendTripleCouplings()
        // std::cout << "MassSquaredGauge = " << MassSquaredGauge << std::endl;

        outfile << sep << vev;
        // std::vector<std::string> labels = modelPointer->addLegendTripleCouplings();

        // change these
        modelPointer->CalculatePhysicalCouplings(modelPointer->MinimizeOrderVEV(vev));
        modelPointer->AdjustRotationMatrix();
        modelPointer->Prepare_Triple(modelPointer->MinimizeOrderVEV(vev));
        modelPointer->TripleHiggsCouplings(modelPointer->MinimizeOrderVEV(vev),T);

        // calculate the Hessian of the potential
        std::vector<std::vector<std::vector<double>>> ThirdNum;
        double eps = 0.1;

        // Define effective potential
        std::function<double(std::vector<double>)> Veff;
        Veff = [&](std::vector<double> effvev)
        {
          // Potential wrapper
          return modelPointer->VEff(effvev, T);
        };

        ThirdNum = ThirdTensorDerivativeNumerical(modelPointer->MinimizeOrderVEV(vev),Veff, eps);

        for (std::size_t i = 0; i < nParCT; i++)
          outfile << sep << parCT[i];
        for (std::size_t i = 0; i < NHiggs; i++)
        {
          for (std::size_t j = i; j < NHiggs; j++)
          {
            for (std::size_t k = j; k < NHiggs; k++)
            {
            //   std::cout << labels.at(i + j * NHiggs + k * NHiggs * NHiggs) << std::endl;
            //   std::cout<<"Couplings_Gauge_Higgs_21 ="<< Couplings_Gauge_Higgs_21 <<std::endl;
            //   std::cout<<"Couplings_Gauge_Higgs_22 ="<< Couplings_Gauge_Higgs_22 <<std::endl;
              outfile << sep
                      << -modelPointer->get_TripleHiggsCorrectionsTreePhysical(
                            i, j, k);
              outfile << sep
                      << -modelPointer->get_TripleHiggsCorrectionsCTPhysical(
                            i, j, k);
              outfile << sep
                      << -modelPointer->get_TripleHiggsCorrectionsCWPhysical(
                            i, j, k);
              outfile << sep
                      << -modelPointer->get_TripleHiggsCorrectionsThermPhysical(
                            i, j, k);
              outfile << sep << ThirdNum.at(i).at(j).at(k);
              // if ((i==4) & (j==4) & (k==4))
              // {
              //   std::cout << "TripleHiggsCorrectionsCWPhysical[" << i << "][" << j << "][" << k << "] = " << modelPointer->get_TripleHiggsCorrectionsCWPhysical(
              //               i, j, k) << std::endl;
              // }

            }
          }
        }
        //break;
        auto time = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::high_resolution_clock::now() - start)
                        .count() /
                    1000.;

        outfile << sep << time;
        outfile << std::endl;
      }

      filecounter++;
      outfile.close();

      auto time = std::chrono::duration_cast<std::chrono::milliseconds>(
                      std::chrono::high_resolution_clock::now() - start)
                      .count() /
                  1000.;

      BSMPT::Logger::Write(BSMPT::LoggingLevel::ProgDetailed,
                           "\nTook\t" + std::to_string(time) + " seconds.\n");
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
        "You set --UseGSL=true but GSL was not found during compilation.");
  }
  if (UseCMAES and not Minimizer::UseLibCMAESDefault)
  {
    throw std::runtime_error(
        "You set --UseCMAES=true but CMAES was not found during compilation.");
  }
  if (UseNLopt and not Minimizer::UseNLoptDefault)
  {
    throw std::runtime_error(
        "You set --UseNLopt=true but NLopt was not found during compilation.");
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
  if (line == 0)
  {
    Logger::Write(LoggingLevel::Default, "line not set.");
    return false;
  }

  if (templow >= temphigh)
  {
    Logger::Write(LoggingLevel::Default,
                  "Invalid temperature choice. Thigh has to be > 0 GeV.");
    return false;
  }
  if (npoints <= 0)
  {
    Logger::Write(LoggingLevel::Default,
                  "Invalid npoints choice. npoints has to be > 0.");
    return false;
  }
  if (CheckEWSymmetryRestoration > 2 or CheckEWSymmetryRestoration < 0)
  {
    Logger::Write(LoggingLevel::Default,
                  "Invalid choice for CheckEWSymmetryRestoration.");
    return false;
  }

  return true;
}

CLIOptions::CLIOptions(const BSMPT::parser &argparser)
{
  std::stringstream ss;
  argparser.check_required_parameters();

  // required arguments
  Model      = BSMPT::ModelID::getModel(argparser.get_value("model"));
  inputfile  = argparser.get_value("input");
  outputfile = argparser.get_value("output");
  line  = argparser.get_value<int>("line");


  // optional arguments
  try
  {
    temphigh = argparser.get_value<double>("thigh");
  }
  catch (BSMPT::parserException &)
  {
    ss << "--thigh not set, using default value: " << temphigh << "\n";
  }

  try
  {
    npoints = argparser.get_value<double>("npoints");
  }
  catch (BSMPT::parserException &)
  {
    ss << "--npoints not set, using default value: " << temphigh << "\n";
  }



  std::string GSLhelp   = Minimizer::UseGSLDefault ? "true" : "false";
  std::string CMAEShelp = Minimizer::UseLibCMAESDefault ? "true" : "false";
  std::string NLoptHelp = Minimizer::UseNLoptDefault ? "true" : "false";
  try
  {
    UseGSL = (argparser.get_value("usegsl") == "true");
  }
  catch (BSMPT::parserException &)
  {
    ss << "--usegsl not set, using default value: " << GSLhelp << "\n";
  }

  try
  {
    UseCMAES = (argparser.get_value("usecmaes") == "true");
  }
  catch (BSMPT::parserException &)
  {
    ss << "--usecmaes not set, using default value: " << CMAEShelp << "\n";
  }

  try
  {
    UseNLopt = (argparser.get_value("usenlopt") == "true");
  }
  catch (BSMPT::parserException &)
  {
    ss << "--usenlopt not set, using default value: " << NLoptHelp << "\n";
  }

  try
  {
    UseMultithreading = (argparser.get_value("usemultithreading") == "true");
  }
  catch (BSMPT::parserException &)
  {
    ss << "--usemultithreading not set, using default value: false\n";
  }

  // UseMultiStepPTMode
  try
  {
    auto multistepPT_string = argparser.get_value("multistepmode");
    if (multistepPT_string == "default")
    {
      UseMultiStepPTMode = MultiStepPTMode::Default;
    }
    else if (multistepPT_string == "auto")
    {
      UseMultiStepPTMode = MultiStepPTMode::Auto;
    }
    else
    {
      int intUseMultiStepPTMode = std::stoi(multistepPT_string);
      switch (intUseMultiStepPTMode)
      {
      case 0: UseMultiStepPTMode = MultiStepPTMode::OneStep; break;
      case 1: UseMultiStepPTMode = MultiStepPTMode::EdgeCoverage; break;
      case 2: UseMultiStepPTMode = MultiStepPTMode::CompleteCoverage; break;

      default:
      {
        ss << "--multistepmode invalid using 'default' instead";
        UseMultiStepPTMode = MultiStepPTMode::Default;
        break;
      }
      }
    }
  }
  catch (BSMPT::parserException &)
  {
    ss << "--multistepmode not set, using default value: default\n";
  }

  try
  {
    num_check_pts = argparser.get_value<int>("num_pts");
  }
  catch (BSMPT::parserException &)
  {
    ss << "--num_check_pts not set, using default value: " << num_check_pts
       << "\n";
  }

  // CheckEWSymmetryRestoration
  try
  {
    auto ewsr_string = argparser.get_value("checkewsr");
    if (ewsr_string == "on")
    {
      CheckEWSymmetryRestoration = 1;
    }
    else if (ewsr_string == "off")
    {
      CheckEWSymmetryRestoration = 0;
    }
  }
  catch (BSMPT::parserException &)
  {
    ss << "--checkewsr not set, using default value: on\n";
  }

  WhichMinimizer = Minimizer::CalcWhichMinimizer(UseGSL, UseCMAES, UseNLopt);

  Logger::Write(LoggingLevel::ProgDetailed, ss.str());
}

BSMPT::parser prepare_parser()
{
  BSMPT::parser argparser(true);
  argparser.add_argument("help", "shows this menu", false);
  argparser.add_argument("model", "[*] model name", true);
  argparser.add_argument("input", "[*] input file (in tsv format)", true);
  argparser.add_argument("output", "[*] output file (in tsv format)", true);
  argparser.add_argument(
      "line", "[*] line number the parameter in input file", true);
  argparser.add_subtext("(expects line 1 to be a legend)");
  argparser.add_argument("npoints", "number temperature points", "100", false);
  argparser.add_argument("thigh", "high temperature [GeV]", "300", false);
  argparser.add_argument(
      "multistepmode", "multi-step PT mode", "default", false);
  argparser.add_subtext("default: default mode");
  argparser.add_subtext("0: single-step PT mode");
  argparser.add_subtext(">0 for multi-step PT modes:");
  argparser.add_subtext("1: tracing coverage");
  argparser.add_subtext("2: global minimum tracing coverage");
  argparser.add_subtext("auto: automatic mode");
  argparser.add_argument(
      "num_pts", "intermediate grid-size for default mode", "10", false);
  argparser.add_argument(
      "checkewsr", "check for EWSR at high temperature", "on", false);
  argparser.add_subtext("on: perform check");
  argparser.add_subtext("off: check disabled");

  std::string GSLhelp   = Minimizer::UseGSLDefault ? "true" : "false";
  std::string CMAEShelp = Minimizer::UseLibCMAESDefault ? "true" : "false";
  std::string NLoptHelp = Minimizer::UseNLoptDefault ? "true" : "false";

  argparser.add_argument(
      "usegsl", "use GSL library for minimization", GSLhelp, false);
  argparser.add_argument(
      "usecmaes", "use CMAES library  for minimization", CMAEShelp, false);
  argparser.add_argument(
      "usenlopt", "use NLopt library for minimization", NLoptHelp, false);
  argparser.add_argument("usemultithreading",
                         "enable multi-threading for minimizers",
                         "false",
                         false);
  argparser.add_argument(
      "json", "use a json file instead of cli parameters", false);

  std::stringstream ss;
  ss << "ThermalMasses traces phases in T = [0, Thigh] GeV and prints the "
        "thermal masses.\n"
        "It is called "
        "by\n\n\t./bin/ThermalMasses model input output line"
        "with arguments\n\n\t./bin/ThermalMasses [arguments]\n\nwith the "
        "following arguments, ([*] are required arguments, others "
        "are optional):\n";
  argparser.set_help_header(ss.str());

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
      arguments.emplace_back("--output=" + std::string(argv[3]));
    }
    if (argc >= 5)
    {
      arguments.emplace_back("--line=" + std::string(argv[4]));
    }
  }
  return arguments;
}
