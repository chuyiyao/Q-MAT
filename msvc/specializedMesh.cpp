#include "specializedMesh.h"
#include "stdio.h"
#include <algorithm>
#include "Vec.h"

std::ostream &operator << (std::ostream &os, const Edge &e) {
	os << "v" << e[0] << "----v" << e[1];
	return os;
}

vertex_id MedialAxisTrans::add_vertex(Point p) {
	vertices.push_back(p);
	return vertices.size() - 1;

}

edge_id MedialAxisTrans::add_edge(vertex_id i, vertex_id j) {
	if (find(neighbors[i].cbegin(), neighbors[i].cend(), j) == neighbors[i].cend()) {
		edges.push_back(Edge(i, j));
		neighbors[i].push_back(j);
		neighbors[j].push_back(i);
		return edges.size() - 1;
	}
	return -1;
}

face_id MedialAxisTrans::add_face(vertex_id i, vertex_id j, vertex_id k) {
	faces.push_back(Face(i, j, k));
	face_id fid = faces.size() - 1;
	adjacentfaces[i].push_back(fid);
	adjacentfaces[j].push_back(fid);
	adjacentfaces[k].push_back(fid);
	add_edge(i, j);
	add_edge(i, k);
	add_edge(j, k);

	return fid;
}

cone_id MedialAxisTrans::add_cone(vertex_id i, vertex_id j) {
	int index = add_edge(i, j);
	if (index == -1)
		return -1;
	cones.push_back(Cone(i, j));
	cone_id cid = cones.size() - 1;
	adjacentcones[i].push_back(cid);
	return cid;
}


//Creat the vertex attributes after adding all the vertex;
void MedialAxisTrans::need_attributes() {
	if (vertices.empty())
		return;
	int nv = vertices.size();
	attributes.resize(nv);

	if (neighbors.empty())
		neighbors.resize(nv);
	if (adjacentfaces.empty())
		adjacentfaces.resize(nv);
	if (adjacentcones.empty())
		adjacentcones.resize(nv);
}

double MedialAxisTrans::compStability(Edge &e){
	double r1 = attributes[e[0]].radius;
	double r2 = attributes[e[1]].radius;
	Point p1 = vertices[e[0]];
	Point p2 = vertices[e[1]];
	double r_res = abs(r1 - r2);
	double c_res = dist(p1, p2);
	double res = c_res - r_res;
	return (res > 0 ? res/c_res : 0);
}

void MedialAxisTrans::Initialize() {
	//minimize the SQE function of each edge to find the contraction target m_g and coresponding SQE

	// initialize the stability ratio for each edge
	int numEdge = edges.size();
	for (int i = 0; i < numEdge; i++) {
		edges[i].stability = compStability(edges[i]);
	}
	// construct the contracting edge queue according to the statbility ratio and SQE error of each edge 
}