#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "TriMesh.h"
#include "XForm.h"
#include "GLCamera.h"
#include "ICP.h"
#include "strutil.h"
#include <GL/glut.h>
#include <string>
#include "volume_data.h"
#include "dist_map.h"
#include "Vec.h"
using namespace std;


// Globals
vector<TriMesh *> meshes;
vector<VolumeData *> vds;
vector<DistMap *> dms;
vector<Point3i> ll;
vector<xform> xforms;
vector<bool> visible;
vector<string> filenames;
vector<Point3i> levelList;
typedef Vec<3, float> Point3d;
TriMesh::BSphere global_bsph;
xform global_xf;
GLCamera camera;

int current_mesh = -1;

bool draw_edges = false;
bool draw_points = false;
bool draw_2side = false;
bool draw_shiny = true;
bool draw_lit = true;
bool draw_falsecolor = false;
bool draw_index = false;
bool white_bg = false;
bool grab_only = false;
bool wireframe = false;

int point_size = 1, line_width = 1;


// Signal a redraw
void need_redraw()
{
	glutPostRedisplay();
}


// Clear the screen
void cls()
{
	glDisable(GL_DITHER);
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_NORMALIZE);
	glDisable(GL_LIGHTING);
	glDisable(GL_NORMALIZE);
	glDisable(GL_COLOR_MATERIAL);
	if (white_bg)
		glClearColor(1, 1, 1, 0);
	else
		glClearColor(0.8f, 0.8f, 0.8f, 0);
	glClearDepth(1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}


