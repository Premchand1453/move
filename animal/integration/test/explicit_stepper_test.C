#include <cstdlib>
#include <cstring>
#include <fstream>
#include <animal/integration/test/explicit_solver_test.h>
#include <animal/integration/test/explicit_stepper_test.h>

using namespace std;

// -----------------------------------------------------------
//  
//  explicit_stepper_test
//  Test of the explicit stepper classes.
//  
//  File: animal/integration/test/explicit_stepper_test.C
//  (creation date: March 28, 2000).
//  
//  Author: David Bourguignon
//  
//  iMAGIS/GRAVIR, 2000
//  
// -----------------------------------------------------------

inline void error(const char* p1, const char* p2="")
{
  cerr << "Error! " << p1 << " " << p2 << endl;
  exit(1);
}

inline void writeInfo(ofstream& file_out,
		      double date, double step_try, double step_did,
		      double pos, double vel,
		      double KE, double PE, double E)
{
  file_out << date << "\t" << step_try << "\t" << step_did << "\t"
	   << pos << "\t" << vel << "\t"
	   << KE << "\t" << PE << "\t" << E
	   << endl;
}

int main(int argc, char** argv)
{
  /* Parsing input arguments */
  
  if ( argc != 4 )
    {
      cerr << "Usage:\texplicit_stepper_test [-options] [file_param] [file_result]" << endl;
      cerr << "\t[-sd Step doubling method]" << endl;
      cerr << "\t[-sbaf Step back and forth method]" << endl;
      exit(1);
    }
  
  ifstream file_in(argv[2], ios::in);
  if ( !file_in ) error("Cannot open input file", argv[2]);
  
  ofstream file_out(argv[3], ios::out);
  if ( !file_out ) error("Cannot open output file", argv[3]);
  
  cout << endl;
  cout << "------------------------------------------" << endl;
  cout << " TEST OF STEPPING METHODS                 " << endl;
  cout << "------------------------------------------" << endl;
  cout << " USING FOURTH-ORDER RUNGE-KUTTA METHOD    " << endl;
  cout << "------------------------------------------" << endl;
  
  enum stepping_method {SD, SBAF};
  stepping_method method;
  
  if ( !strcmp(argv[1], "-sd") )
    {
      method = SD;
      cout << " STEP DOUBLING METHOD                     " << endl;
      cout << "------------------------------------------" << endl;
      cout << endl;
    }
  else if ( !strcmp(argv[1], "-sbaf") )
    {
      method = SBAF;
      cout << " STEP BACK AND FORTH METHOD               " << endl;
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
  double step_try = dt;
  double step_did = 0.0;
  double step_next = 0.0;
  
  file_out << "t\t" << "htry\t" << "hdid\t"
	   << "x\t" << "v\t"
	   << "KE\t" << "PE\t" << "E\t"
	   << endl;
  // Write time, time step used, position, velocity,
  // kinetic energy, potential energy and total energy in file_result
  
  // Energy initial evaluation for particle 0
  double vel = vector_ps[0].vel;
  double pos = vector_ps[0].pos;
  double KE = 0.5*m*vel*vel;
  double PE = 0.5*k*(pos - L0)*(pos - L0);
  double E = KE + PE;
  double initialE = E;
  
  // Solver definition
  typedef animal::integration::Runge_Kutta_4<Particle_System_Traits,
                                             Simple_Derivative,
                                             Simple_Step>
  Simple_Solver;
  
  Simple_Solver solve( vector_ps, Simple_Derivative(), Simple_Step() );
  
  // Stepper definition
  animal::integration::Step_Doubling<Simple_Solver,
                                     Simply_Smaller,
                                     Simply_Larger,
                                     Different_Accuracy>
  step_SD( solve, Simply_Smaller(), Simply_Larger(), Different_Accuracy() );
  
  // Stepper definition
  animal::integration::Step_Back_And_Forth<Simple_Solver,
                                           Simply_Smaller,
                                           Simply_Larger,
                                           Different_Energy>
  step_SBAF( solve, Simply_Smaller(), Simply_Larger(), Different_Energy() );
  
  if ( method == SD )
    {
      while ( date < T )
	{
	  // Energy current evaluation for particle 0
          vel = vector_ps[0].vel;
          pos = vector_ps[0].pos;
          KE = 0.5*m*vel*vel;
          PE = 0.5*k*(pos - L0)*(pos - L0);
          E = KE + PE;
          
          writeInfo(file_out, date, step_try, step_did, pos, vel, KE, PE, E);
          
          // Integration step
          step_SD(vector_psm, vector_ps, date, step_try, step_did, step_next);
          date += step_did;
          step_try = step_next;
	}
    }
  else if ( method == SBAF )
    {
      while ( date < T )
	{
	  // Energy current evaluation for particle 0
          vel = vector_ps[0].vel;
          pos = vector_ps[0].pos;
          KE = 0.5*m*vel*vel;
          PE = 0.5*k*(pos - L0)*(pos - L0);
          E = KE + PE;
          
          writeInfo(file_out, date, step_try, step_did, pos, vel, KE, PE, E);
          
          // Integration step
          step_SBAF(vector_psm, vector_ps, date, step_try, step_did, step_next);
          date += step_did;
          step_try = step_next;
	}
    }
  else
    error("Unknown method");
  
  // Energy final evaluation for particle 0
  vel = vector_ps[0].vel;
  pos = vector_ps[0].pos;
  KE = 0.5*m*vel*vel;
  PE = 0.5*k*(pos - L0)*(pos - L0);
  E = KE + PE;
  double finalE = E;
  
  file_out.close();
  cout << " Recording file "<<argv[3]<<"... "<<"Done!"<< endl;
  
  cout << endl;
  cout << "------------------------------------------" << endl;
  cout << " STEPPING METHOD STATISTICS               " << endl;
  cout << "------------------------------------------" << endl;
  cout << endl;
  
  if ( method == SD )
    {
      cout << " " << step_SD.ngood << " good and "
                  << step_SD.nbad  << " bad (but retried and fixed) steps taken." << endl;
    }
  else if ( method == SBAF )
    {
      cout << " " << step_SBAF.ngood << " good and "
                  << step_SBAF.nbad  << " bad (but retried and fixed) steps taken." << endl;
    }
  
  cout << " Energy initial value\t" << initialE << " S.I." << endl;
  cout << " Energy final value\t" << finalE << " S.I." << endl;
  cout << " Absolute difference\t"
       << finalE - initialE << " S.I." << endl;
  cout << " Relative difference\t"
       << ( (finalE - initialE)/initialE )*100.0 << " %" << endl;
  cout << endl;
  
  return 0;
}
