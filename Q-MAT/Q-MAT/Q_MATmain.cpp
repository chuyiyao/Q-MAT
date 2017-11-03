#include "MAT.h"

int main()
{
	Mesh m;
	// u            x
	// +------------+
	// |            |
	// |            |
	// |      f     |
	// |            |
	// |            |
	// +------------+
	// v            w
	// Add the points as vertices
	vertex_id u = m.add_vertex(K::Point_3(0, 1, 0));
	vertex_id v = m.add_vertex(K::Point_3(0, 0, 0));
	vertex_id w = m.add_vertex(K::Point_3(1, 0, 0));
	vertex_id x = m.add_vertex(K::Point_3(1, 1, 0));
	face_id f = m.add_face(u, v, w, x);

	{
		std::cout << "vertices around vertex " << v << std::endl;
		CGAL::Vertex_around_target_circulator<Mesh> vbegin(m.halfedge(v), m), done(vbegin);
		do {
			std::cout << *vbegin++ << std::endl;
		} while (vbegin != done);
	}

	{
		std::cout << "vertices around face " << f << std::endl;
		CGAL::Vertex_around_face_iterator<Mesh> vbegin, vend;
		for (boost::tie(vbegin, vend) = vertices_around_face(m.halfedge(f), m);
			vbegin != vend;
			++vbegin) {
			std::cout << *vbegin << std::endl;
		}
	}
	// or the same again, but directly with a range based loop
	BOOST_FOREACH(vertex_id vd, vertices_around_face(m.halfedge(f), m)) {
		std::cout << vd << std::endl;
	}

	return 0;
}