// Set up lights and materials
void setup_lighting(int id)
{
	Color c(1.0f);
	glColor3fv(c);

	GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_position[] = { 0.0, 0.0, 1.0, 0.0 };

	GLfloat mat_ambient[] = { 0.7, 0.7, 0.7, 0.3 };
	GLfloat mat_diffuse[] = { 0.2, 0.8, 0.8, 0.3 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 0.3 };
	GLfloat high_shininess[] = {90.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	//glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, high_shininess);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	//glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);



}

// Draw triangle strips.  They are stored as length followed by values.
void draw_tstrips(const TriMesh *themesh)
{
	static bool use_glArrayElement = false;
	static bool tested_renderer = false;
	if (!tested_renderer) {
		use_glArrayElement = !!strstr(
			(const char *)glGetString(GL_RENDERER), "Intel");
		tested_renderer = true;
	}

	const int *t = &themesh->tstrips[0];
	const int *end = t + themesh->tstrips.size();
	if (use_glArrayElement) {
		while (likely(t < end)) {
			glBegin(GL_TRIANGLE_STRIP);
			int striplen = *t++;
			for (int i = 0; i < striplen; i++)
				glArrayElement(*t++);
			glEnd();
		}
	}
	else {
		while (likely(t < end)) {
			int striplen = *t++;
			glDrawElements(GL_TRIANGLE_STRIP, striplen, GL_UNSIGNED_INT, t);
			t += striplen;
		}
	}
	/*static GLfloat sphere_mat[] = {0.0f, 1.0f, 1.0f, .5f};
	int index_p0,index_p1,index_p2;
	//glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, sphere_mat);
	for(int i=0;i<themesh->faces.size();i++)
	//for(int i=0;i<2000;i++)
	{
	index_p0=themesh->faces[i][0];
	index_p1=themesh->faces[i][1];
	index_p2=themesh->faces[i][2];
	glBegin(GL_TRIANGLES );
	glNormal3fv(themesh->normals[index_p0]);
	glVertex3fv(themesh->vertices[index_p0]);
	glNormal3fv(themesh->normals[index_p1]);
	glVertex3fv(themesh->vertices[index_p1]);
	glNormal3fv(themesh->normals[index_p2]);
	glVertex3fv(themesh->vertices[index_p2]);
	glEnd();
	}*/


}
void draw_cube(int i, int j, int k, VolumeData *vd)
{
	Point3d min = vd->min;
	double unitVoxelSize = vd->unitVoxelSize;
	min[0] += i*unitVoxelSize;
	min[1] += j*unitVoxelSize;
	min[2] += k*unitVoxelSize;
	//printf("%lf %10.5f %10.5f %10.5f\n",unitVoxelSize,min[0],min[1],min[2]);
	GLfloat CubeVertices[8][3] =
	{ { min[0],min[1],min[2] + unitVoxelSize },
	{ min[0] + unitVoxelSize,min[1], min[2] + unitVoxelSize },
	{ min[0] + unitVoxelSize,min[1], min[2] },
	{ min[0],min[1],min[2] },
	{ min[0],min[1] + unitVoxelSize,min[2] + unitVoxelSize },
	{ min[0] + unitVoxelSize,min[1] + unitVoxelSize,min[2] + unitVoxelSize },
	{ min[0] + unitVoxelSize,min[1] + unitVoxelSize,min[2] },
	{ min[0],min[1] + unitVoxelSize,min[2] }
	};
	GLfloat CubeNormals[24][3] =
	{
		{ 0, -1.0, 0 },
		{ 0, -1.0, 0 },
		{ 0, -1.0, 0 },
		{ 0, -1.0, 0 },

		{ 0, 1.0, 0 },
		{ 0, 1.0, 0 },
		{ 0, 1.0, 0 },
		{ 0, 1.0, 0 },

		{ 1.0, 0.0, 0 },
		{ 1.0, 0.0, 0 },
		{ 1.0, 0.0, 0 },
		{ 1.0, 0.0, 0 },

		{ -1.0, 0.0, 0 },
		{ -1.0, 0.0, 0 },
		{ -1.0, 0.0, 0 },
		{ -1.0, 0.0, 0 },

		{ 0, 0.0, -1.0 },
		{ 0, 0.0, -1.0 },
		{ 0, 0.0, -1.0 },
		{ 0, 0.0, -1.0 },

		{ 0, 0.0, 1.0 },
		{ 0, 0.0, 1.0 },
		{ 0, 0.0, 1.0 },
		{ 0, 0.0, 1.0 }
	};
	GLuint CubeIndices[6][4] =
	{
		{ 0, 3, 2, 1 },
		{ 4, 5, 6, 7 },
		{ 1, 2, 6, 5 },
		{ 0, 4, 7, 3 },
		{ 2, 3, 7, 6 },
		{ 0, 1, 5, 4 }
	};
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_FLOAT, 0, CubeNormals);
	glVertexPointer(3, GL_FLOAT, 0, CubeVertices);
	glDrawElements(GL_QUADS, 24, GL_UNSIGNED_INT, CubeIndices);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
}
void draw_voxel(VolumeData *vd)
{
	int rows, cols, heis;
	rows = vd->rows;
	cols = vd->cols;
	heis = vd->heis;

	for (int k = 0; k<heis; k++)
		for (int i = 0; i<rows; i++)
			for (int j = 0; j<cols; j++)
				if (vd->at(i, j, k) == 1)
					draw_cube(i, j, k, vd);
}
void draw_sphere(DistMap *dm, VolumeData *vd)
{
	double unitVoxelSize = vd->unitVoxelSize;

	int rows, cols, heis;
	rows = dm->rows;
	cols = dm->cols;
	heis = dm->heis;

	/*for(int i=0;i<rows;i++)
	for(int j=0;j<cols;j++)
	for(int k=0;k<heis;k++)
	{
	if(vd->at(i,j,k)==1)
	{
	Point3d min=vd->min;
	min[0]+=i*unitVoxelSize;
	min[1]+=j*unitVoxelSize;
	min[2]+=k*unitVoxelSize;
	float radius=sqrt((float)dm->at_dist(i,j,k));
	radius*=unitVoxelSize;
	glPushMatrix();
	glTranslatef(min[0]+unitVoxelSize/2,min[1]+unitVoxelSize/2,min[2]+unitVoxelSize/2);
	glutSolidSphere( radius, 15, 15 );
	glPopMatrix();

	}
	}*/
	for (int i = 0; i<levelList.size(); i++)
	{
		Point3d min = vd->min;
		Point3d pnt = (Point3d)levelList.at(i);
		min[0] += pnt[0] * unitVoxelSize;
		min[1] += pnt[1] * unitVoxelSize;
		min[2] += pnt[2] * unitVoxelSize;
		float radius = sqrt((float)dm->at_dist(pnt[0], pnt[1], pnt[2]))*1.1 + 2.0;
		radius *= unitVoxelSize;
		glPushMatrix();
		glTranslatef(min[0] + unitVoxelSize / 2, min[1] + unitVoxelSize / 2, min[2] + unitVoxelSize / 2);
		glutSolidSphere(radius, 15, 15);
		glPopMatrix();
	}

}
void draw_skeleton()
{
	Point3i pt;
	for (int i = 0; i<levelList.size(); i++)
	{
		pt = levelList.at(i);
		draw_cube(pt[0], pt[1], pt[2], vds[0]);
	}
}
// Draw the mesh
void draw_mesh(int i)
{
	const TriMesh *themesh = meshes[i];

	glPushMatrix();
	glMultMatrixd(xforms[i]);

	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);

	if (draw_2side) {
		glDisable(GL_CULL_FACE);
	}
	else {
		glCullFace(GL_BACK);
		glEnable(GL_CULL_FACE);
	}
	// Vertices
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT,
		sizeof(themesh->vertices[0]),
		&themesh->vertices[0][0]);

	// Normals
	if (!themesh->normals.empty() && !draw_index) {
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT,
			sizeof(themesh->normals[0]),
			&themesh->normals[0][0]);
	}
	else {
		glDisableClientState(GL_NORMAL_ARRAY);
	}

	// Colors
	if (!themesh->colors.empty() && !draw_falsecolor && !draw_index) {
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(3, GL_FLOAT,
			sizeof(themesh->colors[0]),
			&themesh->colors[0][0]);
	}
	else {
		glDisableClientState(GL_COLOR_ARRAY);
	}

	// Main drawing pass
	if (draw_points || themesh->tstrips.empty()) {
		// No triangles - draw as points
		glPointSize(point_size);
		glDrawArrays(GL_POINTS, 0, themesh->vertices.size());
		glPopMatrix();
		return;
	}

	if (draw_edges) {
		glPolygonOffset(10.0f, 10.0f);
		glEnable(GL_POLYGON_OFFSET_FILL);
	}

	if (wireframe)							//if Wireframe is checked
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //draw wireframe
	else										//else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //draw filled polygons
												   //glEnable(GL_CULL_FACE);

	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	draw_tstrips(themesh);
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
	glDisable(GL_CULL_FACE);
	draw_skeleton();
	//glDisable(GL_CULL_FACE);
	//draw_voxel(vds[i]);
	//draw_sphere(dms[i],vds[i]);

	glPopMatrix();
}


