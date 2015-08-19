#include <stdlib.h>
#include <fstream>
#include <vector>
#include <animal/geometry/vec3.h>

using namespace std;
using namespace animal::geometry;
typedef Vec3<double> vec3;

void error(const char* p1, const char* p2="")
{
  cerr << "Error! " << p1 << " " << p2 << endl;
  exit(1);
}

int main(int argc, char** argv)
{
  if (argc != 9)
    {
      cerr << "Usage:\tcubeMaker [width] [height] [length] "
	   << "[subdiv w] [subdiv h] [subdiv l] [file out mesh] [file out faces]" << endl;
      exit(1);
    }
  
  double width  = atof(argv[1]);
  double height = atof(argv[2]);
  double length = atof(argv[3]);
  
  int subdiv_w = atoi(argv[4]);
  int subdiv_h = atoi(argv[5]);
  int subdiv_l = atoi(argv[6]);
  
  ofstream file_out_mesh(argv[7], ios::out);
  if ( !file_out_mesh ) error("Cannot open mesh output file", argv[7]);
  
  ofstream file_out_faces(argv[8], ios::out);
  if ( !file_out_faces ) error("Cannot open faces output file", argv[8]);
  
  double unit_w = width/subdiv_w;
  double unit_h = height/subdiv_h;
  double unit_l = length/subdiv_l;
  
  int ncubes = subdiv_w*subdiv_h*subdiv_l;
  int v_w = subdiv_w + 1;
  int v_h = subdiv_h + 1;
  int v_l = subdiv_l + 1;
  int nvertices = v_w*v_h*v_l;
  
  vector<vec3> vertices;
  
  int l, h, w;
  
  for (l = 0; l < v_l; l++)
    for (h = 0; h < v_h; h++)
      for (w = 0; w < v_w; w++)
	{
	  vertices.push_back( vec3(w*unit_w, h*unit_h, l*unit_l) );
	}
  
  vector<int> v_indices;
  
  for (l = 0; l < subdiv_l; l++)
    for (h = 0; h < subdiv_h; h++)
      for (w = 0; w < subdiv_w; w++)
	{
	  v_indices.push_back(  w    +  h   *v_w +  l   *v_w*v_h );
	  v_indices.push_back( (w+1) +  h   *v_w +  l   *v_w*v_h );
	  v_indices.push_back( (w+1) + (h+1)*v_w +  l   *v_w*v_h );
	  v_indices.push_back(  w    + (h+1)*v_w +  l   *v_w*v_h );
	  
	  v_indices.push_back(  w    +  h   *v_w + (l+1)*v_w*v_h );
	  v_indices.push_back( (w+1) +  h   *v_w + (l+1)*v_w*v_h );
	  v_indices.push_back( (w+1) + (h+1)*v_w + (l+1)*v_w*v_h );
	  v_indices.push_back(  w    + (h+1)*v_w + (l+1)*v_w*v_h );
	}
  
  vector<int> v_faces;
  
  l = 0;
  for (h = 0; h < subdiv_h; h++)
    for (w = 0; w < subdiv_w; w++)
      {
	v_faces.push_back(  w    +  h   *v_w +  l   *v_w*v_h );
	v_faces.push_back( (w+1) +  h   *v_w +  l   *v_w*v_h );
	v_faces.push_back( (w+1) + (h+1)*v_w +  l   *v_w*v_h );
	v_faces.push_back(  w    + (h+1)*v_w +  l   *v_w*v_h );
      }
  
  l = subdiv_l;
  for (h = 0; h < subdiv_h; h++)
    for (w = 0; w < subdiv_w; w++)
      {
	v_faces.push_back(  w    +  h   *v_w +  l   *v_w*v_h );
	v_faces.push_back( (w+1) +  h   *v_w +  l   *v_w*v_h );
	v_faces.push_back( (w+1) + (h+1)*v_w +  l   *v_w*v_h );
	v_faces.push_back(  w    + (h+1)*v_w +  l   *v_w*v_h );
      }
  
  h = 0;
  for (l = 0; l < subdiv_l; l++)
    for (w = 0; w < subdiv_w; w++)
      {
	v_faces.push_back(  w    +  h   *v_w +  l   *v_w*v_h );
	v_faces.push_back( (w+1) +  h   *v_w +  l   *v_w*v_h );
	v_faces.push_back( (w+1) +  h   *v_w + (l+1)*v_w*v_h );
	v_faces.push_back(  w    +  h   *v_w + (l+1)*v_w*v_h );
      }
  
  h = subdiv_h;
  for (l = 0; l < subdiv_l; l++)
    for (w = 0; w < subdiv_w; w++)
      {
	v_faces.push_back(  w    +  h   *v_w +  l   *v_w*v_h );
	v_faces.push_back( (w+1) +  h   *v_w +  l   *v_w*v_h );
	v_faces.push_back( (w+1) +  h   *v_w + (l+1)*v_w*v_h );
	v_faces.push_back(  w    +  h   *v_w + (l+1)*v_w*v_h );
      }
  
  w = 0;
  for (l = 0; l < subdiv_l; l++)
    for (h = 0; h < subdiv_h; h++)
      {
	v_faces.push_back(  w    +  h   *v_w +  l   *v_w*v_h );
	v_faces.push_back(  w    + (h+1)*v_w +  l   *v_w*v_h );
	v_faces.push_back(  w    + (h+1)*v_w + (l+1)*v_w*v_h );
	v_faces.push_back(  w    +  h   *v_w + (l+1)*v_w*v_h );
      }
  
  w = subdiv_w;
  for (l = 0; l < subdiv_l; l++)
    for (h = 0; h < subdiv_h; h++)
      {
	v_faces.push_back(  w    +  h   *v_w +  l   *v_w*v_h );
	v_faces.push_back(  w    + (h+1)*v_w +  l   *v_w*v_h );
	v_faces.push_back(  w    + (h+1)*v_w + (l+1)*v_w*v_h );
	v_faces.push_back(  w    +  h   *v_w + (l+1)*v_w*v_h );
      }
  
  file_out_mesh << nvertices << " " << ncubes << endl;
  
  file_out_mesh.setf(ios::fixed,ios::floatfield);
  file_out_mesh.precision(6);
  
  int n;
  
  for (n = 0; n < nvertices; n++)
    file_out_mesh << vertices[n].x() << " "
	          << vertices[n].y() << " "
	          << vertices[n].z() << "\t0" << endl;
  
  for (n = 0; n < ncubes; n++)
    {
      for (int i = n*8; i < n*8 + 8; i++)
	file_out_mesh << v_indices[i] << "\t";
      
      file_out_mesh << endl;
    }
  
  int nfaces = 2*(subdiv_w*subdiv_h + subdiv_w*subdiv_l + subdiv_h*subdiv_l);
  
  file_out_faces << nfaces << endl;
  
  for (n = 0; n < nfaces; n++)
    {
      for (int i = n*4; i < n*4 + 4; i++)
	file_out_faces << v_faces[i] << "\t";
      
      file_out_faces << endl;
    }
  
  file_out_mesh.close();
  file_out_faces.close();
  
  return 0;
}
