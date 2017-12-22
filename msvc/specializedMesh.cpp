#include "specializedMesh.h"
#include <iostream>
#include <algorithm>
#include "Vec.h"
#include <TriMesh.h>

std::ostream &operator << (std::ostream &os, const Edge &e) {
	os << "v" << e[0] << "----v" << e[1];
	return os;
}
std::ostream &operator << (std::ostream &os, const Cone &c) {
	os << "c:v" << c[0] << "----v" << c[1];
	return os;
}
std::ostream &operator << (std::ostream &os, const Face &f) {
	os << "f:v" << f.v[0] << "--v" << f.v[1] << "--v" << f.v[2];
	return os;
}


vertex_id MedialAxisTrans::add_vertex(Point p) {
	vertices.push_back(p);
	VertexAttrib vA;
	vAttributes.push_back(vA);
	std::vector<int> nei;
	neighbors.push_back(nei);
	adjacentfaces.push_back(nei);
	adjacentcones.push_back(nei);
	std::set<int> nei1;
	adjVertices.push_back(nei1);
	adjFaces.push_back(nei1);
	adjCones.push_back(nei1);
	return vertices.size() - 1;

}

edge_id MedialAxisTrans::add_edge(vertex_id i, vertex_id j) {
	if (find(neighbors[i].cbegin(), neighbors[i].cend(), j) == neighbors[i].cend()) {
		edges.push_back(Edge(i, j));
		neighbors[i].push_back(j);
		neighbors[j].push_back(i);
		EdgeAttrib eA;
		eAttributes.push_back(eA);
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
	slabNormal1.push_back(vec(0, 0, 0));
	slabNormal2.push_back(vec(0, 0, 0));
	return fid;
}

cone_id MedialAxisTrans::add_cone(vertex_id i, vertex_id j) {
	int index = add_edge(i, j);
	if (index == -1)
		return -1;
	eAttributes[index].isManifold = false;
	cones.push_back(Cone(i, j));
	cone_id cid = cones.size() - 1;
	adjacentcones[i].push_back(cid);
	return cid;
}


//Creat the vertex attributes after adding all the vertex;
void MedialAxisTrans::need_VertexAttributes() {
	if (vertices.empty())
		return;
	int nv = vertices.size();
	vAttributes.resize(nv);

	if (neighbors.empty())
		neighbors.resize(nv);
	if (adjacentfaces.empty())
		adjacentfaces.resize(nv);
	if (adjacentcones.empty())
		adjacentcones.resize(nv);

}

double MedialAxisTrans::compStability(const edge_id &e){
	double r1 = vAttributes[edges[e][0]].radius;
	double r2 = vAttributes[edges[e][1]].radius;
	Point p1 = vertices[edges[e][0]];
	Point p2 = vertices[edges[e][1]];
	double r_res = abs(r1 - r2);
	double c_res = dist(p1, p2);
	double res = c_res - r_res;
	return (res > 0 ? res/c_res : 0);
}

void MedialAxisTrans::compSlabNormal(const face_id& fid) {
	vec p1 = vertices[faces[fid][0]] - vertices[faces[fid][1]];
	vec p2 = vertices[faces[fid][0]] - vertices[faces[fid][2]];
	double b1 = vAttributes[faces[fid][0]].radius - vAttributes[faces[fid][1]].radius;
	double b2 = vAttributes[faces[fid][0]].radius - vAttributes[faces[fid][2]].radius;
	double detZ = p1[0] * p2[1] - p2[0] * p1[1];
	double detY = p1[0] * p2[2] - p2[0] * p1[2];
	double detX = p1[2] * p2[1] - p2[2] * p1[1];
	if(abs(detZ) > 1e-5)
		solveNormalEq(p1, p2, b1, b2, slabNormal1[fid], slabNormal2[fid]);
	else if (abs(detY) > 1e-5) {
		swapComponent(p1, 1, 2);
		swapComponent(p2, 1, 2);
		solveNormalEq(p1, p2, b1, b2, slabNormal1[fid], slabNormal2[fid]);
		swapComponent(slabNormal1[fid], 1, 2);
		swapComponent(slabNormal2[fid], 1, 2);
	}
	else {
		swapComponent(p1, 0, 2);
		swapComponent(p2, 0, 2);
		solveNormalEq(p1, p2, b1, b2, slabNormal1[fid], slabNormal2[fid]);
		swapComponent(slabNormal1[fid], 0, 2);
		swapComponent(slabNormal2[fid], 0, 2);
	}
}
void MedialAxisTrans::compConeNormal(const cone_id& co, ICPL::Matrix & n1, ICPL::Matrix & n2,
	ICPL::Matrix & n1rot, ICPL::Matrix & n2rot) {
	vec normal1, normal2;
	vec p1 = vertices[cones[co][0]] - vertices[cones[co][1]];
	vec p2;
	if (p1[0] == 0 && p1[1] == 0)
		p2 = p1 CROSS(p1 + vec(1, 0, 0));
	else
		p2 = p1 CROSS(p1 + vec(0, 0, 1));
	double b1 = vAttributes[cones[co][0]].radius - vAttributes[cones[co][1]].radius;
	double b2 = p2 CROSS vertices[cones[co][0]] - p2 DOT vertices[cones[co][0]];
	p2 *= vAttributes[cones[co][0]].radius;
	
	double detZ = p1[0] * p2[1] - p2[0] * p1[1];
	double detY = p1[0] * p2[2] - p2[0] * p1[2];
	double detX = p1[2] * p2[1] - p2[2] * p1[1];
	if (abs(detZ) > 1e-5)
		solveNormalEq(p1, p2, b1, b2, normal1, normal2);
	else if (abs(detY) > 1e-5) {
		swapComponent(p1, 1, 2);
		swapComponent(p2, 1, 2);
		solveNormalEq(p1, p2, b1, b2, normal1, normal2);
		swapComponent(normal1, 1, 2);
		swapComponent(normal2, 1, 2);
	}
	else {
		swapComponent(p1, 0, 2);
		swapComponent(p2, 0, 2);
		solveNormalEq(p1, p2, b1, b2, normal1, normal2);
		swapComponent(normal1, 0, 2);
		swapComponent(normal2, 0, 2);
	}
	vec normal1rot = RotateHalfPi(normal1, p1);
	vec normal2rot = RotateHalfPi(normal2, p1);
	n1.val[0][0] = normal1[0]; n1.val[1][0] = normal1[1]; 
	n1.val[2][0] = normal1[2]; n1.val[3][0] = 1.0;
	n2.val[0][0] = normal2[0]; n2.val[1][0] = normal2[1];
	n2.val[2][0] = normal2[2]; n2.val[3][0] = 1.0;
	n1rot.val[0][0] = normal1rot[0]; n1rot.val[1][0] = normal1rot[1];
	n1rot.val[2][0] = normal1rot[2]; n1rot.val[3][0] = 1.0;
	n2rot.val[0][0] = normal2rot[0]; n2rot.val[1][0] = normal2rot[1];
	n2rot.val[2][0] = normal2rot[2]; n2rot.val[3][0] = 1.0;
}


void MedialAxisTrans::addSlabError(ICPL::Matrix &A, ICPL::Matrix &b, ICPL::Matrix &c, const face_id &fa, const vertex_id &ve) {
	vec4 n1(slabNormal1[fa][0], slabNormal1[fa][1], slabNormal1[fa][2], 1.0f);
	vec4 n2(slabNormal2[fa][0], slabNormal2[fa][1], slabNormal2[fa][2], 1.0f);
	ICPL::Matrix tempA(4, 4);
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			tempA.val[i][j] = n1[i] * n1[j] + n2[i] * n2[j];
			A.val[i][j] += tempA.val[i][j];
		}
	}
	ICPL::Matrix m(4, 1);
	for (int i = 0; i < 3; i++){
		m.val[i][0] = vertices[ve][i];
	}
	m.val[3][0] = vAttributes[ve].radius;
	ICPL::Matrix tempb = tempA*m;
	b += tempb*(-2);
	c += ~m * tempb;
}
void MedialAxisTrans::addConeError(ICPL::Matrix &A, ICPL::Matrix &b, ICPL::Matrix &c, const cone_id &co, const vertex_id &ve){
	ICPL::Matrix n1(4, 1), n2(4, 1), n1rot(4, 1), n2rot(4, 1);
	compConeNormal(co, n1, n2, n1rot, n2rot);
	ICPL::Matrix tempA(4, 4);
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			tempA.val[i][j] = n1.val[i][0] * n1.val[j][0] + n2.val[i][0] * n2.val[j][0];
			A.val[i][j] += tempA.val[i][j];
		}
	}
	ICPL::Matrix m(4, 1);
	for (int i = 0; i < 3; i++) {
		m.val[i][0] = vertices[ve][i];
	}
	m.val[3][0] = vAttributes[ve].radius;
	ICPL::Matrix tempb = tempA*m;
	b += tempb*(-2);
	c += ~m * tempb;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			tempA.val[i][j] = n1rot.val[i][0] * n1rot.val[j][0] + n2rot.val[i][0] * n2rot.val[j][0];
			A.val[i][j] += tempA.val[i][j];
		}
	}
	tempb = tempA*m;
	b += tempb*(-2);
	c += ~m * tempb;

}
double MedialAxisTrans::compContractionTarget(const edge_id &e) {
	ICPL::Matrix A = ICPL::Matrix::zeros(4, 4);
	ICPL::Matrix b = ICPL::Matrix::zeros(4, 1);
	ICPL::Matrix c = ICPL::Matrix::zeros(1, 1);
	double cost = 0;
	int v0 = edges[e][0], v1 = edges[e][1];
	std::vector<int> &neiF0 = adjacentfaces[v0];
	for (int i = 0; i < neiF0.size(); i++)
		addSlabError(A, b, c, neiF0[i], v0);
	std::vector<int> &neiC0 = adjacentcones[v0];
	for (int i = 0; i < neiC0.size(); i++)
		addConeError(A, b, c, neiC0[i], v0);

	std::vector<int> &neiF1 = adjacentfaces[v1];
	for (int i = 0; i < neiF1.size(); i++)
		addSlabError(A, b, c, neiF1[i], v1);
	std::vector<int> &neiC1 = adjacentcones[v1];
	for (int i = 0; i < neiC1.size(); i++)
		addConeError(A, b, c, neiC1[i], v1);

	double detA = A.det();
	ICPL::Matrix m_g = ICPL::Matrix::ones(4, 1);
	if (abs(detA) > 1e-4) {
		m_g = ICPL::Matrix::inv(A) * b * (-0.5);
		eAttributes[e].c_g[0] = m_g.val[0][0];
		eAttributes[e].c_g[1] = m_g.val[1][0];
		eAttributes[e].c_g[2] = m_g.val[2][0];
		eAttributes[e].r_g = m_g.val[3][0];
		ICPL::Matrix ec = (~m_g) * A * m_g + ~b * m_g + c;
		return ec.val[0][0];
	}
	double c0, c1, ch;
	m_g.val[0][0] = vertices[v0][0];
	m_g.val[1][0] = vertices[v0][1];
	m_g.val[2][0] = vertices[v0][2];
	m_g.val[3][0] = vAttributes[v0].radius;
	c0 = ((~m_g) * A * m_g + ~b * m_g + c).val[0][0];
	m_g.val[0][0] = vertices[v1][0];
	m_g.val[1][0] = vertices[v1][1];
	m_g.val[2][0] = vertices[v1][2];
	m_g.val[3][0] = vAttributes[v1].radius;
	c1 = ((~m_g) * A * m_g + ~b * m_g + c).val[0][0];
	m_g.val[0][0] = 0.5 * (vertices[v0][0] + vertices[v1][0]);
	m_g.val[1][0] = 0.5 * (vertices[v0][1] + vertices[v1][1]);
	m_g.val[2][0] = 0.5 * (vertices[v0][2] + vertices[v1][2]);
	m_g.val[3][0] = 0.5 * (vAttributes[v0].radius + vAttributes[v1].radius);
	ch = ((~m_g) * A * m_g + ~b * m_g + c).val[0][0];
	int minx = 0;
	if (c1 < c0)
		minx = ch < c1 ? 2 : 1;
	else
		minx = ch < c0 ? 2 : 0;
	if (minx == 0) {
		eAttributes[e].c_g = vertices[v0];
		eAttributes[e].r_g = vAttributes[v0].radius;
		return c0;
	}
	else if (minx == 1) {
		eAttributes[e].c_g = vertices[v1];
		eAttributes[e].r_g = vAttributes[v1].radius;
		return c1;
	}
	else {
		eAttributes[e].c_g = 0.5f * (vertices[v0] + vertices[v1]);
		eAttributes[e].r_g = 0.5 * (vAttributes[v0].radius + vAttributes[v1].radius);
		return ch;
	}
}
double MedialAxisTrans::NewlyCompContractionTarget(const edge_id &e) {
	ICPL::Matrix A = ICPL::Matrix::zeros(4, 4);
	ICPL::Matrix b = ICPL::Matrix::zeros(4, 1);
	ICPL::Matrix c = ICPL::Matrix::zeros(1, 1);
	double cost = 0;
	int v0 = edges[e][0], v1 = edges[e][1];
	std::set<int>::iterator ite = adjFaces[v0].begin();
	for  (; ite != adjFaces[v0].end(); ite++)
		addSlabError(A, b, c, *ite , v0);

	ite = adjCones[v0].begin();
	for (; ite != adjCones[v0].end(); ite++)
		addConeError(A, b, c, *ite, v0);

	ite = adjFaces[v1].begin();
	for (; ite != adjFaces[v1].end(); ite++)
		addSlabError(A, b, c, *ite, v1);

	ite = adjCones[v1].begin();
	for (; ite != adjCones[v1].end(); ite++)
		addConeError(A, b, c, *ite, v1);

	double detA = A.det();
	ICPL::Matrix m_g = ICPL::Matrix::ones(4, 1);
	if (abs(detA) > 1e-4) {
		m_g = ICPL::Matrix::inv(A) * b * (-0.5);
		eAttributes[e].c_g[0] = m_g.val[0][0];
		eAttributes[e].c_g[1] = m_g.val[1][0];
		eAttributes[e].c_g[2] = m_g.val[2][0];
		eAttributes[e].r_g = m_g.val[3][0];
		ICPL::Matrix ec = (~m_g) * A * m_g + ~b * m_g + c;
		return ec.val[0][0];
	}
	double c0, c1, ch;
	m_g.val[0][0] = vertices[v0][0];
	m_g.val[1][0] = vertices[v0][1];
	m_g.val[2][0] = vertices[v0][2];
	m_g.val[3][0] = vAttributes[v0].radius;
	c0 = ((~m_g) * A * m_g + ~b * m_g + c).val[0][0];
	m_g.val[0][0] = vertices[v1][0];
	m_g.val[1][0] = vertices[v1][1];
	m_g.val[2][0] = vertices[v1][2];
	m_g.val[3][0] = vAttributes[v1].radius;
	c1 = ((~m_g) * A * m_g + ~b * m_g + c).val[0][0];
	m_g.val[0][0] = 0.5 * (vertices[v0][0] + vertices[v1][0]);
	m_g.val[1][0] = 0.5 * (vertices[v0][1] + vertices[v1][1]);
	m_g.val[2][0] = 0.5 * (vertices[v0][2] + vertices[v1][2]);
	m_g.val[3][0] = 0.5 * (vAttributes[v0].radius + vAttributes[v1].radius);
	ch = ((~m_g) * A * m_g + ~b * m_g + c).val[0][0];
	int minx = 0;
	if (c1 < c0)
		minx = ch < c1 ? 2 : 1;
	else
		minx = ch < c0 ? 2 : 0;
	if (minx == 0) {
		eAttributes[e].c_g = vertices[v0];
		eAttributes[e].r_g = vAttributes[v0].radius;
		return c0;
	}
	else if (minx == 1) {
		eAttributes[e].c_g = vertices[v1];
		eAttributes[e].r_g = vAttributes[v1].radius;
		return c1;
	}
	else {
		eAttributes[e].c_g = 0.5f * (vertices[v0] + vertices[v1]);
		eAttributes[e].r_g = 0.5 * (vAttributes[v0].radius + vAttributes[v1].radius);
		return ch;
	}
}