// Draw the scene
void redraw()
{
	timestamp t = now();
	camera.setupGL(global_xf * global_bsph.center, global_bsph.r);

	glLoadIdentity();
	glPushMatrix();
	glMultMatrixd(global_xf);
	cls();
	for (int i = 0; i < meshes.size(); i++) {
		if (!visible[i])
			continue;
		setup_lighting(i);

		draw_mesh(i);
	}

	glPopMatrix();
	glutSwapBuffers();
	if (grab_only) {
		void dump_image();
		dump_image();
		exit(0);
	}
	printf("\r                        \r%.1f msec.", 1000.0f * (now() - t));
	fflush(stdout);
}


// Update global bounding sphere.
void update_bsph()
{
	point boxmin(1e38f, 1e38f, 1e38f);
	point boxmax(-1e38f, -1e38f, -1e38f);
	bool some_vis = false;
	for (int i = 0; i < meshes.size(); i++) {
		if (!visible[i])
			continue;
		some_vis = true;
		point c = xforms[i] * meshes[i]->bsphere.center;
		float r = meshes[i]->bsphere.r;
		for (int j = 0; j < 3; j++) {
			boxmin[j] = min(boxmin[j], c[j] - r);
			boxmax[j] = max(boxmax[j], c[j] + r);
		}
	}
	if (!some_vis)
		return;
	point &gc = global_bsph.center;
	float &gr = global_bsph.r;
	gc = 0.5f * (boxmin + boxmax);
	gr = 0.0f;
	for (int i = 0; i < meshes.size(); i++) {
		if (!visible[i])
			continue;
		point c = xforms[i] * meshes[i]->bsphere.center;
		float r = meshes[i]->bsphere.r;
		gr = max(gr, dist(c, gc) + r);
	}
}


