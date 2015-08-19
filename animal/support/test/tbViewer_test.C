#include <GL/glui.h>
#include <animal/support/trackball.h>



// ------------------------------------------------------------
//  
//  tbGLUTviewer.test
//  Test of the tbGLUTViewer.
//  
//  The tbGLUTViewer displays a reference frame, with options.
//  GLUT is the OpenGL Utility Toolkit.
//  
//  File: animal/support/examples/tbGLUTViewer.test.C
//  (creation date: February 1, 2000).
//  
//  Author: David Bourguignon
//  
//  iMAGIS/GRAVIR, 2000
//  
// ------------------------------------------------------------

// Typedefs
typedef GLdouble Real;
typedef animal::geometry::Vec3<Real> Vec;
typedef animal::geometry::Quaternion<Real> Quat;
typedef animal::support::Trackball<Real> Trackball;



/* Data */

// Trackball
Trackball tb;

// Graphic window
GLdouble aspect;
int main_window;

// Display list
GLuint axes_list;

// Display settings
int white_background, show_ball, show_axes;

// GLUI interface
GLUI* glui;



/* Functions declaration */

// Initialization
void init(char* name);

// Reshape window callback
void reshape (int w, int h);

// Display callback
void display();

// Keyboard callback
void keyboard(unsigned char key, int x, int y);

// Mouse callback
void mouse(int button, int state, int x, int y);

// Motion callback
void motion(int x, int y);

// GLUI interface
void buildGLUIinterface(int gfx_window);

// Used by axes display list
void drawAxes();



/* Functions definition */

void init(char* name)
{
  /* initialize GLUT */
  // Set rendering window
  main_window = glutCreateWindow(name);
  glutInitWindowSize(300, 300);
  
  // Add display, reshape, keyboard, mouse, and motion callbacks
  glutReshapeFunc(reshape);
  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  
  /* initialize GLUI */
  buildGLUIinterface(main_window);
  GLUI_Master.set_glutIdleFunc(0);
  
  /* initialize OpenGL */
  glClearColor(0.0, 0.0, 0.0, 0.0);
  
  // Build display list
  axes_list = glGenLists(1);
  glNewList(axes_list, GL_COMPILE);
  drawAxes();
  glEndList();
  
  // Init trackball and others
  Quat qInit1 = Quat( Vec(1.0, 0.0, 0.0), -M_PI_4/4 );
  Quat qInit2 = Quat( Vec(0.0, 1.0, 0.0), M_PI_4/2 );
  Quat qInit  = qInit1*qInit2;
  Vec  vInit  = Vec(0.0, 0.0, -10.0);
  tb = Trackball(qInit, vInit);
  
  white_background = false;
  show_ball = false;
  show_axes = false;
}

void reshape(int w, int h)
{
  glViewport( 0, 0, static_cast<GLsizei>(w), static_cast<GLsizei>(h) );
  aspect = static_cast<GLfloat>(w) / static_cast<GLfloat>(h);
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0, aspect, 1.0e-2, 1.0e+2);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  tb.reshape(w,h);
}

void display()
{
  static GLdouble m[4][4];
  
  if ( white_background )
    glClearColor(1.0, 1.0, 1.0, 1.0);
  else
    glClearColor(0.0, 0.0, 0.0, 1.0);
  
  glClear(GL_COLOR_BUFFER_BIT);
  
  tb.writeOpenGLTransfMatrix(m);
  
  glPushMatrix();
    glMultMatrixd( &m[0][0] );
    
    // Draw display lists
    if ( white_background )
      glColor3f(0.0, 0.0, 0.0);
    else
      glColor3f(1.0, 1.0, 1.0);
    
    if ( show_ball )
      glutWireSphere(1.0, 10, 10);
    
    if ( show_axes )
      glCallList(axes_list);
  glPopMatrix();
  
  glutSwapBuffers();
  glFlush();
}

void keyboard(unsigned char key, int x, int y)
{
  switch ( key ) {
    
  case 'h':
    printf("\n");
    printf("------------------------------------------------\n");
    printf("         T r a c k b a l l  V i e w e r         \n");
    printf("------------------------------------------------\n");
    printf("\n");
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

void buildGLUIinterface(int gfx_window)
{
  glui = GLUI_Master.create_glui("GUI");
  
  // Options panel
  GLUI_Panel* options_panel = glui->add_panel("Options");
  glui->add_checkbox_to_panel(options_panel, "White background", &white_background);
  glui->add_checkbox_to_panel(options_panel, "Show ball", &show_ball);
  glui->add_checkbox_to_panel(options_panel, "Show axes", &show_axes);
  
  glui->set_main_gfx_window(gfx_window);
}

void drawAxes()
{
  glPushAttrib(GL_POLYGON_BIT);
  glPushAttrib(GL_LINE_BIT);
  
  glPolygonMode(GL_FRONT, GL_FILL);
  glLineWidth(2.0);
  
  glBegin(GL_LINES);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(1.0, 0.0, 0.0);
    
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 1.0, 0.0);
    
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 1.0);
  glEnd();
  
  glColor3f(1.0, 1.0, 0.0);
  glutSolidCube(0.075);
  
  glPushMatrix();
    glTranslatef(1.0, 0.0, 0.0);
    glColor3f(1.0, 0.0, 0.0);
    glutSolidCube(0.075);
  glPopMatrix();
  
  glPushMatrix();
    glTranslatef(0.0, 1.0, 0.0);
    glColor3f(0.0, 1.0, 0.0);
    glutSolidCube(0.075);
  glPopMatrix();
  
  glPushMatrix();
    glTranslatef(0.0, 0.0, 1.0);
    glColor3f(0.0, 0.0, 1.0);
    glutSolidCube(0.075);
  glPopMatrix();
  
  glPopAttrib();
  glPopAttrib();
}



/* Main program */

int main(int argc, char** argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB );
  init("Trackball Viewer");
  glutMainLoop();
  
  return 0;
}