void MedialAxisTrans::Initialize(double k) {
	// Initialize slab normal & intialized the remained id of all vertices,faces,cones
	int num = faces.size();
	for (int i = 0; i < num; i++) {
		compSlabNormal(i);
		FacesRemained.insert(i);
	}
	num = cones.size();
	for (int i = 0; i < num; i++) {
		ConesRemained.insert(i);
	}
	num = vertices.size();
	for (int i = 0; i < num; i++) {
		VerticesRemained.insert(i);
		int numm = neighbors[i].size();
		for (int j = 0; j < numm; j++)
		{
			adjVertices[i].insert(neighbors[i][j]);
		}
		numm = adjacentfaces[i].size();
		for (int j = 0; j < numm; j++)
		{
			adjFaces[i].insert(adjacentfaces[i][j]);
		}
		numm = adjacentcones[i].size();
		for (int j = 0; j < numm; j++)
		{
			adjCones[i].insert(adjacentcones[i][j]);
		}
	}
	//minimize the SQE function of each edge to find the contraction target m_g and coresponding SQE
	// initialize the stability ratio for each edge and cost
	int numEdge = edges.size();
	for (int i = 0; i < numEdge; i++) {
		eAttributes[i].stability = compStability(i);
		eAttributes[i].cost = compContractionTarget(i);
		double mc = (eAttributes[i].cost + k)*eAttributes[i].stability*eAttributes[i].stability;
		EdgeIdCost eic(i, mc);
		prioQue.push(eic);
	}
	// construct the contracting edge queue according to the statbility ratio and SQE error of each edge 
	//InitializeEdgeQueue(k);


}
void MedialAxisTrans::connectFace2Target(const face_id &fa, const vertex_id &v0, const vertex_id &vid, std::vector<int> &edgelist) {
	Face fac;
	int idx = faces[fa].indexof(v0);
	if (idx == 0) {
		fac[0] = vid;
		fac[1] = faces[fa][1]; 
		fac[2] = faces[fa][2]; 
	}
	else {
		fac[0] = faces[fa][0]; 
		if (idx == 1) {
			fac[1] = vid;
			fac[2] = faces[fa][2]; 
		}
		else {
			fac[1] = faces[fa][1]; 
			fac[2] = vid;
		}
	}
	faces.push_back(fac);
	face_id fid = faces.size() - 1;
	FacesRemained.insert(fid);
	FacesRemained.erase(fa);
	adjFaces[fac[0]].insert(fid);
	adjFaces[fac[1]].insert(fid);
	adjFaces[fac[2]].insert(fid);
	slabNormal1.push_back(vec(0, 0, 0));
	slabNormal2.push_back(vec(0, 0, 0));
	compSlabNormal(fid);

	int v1 = fac[(idx + 1) % 3], v2 = fac[(idx + 2) % 3];
	adjFaces[v1].erase(fa);
	adjFaces[v2].erase(fa);
	if (find(adjVertices[vid].begin(), adjVertices[vid].end(),v1 ) == adjVertices[vid].end()) {
		adjVertices[vid].insert(v1); 
		adjVertices[v1].insert(vid);
		edges.push_back(Edge(vid, v1));
		int eid = edges.size() - 1;
		
		EdgeAttrib eA;
		eAttributes.push_back(eA);
		eAttributes[eid].stability = compStability(eid);
		edgelist.push_back(eid);
	}
	if (find(adjVertices[vid].begin(), adjVertices[vid].end(), v2) == adjVertices[vid].end()) {
		adjVertices[vid].insert(v2);
		adjVertices[v2].insert(vid);
		edges.push_back(Edge(vid, v2));
		int eid = edges.size() - 1;

		EdgeAttrib eA;
		eAttributes.push_back(eA);
		eAttributes[eid].stability = compStability(eid);
		edgelist.push_back(eid);
	}


}
void MedialAxisTrans::connectCone2Target(const cone_id &co, const vertex_id &v0, const vertex_id &vid, std::vector<int> &edgelist) {
	Cone con;
	int idx;
	if (cones[co][0]==v0)
	{
		idx = 0;
		con[0] = vid;
		con[1] = cones[co][1];
	}
	else
	{
		idx = 1;
		con[0] = cones[co][1];
		con[1] = vid;
	}
	cones.push_back(con);
	cone_id cid = cones.size() - 1;
	ConesRemained.insert(cid);
	ConesRemained.erase(co);
	adjCones[con[0]].insert(cid);
	adjCones[con[1]].insert(cid);

	int v1 = con[(idx + 1) % 2];
	adjFaces[v1].erase(co);
	if (find(adjVertices[vid].begin(), adjVertices[vid].end(), v1) == adjVertices[vid].end()) {
		adjVertices[vid].insert(v1);
		adjVertices[v1].insert(vid);
		edges.push_back(Edge(vid, v1));
		int eid = edges.size() - 1;

		EdgeAttrib eA;
		eAttributes.push_back(eA);
		eAttributes[eid].stability = compStability(eid);
		eAttributes[eid].isManifold = false;
		edgelist.push_back(eid);
	}
}
int MedialAxisTrans::Contraction(int sigma, double k) {
	int iterations = 0;
	while (!prioQue.empty() && iterations < sigma)
	{
		EdgeIdCost eij = prioQue.top();
		prioQue.pop();
		int v0 = edges[eij.id][0];
		int v1 = edges[eij.id][1];
		if (!vAttributes[v0].isvalid || !vAttributes[v1].isvalid)
			continue;
		//if (!topologyCheck())
		//	continue;
		
		int vid = add_vertex(eAttributes[eij.id].c_g);
		vAttributes[vid].radius = eAttributes[eij.id].r_g;

		std::vector<int> edgelist;
		std::set<int>::iterator ite = adjFaces[v0].begin();
		for (; ite != adjFaces[v0].end(); ite++)
		{
			connectFace2Target(*ite, v0, vid, edgelist);
		}
		ite = adjFaces[v1].begin();
		for (; ite != adjFaces[v1].end(); ite++)
		{
			connectFace2Target(*ite, v1, vid, edgelist);
		}
		ite = adjCones[v0].begin();
		for (; ite != adjCones[v0].end(); ite++)
		{
			connectCone2Target(*ite, v0, vid, edgelist);
		}
		ite = adjCones[v1].begin();
		for (; ite != adjCones[v1].end(); ite++)
		{
			connectCone2Target(*ite, v1, vid, edgelist);
		}

		std::vector<int>::iterator itera = edgelist.begin();
		for (; itera != edgelist.end(); itera++)
		{
			eAttributes[*itera].cost = compContractionTarget(*itera);
			double mc = (eAttributes[*itera].cost + k)*eAttributes[*itera].stability*eAttributes[*itera].stability;
			EdgeIdCost eic(*itera, mc);
			prioQue.push(eic);
		}
		vAttributes[v0].isvalid = false;
		vAttributes[v1].isvalid = false;
		VerticesRemained.erase(v0);
		VerticesRemained.erase(v1);

	}
	return iterations;
}