// Set the view...
void resetview()
{
	camera.stopspin();

	// Reload mesh xforms
	for (int i = 0; i < meshes.size(); i++)
		if (!xforms[i].read(xfname(filenames[i])))
			xforms[i] = xform();

	update_bsph();

	// Set camera to first ".camxf" if we have it...
	for (int i = 0; i < filenames.size(); i++) {
		if (global_xf.read(replace_ext(filenames[i], "camxf")))
			return;
	}

	// else default view
	global_xf = xform::trans(0, 0, -5.0f * global_bsph.r) *
		xform::trans(-global_bsph.center);
}


// Make some mesh current
void set_current(int i)
{
	camera.stopspin();
	if (i >= 0 && i < meshes.size() && visible[i])
		current_mesh = i;
	else
		current_mesh = -1;
	need_redraw();
}


// Change visiblility of a mesh
void toggle_vis(int i)
{
	if (i >= 0 && i < meshes.size())
		visible[i] = !visible[i];
	if (current_mesh == i && !visible[i])
		set_current(-1);
	update_bsph();
	need_redraw();
}


// Save the current image to a PPM file.
// Uses the next available filename matching filenamepattern
void dump_image()
{
	// Find first non-used filename
	const char filenamepattern[] = "img%d.ppm";
	int imgnum = 0;
	FILE *f;
	while (1) {
		char filename[1024];
		sprintf(filename, filenamepattern, imgnum++);
		f = fopen(filename, "rb");
		if (!f) {
			f = fopen(filename, "wb");
			printf("\n\nSaving image %s... ", filename);
			fflush(stdout);
			break;
		}
		fclose(f);
	}

	// Read pixels
	GLint V[4];
	glGetIntegerv(GL_VIEWPORT, V);
	GLint width = V[2], height = V[3];
	char *buf = new char[width*height * 3];
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels(V[0], V[1], width, height, GL_RGB, GL_UNSIGNED_BYTE, buf);

	// Flip top-to-bottom
	for (int i = 0; i < height / 2; i++) {
		char *row1 = buf + 3 * width * i;
		char *row2 = buf + 3 * width * (height - 1 - i);
		for (int j = 0; j < 3 * width; j++)
			swap(row1[j], row2[j]);
	}

	// Write out file
	fprintf(f, "P6\n#\n%d %d\n255\n", width, height);
	fwrite(buf, width*height * 3, 1, f);
	fclose(f);
	delete[] buf;

	printf("Done.\n\n");
}


// Save scan transforms
void save_xforms()
{
	for (int i = 0; i < xforms.size(); i++) {
		string xffile = xfname(filenames[i]);
		printf("Writing %s\n", xffile.c_str());
		xforms[i].write(xffile);
	}
}


// Save camera xform
void save_cam_xform()
{
	std::string camfile = replace_ext(filenames[0], "camxf");
	printf("Writing %s\n", camfile.c_str());
	global_xf.write(camfile);
}


// ICP
void do_icp(int n)
{
	camera.stopspin();

	if (current_mesh < 0 || current_mesh >= meshes.size())
		return;
	if (n < 0 || n >= meshes.size())
		return;
	if (!visible[n] || !visible[current_mesh] || n == current_mesh)
		return;
	ICP(meshes[n], meshes[current_mesh], xforms[n], xforms[current_mesh], 2);
	update_bsph();
	need_redraw();
}


// Handle mouse button and motion events
static unsigned buttonstate = 0;

void doubleclick(int button, int x, int y)
{
	// Render and read back ID reference image
	camera.setupGL(global_xf * global_bsph.center, global_bsph.r);
	glDisable(GL_BLEND);
	glDisable(GL_LIGHTING);
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	draw_index = true;
	glPushMatrix();
	glMultMatrixd(global_xf);
	for (int i = 0; i < meshes.size(); i++) {
		if (!visible[i])
			continue;
		glColor3ub((i >> 16) & 0xff,
			(i >> 8) & 0xff,
			i & 0xff);
		draw_mesh(i);
	}
	glPopMatrix();
	draw_index = false;
	GLint V[4];
	glGetIntegerv(GL_VIEWPORT, V);
	y = int(V[1] + V[3]) - 1 - y;
	unsigned char pix[3];
	glReadPixels(x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pix);
	int n = (pix[0] << 16) + (pix[1] << 8) + pix[2];

	if (button == 0 || buttonstate == (1 << 0)) {
		// Double left click - select a mesh
		set_current(n);
	}
	else if (button == 2 || buttonstate == (1 << 2)) {
		// Double right click - ICP current to clicked-on
		do_icp(n);
	}
}

