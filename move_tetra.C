#include <cstdlib>
#include <time.h>
#include <sys/time.h>
#include <fstream>
#include <algorithm>
#include <functional>
#include <map>
#include <GL/glut.h>
#include <animal/support/trackball.h>
#include <animal/integration/explicit_driver.h>
#include <intersect_triangle.h>
#include "scheme.h"

/* Parameters setting */
#define FIXED_FRAME    1 /* for examples 1,2,3 only */
#define CIRCULAR_FRAME 0 /* for examples 4,5 only   */
#define RANDOM_FRAME   0 /* for example  6 only     */

#define EXAMPLE_1 1
#define EXAMPLE_2 0
#define EXAMPLE_3 0
#define EXAMPLE_4 0
#define EXAMPLE_5 0
#define EXAMPLE_6 0

#define CUBE_PARAMS 0
#define BEAM_PARAMS 1
#define PUSH_PARAMS 0

#if MEASURE
#define CUBE_PARAMS    0
#define BEAM_PARAMS    0
#define PUSH_PARAMS    0
#define MEASURE_PARAMS 1
#endif

using namespace std;

const double RAND_MAX_INV = 1.0/RAND_MAX;

/* Trackball & window */
typedef animal::geometry::Quaternion<Real> Quat;
typedef animal::support::Trackball<Real> Trackball;
Trackball tb;
GLdouble aspect;
GLint viewport[4];
int main_window;
enum commands {PLAY, PAUSE, STEP};
commands commands_mode = PAUSE;

/* Time */
timeval last_t, current_t;
int nframes = 0;
double frames_per_sec = 0.0;
char fps[256] = "";

/* Volume informations */
#if VOLINFO
ofstream* file_out;
Real V0 = 0.0;
#endif

/* Display datastruct */
struct edge_index
{
  int p0, p1;
  
  edge_index(int i0, int i1)
    {
      p0 = i0; p1 = i1;
    }
  bool operator==(const edge_index& edge) const
    {
      return ( ( (edge.p0 == p0) && (edge.p1 == p1) )
               ||
               ( (edge.p0 == p1) && (edge.p1 == p0) ) );
    }
  friend ostream& operator<<(ostream& s, const edge_index& edge)
    {
      return s << edge.p0 << " " << edge.p1;
    }
};
struct tetra_index
{
  int p0, p1, p2, p3;
  
  tetra_index(int i0, int i1, int i2, int i3)
    {
      p0 = i0; p1 = i1; p2 =i2; p3 =i3;
    }
};
typedef std::vector<edge_index> edge_index_v;
edge_index_v edge_indices;
typedef std::vector<tetra_index> tetra_index_v;
tetra_index_v tetra_indices;

/* Triangle */
struct triang
{
  int p0, p1, p2;
  Vec3 vtc[3];
  
  triang()
    {}
  triang(int i0, int i1, int i2, Vec3 v[3])
    {
      p0 = i0; p1 = i1; p2 = i2;
      
      for (int i = 0; i < 3; i++)
	vtc[i] = v[i];
    }
};
typedef std::vector<triang> triang_v;
triang_v triangles(4);

/* Integration */
typedef std::vector<TetraSpring> tetraspring_v;
tetraspring_v tetrasprings;
typedef std::vector<Particle_State> ps_v;
ps_v state;
std::vector<Particle_Model> model;
typedef animal::integration::Euler<Particle_Traits,
                                   Stoermer_Derivative<tetraspring_v>,
                                   Stoermer_Step> Euler_Solver;
Euler_Solver solve_euler;
typedef animal::integration::Solver_Driver<Euler_Solver> Driver;
Driver drive;

/* Declarations */
void init(char* name);

void reshape(int w, int h);
void display();
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void idle();

inline void computeFrameRate();
inline void displayText(GLuint x, GLuint y, GLdouble scale, char *t);
inline void draw();
inline void animate();
#if VOLINFO
inline void writeVolume(Real t);
#endif

/* Definitions */
void init(char* name)
{
  glutInitWindowSize(512, 512);
  main_window = glutCreateWindow(name);
  glutReshapeFunc(reshape);
  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  glutIdleFunc(idle);
  
  // Init trackball
  Quat qInit1 = Quat( Vec3(1.0, 0.0, 0.0), -M_PI_4/4 );
  Quat qInit2 = Quat( Vec3(0.0, 1.0, 0.0), M_PI_4/2 );
  Quat qInit  = qInit1*qInit2;
  Vec3 vInit  = Vec3(0.0, 0.0, -10.0);
  tb = Trackball(qInit, vInit);
  
  gettimeofday(&last_t, NULL);
}

