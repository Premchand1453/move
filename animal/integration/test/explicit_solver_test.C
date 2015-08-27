#include <cstdlib>
#include <cstring>
#include <fstream>
#include <animal/integration/test/explicit_solver_test.h>

using namespace std;

// ----------------------------------------------------------
//  
//  explicit_solver_test
//  Test of the explicit integration classes.
//  
//  File: animal/integration/test/explicit_solver_test.C
//  (creation date: March 28, 2000).
//  
//  Author: David Bourguignon
//  
//  iMAGIS/GRAVIR, 2000
//  
// ----------------------------------------------------------

inline void error(const char* p1, const char* p2="")
{
  cerr << "Error! " << p1 << " " << p2 << endl;
  exit(1);
}

inline void writeInfo(ofstream& file_out,
		      double date, double pos, double vel,
		      double KE, double PE, double E)
{
  file_out << date << "\t" << pos << "\t" << vel << "\t"
	   << KE << "\t" << PE << "\t" << E
	   << endl;
}

int main(int argc, char** argv)
{
  /* Parsing input arguments */
  
  if ( argc != 4 )
    {
      cerr << "Usage:\texplicit_solver_test [-options] [file_param] [file_result]" << endl;
      cerr << "\t[-euler Euler method]" << endl;
      cerr << "\t[-meuler Modified Euler method]" << endl;
      cerr << "\t[-seuler Stoermer Euler method]" << endl;
      cerr << "\t[-rk2 Second-order Runge-Kutta method]" << endl;
      cerr << "\t[-rk4 Fourth-order Runge-Kutta method]" << endl;
      cerr << "\t[-mm Modified midpoint method]" << endl;
      exit(1);
    }
  
  ifstream file_in(argv[2], ios::in);
  if ( !file_in ) error("Cannot open input file", argv[2]);
  
  ofstream file_out(argv[3], ios::out);
  if ( !file_out ) error("Cannot open output file", argv[3]);
  
  cout << endl;
  cout << "------------------------------------------" << endl;
  cout << " TEST OF EXPLICIT INTEGRATION METHODS     " << endl;
  cout << "------------------------------------------" << endl;
  
  enum integration_method {EULER, MEULER, SEULER, RK2, RK4, MM};
  integration_method method;
  
  if ( !strcmp(argv[1], "-euler") )
    {
      method = EULER;
      cout << " EULER METHOD                             " << endl;
      cout << "------------------------------------------" << endl;
      cout << endl;
    }
  else if ( !strcmp(argv[1], "-meuler") )
    {
      method = MEULER;
      cout << " MODIFIED EULER METHOD                    " << endl;
      cout << "------------------------------------------" << endl;
      cout << endl;
    }
  else if ( !strcmp(argv[1], "-seuler") )
    {
      method = SEULER;
      cout << " STOERMER EULER METHOD                    " << endl;
      cout << "------------------------------------------" << endl;
      cout << endl;
    }
  else if ( !strcmp(argv[1], "-rk2") )
    {
      method = RK2;
      cout << " SECOND-ORDER RUNGE-KUTTA METHOD          " << endl;
      cout << "------------------------------------------" << endl;
      cout << endl;
    }
  else if ( !strcmp(argv[1], "-rk4") )
    {
      method = RK4;
      cout << " FOURTH-ORDER RUNGE-KUTTA METHOD          " << endl;
      cout << "------------------------------------------" << endl;
      cout << endl;
    }
  else if ( !strcmp(argv[1], "-mm") )
    {
      method = MM;
      cout << " MODIFIED MIDPOINT METHOD                 " << endl;
      cout << "------------------------------------------" << endl;
      cout << endl;
    }
  else
    error("Unknown mode");
  
  double x, v, m, k, L0, T, dt;
  char line[256];
  
  file_in.getline(line, 256, '\n');
  sscanf(line, "x=%lf v=%lf m=%lf k=%lf L0=%lf T=%lf dt=%lf",
         &x, &v, &m, &k, &L0, &T, &dt);
  file_in.close();
  
  cout << " x=" << x << " v=" << v << " m=" << m << " k=" << k << " L0=" << L0
       << " T=" << T <<" dt=" << dt << " S.I. units"
       << endl;
  
  
  /* Integration loop */
  
  Particle_State ps(v, x);
  std::vector<Particle_State> vector_ps(100, ps); // 100 particles!
  
  Particle_System_Model psm(m, k, L0);
  std::vector<Particle_System_Model> vector_psm(100, psm);
  
  double date = 0.0;
  double time_step = dt;
  int N = static_cast<int>( floor(T/dt) ); // double floor(double)
  
  file_out << "t\t" << "x\t" << "v\t"
	   << "KE\t" << "PE\t" << "E\t"
	   << endl;
  // Write time, position, velocity,
  // kinetic energy, potential energy and total energy in file_result
  
  // Energy initial evaluation for particle 0
  double vel = vector_ps[0].vel;
  double pos = vector_ps[0].pos;
  double KE = 0.5*m*vel*vel;
  double PE = 0.5*k*(pos - L0)*(pos - L0);
  double E = KE + PE;
  double initialE = E;
  
  writeInfo(file_out, date, pos, vel, KE, PE, E);
  
  if ( method == EULER )
    {
      animal::integration::Euler<Particle_System_Traits,
                                 Simple_Derivative,
                                 Simple_Step>
      solve( vector_ps, Simple_Derivative(), Simple_Step() );
      
      for (int n=0; n < N; n++)
	{
	  solve(vector_psm, vector_ps, date, time_step);
          date += time_step;
          
          // Energy current evaluation for particle 0
          vel = vector_ps[0].vel;
          pos = vector_ps[0].pos;
          KE = 0.5*m*vel*vel;
          PE = 0.5*k*(pos - L0)*(pos - L0);
          E = KE + PE;
          
          writeInfo(file_out, date, pos, vel, KE, PE, E);
	}
    }
  else if ( method == MEULER )
    {
      animal::integration::Euler<Particle_System_Traits,
                                 Simple_Derivative,
                                 Modified_Step>
      solve( vector_ps, Simple_Derivative(), Modified_Step() );
      
      for (int n=0; n < N; n++)
	{
	  solve(vector_psm, vector_ps, date, time_step);
          date += time_step;
          
          // Energy current evaluation for particle 0
          vel = vector_ps[0].vel;
          pos = vector_ps[0].pos;
          KE = 0.5*m*vel*vel;
          PE = 0.5*k*(pos - L0)*(pos - L0);
          E = KE + PE;
          
          writeInfo(file_out, date, pos, vel, KE, PE, E);
	}
    }
  else if ( method == SEULER )
    {
      animal::integration::Euler<Particle_System_Traits,
                                 Stoermer_Derivative,
                                 Stoermer_Step>
      solve( vector_ps, Stoermer_Derivative(), Stoermer_Step() );
      
      for (int n=0; n < N; n++)
	{
	  solve(vector_psm, vector_ps, date, time_step);
          date += time_step;
          
          // Energy current evaluation for particle 0
          vel = vector_ps[0].vel;
          pos = vector_ps[0].pos;
          KE = 0.5*m*vel*vel;
          PE = 0.5*k*(pos - L0)*(pos - L0);
          E = KE + PE;
          
          writeInfo(file_out, date, pos, vel, KE, PE, E);
	}
    }
  else if ( method == RK2 )
    {
      animal::integration::Runge_Kutta_2<Particle_System_Traits,
                                         Simple_Derivative,
                                         Simple_Step>
      solve( vector_ps, Simple_Derivative(), Simple_Step() );
      
      for (int n=0; n < N; n++)
	{
	  solve(vector_psm, vector_ps, date, time_step);
          date += time_step;
          
          // Energy current evaluation for particle 0
          vel = vector_ps[0].vel;
          pos = vector_ps[0].pos;
          KE = 0.5*m*vel*vel;
          PE = 0.5*k*(pos - L0)*(pos - L0);
          E = KE + PE;
          
          writeInfo(file_out, date, pos, vel, KE, PE, E);
	}
    }
  else if ( method == RK4 )
    {
      animal::integration::Runge_Kutta_4<Particle_System_Traits,
                                         Simple_Derivative,
                                         Simple_Step>
      solve( vector_ps, Simple_Derivative(), Simple_Step() );
      
      for (int n=0; n < N; n++)
	{
	  solve(vector_psm, vector_ps, date, time_step);
          date += time_step;
          
          // Energy current evaluation for particle 0
          vel = vector_ps[0].vel;
          pos = vector_ps[0].pos;
          KE = 0.5*m*vel*vel;
          PE = 0.5*k*(pos - L0)*(pos - L0);
          E = KE + PE;
          
          writeInfo(file_out, date, pos, vel, KE, PE, E);
	}
    }
  else if ( method == MM )
    {
      animal::integration::Modified_Midpoint<Particle_System_Traits,
                                             Simple_Derivative,
                                             Modified_Midpoint_Step,
                                             Constant_NStep>
      solve( vector_ps, Simple_Derivative(), Modified_Midpoint_Step(), Constant_NStep() );
      
      for (int n=0; n < N; n++)
	{
	  solve(vector_psm, vector_ps, date, time_step);
          date += time_step;
          
          // Energy current evaluation for particle 0
          vel = vector_ps[0].vel;
          pos = vector_ps[0].pos;
          KE = 0.5*m*vel*vel;
          PE = 0.5*k*(pos - L0)*(pos - L0);
          E = KE + PE;
          
          writeInfo(file_out, date, pos, vel, KE, PE, E);
	}
    }
  else
    error("Unknown method");
  
  // Energy final evaluation for particle 0
  double finalE = E;
  
  file_out.close();
  cout << " Recording file "<<argv[3]<<"... "<<"Done!"<< endl;
  
  cout << endl;
  cout << "------------------------------------------" << endl;
  cout << " INTEGRATION METHOD STATISTICS            " << endl;
  cout << "------------------------------------------" << endl;
  cout << endl;
  cout << " " << N << " steps taken." << endl;
  cout << " Energy initial value\t" << initialE << " S.I." << endl;
  cout << " Energy final value\t" << finalE << " S.I." << endl;
  cout << " Absolute difference\t"
       << finalE - initialE << " S.I." << endl;
  cout << " Relative difference\t"
       << ( (finalE - initialE)/initialE )*100.0 << " %" << endl;
  cout << endl;
  
  return 0;
}
