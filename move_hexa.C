#include <stdio.h>
#include <stdlib.h>
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
#define CUBE_PARAMS 0
#define BEAM_PARAMS 1

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
struct hexa_index
{
  int p0, p1, p2, p3, p4, p5, p6, p7;
  
  hexa_index(int i0, int i1, int i2, int i3, int i4, int i5, int i6, int i7)
    {
      p0 = i0; p1 = i1; p2 =i2; p3 =i3;
      p4 = i4; p5 = i5; p6 =i6; p7 =i7;
    }
};
typedef std::vector<edge_index> edge_index_v;
edge_index_v edge_indices;
typedef std::vector<hexa_index> hexa_index_v;
hexa_index_v hexa_indices;

/* Quadrangle */
struct quadrang
{
  int p0, p1, p2, p3;
  Vec3 vtc[4];
  
  quadrang()
    {}
  quadrang(int i0, int i1, int i2, int i3, Vec3 v[4])
    {
      p0 = i0; p1 = i1; p2 = i2; p3 = i3;
      
      for (int i = 0; i < 4; i++)
	vtc[i] = v[i];
    }
};
typedef std::vector<quadrang> quadrang_v;
quadrang_v quadrangles(6);

/* Integration */
typedef std::vector<HexaSpring> hexaspring_v;
hexaspring_v hexasprings;
typedef std::vector<Particle_State> ps_v;
ps_v state;
std::vector<Particle_Model> model;
typedef animal::integration::Euler<Particle_Traits,
                                   Stoermer_Derivative<hexaspring_v>,
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
  
  displayText(10, 10, 4e-04, fps);
  
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
    for (hexaspring_v::iterator firstt = drive.compute.writeDerivative.F.begin();
         firstt != drive.compute.writeDerivative.F.end();
         ++firstt)
      {
	// glVertex3dv( &(*firstt).f1[0]);
	// glVertex3dv(&(*firstt).ff1[0]);
	glVertex3dv( &(*firstt).f2[0]);
	glVertex3dv(&(*firstt).ff2[0]);
	// glVertex3dv( &(*firstt).f3[0]);
	// glVertex3dv(&(*firstt).ff3[0]);
      }
  glEnd();
}

