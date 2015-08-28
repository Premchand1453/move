#include <cstdlib>
#include <sys/time.h>
#include <fstream>
#include <algorithm>
#include <functional>
#include <GL/glut.h>
#include <animal/support/trackball.h>
#include <animal/integration/explicit_driver.h>
#include "scheme.h"

using namespace std;

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
typedef std::vector<edge_index> edge_index_v;
edge_index_v edge_indices;

/* Integration */
typedef std::vector<Spring> spring_v;
spring_v springs;
typedef std::vector<Particle_State> ps_v;
ps_v state;
std::vector<Particle_Model> model;
typedef animal::integration::Euler<Particle_Traits,
                                   Stoermer_Derivative<spring_v>,
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
  
  // Vertices
  glBegin(GL_POINTS);
    for (ps_v::const_iterator firstp = state.begin();
	 firstp != state.end();
	 ++firstp)
      {
	glVertex3dv(&(*firstp).pos[0]);
      }
  glEnd();
  
  // Hexas
  glBegin(GL_LINES);
    for (edge_index_v::const_iterator firste = edge_indices.begin();
	 firste != edge_indices.end();
	 ++firste)
      {
	glVertex3dv(&state[(*firste).p0].pos[0]);
	glVertex3dv(&state[(*firste).p1].pos[0]);
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
  if ( argc != 2 ) error("Usage: move_hexa_ms [mesh file]");
  
  ifstream file_in(argv[1], ios::in);
  if ( !file_in ) error("Cannot open input file", argv[1]);
  
  int nvertex, nhexa;
  GLdouble x, y, z;
  Particle_State::constraint_mode c;
  int p0, p1, p2, p3, p4, p5, p6, p7;
  char line[256];
  
  file_in.getline(line, 256, '\n');
  sscanf(line, "%d %d", &nvertex, &nhexa);
  
  cout << "Model contains:" << endl;
  cout << nvertex << " vertices" << endl;
  cout << nhexa << " hexaedra" << endl;
  
  for (int nv = 0; nv < nvertex; ++nv)
    {
      file_in.getline(line, 256, '\n');
      sscanf(line, "%lf %lf %lf\t%d", &x, &y, &z, &c);
      
      state.push_back( Particle_State(Vec3::null(), Vec3(x, y, z), c) );
      model.push_back( Particle_Model(1.0e-02, Vec3::null()) ); // mass = 0.01 kg
    }
  
  for (int nh = 0; nh < nhexa; ++nh)
    {
      file_in.getline(line, 256, '\n');
      sscanf(line, "%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d", &p0, &p1, &p2, &p3, &p4, &p5, &p6, &p7);
      
      // Stretch springs
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
      
      // Shear springs
      edge_indices.push_back( edge_index(p0, p6) );
      edge_indices.push_back( edge_index(p1, p7) );
      edge_indices.push_back( edge_index(p2, p4) );
      edge_indices.push_back( edge_index(p3, p5) );
    }
  
  edge_index_v::iterator first;
  
  for (first = edge_indices.begin();
       first != edge_indices.end();
       ++first)
    {
      edge_index_v::iterator f = first;
      ++f;
      
      edge_indices.erase
        (
	  std::remove_if
	    (
	      f,
	      edge_indices.end(),
	      std::bind2nd( std::equal_to<edge_index>(), (*first) )
	    ),
	  edge_indices.end()
	);
    }
  
  int nsprings = edge_indices.size();
  cout << nsprings << " springs" << endl;
  cout << "Mean spring/mass ratio is "
       << static_cast<double>(nsprings)/static_cast<double>(nvertex) << endl;
  cout << "Within animation window, type h for help" << endl;
  cout << endl;
  
  for (first = edge_indices.begin();
       first != edge_indices.end();
       ++first)
    {
      // ks = 2.5 kd = 10.0
      Real rest_length = (state[(*first).p0].pos - state[(*first).p1].pos).norm();
      springs.push_back( Spring((*first).p0, (*first).p1, 2.5, 10.0, rest_length) );
    }
  
  double t = 0.0;
  double dt = 0.01; // 0.04 for 25 Hz
  solve_euler = Euler_Solver( state,
			      Stoermer_Derivative<spring_v>(springs),
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
