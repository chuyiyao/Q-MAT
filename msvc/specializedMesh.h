#pragma once
//#include "Vec.h"

#include <vector>
#include <TriMesh.h>
#include "matrix.h"
#include <queue>
#include <set>
#define MAXVALUE 99999

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
		float radius;
		bool isvalid = true;
		VertexAttrib(): radius(1){}
		VertexAttrib(double r) : radius(r) {}

	};
	struct EdgeAttrib {
		Point c_g = Point(0.0f, 0.0f, 0.0f);
		double r_g = 0;
		double cost = 0;
		double stability = 0;
		bool isManifold = false;
	};

	int vN = 0;
	int fN = 0;
	int cN = 0;
	std::vector<Edge> edges;
	std::vector<Cone> cones;
	std::vector<VertexAttrib> vAttributes;
	std::vector<EdgeAttrib> eAttributes;
	std::vector<vec> slabNormal1;
	std::vector<vec> slabNormal2;
	std::vector<std::vector<cone_id> > adjacentcones;
	std::vector<std::vector<edge_id> > adjacentedges;
	int abnormalSlabNum = 0;
	int ignoredvalue = 0;

	struct EdgeIdCost {
		edge_id id;
		double mcost = 0;

		EdgeIdCost() {}
		EdgeIdCost(const edge_id &idx, double & c){
			id = idx;	mcost = c;
		}
	};
	struct cmp {
		bool operator ()(const EdgeIdCost& a, const EdgeIdCost& b) {
			return a.mcost > b.mcost;  // small heap
		}
	};
	std::priority_queue<EdgeIdCost, std::vector<EdgeIdCost>, cmp> prioQue;
	std::set<vertex_id> VerticesRemained;
	std::set<face_id> FacesRemained;
	std::set<cone_id> ConesRemained;
	//std::set<edge_id> EdgesRemained;  //seem useless

	// connectivity after contraction
	std::vector<Face> faces_c;
	std::vector<Cone> cones_c;
	std::vector<Edge> edges_c;
	void produce_contracted_connectivity();

	std::vector<std::set<int> > adjFaces;
	std::vector<std::set<int> > adjCones;
	std::vector<std::set<int> > adjVertices;

	vertex_id add_vertex(Point p);
	edge_id add_edge(vertex_id i, vertex_id j);
	face_id add_face(vertex_id i, vertex_id j, vertex_id k);
	cone_id add_cone(vertex_id i, vertex_id j);
	
	void need_VertexAttributes();
	void need_normalize();
	double compStability(const edge_id &e);

	MedialAxisTrans() : TriMesh() { };
	//~MedialSurface();
	bool compSlabNormal(const face_id &fid); // bool return whether the delta is not negative
	void compConeNormal(const cone_id &co, ICPL::Matrix &n1, ICPL::Matrix &n2, ICPL::Matrix &n1rot, ICPL::Matrix &n2rot);
	
	void addSlabError(ICPL::Matrix &A, ICPL::Matrix &b, ICPL::Matrix &c, const face_id &fa, const vertex_id &ve);
	void addConeError(ICPL::Matrix &A, ICPL::Matrix &b, ICPL::Matrix &c, const cone_id &co, const vertex_id &ve);
	double compContractionTarget(const edge_id &e);
	double NewlyCompContractionTarget(const edge_id &e);

	void Initialize(double k);
	//void contractEdge(edge_id e);
	void connectFace2Target(const face_id &fa, const vertex_id &v0, const vertex_id &vid, std::vector<int> &edgelist);
	void connectCone2Target(const cone_id &co, const vertex_id &v0, const vertex_id &vid, std::vector<int> &edgelist);
	int Contraction(int sigma, double k); //doing the contraction, where sigma is the maximum iterations

	//for rendering 
	//void indexVBO_V_vN(std::vector<unsigned short> &ind,std::vector<point> &outV, std::vector<vec> &outN);
	//void indexVBO_V_fN(std::vector<unsigned short> &ind,std::vector<point> &outV, std::vector<vec> &outN);
	static void read_ma(const char *filename, MedialAxisTrans &mesh);
	bool write(const char *filename);
	void projection();
};

bool solveNormalEq(const vec &p1, const vec &p2, const double &b1,
	const double &b2, vec &n1, vec &n2);

void swapComponent(point &p, int i, int j);
vec RotateHalfPi(vec &n, vec& axis);