void animate()
{
  drive(model, state);
  
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

void error(const char* c1, const char* c2="")
{
  cerr << "Error! " << c1 << " " << c2 << endl;
  exit(1);
}

void parse(int argc, char** argv)
{
#if SURFACE
  if ( argc != 3 ) error("Usage: move_hexa [mesh file] [faces file]");
  
  ifstream file_in(argv[1], ios::in);
  if ( !file_in ) error("Cannot open input file", argv[1]);
  
  ifstream file_in_faces(argv[2], ios::in);
  if ( !file_in_faces ) error("Cannot open faces file", argv[2]);
#else
  if ( argc != 2 ) error("Usage: move_hexa [mesh file]");
  
  ifstream file_in(argv[1], ios::in);
  if ( !file_in ) error("Cannot open input file", argv[1]);
#endif
  
  int nvertex, nhexa;
  GLdouble x, y, z;
  Particle_State::constraint_mode c;
  int p0, p1, p2, p3, p4, p5, p6, p7;
  char line[256];
  
  file_in.getline(line, 256, '\n');
  sscanf(line, "%d %d", &nvertex, &nhexa);
  
  cout << nvertex << " vertices" << endl;
  cout << nhexa << " hexaedra" << endl;
  cout << endl;
  
  for (int nv = 0; nv < nvertex; ++nv)
    {
      file_in.getline(line, 256, '\n');
      sscanf(line, "%lf %lf %lf\t%d", &x, &y, &z, &c);
      
      state.push_back( Particle_State(Vec3::null(), Vec3(x, y, z), c) );
      model.push_back( Particle_Model(1.0e-02, Vec3::null()) ); // mass = 0.01 kg
    }
  
#if SURFACE
  int nface;
  
  file_in_faces.getline(line, 256, '\n');
  sscanf(line, "%d", &nface);
  
  for (int nf = 0; nf < nface; ++nf)
    {
      file_in_faces.getline(line, 256, '\n');
      sscanf(line, "%d %d %d %d", &p0, &p1, &p2, &p3);
      
      edge_indices.push_back( edge_index(p0, p1) );
      edge_indices.push_back( edge_index(p1, p2) );
      edge_indices.push_back( edge_index(p2, p3) );
      edge_indices.push_back( edge_index(p3, p0) );
    }
  
  for (int nh = 0; nh < nhexa; ++nh)
    {
      file_in.getline(line, 256, '\n');
      sscanf(line, "%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d", &p0, &p1, &p2, &p3, &p4, &p5, &p6, &p7);
      
      hexa_indices.push_back( hexa_index(p0, p1, p2, p3, p4, p5, p6, p7) );
    }
#else
  for (int nh = 0; nh < nhexa; ++nh)
    {
      file_in.getline(line, 256, '\n');
      sscanf(line, "%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d", &p0, &p1, &p2, &p3, &p4, &p5, &p6, &p7);
      
      edge_indices.push_back( edge_index(p0, p1) );
      edge_indices.push_back( edge_index(p1, p2) );
      edge_indices.push_back( edge_index(p2, p3) );
      edge_indices.push_back( edge_index(p3, p0) );
      
      edge_indices.push_back( edge_index(p4, p5) );
      edge_indices.push_back( edge_index(p5, p6) );
      edge_indices.push_back( edge_index(p6, p7) );
      edge_indices.push_back( edge_index(p7, p4) );
      
      edge_indices.push_back( edge_index(p0, p4) );
      edge_indices.push_back( edge_index(p1, p5) );
      edge_indices.push_back( edge_index(p2, p6) );
      edge_indices.push_back( edge_index(p3, p7) );
      
      hexa_indices.push_back( hexa_index(p0, p1, p2, p3, p4, p5, p6, p7) );
    }
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
  
  Real azi = 0.0; // in [0, PI[
  Real ele = 0.0; // in [-PI/2, +PI/2[
  
  Vec3 f1 = Vec3( cos(azi)*cos(ele), sin(ele), sin(azi)*cos(ele) );
  Vec3 ff1= -f1;
  Vec3 f3 = Vec3(-sin(azi)*cos(ele), sin(ele), cos(azi)*cos(ele) );
  Vec3 ff3= -f3;
  Vec3 f2 = cross(f3,f1);
  Vec3 ff2= -f2;
  
  Vec3 f123[6] = { f1, ff1, f2, ff2, f3, ff3 };
  
  std::map<int,int> missed;
  std::map<int,double> problem;
  
  srand( static_cast<unsigned int>(time(0)) );
  
  for (hexa_index_v::iterator firsth = hexa_indices.begin();
       firsth != hexa_indices.end();
       ++firsth)
    {
      Vec3 v0( state[(*firsth).p0].pos.x(),
	       state[(*firsth).p0].pos.y(),
	       state[(*firsth).p0].pos.z() );
      
      Vec3 v1( state[(*firsth).p1].pos.x(),
	       state[(*firsth).p1].pos.y(),
	       state[(*firsth).p1].pos.z() );
      
      Vec3 v2( state[(*firsth).p2].pos.x(),
	       state[(*firsth).p2].pos.y(),
	       state[(*firsth).p2].pos.z() );
      
      Vec3 v3( state[(*firsth).p3].pos.x(),
	       state[(*firsth).p3].pos.y(),
	       state[(*firsth).p3].pos.z() );
      
      Vec3 v4( state[(*firsth).p4].pos.x(),
	       state[(*firsth).p4].pos.y(),
	       state[(*firsth).p4].pos.z() );
      
      Vec3 v5( state[(*firsth).p5].pos.x(),
	       state[(*firsth).p5].pos.y(),
	       state[(*firsth).p5].pos.z() );
      
      Vec3 v6( state[(*firsth).p6].pos.x(),
	       state[(*firsth).p6].pos.y(),
	       state[(*firsth).p6].pos.z() );
      
      Vec3 v7( state[(*firsth).p7].pos.x(),
	       state[(*firsth).p7].pos.y(),
	       state[(*firsth).p7].pos.z() );
      
      Vec3 g_pos = 0.125*(v0 + v1 + v2 + v3 + v4 + v5 + v6 + v7);
      
      Vec3 q0123[4] = { v0,v1,v2,v3 };
      Vec3 q4567[4] = { v4,v5,v6,v7 };
      
      Vec3 q0473[4] = { v0,v4,v7,v3 };
      Vec3 q1562[4] = { v1,v5,v6,v2 };
      
      Vec3 q4510[4] = { v4,v5,v1,v0 };
      Vec3 q7623[4] = { v7,v6,v2,v3 };
      
      quadrangles[0] = quadrang(0, 1, 2, 3, q0123);
      quadrangles[1] = quadrang(4, 5, 6, 7, q4567);
      
      quadrangles[2] = quadrang(0, 4, 7, 3, q0473);
      quadrangles[3] = quadrang(1, 5, 6, 2, q1562);
      
      quadrangles[4] = quadrang(4, 5, 1, 0, q4510);
      quadrangles[5] = quadrang(7, 6, 2, 3, q7623);
      
      Vec3 normal;
      std::vector<Vec3> intersections;
      
      int vertex_indices[6][4];
      Real coefs[6][4];
      
      for (int i = 0; i < 6; i++)
	{
	  int nfound_before = intersections.size();
	  Vec3 f = f123[i];
	  
	  while ( intersections.size() == nfound_before )
	    {
	      for (int j = 0; j < 6; j++)
		{
		  Vec3 ip;
                  
		  double orig[3], dir[3], vert0[3], vert1[3], vert2[3], vert3[3];
                  double t = 0, u = 0, v = 0;
                  
                  for (int index = 0; index < 3; index++) 
                    {
                      orig[index] = g_pos[index];
                      dir[index] = f[index];
                      vert0[index] = quadrangles[j].vtc[0][index];
                      vert1[index] = quadrangles[j].vtc[1][index];
                      vert2[index] = quadrangles[j].vtc[2][index];
                      vert3[index] = quadrangles[j].vtc[3][index];
                    }
                  
                  bool is_intersected = false;
                  
                  if ( intersect_triangle(orig, dir, vert0, vert1, vert2, &t, &u, &v) )
                    {
                      is_intersected = true;
                      ip = quadrangles[j].vtc[0] * (1.0 - u - v)
                         + quadrangles[j].vtc[1] * u
                         + quadrangles[j].vtc[2] * v;
                    }
                  else if ( intersect_triangle(orig, dir, vert0, vert2, vert3, &t, &u, &v) )
                    {
                      is_intersected = true;
                      ip = quadrangles[j].vtc[0] * (1.0 - u - v)
                         + quadrangles[j].vtc[2] * u
                         + quadrangles[j].vtc[3] * v;
                    }
                    
                  if (is_intersected)
                    {
#if DEBUG
		      cout << "Found " << ip << endl;
		      cout << endl;
#endif
		      
		      intersections.push_back(ip);
		      
		      vertex_indices[i][0] = quadrangles[j].p0;
		      vertex_indices[i][1] = quadrangles[j].p1;
		      vertex_indices[i][2] = quadrangles[j].p2;
		      vertex_indices[i][3] = quadrangles[j].p3;
		      
		      Vec3 l1 = quadrangles[j].vtc[1] - quadrangles[j].vtc[0];
		      Vec3 l3 = quadrangles[j].vtc[3] - quadrangles[j].vtc[0];
		      Vec3 l  = ip - quadrangles[j].vtc[0];
		      
		      Real L1 = l1.norm();
		      Real L3 = l3.norm();
		      Real L  = l.norm();
		      
		      Real cosang1 = dot(l1,l)/(L1*L);
		      Real cosang3 = dot(l3,l)/(L3*L);
		      
		      coefs[i][0] = (L1 - cosang1*L)/L1;
		      coefs[i][1] = (L3 - cosang3*L)/L3;
		      coefs[i][2] = 1.0 - coefs[i][0];
		      coefs[i][3] = 1.0 - coefs[i][1];
		      
		      if ( coefs[i][0] < 0.0 || coefs[i][0] > 1.0 ||
		           coefs[i][1] < 0.0 || coefs[i][1] > 1.0    )
			{
		          cout << "Problem! zeta = " << coefs[i][0]
			       << " eta = " << coefs[i][1] << endl;
		          cout << endl;
		          problem[firsth - hexa_indices.begin()] = coefs[i][0]; // useless
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
		      printf("Hexa %d ", firsth - hexa_indices.begin());
		      printf("Quadr %d ", j);
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
	  missed[firsth - hexa_indices.begin()] = intersections.size();
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
	      for (j = 0; j < 4; j++)
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
          Vec3 l4 = v4 - g_pos;
          Vec3 l5 = v5 - g_pos;
          Vec3 l6 = v6 - g_pos;
          Vec3 l7 = v7 - g_pos;
          
          Real L0 = l0.norm();
          Real L1 = l1.norm();
          Real L2 = l2.norm();
          Real L3 = l3.norm();
          Real L4 = l4.norm();
          Real L5 = l5.norm();
          Real L6 = l6.norm();
          Real L7 = l7.norm();
          
          Real rest_length = L0 + L1 + L2 + L3 + L4 + L5 + L6 + L7;
#if CUBE_PARAMS
          hexasprings.push_back
            (
              HexaSpring( (*firsth).p0, (*firsth).p1, (*firsth).p2, (*firsth).p3,
                          (*firsth).p4, (*firsth).p5, (*firsth).p6, (*firsth).p7,
                          vertex_indices, coefs, 2.5, 2.5, 2.5, 2.5, 2.5, 2.5,
                          10.0, 10.0, 10.0, 5.0, 10.0, rest_length,
			  L0, L1, L2, L3, L4, L5, L6, L7,
                          ip1, ip2, ip3, ip4, ip5, ip6 )
            );
#elif BEAM_PARAMS
          hexasprings.push_back
            (
              HexaSpring( (*firsth).p0, (*firsth).p1, (*firsth).p2, (*firsth).p3,
                          (*firsth).p4, (*firsth).p5, (*firsth).p6, (*firsth).p7,
                          vertex_indices, coefs, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0,
                          10.0, 10.0, 10.0, 5.0, 10.0, rest_length,
			  L0, L1, L2, L3, L4, L5, L6, L7,
                          ip1, ip2, ip3, ip4, ip5, ip6 )
            );
#endif
	}
    }
  
  cout << "Nb of hexa missed " << missed.size() << endl;
  for (std::map<int,int>::iterator firstm = missed.begin();
       firstm != missed.end();
       ++firstm)
    cout << firstm->first << " " << firstm->second << endl;
  cout << endl;
  
  cout << "Nb of problems encountered " << problem.size() << endl;
  for (std::map<int,double>::iterator firstp = problem.begin();
       firstp != problem.end();
       ++firstp)
    cout << firstp->first << " " << firstp->second << endl;
  cout << endl;
  
  double t = 0.0;
  double dt = 0.004; // 0.04 for 25 Hz
  solve_euler = Euler_Solver( state,
			      Stoermer_Derivative<hexaspring_v>(hexasprings),
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
