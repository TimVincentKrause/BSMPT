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



      // take only relevant lines from linestring
      // linestr_store;  linestr;
      //
      std::stringstream linstor(linestr_store);
      std::stringstream linstr(linestr);

      std::vector<std::string> linstor_vec;
      std::vector<std::string> linstr_vec;

      std::string tmpstring;
      while (linstor >> tmpstring){linstor_vec.push_back(tmpstring);}
      while (linstr >> tmpstring){linstr_vec.push_back(tmpstring);}

      std::string save_head = "";
      std::string save_numb = "";
      std::vector<std::string> wanted_header = {"L1","L2","L3","L4","L5","L6","L7","L8","Tr","Ti","m22sq","mssq"};
      for (std::size_t ij = 0; ij < linstor_vec.size(); ij++)
      {
          bool addstring = 0;
          for (std::size_t ijk =0; ijk < wanted_header.size(); ijk++)
              addstring += (linstor_vec.at(ij) == wanted_header.at(ijk));
          if (addstring)
          {
              save_head += linstor_vec.at(ij) + "\t";
              save_numb += linstr_vec.at(ij+1) + "\t"; //bc/ of index
          }


      }

      // // NLO stability check
      // bool nlostable = modelPointer->CheckNLOVEV(
      //     MinTracer->ConvertToVEVDim(MinTracer->GetGlobalMinimum(0)));
      // StatusNLOStability status_nlostable =
      //     MinTracer->GetStatusNLOVEV(nlostable);
      // Logger::Write(LoggingLevel::ProgDetailed,
      //               "Status of NLO stability check is: " +
      //                   StatusNLOStabilityToString.at(status_nlostable));

      // // EWSR check
      // double EWSymmetryRestoration_status = 0;
      // StatusEWSR status_ewsr              = StatusEWSR::Off;

      // if (args.CheckEWSymmetryRestoration > 0)
      // {
      //   EWSymmetryRestoration_status =
      //       MinTracer->IsThereEWSymmetryRestoration();
      //   status_ewsr = MinTracer->GetStatusEWSR(EWSymmetryRestoration_status);
      // }
      // else
      // {
      //   Logger::Write(LoggingLevel::ProgDetailed,
      //                 "Check for EW symmetry restoration is disabled.\n");
      // }

      // phase tracking
      Logger::Write(
          LoggingLevel::ProgDetailed,
          "Track phases in between T_low = " + std::to_string(args.templow) +
              " and T_high = " + std::to_string(args.temphigh));

      //bool do_only_tracing = true;

      Vacuum vac(args.templow,
                 args.temphigh,
                 MinTracer,
                 modelPointer,
                 args.UseMultiStepPTMode,
                 args.num_check_pts);

      Logger::Write(LoggingLevel::ProgDetailed,
                    "Found and traced " +
                        std::to_string(vac.PhasesList.size()) +
                        " minima with status = " +
                        StatusTracingToString.at(vac.status_vacuum) +
                        ".\n-------------------------------");

      if ((vac.PhasesList.size() != 2) and (vac.PhasesList.size() != 1))
        throw std::runtime_error(
            "This code is not ready for handling anything other than "
            "two phases!\n");


      // prepare legend
      std::vector<std::string> LegendMinima;
      //LegendMinima.push_back("status_nlo_stability");
      //LegendMinima.push_back("status_ewsr");
      LegendMinima.push_back("status_tracing");
      LegendMinima.push_back("Tcrit");


      LegendMinima.push_back("Temp");

      for (std::size_t j = 0; j < modelPointer->get_nVEV(); j++)
        LegendMinima.push_back(modelPointer->addLegendVEV().at(j));
      // only v dependent masses
      for (std::size_t j = 0; j < modelPointer->get_NHiggs(); j++)
        LegendMinima.push_back("mS_" + to_string(j) + "sq");
      // v and T dependent masses
      // for (std::size_t j = 0; j < modelPointer->get_NHiggs(); j++)
      //   LegendMinima.push_back("mS_" + to_string(j) + "sq_T");
      for (std::size_t j = 0; j < modelPointer->get_NLepton(); j++)
        LegendMinima.push_back("mL_" + to_string(j) + "sq");
      for (std::size_t j = 0; j < modelPointer->get_NQuarks(); j++)
        LegendMinima.push_back("mQ_" + to_string(j) + "sq");
      // only v dependent masses
      for (std::size_t j = 0; j < modelPointer->get_NGauge(); j++)
        LegendMinima.push_back("mG_" + to_string(j) + "sq");
      // v and T dependent masses
      for (std::size_t j = 0; j < modelPointer->get_NGauge(); j++)
        LegendMinima.push_back("mG_" + to_string(j) + "sq_T");

      /*
      for (std::size_t j = 0; j < 3; j++){
        for (std::size_t k = 0; k < 3; k++)
          LegendMinima.push_back("MS_" + to_string(j) + to_string(k));
      }
      // Hessian diagonals
      for (std::size_t j = 0; j < 3; j++){
        for (std::size_t k = 0; k < 3; k++)
          LegendMinima.push_back("VeffHess_" + to_string(j)+ to_string(k));
      }
      */
      // // Rotation Matrix before checking of convention
      // for (std::size_t j = 0; j < 3; j++){
      //   for (std::size_t k = 0; k < 3; k++)
      //     LegendMinima.push_back("Rbf" + to_string(j) + to_string(k));
      // }
      // thermal masses
      LegendMinima.push_back("m_thm_Gmsq");
      LegendMinima.push_back("m_thm_Gpsq");
      LegendMinima.push_back("m_thm_G0sq");
      LegendMinima.push_back("m_thm_Hsmsq");
      LegendMinima.push_back("m_thm_H1sq");
      LegendMinima.push_back("m_thm_H2sq");
      LegendMinima.push_back("m_thm_H3sq");
      LegendMinima.push_back("m_thm_Hmsq");
      LegendMinima.push_back("m_thm_Hpsq");

      // potential masses
      LegendMinima.push_back("m_pot_Gmsq");
      LegendMinima.push_back("m_pot_Gpsq");
      LegendMinima.push_back("m_pot_G0sq");
      LegendMinima.push_back("m_pot_Hsmsq");
      LegendMinima.push_back("m_pot_H1sq");
      LegendMinima.push_back("m_pot_H2sq");
      LegendMinima.push_back("m_pot_H3sq");
      LegendMinima.push_back("m_pot_Hmsq");
      LegendMinima.push_back("m_pot_Hpsq");

      // thermal DM-Rotation matrix (this is only for CPintheDark)
      for (std::size_t j = 0; j < 3; j++){
        for (std::size_t k = 0; k < 3; k++)
          LegendMinima.push_back("R_thm_" + to_string(j) + to_string(k));
      }

      // potential DM-Rotation matrix (this is only for CPintheDark)
      for (std::size_t j = 0; j < 3; j++){
        for (std::size_t k = 0; k < 3; k++)
          LegendMinima.push_back("R_pot_" + to_string(j) + to_string(k));
      }

      LegendMinima.push_back("runtime");
      outfile << save_head << sep //<< modelPointer->addLegendCT() << sep
              << LegendMinima << std::endl;

      std::size_t length = 0;
      for (std::size_t j = 0; j < vac.PhasesList.size(); j++)
      {
        if (length == 0)
        {
          length = vac.PhasesList.at(j).MinimumPhaseVector.size();
        }
        else if (length < vac.PhasesList.at(j).MinimumPhaseVector.size())
        {
          length = vac.PhasesList.at(j).MinimumPhaseVector.size();
        }
      }

      if (length == 0)
      {
        outfile << std::setprecision(16);
        outfile << save_numb;
        //outfile << sep << parameters.second;
        //outfile << sep << status_nlostable;
        //outfile << sep << status_ewsr;
        outfile << sep << vac.status_vacuum;
        auto time = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::high_resolution_clock::now() - start)
                        .count() /
                    1000.;

        outfile << sep << time;
        outfile << std::endl;
      }


      std::vector<double> T_list;

      for (int n = 0; n < args.npoints; n++)
        T_list.push_back(args.templow +
                         n * (args.temphigh - args.templow) / args.npoints);

      // std::cout << "tcrit=" << vac.CoexPhasesList.at(0).crit_temp << std::endl;
      double Tcrit = vac.CoexPhasesList.at(0).crit_temp;
      std::cout << "Tcrit = " << Tcrit << std::endl;
      if (vac.PhasesList.size() == 2)
      {
        T_list.insert(std::lower_bound(T_list.begin(),
                                      T_list.end(),
                                      vac.CoexPhasesList.at(0).crit_temp),
                      vac.CoexPhasesList.at(0).crit_temp);
      }



      // these are needed later for correct rotation matrix after symmetry breaking

      double m22sq = modelPointer->Get_Curvature_Higgs_L2().at(7).at(7);
      double mSsq = modelPointer->Get_Curvature_Higgs_L2().at(8).at(8);
      double roteps = 1e-2;

      // used for smoothness of rotation matrix
      // difference
      bool diagm22sq, diagm22sq_T;
      // save sign of rotation matrix in relevant elements
      std::vector<double> rotsgn = {0,0,0};
      std::vector<double> rotsgn_T = {0,0,0};

      // for every temperature:
      for (const auto T : T_list)
      {
        outfile << std::setprecision(16);
        outfile << save_numb;
        //outfile << sep << parameters.second;
        //outfile << sep << status_nlostable;
        //outfile << sep << status_ewsr;
        outfile << sep << vac.status_vacuum;
        outfile << sep << Tcrit;
        outfile << sep << T;

        std::vector<double> vev;
        if (T < 0) {break;}



        if (vac.PhasesList.size() == 2)
        {
          if (T <= vac.CoexPhasesList.at(0).crit_temp)
          {
            vev = vac.PhasesList.at(1).Get(T).point;
          }
          else
          {
            vev = vac.PhasesList.at(0).Get(T).point;
          }
        }
        else
        {
          vev = vac.PhasesList.at(0).Get(T).point;
        }

        outfile << sep << vev;
        outfile << sep
                << modelPointer->HiggsMassesSquared(
                       modelPointer->MinimizeOrderVEV(vev), 0);
        // outfile << sep
        //         << modelPointer->HiggsMassesSquared(
        //                modelPointer->MinimizeOrderVEV(vev), T);
        outfile << sep
                << modelPointer->LeptonMassesSquared(
                       modelPointer->MinimizeOrderVEV(vev));
        outfile << sep
                << modelPointer->QuarkMassesSquared(
                       modelPointer->MinimizeOrderVEV(vev));
        outfile << sep
                << modelPointer->GaugeMassesSquared(
                       modelPointer->MinimizeOrderVEV(vev), 0);
        outfile << sep
                << modelPointer->GaugeMassesSquared(
                       modelPointer->MinimizeOrderVEV(vev), T);


        // Temperature dependent Higgs Mass-Matrix
        MatrixXd HiggsMassMatrix = modelPointer->HiggsMassMatrix(modelPointer->MinimizeOrderVEV(vev), T,0);
        MatrixXd NeutralDSMatrix(3,3);

        
        for (std::size_t j = 0; j < 3; j++){
          for (std::size_t k = 0; k < 3; k++) {
              NeutralDSMatrix(j,k) = HiggsMassMatrix(6+j,6+k);
              //outfile << sep << NeutralDSMatrix(j,k);
          }
        }
        
        /* --- */

        // Hessian of Potential - for high masses eps = 10
        double eps = 10;

        // Diagonal Hessian
        MatrixXd Hessian(modelPointer->get_NHiggs(),modelPointer->get_NHiggs());
        MatrixXd NeutralDSHessian(3,3);

        // Veff as a function
        std::function<double(std::vector<double>)> Veff;
        Veff = [&](std::vector<double> effvev)
            {return modelPointer->VEff(effvev,T,0);};

        for (std::size_t i = 0; i < modelPointer->get_NHiggs();i++)
        {
            // dVeff/dw_i as a function
            std::function<double(std::vector<double>)> dVeff;
            dVeff = [&](std::vector<double> effvev)
                {return NablaNumerical(effvev,Veff,eps).at(i);};

            // set diagonals of Mass Matrix
            Hessian(i,i) = NablaNumerical(modelPointer->MinimizeOrderVEV(vev),dVeff, eps).at(i);

            // if index reaches neutral DS Mass Matrix -> add offdiagonals;
            if ((i == 6) || (i == 7))
            {
                Hessian(i,8) = NablaNumerical(modelPointer->MinimizeOrderVEV(vev),dVeff, eps).at(8);
                Hessian(8,i) = Hessian(i,8);
            }
        }

        // std::vector<std::vector<double>> TestNumdiff = HessianNablaNumerical(modelPointer->MinimizeOrderVEV(vev),Veff,eps);

        // MatrixXd TestMatrix(modelPointer->get_NHiggs(),modelPointer->get_NHiggs());
        // for (std::size_t j = 0; j < modelPointer->get_NHiggs(); j++){
        //   for (std::size_t k = 0; k < modelPointer->get_NHiggs(); k++) {
        //       TestMatrix(j,k) = TestNumdiff.at(j).at(k);
        //   }
        // }

        // std::cout << "--- T = " << T << " ---" << std::endl;
        // std::cout << TestMatrix << std::endl;


        
        for (std::size_t j = 0; j < 3; j++){
          for (std::size_t k = 0; k < 3; k++) {
              NeutralDSHessian(j,k) = Hessian(6+j,6+k);
              //outfile << sep << NeutralDSHessian(j,k);
          }
        }
        


        std::vector<double> MassSquaredHiggsTherm(modelPointer->get_NHiggs());
        std::vector<double> MassSquaredHiggsHessi(modelPointer->get_NHiggs());
        MatrixXd HiggsRotTherm(3,3);
        MatrixXd HiggsRotHessi(3,3);
        std::size_t i_mG0, i_mGm, i_mGp, i_mHsm, i_mH1, i_mH2, i_mH3, i_mHm, i_mHp;
        std::size_t i_mG0_T, i_mGm_T, i_mGp_T, i_mHsm_T, i_mH1_T, i_mH2_T, i_mH3_T, i_mHm_T, i_mHp_T;

        if (T < Tcrit)//(vev.at(0) > 1e-2)
        {
            Eigen::SelfAdjointEigenSolver<MatrixXd> es;

            es.compute(Hessian, Eigen::EigenvaluesOnly);
            // eigenvalues
            for (std::size_t i = 0; i < modelPointer->get_NHiggs(); i++)
                MassSquaredHiggsHessi[i] = es.eigenvalues()[i];

            es.compute(HiggsMassMatrix, Eigen::EigenvaluesOnly);
            // eigenvalues
            for (std::size_t i = 0; i < modelPointer->get_NHiggs(); i++)
                MassSquaredHiggsTherm[i] = es.eigenvalues()[i];

            // this is known from mHsm << Dark Sector masses
            // (and DM can't be charged)
            i_mG0 = 0; i_mGm = 1; i_mGp = 2; i_mHsm = 3; i_mH1 = 4;

            // calculation of order of Higgs particles
            double tmpmass = MassSquaredHiggsHessi[5];

            i_mHp = 6;

            while (i_mHp<8){
            if (std::abs(tmpmass - MassSquaredHiggsHessi[i_mHp]) < 1) {break;}
            else {tmpmass = MassSquaredHiggsHessi[i_mHp];}
            i_mHp++;
            }

            if      (i_mHp==6){i_mH2 = 7; i_mH3 = 8;}
            else if (i_mHp==7){i_mH2 = 5; i_mH3 = 8;}
            else if (i_mHp==8){i_mH2 = 5; i_mH3 = 6;}

            i_mHm = i_mHp - 1;

            // for thermal rotation
            // this is known from mHsm << Dark Sector masses
            // (and DM can't be charged)
            i_mG0_T = 0; i_mGm_T = 1; i_mGp_T = 2; i_mHsm_T = 3; i_mH1_T = 4;

            // calculation of order of Higgs particles
            tmpmass = MassSquaredHiggsTherm[5];

            i_mHp_T = 6;

            while (i_mHp_T<8){
            if (std::abs(tmpmass - MassSquaredHiggsTherm[i_mHp_T]) < 1) {break;}
            else {tmpmass = MassSquaredHiggsTherm[i_mHp_T];}
            i_mHp_T++;
            }

            if      (i_mHp_T==6){i_mH2_T = 7; i_mH3_T = 8;}
            else if (i_mHp_T==7){i_mH2_T = 5; i_mH3_T = 8;}
            else if (i_mHp_T==8){i_mH2_T = 5; i_mH3_T = 6;}

            i_mHm_T = i_mHp_T - 1;


            // Diagonalisation of DM-Mass Matrix
            es.compute(NeutralDSHessian);
            HiggsRotHessi = es.eigenvectors().transpose();
            es.compute(NeutralDSMatrix);
            HiggsRotTherm = es.eigenvectors().transpose();

            // Set correct parametrisation
            if (HiggsRotHessi(0,0) < 0)          {HiggsRotHessi.row(0) *= -1;}
            if (HiggsRotHessi(2,2) < 0)          {HiggsRotHessi.row(2) *= -1;}
            if (HiggsRotHessi.determinant() < 0) {HiggsRotHessi.row(1) *= -1;}

            if (HiggsRotTherm(0,0) < 0)          {HiggsRotTherm.row(0) *= -1;}
            if (HiggsRotTherm(2,2) < 0)          {HiggsRotTherm.row(2) *= -1;}
            if (HiggsRotTherm.determinant() < 0) {HiggsRotTherm.row(1) *= -1;}

            // save diagonalisation for smoothness of matrix values
            // before phase transition

            if (mSsq < m22sq)
            {
                rotsgn.at(2) = std::copysign(1.,HiggsRotHessi(0,2));
                if ( std::abs(HiggsRotHessi(1,0)) -1 < roteps )
                {
                    diagm22sq = 1;
                    rotsgn.at(0) = std::copysign(1.,HiggsRotHessi(1,0));
                    rotsgn.at(1) = std::copysign(1.,HiggsRotHessi(2,1));
                }
                else if ( std::abs(HiggsRotHessi(1,1)) -1 < roteps )
                {
                    diagm22sq = 0;
                    rotsgn.at(0) = std::copysign(1.,HiggsRotHessi(1,1));
                    rotsgn.at(1) = std::copysign(1.,HiggsRotHessi(2,0));
                }
            }
            else
            {
                rotsgn.at(2) = std::copysign(1.,HiggsRotHessi(2,2));
                if ( std::abs(HiggsRotHessi(0,0)) -1 < roteps )
                {
                    diagm22sq = 1;
                    rotsgn.at(0) = std::copysign(1.,HiggsRotHessi(0,0));
                    rotsgn.at(1) = std::copysign(1.,HiggsRotHessi(1,1));
                }
                else if ( std::abs(HiggsRotHessi(0,1)) -1 < roteps )
                {
                    diagm22sq = 0;
                    rotsgn.at(0) = std::copysign(1.,HiggsRotHessi(0,1));
                    rotsgn.at(1) = std::copysign(1.,HiggsRotHessi(1,0));
                }
            }

            // same for thermal masses
            if (mSsq < m22sq)
            {
                rotsgn_T.at(2) = std::copysign(1.,HiggsRotTherm(0,2));
                if ( std::abs(HiggsRotTherm(1,0)) -1 < roteps )
                {
                    diagm22sq_T = 1;
                    rotsgn_T.at(0) = std::copysign(1.,HiggsRotTherm(1,0));
                    rotsgn_T.at(1) = std::copysign(1.,HiggsRotTherm(2,1));
                }
                else if ( std::abs(HiggsRotTherm(1,1)) -1 < roteps )
                {
                    diagm22sq_T = 0;
                    rotsgn_T.at(0) = std::copysign(1.,HiggsRotTherm(1,1));
                    rotsgn_T.at(1) = std::copysign(1.,HiggsRotTherm(2,0));
                }
            }
            else
            {
                rotsgn_T.at(2) = std::copysign(1.,HiggsRotTherm(2,2));
                if ( std::abs(HiggsRotTherm(0,0)) -1 < roteps )
                {
                    diagm22sq_T = 1;
                    rotsgn_T.at(0) = std::copysign(1.,HiggsRotTherm(0,0));
                    rotsgn_T.at(1) = std::copysign(1.,HiggsRotTherm(1,1));
                }
                else if ( std::abs(HiggsRotTherm(0,1)) -1 < roteps )
                {
                    diagm22sq_T = 0;
                    rotsgn_T.at(0) = std::copysign(1.,HiggsRotTherm(0,1));
                    rotsgn_T.at(1) = std::copysign(1.,HiggsRotTherm(1,0));
                }
            }



        }
        else
        {
            for (std::size_t i = 0; i <modelPointer->get_NHiggs(); i++)
                MassSquaredHiggsHessi.at(i) = Hessian(i,i);
            for (std::size_t i = 0; i <modelPointer->get_NHiggs(); i++)
                MassSquaredHiggsTherm.at(i) = HiggsMassMatrix(i,i);

            // per definition / doesn't matter
            i_mGm=0;i_mGp=1;i_mG0=5; i_mHm = 2; i_mHp = 3; i_mHsm = 4;
            i_mGm_T=0;i_mGp_T=1;i_mG0_T=5; i_mHm_T = 2; i_mHp_T = 3; i_mHsm_T = 4;

            // rotation matrix with smooth swith;
            if (mSsq < m22sq)
            {
                i_mH1 = 8; i_mH1_T = 8;
                if (diagm22sq)
                {
                    HiggsRotHessi << 0, 0, rotsgn.at(2), rotsgn.at(0), 0, 0, 0, rotsgn.at(1), 0;
                    i_mH2 = 6; i_mH3 = 7;
                }
                else
                {
                    HiggsRotHessi << 0, 0, rotsgn.at(2), 0, rotsgn.at(1), 0, rotsgn.at(0), 0, 0;
                    i_mH2 = 7; i_mH3 = 6;
                }
                if (diagm22sq_T)
                {
                    HiggsRotTherm << 0, 0, rotsgn_T.at(2), rotsgn_T.at(0), 0, 0, 0, rotsgn_T.at(1), 0;
                    i_mH2_T = 6; i_mH3_T = 7;
                }
                else
                {
                    HiggsRotTherm << 0, 0, rotsgn_T.at(2), 0, rotsgn_T.at(1), 0, rotsgn_T.at(0), 0, 0;
                    i_mH2_T = 7; i_mH3_T = 6;
                }
            }
            else
            {
                i_mH3 = 8; i_mH3_T = 8;
                if (diagm22sq)
                {
                    HiggsRotHessi << rotsgn.at(0), 0, 0, 0, rotsgn.at(1), 0, 0, 0, rotsgn.at(2);
                    i_mH1 = 6; i_mH2 = 7;
                }
                else
                {
                    HiggsRotHessi << 0, rotsgn.at(1), 0, rotsgn.at(0), 0, 0, 0, 0, rotsgn.at(2);
                    i_mH1 = 7; i_mH2 = 6;
                }
                if (diagm22sq_T)
                {
                    HiggsRotTherm << rotsgn_T.at(0), 0, 0, 0, rotsgn_T.at(1), 0, 0, 0, rotsgn_T.at(2);
                    i_mH1_T = 6; i_mH2_T = 7;
                }
                else
                {
                    HiggsRotTherm << 0, rotsgn_T.at(1), 0, rotsgn_T.at(0), 0, 0, 0, 0, rotsgn_T.at(2);
                    i_mH1_T = 7; i_mH2_T = 6;
                }
            }
        }

        // thermal masses
        outfile << sep << MassSquaredHiggsTherm[i_mG0_T];
        outfile << sep << MassSquaredHiggsTherm[i_mGm_T];
        outfile << sep << MassSquaredHiggsTherm[i_mGp_T];
        outfile << sep << MassSquaredHiggsTherm[i_mHsm_T];
        outfile << sep << MassSquaredHiggsTherm[i_mH1_T];
        outfile << sep << MassSquaredHiggsTherm[i_mH2_T];
        outfile << sep << MassSquaredHiggsTherm[i_mH3_T];
        outfile << sep << MassSquaredHiggsTherm[i_mHm_T];
        outfile << sep << MassSquaredHiggsTherm[i_mHp_T];

        // potential masses
        outfile << sep << MassSquaredHiggsHessi[i_mG0];
        outfile << sep << MassSquaredHiggsHessi[i_mGm];
        outfile << sep << MassSquaredHiggsHessi[i_mGp];
        outfile << sep << MassSquaredHiggsHessi[i_mHsm];
        outfile << sep << MassSquaredHiggsHessi[i_mH1];
        outfile << sep << MassSquaredHiggsHessi[i_mH2];
        outfile << sep << MassSquaredHiggsHessi[i_mH3];
        outfile << sep << MassSquaredHiggsHessi[i_mHm];
        outfile << sep << MassSquaredHiggsHessi[i_mHp];

        for (std::size_t j = 0; j < 3; j++){
          for (std::size_t k = 0; k < 3; k++){
          outfile << sep
                  << HiggsRotTherm(j,k) ;}
        }

        for (std::size_t j = 0; j < 3; j++){
          for (std::size_t k = 0; k < 3; k++){
          outfile << sep
                  << HiggsRotHessi(j,k) ;}
        }

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
