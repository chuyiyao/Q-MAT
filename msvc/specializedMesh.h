#pragma once
//#include "Vec.h"

#include <vector>
#include <TriMesh.h>
#include "matrix.h"

typedef Vec<3, float> Point;
typedef int vertex_id;
typedef int edge_id;
typedef int face_id;
typedef int cone_id;

class Edge{
public:
	vertex_id v[2];

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
std::ostream &operator << (std::ostream &os, const Cone &c);

std::ostream &operator << (std::ostream &os, const Face &f);

class MedialAxisTrans:public TriMesh {
private:


public:
	struct VertexAttrib {
		double radius;

		VertexAttrib(): radius(1){}
		VertexAttrib(double r) : radius(r) {}

	};
	struct EdgeAttrib {
		Point c_g = Point(0.0f, 0.0f, 0.0f);
		double r_g = 0;
		double cost = 0;
		double stability = 0;
		bool isManifold = true;
	};

	std::vector<Edge> edges;
	std::vector<Cone> cones;

	std::vector<VertexAttrib> vAttributes;
	std::vector<EdgeAttrib> eAttributes;

	std::vector<vec> slabNormal1;
	std::vector<vec> slabNormal2;
	std::vector<std::vector<cone_id> > adjacentcones;

	vertex_id add_vertex(Point p);
	edge_id add_edge(vertex_id i, vertex_id j);
	face_id add_face(vertex_id i, vertex_id j, vertex_id k);
	cone_id add_cone(vertex_id i, vertex_id j);
	
	void need_VertexAttributes();
	double compStability(Edge &e);


	MedialAxisTrans() : TriMesh() { };
	//~MedialSurface();
	void compSlabNormal(face_id fid);
	void InitializeSlabNormal();

	void addSlabError(ICPL::Matrix &A, ICPL::Matrix &b, ICPL::Matrix &c, const face_id &fa, const vertex_id &ve);
	void addConeError(ICPL::Matrix &A, ICPL::Matrix &b, ICPL::Matrix &c, const cone_id &co);

	double minimizeError(ICPL::Matrix &A, ICPL::Matrix &b, ICPL::Matrix &c,EdgeAttrib &eAttri);
	double compContractionTarget(edge_id e);

	void Initialize();

	void contractEdge(edge_id e);
	

	//for rendering 
	//void indexVBO_V_vN(std::vector<unsigned short> &ind,std::vector<point> &outV, std::vector<vec> &outN);
	//void indexVBO_V_fN(std::vector<unsigned short> &ind,std::vector<point> &outV, std::vector<vec> &outN);

};

void solveNormalEq(const point &p1, const point &p2, const double &b1,
	const double &b2, vec &n1, vec &n2);

void swapComponent(point &p, int i, int j);
