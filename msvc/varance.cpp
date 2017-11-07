//#include "Rendering.h"
#include "specializedMesh.h"
#include<Vec.h>
#include <set>
#include <map>
typedef Vec<3, float> Point;

class Compare
{
public:
	bool operator()(const int& a, const int &b)
	{
		return a<b;
	}
};

//LARGE_INTEGER time1, time2, freq;



int main(int argc, char *argv[])
{
	MedialAxisTrans MAT;
	vertex_id v0 = MAT.add_vertex(Point(0, 0, 0));
	vertex_id v1 = MAT.add_vertex(Point(1, 0, 0));
	vertex_id v2 = MAT.add_vertex(Point(1, 1, 0));
	vertex_id v3 = MAT.add_vertex(Point(0, 1, 1));
	vertex_id v4 = MAT.add_vertex(Point(-1, 0, 0));
	vertex_id v5 = MAT.add_vertex(Point(0, 0, 1));
	vertex_id v6 = MAT.add_vertex(Point(0, -1, 0));
	vertex_id v7 = MAT.add_vertex(Point(0, 1, 0));
	MAT.need_attributes();

	face_id f = MAT.add_face(v0, v1, v2);
	MAT.add_face(v0, v2, v3);
	MAT.add_face(v0, v3, v4);
	MAT.add_face(v0, v4, v5);
	MAT.add_face(v0, v5, v6);
	MAT.add_face(v0, v2, v7);
	edge_id e = MAT.add_cone(v6, v1);

	MAT.attributes[v0].radius = 0.5;
	MAT.Initialize();

	//for (auto i = 0; i < MAT.vertices.size(); i++) {
	//	std::cout << MAT.attributes[i].radius << std::endl;
	//}
	std::cout << "v_num " << MAT.vertices.size() << "  f_num " << MAT.faces.size() <<
		"  c_num " << MAT.cones.size() << "  e_num " << MAT.edges.size() << std::endl;

	for (auto i = MAT.edges.begin(); i != MAT.edges.end(); i++)
		std::cout << *i << "  " << (*i).stability << std::endl;

	//::QueryPerformanceFrequency(&freq);
	//::QueryPerformanceCounter(&time1);


	//::QueryPerformanceCounter(&time2);
	//float iter_f = (float)(time2.QuadPart - time1.QuadPart) / freq.QuadPart;
	//std::cout<< "Voxel time:%10.5f\n" << iter_f <<std::endl;

	
	



}


