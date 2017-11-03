#pragma once
#ifndef MAT_H
#define MAT_H
#include <vector>
#include <boost/foreach.hpp>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>

typedef CGAL::Simple_cartesian<double> K;
typedef CGAL::Surface_mesh<K::Point_3> Mesh;
typedef Mesh::Vertex_index vertex_id;
typedef Mesh::Face_index face_id;
typedef Mesh::Edge_index edge_id;

class MedialAxisTrans {
private:
	Mesh M;

public:
	MedialAxisTrans();
	~MedialAxisTrans();


};
#endif // MY_POINTC2_H