void reshape(int w, int h)
{
  glViewport( 0, 0, static_cast<GLsizei>(w), static_cast<GLsizei>(h) );
  aspect = static_cast<GLfloat>(w) / static_cast<GLfloat>(h);
  
  glGetIntegerv(GL_VIEWPORT, viewport);
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0, aspect, 1.0e-2, 1.0e+2);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  tb.reshape(w,h);
}

void display()
{
  computeFrameRate();
  
  static GLdouble m[4][4];
  
  glClearColor(0.0, 0.0, 0.0, 1.0);
  
  glClear(GL_COLOR_BUFFER_BIT);
  
  displayText(10, 10, 5e-04, fps);
  
  tb.writeOpenGLTransfMatrix(m);
  
  glPushMatrix();
    glMultMatrixd( &m[0][0] );
    draw();
  glPopMatrix();
  
#if DEBUG
  // Error control
  static GLenum err_code;
  static const GLubyte* err_string;
  
  if ( (err_code = glGetError()) != GL_NO_ERROR )
    {
      err_string = gluErrorString(err_code);
      cerr << "OpenGL Error: " << err_string << endl;
    }
#endif
  
  glutSwapBuffers();
  glFlush();
}

void keyboard(unsigned char key, int x, int y)
{
  switch ( key ) {
    
  case '1':
    commands_mode = PLAY;
    break;
    
  case '2':
    commands_mode = PAUSE;
    break;
    
  case '3':
    commands_mode = STEP;
    break;
    
  case 'h':
    printf(" MOUSE\n");
    printf("+-------------+---------------+----------------+\n");
    printf("| Left button | Middle button |  Right button  |\n");
    printf("+-------------+---------------+----------------+\n");
    printf("| Rotation    | TranslationZ  |  TranslationXY |\n");
    printf("+-------------+---------------+----------------+\n");
    printf("\n");
    printf(" KEYBOARD");
    printf(" \to\tback to Origin\n");
    printf(" \t\tq\tQuit\n");
    printf(" \t\th\tdisplay Help\n");
    printf("\n");
    printf(" \t\t1\tplay\n");
    printf(" \t\t2\tpause\n");
    printf(" \t\t3\tstep\n");
    printf("\n");
    break;
    
  case 'o':
    tb.reinitializeTransf();
    glutPostRedisplay();
    break;
    
  case 'q':
    exit(EXIT_SUCCESS);
    break;
    
  default:
    fprintf(stderr, "Error: unknown option !\n");
    break;
  }
}

void mouse(int button, int state, int x, int y)
{
  switch ( button ) {
    
  case GLUT_LEFT_BUTTON:
    if ( state == GLUT_DOWN )
      tb.startRotation(x,y);
    else
      tb.stopRotation();
    break;
    
  case GLUT_MIDDLE_BUTTON:
    if ( state == GLUT_DOWN )
      tb.startTranslationZ(x,y);
    else
      tb.stopTranslationZ();
    break;
    
  case GLUT_RIGHT_BUTTON:
    if ( state == GLUT_DOWN )
      tb.startTranslationXY(x,y);
    else
      tb.stopTranslationXY();
    break;
  }
}

void motion(int x, int y)
{
  tb.move(x,y);
  glutPostRedisplay();
}

void idle()
{
  switch ( commands_mode ) {
    
  case PAUSE:
    break;
    
  case PLAY:
    animate();
    break;
    
  case STEP:
    animate();
    commands_mode = PAUSE;
    break;
  }
}

void computeFrameRate()
{
  if (nframes < 10)
    {
      nframes++;
    }
  else
    {
      gettimeofday(&current_t, NULL);
      
      frames_per_sec = static_cast<double>(nframes) /
	                 (  (current_t.tv_sec - last_t.tv_sec)
			  + (current_t.tv_usec - last_t.tv_usec)/1.0e6  );
      
      sprintf(fps, "%.2lf fps", frames_per_sec);
      
      last_t = current_t;
      nframes = 1;
    }
}