//prepare for rendering(compute indexed vertices and corresponding normals)
bool is_near(float v1, float v2) {
	return fabs(v1 - v2) < 0.01f;
}

bool getSimilarVertexIndex(
	point & in_vertex, vec & in_normal,
	std::vector<point> & out_vertices, std::vector<vec> & out_normals,
	unsigned short & result){
	for (unsigned int i = 0; i<out_vertices.size(); i++) {
		if (
			is_near(in_vertex[0], out_vertices[i][0]) &&
			is_near(in_vertex[1], out_vertices[i][1]) &&
			is_near(in_vertex[2], out_vertices[i][2]) &&
			is_near(in_normal[0], out_normals[i][0]) &&
			is_near(in_normal[1], out_normals[i][1]) &&
			is_near(in_normal[2], out_normals[i][2])
			) {
			result = i;
			return true;
		}
	}
	return false;
}

//void MedialAxisTrans::indexVBO_V_vN(std::vector<unsigned short> &ind,
//	std::vector<point> &outV, std::vector<vec> &outN) {
//
//	
//
//	for (unsigned int i = 0; i < faces.size(); i++)
//		for (unsigned int j = 0; j < 3; j++) {
//		
//		// Try to find a similar vertex in out_XXXX
//		unsigned short index;
//		vec normal = 
//		bool found = getSimilarVertexIndex(vertices[faces[i].v[j]], in_normals[i], out_vertices, out_uvs, out_normals, index);
//
//		if (found) { // A similar vertex is already in the VBO, use it instead !
//			out_indices.push_back(index);
//		}
//		else { // If not, it needs to be added in the output data.
//			out_vertices.push_back(in_vertices[i]);
//			out_uvs.push_back(in_uvs[i]);
//			out_normals.push_back(in_normals[i]);
//			out_indices.push_back((unsigned short)out_vertices.size() - 1);
//		}
//	}
//}



