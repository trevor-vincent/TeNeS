#ifndef _PEPS_PARAMETERS_HPP_
#define _PEPS_PARAMETERS_HPP_

#include <fstream>
#include <mpi.h>
#include <sstream>
#include <string>
#include <vector>

#include <toml11/toml/types.hpp>

class PEPS_Parameters {
public:
  // Tensor
  int D;    // Bond dimension for central tensor
  int CHI;  // Bond dimension for environment tensor

  // Debug
  bool Debug_flag;
  bool Warning_flag;

  // Simple update
  double Inverse_lambda_cut;

  // Environment
  double Inverse_projector_cut;
  double CTM_Convergence_Epsilon;
  int Max_CTM_Iteration;
  bool CTM_Projector_corner;
  bool Use_RSVD;
  int RSVD_Oversampling_factor;

  // Full update
  double Inverse_Env_cut;
  double Full_Inverse_precision;
  double Full_Convergence_Epsilon;
  int Full_max_iteration;
  bool Full_Gauge_Fix;
  bool Full_Use_FFU; // Fast Full Update

  PEPS_Parameters() {
    // Tensor
    D = 2;
    CHI = D * D;

    // Debug
    Debug_flag = false;
    Warning_flag = true;

    // Simple update
    Inverse_lambda_cut = 1e-12;

    // Environment
    Inverse_projector_cut = 1e-12;
    CTM_Convergence_Epsilon = 1e-10;
    Max_CTM_Iteration = 100;
    CTM_Projector_corner = false;
    Use_RSVD = false;
    RSVD_Oversampling_factor = 2;

    // Full update
    Inverse_Env_cut = 1e-12;
    Full_Inverse_precision = 1e-12;
    Full_Convergence_Epsilon = 1e-12;
    Full_max_iteration = 1000;
    Full_Gauge_Fix = true;
    Full_Use_FFU = true;
  }

  explicit PEPS_Parameters(const char *filename): PEPS_Parameters(){ set(filename); }
  PEPS_Parameters(toml::Table);
  void set(const char *filename);
  void set(toml::Table data);

  void output_parameters(const char *filename, bool append) {
    std::ofstream ofs;
    if (append) {
      ofs.open(filename, std::ios::out | std::ios::app);
    } else {
      ofs.open(filename, std::ios::out);
    }
    // Tensor
    ofs << "D " << D << std::endl;
    ofs << "CHI " << CHI << std::endl;

    // Debug
    ofs << "Debug_flag " << Debug_flag << std::endl;
    ofs << "Warning_flag " << Warning_flag << std::endl;

    // Simple update
    ofs << "Inverse_lambda_cut " << Inverse_lambda_cut << std::endl;

    // Environment
    ofs << "Inverse_projector_cut " << Inverse_projector_cut << std::endl;
    ofs << "CTM_Convergence_Epsilon " << CTM_Convergence_Epsilon << std::endl;
    ofs << "Max_CTM_Iteration " << Max_CTM_Iteration << std::endl;
    ofs << "CTM_Projector_corner " << CTM_Projector_corner << std::endl;
    ofs << "Use_RSVD " << Use_RSVD << std::endl;
    ofs << "RSVD_Oversampling_factor " << RSVD_Oversampling_factor << std::endl;

    // Full update
    ofs << "Inverse_Env_cut " << Inverse_Env_cut << std::endl;
    ofs << "Full_Inverse_precision " << Full_Inverse_precision << std::endl;
    ofs << "Full_Convergence_Epsilon " << Full_Convergence_Epsilon << std::endl;
    ofs << "Full_max_iteration " << Full_max_iteration << std::endl;
    ofs << "Full_Gauge_Fix " << Full_Gauge_Fix << std::endl;
    ofs << "Full_Use_FFU " << Full_Use_FFU << std::endl;

    ofs.close();
  }
  void output_parameters(const char *filename) {
    output_parameters(filename, false);
  }

  void output_parameters_append(const char *filename) {
    output_parameters(filename, true);
  }

  void Bcast_parameters(MPI_Comm comm) {
    int irank;
    MPI_Comm_rank(MPI_COMM_WORLD, &irank);

    std::vector<double> params_double(6);
    std::vector<int> params_int(11);

    if (irank == 0) {
      // Tensor
      params_int[0] = D;
      params_int[1] = CHI;
      // Debug
      params_int[2] = Debug_flag;
      params_int[3] = Warning_flag;
      // Environment
      params_int[4] = Max_CTM_Iteration;
      params_int[5] = CTM_Projector_corner;
      params_int[6] = Use_RSVD;
      params_int[7] = RSVD_Oversampling_factor;
      // Full update
      params_int[8] = Full_max_iteration;
      params_int[9] = Full_Gauge_Fix;
      params_int[10] = Full_Use_FFU;

      // Simple update
      params_double[0] = Inverse_lambda_cut;
      // Environment
      params_double[1] = Inverse_projector_cut;
      params_double[2] = CTM_Convergence_Epsilon;
      // Full update
      params_double[3] = Inverse_Env_cut;
      params_double[4] = Full_Inverse_precision;
      params_double[5] = Full_Convergence_Epsilon;

      MPI_Bcast(&params_int.front(), 11, MPI_INT, 0, comm);
      MPI_Bcast(&params_double.front(), 6, MPI_DOUBLE, 0, comm);
    } else {
      MPI_Bcast(&params_int.front(), 11, MPI_INT, 0, comm);
      MPI_Bcast(&params_double.front(), 6, MPI_DOUBLE, 0, comm);

      // Tensor
      D = params_int[0];
      CHI = params_int[1];
      // Debug
      Debug_flag = params_int[2];
      Warning_flag = params_int[3];
      // Environment
      Max_CTM_Iteration = params_int[4];
      CTM_Projector_corner = params_int[5];
      Use_RSVD = params_int[6];
      RSVD_Oversampling_factor = params_int[7];
      // Full update
      Full_max_iteration = params_int[8];
      Full_Gauge_Fix = params_int[9];
      Full_Use_FFU = params_int[10];

      // Simple update
      Inverse_lambda_cut = params_double[0];
      // Environment
      Inverse_projector_cut = params_double[1];
      CTM_Convergence_Epsilon = params_double[2];
      // Full update
      Inverse_Env_cut = params_double[3];
      Full_Inverse_precision = params_double[4];
      Full_Convergence_Epsilon = params_double[5];
    }
  }
};
#endif // _PEPS_PARAMETERS_HPP_
