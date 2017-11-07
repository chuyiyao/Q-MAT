#pragma once
//#include "Vec.h"

#include <vector>
#include <TriMesh.h>

typedef Vec<3, float> Point;
typedef int vertex_id;
typedef int edge_id;
typedef int face_id;
typedef int cone_id;

class Edge{
public:
	vertex_id v[2];
	Point c_g = Point(0.0f, 0.0f, 0.0f);
	double r_g = 0;
	double cost = 0;
	double stability = 0;

	Edge(){}
	Edge(const int &v0, const int &v1)
	{ v[0] = v0; v[1] = v1; }
	int &operator[] (int i) { return v[i]; }
	const int &operator[] (int i) const { return v[i]; }
};
std::ostream &operator << (std::ostream &os, const Edge &e);

class Cone{
public:
	vertex_id v[2];

	Cone(){}
	Cone(const int &v0, const int &v1)
	{ v[0] = v0; v[1] = v1; }
	int &operator[] (int i) { return v[i]; }
	const int &operator[] (int i) const { return v[i]; }
};

class MedialAxisTrans:public TriMesh {
private:


public:
	struct VertexAttrib {
		double radius;
		std::vector<vec> n1;
		std::vector<vec> n2;

		VertexAttrib(): radius(0){}
		VertexAttrib(double r) : radius(r) {}

	};

	std::vector<Edge> edges;
	std::vector<Cone> cones;

	std::vector<VertexAttrib> attributes;
	std::vector<std::vector<cone_id> > adjacentcones;

	vertex_id add_vertex(Point p);
	edge_id add_edge(vertex_id i, vertex_id j);
	face_id add_face(vertex_id i, vertex_id j, vertex_id k);
	cone_id add_cone(vertex_id i, vertex_id j);
	
	void need_attributes();
	double compStability(Edge &e);


	MedialAxisTrans() : TriMesh() { };
	//~MedialSurface();
	void Initialize();

};