void solveNormalEq(const vec &p1, const vec &p2, const double &b1,
	const double &b2, vec &n1, vec &n2) {
	point p = p1 CROSS p2;
	double xb = p1[0] * b2 - p2[0] * b1, yb = p1[1] * b2 - p2[1] * b1;
	float A = len2(p);
	float B = 2 * (yb*p[0] - xb*p[1]);
	float C = xb*xb + yb*yb - p[2] * p[2];
	float delta = B*B - 4 * A*C;
	if (delta < 0) perror("Wrong delta!\n");
	n1[2] = (-B + sqrt(delta)) / 2 / A;
	n1[0] = (yb + p[0] * n1[2]) / p[2];
	n1[1] = (p[1] * n1[2] - xb) / p[2];

	n2[2] = (-B - sqrt(delta)) / 2 / A;
	n2[0] = (yb + p[0] * n2[2]) / p[2];
	n2[1] = (p[1] * n2[2] - xb) / p[2];
}

void swapComponent(point &p, int i, int j) {
	float temp = p[i];
	p[i] = p[j];
	p[j] = temp;
}

vec RotateHalfPi(vec &n, vec& axis) {
	// inverse axis first
	vec tempk = (-(axis DOT n) / len(axis) ) * axis;
	vec after = tempk + ((tempk - n) CROSS tempk) / len(tempk);

	return after;
}