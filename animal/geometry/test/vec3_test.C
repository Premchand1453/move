#include <animal/geometry/vec3.h>

using namespace std;

// --------------------------------------------
//  
//  vec3.test
//  Test of the Vec3 class.
//  
//  File: animal/geometry/examples/vec3.test.C
//  (creation date: December 6, 1999).
//  
//  Author: David Bourguignon
//  
//  iMAGIS/GRAVIR, 1999
//  
// --------------------------------------------

typedef animal::geometry::Vec3<double> Vec3;

int main()
{
  cout << endl;
  cout << "---------------------------------------------" << endl;
  cout << " T E S T   O F   T H E   V E C 3   C L A S S " << endl;
  cout << "---------------------------------------------" << endl;
  cout << "# Size of Vec3: " << sizeof(Vec3) << " bytes" << endl;
  
  //  Constructors
  Vec3 A;
  cout << "# Create  A un-initialized" << endl;
  cout << A << endl;
  
  cout << "# Initialize A to (10.2, -1.0, 2.5)" << endl;
  A = Vec3(10.2, -1.0, 2.5);
  cout << A << endl;
  
  cout << "# Create B initialized to (1.1, 2.1, 1.1)" << endl;
  Vec3 B(1.1, 2.1, 1.1);
  cout << B << endl;
  
  cout << "# Create C initialized to (0.1, 0.1, 0.1)" << endl;
  Vec3 C(0.1, 0.1, 0.1);
  cout << C << endl;
  
  cout << "# Create Vector CB initialized to B - C => CB (1.0, 2.0, 1.0) " << endl;
  Vec3 CB(B - C);
  cout << CB << endl;
  
  //  Methods
  cout << "# Get (CB.x, CB.y, CB.z) values: two ways" << endl;
  cout << CB.x() << " " << CB.y() << " " << CB.z() << endl;
  cout << CB[0] << " " << CB[1] << " " << CB[2] << endl;
  
  cout << "# Set A.x to -1.0 => A (-1.0, -1.0, 2.5): two ways" << endl;
  A.setx(-1.0); cout << A << endl;
  A[0] = -1.0; cout << A << endl;
  
  cout << "# Set A.y to 20.0 => A (-1.0, 20.0, 2.5): two ways" << endl;
  A.sety(20.0); cout << A << endl;
  A[1] = 20.0; cout << A << endl;
  
  cout << "# Set A.z to 2.3 => A (-1.0, 20.0, 2.3): two ways" << endl;
  A.setz(2.3); cout << A << endl;
  A[2] = 2.3; cout << A << endl;
  
  cout << "# B += (0.1, 10.5, -93.0) => B (1.2, 12.6, -91.9)" << endl;
  B += Vec3(0.1, 10.5, -93.0);
  cout << B << endl;
  
  cout << "# B -= (0.1, 10.5, -93.0) => B (1.1, 2.1, 1.1)" << endl;
  B -= Vec3(0.1, 10.5, -93.0);
  cout << B << endl;
  
  cout << "# B *= -2 => B (-2.2, -4.2, -2.2)" << endl;
  B *= -2;
  cout << B << endl;
  
  cout << "# B /= -2 => B (1.1, 2.1, 1.1)" << endl;
  B /= -2;
  cout << B << endl;
  
  cout << "# D = B + C => D (1.2, 2.2, 1.2)" << endl;
  Vec3 D = B + C;
  cout << D << endl;
  
  cout << "# D = B - C => D (1.0, 2.0, 1.0)" << endl;
  D = B - C;
  cout << D << endl;
  
  cout << "# B * -3.2 => (-3.52, -6.72, -3.52)" << endl;
  cout << B*-3.2 << endl;
  
  cout << "# -3.2 * B => (-3.52, -6.72, -3.52)" << endl;
  cout << -3.2*B << endl;
  
  cout << "# B/-3.2 => (-0.34375, -0.65625, -0.34375)" << endl;
  cout << B/-3.2 << endl;
  
  cout << "# -B => (-1.1, -2.1, -1.1)" << endl;
  cout << -B << endl;
  
  cout << "# (B == B) => true" << endl;
  cout << ((B == B) ? 1 : 0) << endl;
  
  cout << "# (B == C) => false" << endl;
  cout << ((B == C) ? 1 : 0) << endl;
  
  cout << "# (B != C) => true" << endl;
  cout << ((B != C) ? 1 : 0) << endl;
  
  cout << "# (B != B) => false" << endl;
  cout << ((B != B) ? 1 : 0) << endl;
  
  cout << "# Operator << checked before" << endl;
  
  cout << "# Operator >>: please set A.x A.y A.z to -1.1 2.0 1.1" << endl;
  cin >> A;
  cout << "A = " << A << endl;
  
  cout << "# Norm CB " << CB << " = 2.4494897" << endl;
  cout << CB.norm() << endl;
  
  cout << "# Square norm CB " << CB << " = 6.0" << endl;
  cout << CB.sqnorm() << endl;
  
  cout << "# Infinite norm CB " << CB << " = 2.0" << endl;
  cout << CB.inftNorm() << endl;
  
  cout << "# Square norm normalized CB (0.40824829, 0.81649658, 0.40824829) = 1.0" << endl;
  CB.normalize();
  cout << "(" << CB << ") " << CB.sqnorm() << endl;
  
  cout << "# null (0.0, 0.0, 0.0)" << endl;
  cout << Vec3::null() << endl;
  
  //  Class-related methods
  cout << "# dot(A,B) = 4.2" << endl;
  cout << animal::geometry::dot(A,B) << endl;
  
  cout << "# cross(A,B) = (-0.11, 2.42, -4.51)" << endl;
  cout << animal::geometry::cross(A,B) << endl;
  
  cout << "# Distance between A and B = 2.2022716" << endl;
  cout << animal::geometry::dist(A,B) << endl;
  
  cout << "# Square distance between A and B = 4.85" << endl;
  cout << animal::geometry::sqdist(A,B) << endl;
  
  cout << "# Cosinus of (A,B) angle = 0.63426598" << endl;
  cout << animal::geometry::cosAng(A,B) << endl;
  
  return 0;
}