void mousehelperfunc(int x, int y)
{
	static const Mouse::button physical_to_logical_map[] = {
		Mouse::NONE, Mouse::ROTATE, Mouse::MOVEXY, Mouse::MOVEZ,
		Mouse::MOVEZ, Mouse::MOVEXY, Mouse::MOVEXY, Mouse::MOVEXY,
	};

	Mouse::button b = Mouse::NONE;
	if (buttonstate & (1 << 3))
		b = Mouse::WHEELUP;
	else if (buttonstate & (1 << 4))
		b = Mouse::WHEELDOWN;
	else if (buttonstate & (1 << 30))
		b = Mouse::LIGHT;
	else
		b = physical_to_logical_map[buttonstate & 7];

	if (current_mesh < 0) {
		camera.mouse(x, y, b,
			global_xf * global_bsph.center, global_bsph.r,
			global_xf);
	}
	else {
		xform tmp_xf = global_xf * xforms[current_mesh];
		camera.mouse(x, y, b,
			tmp_xf * meshes[current_mesh]->bsphere.center,
			meshes[current_mesh]->bsphere.r,
			tmp_xf);
		xforms[current_mesh] = inv(global_xf) * tmp_xf;
		update_bsph();
	}
}

void mousemotionfunc(int x, int y)
{
	mousehelperfunc(x, y);
	if (buttonstate)
		need_redraw();
}

void mousebuttonfunc(int button, int state, int x, int y)
{
	static timestamp last_click_time;
	static unsigned last_click_buttonstate = 0;
	static float doubleclick_threshold = 0.4f;

	if (glutGetModifiers() & GLUT_ACTIVE_CTRL)
		buttonstate |= (1 << 30);
	else
		buttonstate &= ~(1 << 30);

	if (state == GLUT_DOWN) {
		buttonstate |= (1 << button);
		if (buttonstate == last_click_buttonstate &&
			now() - last_click_time < doubleclick_threshold) {
			doubleclick(button, x, y);
			last_click_buttonstate = 0;
		}
		else {
			last_click_time = now();
			last_click_buttonstate = buttonstate;
		}
	}
	else {
		buttonstate &= ~(1 << button);
	}

	mousehelperfunc(x, y);
	if (buttonstate & ((1 << 3) | (1 << 4))) // Wheel
		need_redraw();
}


// Idle callback
void idle()
{
	xform tmp_xf = global_xf;
	if (current_mesh >= 0)
		tmp_xf = global_xf * xforms[current_mesh];

	if (camera.autospin(tmp_xf))
		need_redraw();
	else
		usleep(10000);

	if (current_mesh >= 0) {
		xforms[current_mesh] = inv(global_xf) * tmp_xf;
		update_bsph();
	}
	else {
		global_xf = tmp_xf;
	}
}


// Keyboard
#define Ctrl (1-'a')
void keyboardfunc(unsigned char key, int x, int y)
{
	switch (key) {
	case ' ':
		if (current_mesh < 0)
			resetview();
		else
			set_current(-1);
		break;
	case '@': // Shift-2
		draw_2side = !draw_2side; break;
	case 'e':
		wireframe = !wireframe; break;
	case 'f':
		draw_falsecolor = !draw_falsecolor; break;
	case 'l':
		draw_lit = !draw_lit; break;
	case 'p':
		point_size++; break;
	case 'P':
		point_size = max(1, point_size - 1); break;
	case Ctrl + 'p':
		draw_points = !draw_points; break;
	case 's':
		draw_shiny = !draw_shiny; break;
	case 't':
		line_width++; break;
	case 'T':
		line_width = max(1, line_width - 1); break;
	case 'w':
		white_bg = !white_bg; break;
	case 'I':
		dump_image(); break;
	case Ctrl + 'x':
		save_xforms();
		break;
	case Ctrl + 'c':
		save_cam_xform();
		break;
	case '\033': // Esc
	case Ctrl + 'q':
	case 'Q':
	case 'q':
		exit(0);
	case '0':
		toggle_vis(9); break;
	case '-':
		toggle_vis(10); break;
	case '=':
		toggle_vis(11); break;
	default:
		if (key >= '1' && key <= '9') {
			int m = key - '1';
			toggle_vis(m);
		}
	}
	need_redraw();
}