void displayText(GLuint x, GLuint y, GLdouble scale, char *t)
{
  char *c;
  
  GLdouble w = viewport[2];
  GLdouble h = viewport[3];
  
  GLdouble scalef = scale*h;
  
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, w, 0.0, h);
    
    glTranslatef(x, y, 0.0);
    glScaled(scalef, scalef, scalef);
    
    glColor3f(1.0, 1.0, 1.0);
    
    for (c = t; *c; c++)
      glutStrokeCharacter(GLUT_STROKE_ROMAN, *c);
  glPopMatrix();
}

void draw()
{
  glPointSize(5.0);
  glColor3f(1.0, 1.0, 0.0);
  
#ifndef SURFACE
  // Vertices
  glBegin(GL_POINTS);
    for (ps_v::const_iterator firstp = state.begin();
	 firstp != state.end();
	 ++firstp)
      {
	glVertex3dv(&(*firstp).pos[0]);
      }
  glEnd();
#endif
  
  // Edges
  glBegin(GL_LINES);
    for (edge_index_v::const_iterator firste = edge_indices.begin();
	 firste != edge_indices.end();
	 ++firste)
      {
	glVertex3dv(&state[(*firste).p0].pos[0]);
	glVertex3dv(&state[(*firste).p1].pos[0]);
      }
  glEnd();
  
  glColor3f(1.0, 0.0, 1.0);
  
  // Fiber
  glBegin(GL_LINES);
    for (tetraspring_v::iterator firstt = drive.compute.writeDerivative.F.begin();
         firstt != drive.compute.writeDerivative.F.end();
         ++firstt)
      {
	glVertex3dv( &(*firstt).f1[0]);
	glVertex3dv(&(*firstt).ff1[0]);
	// glVertex3dv( &(*firstt).f2[0]);
	// glVertex3dv(&(*firstt).ff2[0]);
	// glVertex3dv( &(*firstt).f3[0]);
	// glVertex3dv(&(*firstt).ff3[0]);
      }
  glEnd();
}

void animate()
{
  drive(model, state);
  
#if MEASURE
  cout << drive.date << " ";
  if (drive.date > 20.0) {
    exit(0);
  }
#endif
  
#if VOLINFO
  writeVolume(drive.date);
#endif
  
#if BENCHMARK
  static double date = 0.0;
  static timeval last_time, current_time;
  static double elapsed_time;
  
  if ( drive.date > date )
    {
      cout << "Date = " << drive.date << " s" << endl;
      
      gettimeofday(&current_time, NULL);
      
      elapsed_time = (current_time.tv_sec - last_time.tv_sec)
	               + (current_time.tv_usec - last_time.tv_usec)/1.0e6;
      
      cout << "Elapsed CPU time = " << elapsed_time << " s" << endl;
      
      last_time = current_time;
      
      date += 1.0; // every second
    }
#else
  static int t = 0;
  
  if ( drive.date > 0.04*t ) // at 25 Hz
    {
      glutPostRedisplay();
      t++;
    }
#endif
}

#if VOLINFO
void writeVolume(Real t)
{
  Real V = 0.0;
  
  for (tetra_index_v::iterator firstt = tetra_indices.begin();
       firstt != tetra_indices.end();
       ++firstt)
    {
      Vec3 v0( state[(*firstt).p0].pos );
      
      Vec3 v01( state[(*firstt).p1].pos - v0 );
      Vec3 v02( state[(*firstt).p2].pos - v0 );
      Vec3 v03( state[(*firstt).p3].pos - v0 );
      
      V += 0.16666667 * fabs( dot( cross( v01, v02 ), v03 ) );
      // one-sixth
    }
  
  (*file_out) << t << "\t" << V << "\t" << 100.0*( (V - V0)/V0 ) << endl;
}
#endif

void error(const char* c1, const char* c2="")
{
  cerr << "Error! " << c1 << " " << c2 << endl;
  exit(1);
}

