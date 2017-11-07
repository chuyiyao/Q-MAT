#include "TriMesh.h"

//update the bounding box bbox of trimesh 
void TriMesh::bound_box()
{
	Point min, max;
	min = max = vertices[0];
	for (int i = 1; i<vertices.size(); i++)
	{
		if (vertices[i][0]<min[0]) min[0] = vertices[i][0];
		else if (vertices[i][0]>max[0]) max[0] = vertices[i][0];

		if (vertices[i][1]<min[1]) min[1] = vertices[i][1];
		else if (vertices[i][1]>max[1]) max[1] = vertices[i][1];

		if (vertices[i][2]<min[2]) min[2] = vertices[i][2];
		else if (vertices[i][2]>max[2]) max[2] = vertices[i][2];

	}
	dprintf("%10.5f %10.5f %10.5f %10.5f %10.5f %10.5f ", min[0], min[1], min[2],
		max[0], max[1], max[2]);
	bbox.min = min;
	bbox.max = max;
}