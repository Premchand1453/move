#include <animal/geometry/quaternion.h>

using namespace std;

// ===============================================
// start first variant: use standard ANIMAL types.

typedef double Real;
typedef animal::geometry::Vec3<Real> Vec;
typedef animal::geometry::Quaternion<Real> Quat;

// end first variant.



// ===============================================
// start second variant: use other types
/*
class MyVec
{

public:
  
  MyVec(double x=0.0, double y=0.0, double z=0.0)
    { a[0]=x; a[1]=y; a[2]=z; }
  
  double operator[](int i) const { return a[i]; }
  double& operator[](int i) { return a[i]; }
  
  MyVec operator-(const MyVec& m) const
    { return MyVec(a[0]-m.a[0], a[1]-m.a[1], a[2]-m.a[2]); }
  
  friend ostream& operator<<(ostream& str, const MyVec& v)
    { str<<v.a[0]<<" "<<v.a[1]<<" "<<v.a[2]; return str; }
  
  friend istream& operator>>(istream& str, MyVec& v)
    { str>>v.a[0]>>v.a[1]>>v.a[2]; return str; }
  
private:
  
  double a[3];
  
};



class My_Quat_Traits
{

public:
  
  typedef double Real_t;
  typedef MyVec Vec3_t;
  
  static Real_t x(const Vec3_t& v) { return v[0]; }
  static Real_t y(const Vec3_t& v) { return v[1]; }
  static Real_t z(const Vec3_t& v) { return v[2]; }
  
  static void normalize(Vec3_t& v)
    {
      Real_t nrm = sqrt( v[0]*v[0] + v[1]*v[1] + v[2]*v[2] );
      Real_t tmp = 1.0/nrm;
      v[0]=v[0]*tmp; v[1]=v[1]*tmp; v[2]=v[2]*tmp;
    }
  
};



typedef double Real;
typedef MyVec Vec;
typedef animal::geometry::Quaternion<Real,animal::traits::Numerics<Real>,My_Quat_Traits> Quat;
*/
// end second variant



// ===============================================
// main program.

int main()
{
  cout<<"\n============================================================\n";
  cout<<"Test program for class Quaternion"<<endl;
  cout<<"Quaternion is a unit quaternion used to represent a rotation"<<endl;
  cout<<"============================================================\n"<<endl;
  
  // enter data
  cout<<"\n==== enter a quaternion q in the form: axis angle\n(example: 1.0 1.0 1.0 1.5708)"
      <<endl;
  Quat q;
  cin>>q;
  cout<<"unit quaternion q: "<<q<<endl;
  cout<<"\n==== enter a quaternion q1 in the form: axis angle\n(example: 1.0 1.0 1.0 0.5708)"
      <<endl;
  Quat q1;
  cin>>q1;
  cout<<"unit quaternion q1: "<<q1<<endl;
  cout<<"\n==== enter a vector v\n(example 1.0 0.0 0.0)"<<endl;
  Vec v;
  cin>>v;
  
  // axis, angle
  cout<<"\n==== get q axis and angle"<<endl;
  Real x,y,z;
  Real angle;
  q.getAxisAngle(x,y,z, angle);
  Vec axis(x,y,z);
  cout<<"axis: "<<axis<<endl;
  cout<<"angle: "<<angle<<endl;
  
  q.getAxisAngle(axis, angle);
  cout<<"axis (second version): "<<axis<<endl;
  cout<<"angle (second version): "<<angle<<endl;
  
  cout<<"\n==== set q axis and angle"<<endl;
  q.setAxisAngle(axis, angle);
  Vec ax; Real an;
  q.getAxisAngle(ax, an);
  cout<<"axis: "<<ax<<endl;
  cout<<"angle: "<<an<<endl;
  cout<<"difference (correct value is 0 0 0 0): "<<axis-ax<<" "<<angle-an<<endl;
  
  q.setAxisAngle(x,y,z, angle);
  q.getAxisAngle(ax, an);
  cout<<"axis(second version): "<<ax<<endl;
  cout<<"angle(second version): "<<an<<endl;
  cout<<"difference (correct value is 0 0 0 0): "<<axis-ax<<" "<<angle-an<<endl;
  
  cout<<"q.setw(q.w()).setx(q.x()).sety(q.y()).setz(q.z()):"<<endl;
  Quat qq;
  qq.setw(q.w()).setx(q.x()).sety(q.y()).setz(q.z());
  cout<<"before: "<<q<<endl;
  cout<<"after:  "<<qq<<endl;
  cout<<"(must be equal)."<<endl;
  
  // Norm
  qq.normalize();
  cout<<"\n==== check normalization (correct value is 0.0): "
      <<qq.norm() - 1.0<<endl;
  
  // various rotation models
  cout<<"\n==== set q with Euler angles: to be tested in the future!..."<<endl;
  
  // product to another quaternion: q *= q1, q /= q1
  cout<<"\n==== test products *= and divisions /="<<endl;
  cout<<"(q *= q1) = "<<(q *= q1)<<endl;
  cout<<"(q /= q1) = "<<(q /= q1)<<endl;
  qq = q;
  cout<<"q /= q (correct value is 1 0 0 0): "<<(qq /= qq)<<endl;
  
  // product to another quaternion with creation q*q1, q/q1
  cout<<"\n==== test products * and divisions /"<<endl;
  cout<<"q       = "<<q<<endl;
  cout<<"q*q1    = "<<q*q1<<endl;
  cout<<"q*q1/q1 = "<<q*q1/q1<<endl;
  cout<<"q*q1/q1/q (correct value is 1 0 0 0) = "<<q*q1/q1/q<<endl;
  
  // product with its inverse
  qq = q;
  cout<<"\n==== product of q with its inverse (correct value is 1 0 0 0): "<<(qq*qq.inv())<<endl;
  
  // product with a vector
  cout<<"\n==== product with q and v:"<<endl;
  cout<<"q*v = "<<q*v<<endl;
  cout<<"v*q = "<<v*q<<endl;
  cout<<"(v*q)/q - v (correct value is 0 0 0) = "<<(v*q)/q - v<<endl;
  cout<<"q/v = "<<q/v<<endl;
  cout<<"v/q = "<<v/q<<endl;
  cout<<"(v/q)*q - v (correct value is 0 0 0) = "<<(v/q)*q - v<<endl;
  
  // write rotation matrix
  cout<<"\n==== write rotation matrix m corresponding to quaternion q"<<endl;
  Real mat[3][3];
  animal::geometry::writeRotMatrix(q,mat);
  Vec u;
  int i, j;
  
  for (i=0; i<3; i++)
    for (j=0; j<3; j++)
      u[i] += mat[j][i]*v[j]; // Right-multiply m!
  
  cout<<"product v*q = "<<v*q<<endl;
  cout<<"product v*m = "<<u<<endl;
  cout<<"Rotation matrix must be right-multiplied!"<<endl;
  
  // write OpenGL rotation matrix
  cout<<"\n==== write OpenGL rotation matrix m corresponding to quaternion q"<<endl;
  Real matrix[4][4];
  animal::geometry::writeOpenGLRotMatrix(q,matrix);
  u[0] = 0.0; u[1] = 0.0; u[2] = 0.0;
  
  for (i=0; i<3; i++)
    for (j=0; j<3; j++)
      u[i] += matrix[i][j]*v[j]; // Right-multiply m + OpenGL convention!
  
  cout<<"product v*q = "<<v*q<<endl;
  cout<<"product v*m = "<<u<<endl;
  cout<<"Rotation matrix must be right-multiplied!"<<endl;
  
  return 0;
}