void parse(int argc, char** argv)
{
#if SURFACE
  if ( argc != 3 ) error("Usage: move_tetra [mesh file] [faces file]");
  
  ifstream file_in(argv[1], ios::in);
  if ( !file_in ) error("Cannot open input file", argv[1]);
  
  ifstream file_in_faces(argv[2], ios::in);
  if ( !file_in_faces ) error("Cannot open faces file", argv[2]);
#else
  if ( argc != 2 ) error("Usage: move_tetra [mesh file]");
  
  ifstream file_in(argv[1], ios::in);
  if ( !file_in ) error("Cannot open input file", argv[1]);
#endif
  
#if VOLINFO
  file_out = new ofstream("vol.dat", ios::out); // Never deleted!
  if ( !file_out ) error("Cannot open output file", "vol.dat");
  
  (*file_out) << "t\t" << "V\t" << "DV/V0 in %" << endl;
#endif
  
  int nvertex, ntetra;
  GLdouble x, y, z;
  Particle_State::constraint_mode c;
  int p0, p1, p2, p3;
  char line[256];
  
  file_in.getline(line, 256, '\n');
  sscanf(line, "%d %d", &nvertex, &ntetra);
  
  cout << "Model contains:" << endl;
  cout << nvertex << " vertices" << endl;
  cout << ntetra << " tetrahedra" << endl;
  
  for (int nv = 0; nv < nvertex; ++nv)
    {
      file_in.getline(line, 256, '\n');
      sscanf(line, "%lf %lf %lf\t%d", &x, &y, &z, &c);
      
      state.push_back( Particle_State(Vec3::null(), Vec3(x, y, z), c) );
#if MEASURE
      /* Particle weight is volume-dependent (see below) */
      model.push_back( Particle_Model(0.0, Vec3::null()) );
#else
      /* Every particle weights always 10 g */
      model.push_back( Particle_Model(1.0e-02, Vec3::null()) ); // mass = 0.01 kg
#endif
    }
  
#if SURFACE
  int nface;
  
  file_in_faces.getline(line, 256, '\n');
  sscanf(line, "%d", &nface);
  
  for (int nf = 0; nf < nface; ++nf)
    {
      file_in_faces.getline(line, 256, '\n');
      sscanf(line, "%d %d %d %d", &p0, &p1, &p2, &p3); // p0 is for dump char
      
      edge_indices.push_back( edge_index(p1 - 1, p2 - 1) );
      edge_indices.push_back( edge_index(p1 - 1, p3 - 1) );
      edge_indices.push_back( edge_index(p2 - 1, p3 - 1) );
    }
  
  for (int nt = 0; nt < ntetra; ++nt)
    {
      file_in.getline(line, 256, '\n');
      sscanf(line, "%d %d %d %d", &p0, &p1, &p2, &p3);
      
      tetra_indices.push_back( tetra_index(p0, p1, p2, p3) );
    }
#else
  for (int nt = 0; nt < ntetra; ++nt)
    {
      file_in.getline(line, 256, '\n');
      sscanf(line, "%d %d %d %d", &p0, &p1, &p2, &p3);
      
      edge_indices.push_back( edge_index(p0, p1) );
      edge_indices.push_back( edge_index(p0, p2) );
      edge_indices.push_back( edge_index(p0, p3) );
      edge_indices.push_back( edge_index(p1, p2) );
      edge_indices.push_back( edge_index(p1, p3) );
      edge_indices.push_back( edge_index(p2, p3) );
      
      tetra_indices.push_back( tetra_index(p0, p1, p2, p3) );
    }
#endif
  
#if defined(VOLINFO) || defined(MEASURE)
  for (tetra_index_v::iterator firstt = tetra_indices.begin();
       firstt != tetra_indices.end();
       ++firstt)
    {
      Vec3 v0( state[(*firstt).p0].pos );
      
      Vec3 v01( state[(*firstt).p1].pos - v0 );
      Vec3 v02( state[(*firstt).p2].pos - v0 );
      Vec3 v03( state[(*firstt).p3].pos - v0 );
      
      Real volume = 0.16666667 * fabs( dot( cross( v01, v02 ), v03 ) );
      // one-sixth
      
#if VOLINFO
      V0 += volume;
#elif MEASURE
      const Real density = 1000.0; // 1000 kg.m-3 (as water)
      /* From Gilles Debunne, should be replaced by Voronoi volumes */
      Real mass = 0.25 * density * volume;
      model[(*firstt).p0].m += mass;
      model[(*firstt).p1].m += mass;
      model[(*firstt).p2].m += mass;
      model[(*firstt).p3].m += mass;
#endif
    }
#endif
  
#if DEBUG
  // for (int i = 0; i < model.size(); i++) {
  //   printf("%i %f\n", i, model[i].m);
  // }
#endif
  
  for (edge_index_v::iterator firste = edge_indices.begin();
       firste != edge_indices.end();
       ++firste)
    {
      edge_index_v::iterator f = firste;
      ++f;
      
      edge_indices.erase
        (
	  std::remove_if
	    (
	      f,
	      edge_indices.end(),
	      std::bind2nd( std::equal_to<edge_index>(), (*firste) )
	    ),
	  edge_indices.end()
	);
    }
  
  std::map<int,int> missed;
  std::map<int,double> problem;
  
  Real ks1, ks2, ks3, ks4, ks5, ks6;
  Real kd1, kd2, kd3;
  Real kvs;
  
  srand( static_cast<unsigned int>(time(0)) );
  
  for (tetra_index_v::iterator firstt = tetra_indices.begin();
       firstt != tetra_indices.end();
       ++firstt)
    {
      Vec3 v0( state[(*firstt).p0].pos.x(),
	       state[(*firstt).p0].pos.y(),
	       state[(*firstt).p0].pos.z() );
      
      Vec3 v1( state[(*firstt).p1].pos.x(),
	       state[(*firstt).p1].pos.y(),
	       state[(*firstt).p1].pos.z() );
      
      Vec3 v2( state[(*firstt).p2].pos.x(),
	       state[(*firstt).p2].pos.y(),
	       state[(*firstt).p2].pos.z() );
      
      Vec3 v3( state[(*firstt).p3].pos.x(),
	       state[(*firstt).p3].pos.y(),
	       state[(*firstt).p3].pos.z() );
      
      Vec3 g_pos = 0.25*(v0 + v1 + v2 + v3);
      
      Vec3 v012[3] = { v0,v1,v2 };
      Vec3 v013[3] = { v0,v1,v3 };
      Vec3 v023[3] = { v0,v2,v3 };
      Vec3 v123[3] = { v1,v2,v3 };
      
      triangles[0] = triang(0, 1, 2, v012);
      triangles[1] = triang(0, 1, 3, v013);
      triangles[2] = triang(0, 2, 3, v023);
      triangles[3] = triang(1, 2, 3, v123);
      
#if FIXED_FRAME
#if EXAMPLE_1
      Real azi = M_PI_2;
      Real ele = M_PI_2;
#elif EXAMPLE_2
      Real azi = M_PI_2;
      Real ele = 0.0;
#elif EXAMPLE_3
      Real azi = M_PI_2;
      Real ele = M_PI_4;
#endif
      
      Vec3 f1 = Vec3( cos(azi)*cos(ele), sin(ele), sin(azi)*cos(ele) );
      Vec3 ff1= -f1;
      Vec3 f2 = cross( f1, Vec3(1.0, 0.0, 0.0) ); // arbitrary!
      Vec3 ff2= -f2;
      Vec3 f3 = cross(f1,f2);
      Vec3 ff3= -f3;
#elif RANDOM_FRAME
#if EXAMPLE_6
      Real azi = (rand()*RAND_MAX_INV) * M_PI;          // in [0, PI[
      Real ele = (rand()*RAND_MAX_INV) * M_PI - M_PI_2; // in [-PI/2, +PI/2[
      
      Vec3 f1 = Vec3( cos(azi)*cos(ele), sin(ele), sin(azi)*cos(ele) );
      Vec3 ff1= -f1;
      Vec3 f2 = cross( f1, Vec3(1.0, 0.0, 0.0) ); // arbitrary!
      Vec3 ff2= -f2;
      Vec3 f3 = cross(f1,f2);
      Vec3 ff3= -f3;
#endif
#elif CIRCULAR_FRAME
#if EXAMPLE_4
      Vec3 O(g_pos[0], 2.5, 3.0);
      Vec3 n(-1.0, 0.0, 0.0);
#elif EXAMPLE_5
      Vec3 O(1.5, g_pos[1], 1.5);
      Vec3 n(0.0, 1.0, 0.0);
#endif
      
      Vec3 OP( g_pos - O );
      Vec3 v_ele( cross(OP,n) );
      v_ele.normalize();
      
#if EXAMPLE_4
      Real azi = M_PI_2;                    // in [0, PI[
      Real ele = atan( v_ele[1]/v_ele[2] ); // in [-PI/2, +PI/2[
#elif EXAMPLE_5
      Real azi = atan( v_ele[2]/v_ele[0] ); // in [0, PI[
      Real ele;
      if ( g_pos[2] <= 1.5 ) ele = M_PI_4;
      else ele = - M_PI_4;                  // in [-PI/2, +PI/2[
#endif
      
      Vec3 f1 = Vec3( cos(azi)*cos(ele), sin(ele), sin(azi)*cos(ele) );
      Vec3 ff1= -f1;
      Vec3 f2 = cross( f1, Vec3(1.0, 0.0, 0.0) ); // arbitrary!
      Vec3 ff2= -f2;
      Vec3 f3 = cross(f1,f2);
      Vec3 ff3= -f3;
#endif
      
      Vec3 f123[6] = { f1, ff1, f2, ff2, f3, ff3 };
      
      Vec3 normal;
      std::vector<Vec3> intersections;
      
      int vertex_indices[6][3];
      Real coefs[6][3];
      
      for (int i = 0; i < 6; i++)
	{
	  int nfound_before = intersections.size();
	  Vec3 f = f123[i];
          
	  while ( intersections.size() == nfound_before )
	    {
	      for (int j = 0; j < 4; j++)
		{
		  Vec3 ip;
                  
                  double orig[3], dir[3], vert0[3], vert1[3], vert2[3];
                  double t = 0, u = 0, v = 0;
                  
                  for (int index = 0; index < 3; index++) 
                    {
                      orig[index] = g_pos[index];
                      dir[index] = f[index];
                      vert0[index] = triangles[j].vtc[0][index];
                      vert1[index] = triangles[j].vtc[1][index];
                      vert2[index] = triangles[j].vtc[2][index];
                    }
                  
                  if ( intersect_triangle(orig, dir, vert0, vert1, vert2, &t, &u, &v) )
                    {
                      ip = triangles[j].vtc[0] * (1.0 - u - v)
                         + triangles[j].vtc[1] * u
                         + triangles[j].vtc[2] * v;
#if DEBUG
		      cout << "Found " << ip << endl;
		      cout << endl;
#endif
		      
		      intersections.push_back(ip);
		      
		      vertex_indices[i][0] = triangles[j].p0;
		      vertex_indices[i][1] = triangles[j].p1;
		      vertex_indices[i][2] = triangles[j].p2;
                      
		      // Redundent since u, v are equivalent to coefs[][]
		      Real A  =
			0.5*( cross( (triangles[j].vtc[1] - triangles[j].vtc[0]),
		                     (triangles[j].vtc[2] - triangles[j].vtc[0]) ) ).norm();
		      
		      Real a0 =
		        0.5*( cross( (triangles[j].vtc[1] - ip),
		                     (triangles[j].vtc[2] - ip) ) ).norm();
		      
		      Real a1 =
		        0.5*( cross( (triangles[j].vtc[0] - ip),
		                     (triangles[j].vtc[2] - ip) ) ).norm();
		      
		      Real a2 =
		        0.5*( cross( (triangles[j].vtc[0] - ip),
		                     (triangles[j].vtc[1] - ip) ) ).norm();
		      
		      coefs[i][0] = a0/A;
		      coefs[i][1] = a1/A;
		      coefs[i][2] = a2/A;
		      
		      Real sum = coefs[i][0] + coefs[i][1] + coefs[i][2];
		      if ( sum > 1.01 ) // numerical round-off errors!
			{
			  cout << "Problem! Coefs sum = " << sum << endl;
		          cout << endl;
		          problem[firstt - tetra_indices.begin()] = sum;
			}
                      
                      unsigned int size = intersections.size();
                      if (size > 1 &&
                          intersections[size - 1] == intersections[size - 2])
                        intersections.pop_back();
                      else
                        break;
		    }
		  else
	            {
#if DEBUG
		      printf("No intersection point! ");
		      printf("Tetra %d ", firstt - tetra_indices.begin());
		      printf("Tri %d ", j);
		      printf("Axis %d\n\n", i);
#endif
		    }
		}
              
	      f = f123[i] + 1.0e-06*Vec3( (rand()*RAND_MAX_INV),
                                          (rand()*RAND_MAX_INV),
                                          (rand()*RAND_MAX_INV) );
	    }
	}
      
      if ( intersections.size() != 6 )
	{
	  printf("%d intersection points!\n\n", intersections.size());
	  missed[firstt - tetra_indices.begin()] = intersections.size();
	}
      else
	{
	  Vec3 ip1(intersections[0][0], intersections[0][1], intersections[0][2]);
	  Vec3 ip2(intersections[1][0], intersections[1][1], intersections[1][2]);
	  Vec3 ip3(intersections[2][0], intersections[2][1], intersections[2][2]);
	  Vec3 ip4(intersections[3][0], intersections[3][1], intersections[3][2]);
	  Vec3 ip5(intersections[4][0], intersections[4][1], intersections[4][2]);
	  Vec3 ip6(intersections[5][0], intersections[5][1], intersections[5][2]);
	  
#if DEBUG
	  int i, j;
	  
	  for (i = 0; i < 6; i++)
	    {
	      for (j = 0; j < 3; j++)
	        {
		  cout << vertex_indices[i][j] << " ";
		  cout << coefs[i][j] << endl;
	        }
	      cout << endl;
	    }
#endif
	  
          Vec3 l0 = v0 - g_pos;
          Vec3 l1 = v1 - g_pos;
          Vec3 l2 = v2 - g_pos;
          Vec3 l3 = v3 - g_pos;
          
          Real L0 = l0.norm();
          Real L1 = l1.norm();
          Real L2 = l2.norm();
          Real L3 = l3.norm();
          
          Real rest_length = L0 + L1 + L2 + L3;
	  
#if CUBE_PARAMS
	  ks1 = ks2 = ks3 = ks4 = ks5 = ks6 = 2.5;
	  kd1 = kd2 = kd3 = 10.0;
	  kvs = 5.0;
#elif BEAM_PARAMS
	  ks1 = ks2 = ks3 = ks4 = ks5 = ks6 = 5.0;
	  kd1 = kd2 = kd3 = 10.0;
	  kvs = 5.0;
#elif PUSH_PARAMS
	  ks1 = 17.5;
	  ks2 = ks3 = 0.25;
	  ks4 = ks5 = 2.5;
	  ks6 = 0.25;
	  kd1 = kd2 = kd3 = 10.0;
	  kvs = 5.0;
#elif MEASURE_PARAMS
#if FIXED_FRAME
	  ks1 = ks2 = ks3 = ks4 = ks5 = ks6 = 2.5;
#elif RANDOM_FRAME
	  ks1 = 17.5;
	  ks2 = ks3 = 0.25;
	  ks4 = ks5 = 2.5;
	  ks6 = 0.25;
#endif
          kd1 = kd2 = kd3 = 10.0;
	  kvs = 5.0;
#endif
	  
	  tetrasprings.push_back
	    (
	      TetraSpring( (*firstt).p0, (*firstt).p1, (*firstt).p2, (*firstt).p3,
			   vertex_indices, coefs, ks1, ks2, ks3, ks4, ks5, ks6,
			   kd1, kd2, kd3, kvs, rest_length,
			   ip1, ip2, ip3, ip4, ip5, ip6 )
	    );
	}
    }
  
  cout << "Nb of tetra missed: " << missed.size() << endl;
  for (std::map<int,int>::iterator firstm = missed.begin();
       firstm != missed.end();
       ++firstm)
    cout << firstm->first << " " << firstm->second << endl;
  
  cout << "Nb of problems encountered: " << problem.size() << endl;
  for (std::map<int,double>::iterator firstp = problem.begin();
       firstp != problem.end();
       ++firstp)
    cout << firstp->first << " " << firstp->second << endl;
  
  cout << "Simulation parameters:" << endl;
  cout << "ks1 = " << ks1 << " ks2 = " << ks2 << " ks3 = " << ks3
       << " ks4 = " << ks4 << " ks5 = " << ks5 << " ks6 = " << ks6
       << endl;
  cout << "kd1 = " << kd1 << " kd2 = " << kd2 << " kd3 = " << kd3
       << endl;
  cout << "kvs = " << kvs << endl;
  
  double t = 0.0;
  double dt = 0.01; // 0.04 for 25 Hz
  cout << "dt = " << dt << endl;
  
  cout << "Within animation window, type h for help" << endl;
  cout << endl;
  
#if MEASURE
  cout << "t x y z" << endl;
  cout << t << " ";
#endif
  
  solve_euler = Euler_Solver( state,
			      Stoermer_Derivative<tetraspring_v>(tetrasprings),
			      Stoermer_Step() );  
  drive = Driver(solve_euler, t, dt);
  
  file_in.close();
}

int main(int argc, char** argv)
{
  parse(argc, argv);
  glutInit(&argc, argv);
  glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_MULTISAMPLE );
  init(argv[0]);
  glutMainLoop();
  
  return 0;
}