void usage(const char *myname)
{
	fprintf(stderr, "Usage: %s [-grab] infile...\n", myname);
	exit(1);
}
void normalization(vector <double> &vec, double length, int *cdf)
{
	vector<double>::iterator iter;
	bool flagDirection = true;
	double histgram[256];
	int size = vec.size();
	float a, b;
	memset(histgram, 0, 256 * sizeof(double));
	if (length<0)
	{
		flagDirection = false;
		length = length - 1;
	}
	else
		length += 1;
	for (iter = vec.begin(); iter != vec.end(); iter++)
	{
		*iter /= length;
		b = *iter;
		a = int(*iter * 256);
		histgram[int(*iter * 256)]++;
		if (a>200)
			a = 1;

	}
	cdf[0] = histgram[0];
	for (int i = 1; i<256; i++)
	{
		cdf[i] = cdf[i - 1] + histgram[i];
	}
	for (iter = vec.begin(); iter != vec.end(); iter++)
		*iter = cdf[int(*iter * 256)] / size * 256;
	if (!flagDirection)
		for (iter = vec.begin(); iter != vec.end(); iter++)
			*iter = -*iter;
}
void shader(TriMesh *themesh)
{
	int nv = themesh->vertices.size();
	float max_k1, min_k1, max_k2, min_k2;
	double k1, k2;
	int color_Code;
	int cdf_1[256], cdf_2[256], cdf_3[256], cdf_4[256];
	vector <double> positiveValueK1;
	vector <double> positiveValueK2;
	vector <double> negtiveValueK1;
	vector <double> negtiveValueK2;
	memset(cdf_1, 0, 256 * sizeof(int));
	memset(cdf_2, 0, 256 * sizeof(int));
	memset(cdf_3, 0, 256 * sizeof(int));
	memset(cdf_4, 0, 256 * sizeof(int));
	max_k1 = max_k2 = 0.0f;
	min_k1 = min_k2 = 0.0f;
	FILE*file = fopen("curance.txt", "w+");
	for (int i = 0; i<nv; i++)
	{
		if (themesh->curv1[i] >= 0)
			positiveValueK1.push_back(themesh->curv1[i]);
		else
			negtiveValueK1.push_back(themesh->curv1[i]);
		if (themesh->curv2[i] >= 0)
			positiveValueK2.push_back(themesh->curv2[i]);
		else
			negtiveValueK2.push_back(themesh->curv2[i]);

		if (themesh->curv1[i]>max_k1)
			max_k1 = themesh->curv1[i];
		else if (themesh->curv1[i]<min_k1)
			min_k1 = themesh->curv1[i];
		if (themesh->curv2[i]>max_k2)
			max_k2 = themesh->curv2[i];
		else if (themesh->curv2[i]<min_k2)
			min_k2 = themesh->curv2[i];
	}
	fprintf(file, "%10d\n\n", nv);
	normalization(positiveValueK1, max_k1, cdf_1);
	normalization(negtiveValueK1, min_k1, cdf_2);
	normalization(positiveValueK2, max_k2, cdf_3);
	normalization(negtiveValueK2, min_k2, cdf_4);
	fprintf(file, "%d\n", positiveValueK1.size());
	fprintf(file, "%d\n", negtiveValueK1.size());
	fprintf(file, "%d\n", positiveValueK2.size());
	fprintf(file, "%d\n\n", negtiveValueK2.size());
	Color *color;
	int a, b, c, d;
	a = b = c = d = 0;

	//第一象限的颜色表
	double rgb_1[3] = { 0.6,0.6,0.6 };
	double rgb_2[3] = { 0.6,0.6,0.6 };
	double rgb_3[3] = { 1.0,0.0,0.0 };
	double rgb_4[3] = { 0.9,0.9,0.9 };

	//第二象限的颜色表
	double rgb2_1[3] = { 0.6,0.6,0.6 };
	double rgb2_2[3] = { 0.6,1.0,1.0 };
	double rgb2_3[3] = { 0.0,1.0,0.0 };
	double rgb2_4[3] = { 0.9,0.9,0.9 };

	//第三象限的颜色表
	double rgb3_1[3] = { 0.6,0.6,0.6 };
	double rgb3_2[3] = { 0.6,1.0,1.0 };
	double rgb3_3[3] = { 0.0,0.0,1.0 };
	double rgb3_4[3] = { 0.6,1.0,1.0 };
	double rgb[3] = { 0.0,0,0.0 };

	//第四象限的颜色表
	double rgb4_1[3] = { 0.6,0.6,0.6 };
	double rgb4_2[3] = { 0.9,0.9,0.9 };
	double rgb4_3[3] = { 0.0,1.0,0.0 };
	double rgb4_4[3] = { 0.6,1.0,1.0 };
	for (int i = 0; i<nv; i++)
	{
		k1 = themesh->curv1[i];
		k2 = themesh->curv2[i];
		if (k1*k2 >= 0)
			if (k1>0 || k2>0)
			{
				color_Code = 1;
				a++;
			}
			else
			{
				color_Code = 3;
				c++;
			}
		else
			if (k1<0)
			{
				color_Code = 2;
				b++;
			}
			else
			{
				color_Code = 4;
				d++;
			}
		switch (color_Code)
		{
		case 1:
			k1 = cdf_1[int(k1 / (max_k1 + 1) * 256)] / (positiveValueK1.size()*1.0);
			k2 = cdf_3[int(k2 / (max_k2 + 1) * 256)] / (positiveValueK2.size()*1.0);
			// k1=k2=0.1;
			rgb[0] = (1.0 - k2)*((1.0 - k1)*rgb_1[0] + k1*rgb_2[0])
				+ k2*((1.0 - k1)*rgb_4[0] + k1*rgb_3[0]);
			rgb[1] = (1.0 - k2)*((1.0 - k1)*rgb_1[1] + k1*rgb_2[1])
				+ k2*((1.0 - k1)*rgb_4[1] + k1*rgb_3[1]);
			rgb[2] = (1.0 - k2)*((1.0 - k1)*rgb_1[2] + k1*rgb_2[2])
				+ k2*((1.0 - k1)*rgb_4[2] + k1*rgb_3[2]);
			break;
		case 2:
			fprintf(file, "%10.5f %10.5f %10.5f %10.5f\n", k1 / (-min_k1 + 1), k2 / (max_k2 + 1),
				cdf_2[int(-k1 / (-min_k1 + 1) * 256)] / (negtiveValueK1.size()*1.0),
				cdf_3[int(k2 / (max_k2 + 1) * 256)] / (positiveValueK2.size()*1.0));

			k1 = cdf_2[int(-k1 / (-min_k1 + 1) * 256)] / (negtiveValueK1.size()*1.0);
			k2 = cdf_3[int(k2 / (max_k2 + 1) * 256)] / (positiveValueK2.size()*1.0);
			/*k1=k2=0.8;
			rgb_1[0]=(1-k1);rgb_1[1]=1.0;rgb_1[2]=1.0;
			rgb_2[0]=(1-k1);rgb_2[1]=1.0;rgb_2[2]=0.0;
			rgb[0]=(1-k2)*rgb_1[0]+k2*rgb_2[0];rgb[1]=1.0;rgb[2]=(1-k2);*/
			rgb[0] = (1.0 - k2)*((1.0 - k1)*rgb2_1[0] + k1*rgb2_2[0])
				+ k2*((1.0 - k1)*rgb2_4[0] + k1*rgb2_3[0]);
			rgb[1] = (1.0 - k2)*((1.0 - k1)*rgb2_1[1] + k1*rgb2_2[1])
				+ k2*((1.0 - k1)*rgb2_4[1] + k1*rgb2_3[1]);
			rgb[2] = (1.0 - k2)*((1.0 - k1)*rgb2_1[2] + k1*rgb2_2[2])
				+ k2*((1.0 - k1)*rgb2_4[2] + k1*rgb2_3[2]);
			break;
		case 3:
			k1 = cdf_2[int(-k1 / (-min_k1 + 1) * 256)] / (negtiveValueK1.size()*1.0);
			k2 = cdf_4[int(-k2 / (-min_k2 + 1) * 256)] / (negtiveValueK2.size()*1.0);
			/*fprintf(file,"%10.5f %10.5f %10.5f %10.5f\n",k1/(-min_k1+1),k2/(-min_k2+1),
			cdf_2[int(-k1/(-min_k1+1)*256)]/(negtiveValueK1.size()*1.0),
			cdf_4[int(-k2/(-min_k2+1)*256)]/(negtiveValueK2.size()*1.0));*/
			/* k1=k2=0.8;
			rgb_1[0]=(1-k1);rgb_1[1]=1.0;rgb_1[2]=1.0;
			rgb_2[0]=0.0;rgb_2[1]=(1-k1);rgb_2[2]=1.0;
			rgb[0]=(1.0-k2)*rgb_1[0];rgb[1]=(1-k2)*rgb_1[1]+k2*rgb_2[1];
			rgb[2]=1.0;*/
			rgb[0] = (1.0 - k2)*((1.0 - k1)*rgb3_1[0] + k1*rgb3_2[0])
				+ k2*((1.0 - k1)*rgb3_4[0] + k1*rgb3_3[0]);
			rgb[1] = (1.0 - k2)*((1.0 - k1)*rgb3_1[1] + k1*rgb3_2[1])
				+ k2*((1.0 - k1)*rgb3_4[1] + k1*rgb3_3[1]);
			rgb[2] = (1.0 - k2)*((1.0 - k1)*rgb3_1[2] + k1*rgb3_2[2])
				+ k2*((1.0 - k1)*rgb3_4[2] + k1*rgb3_3[2]);
			break;
		case 4:
			k1 = cdf_1[int(k1 / (max_k1 + 1) * 256)] / (positiveValueK1.size()*1.0);
			k2 = cdf_4[int(-k2 / (-min_k2 + 1) * 256)] / (negtiveValueK2.size()*1.0);
			/*k1=k2=0.8;
			rgb_1[0]=1.0;rgb_1[1]=1.0;rgb_1[2]=(1-k1);
			rgb_2[0]=0.0;rgb_2[1]=1.0;rgb_2[2]=(1-k1);
			rgb[0]=(1-k2);rgb[1]=1.0;rgb[2]=(1-k2)*rgb_1[2]+k2*rgb_2[2];*/
			rgb[0] = (1.0 - k2)*((1.0 - k1)*rgb4_1[0] + k1*rgb4_2[0])
				+ k2*((1.0 - k1)*rgb4_4[0] + k1*rgb4_3[0]);
			rgb[1] = (1.0 - k2)*((1.0 - k1)*rgb4_1[1] + k1*rgb4_2[1])
				+ k2*((1.0 - k1)*rgb4_4[1] + k1*rgb4_3[1]);
			rgb[2] = (1.0 - k2)*((1.0 - k1)*rgb4_1[2] + k1*rgb4_2[2])
				+ k2*((1.0 - k1)*rgb4_4[2] + k1*rgb4_3[2]);
			break;
		}
		/*if(themesh->curv1[i]*themesh->curv2[i]>0)
		{
		if(themesh->curv1[i]>0)
		{
		double temp=themesh->curv1[i]/(2.0f*(max_k1-min_k1))+
		themesh->curv2[i]/(2.0f*(max_k2-min_k2));
		double remain=1-temp;
		themesh->colors.push_back(*new Color(temp+remain/3,remain/3,remain/3));

		}else{
		double temp=abs(themesh->curv1[i])/(2.0f*(max_k1-min_k1))+
		abs(themesh->curv2[i])/(2.0f*(max_k2-min_k2));
		double remain=1-temp;
		themesh->colors.push_back(*new Color(remain/3,remain/3,temp+remain/3));
		}
		}else if(themesh->curv1[i]*themesh->curv2[i]<0)
		{
		double temp=abs(themesh->curv1[i])/(2.0f*(max_k1-min_k1))+
		abs(themesh->curv2[i])/(2.0f*(max_k2-min_k2));
		double remain=1-temp;
		themesh->colors.push_back(*new Color(remain/3,temp+remain/3,remain/3));
		}else{
		themesh->colors.push_back(*new Color(1.0,1.0,1.0));
		}*/

		themesh->colors.push_back(*new Color(rgb[0], rgb[1], rgb[2]));
	}
	fprintf(file, "%10d %10d %10d %10d\n", a, b, c, d);
	fclose(file);
}
