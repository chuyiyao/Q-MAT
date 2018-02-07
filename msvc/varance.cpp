//#include "Rendering.h"
#include "specializedMesh.h"
#include <algorithm>
#include <Vec.h>
#include <set>
#include <map>

//#define GLEW_STATIC
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include "shader.hpp"
#include "SLrendering.h"

typedef Vec<3, float> Point;


//LARGE_INTEGER time1, time2, freq;

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

void framebuffer_size_callback(GLFWwindow* window, int w, int h)
{

	glViewport(0, 0, w, h); // Setup viewport  
                  // Reset The Current Viewport  
	TwWindowSize(w, h);
	ratio = float(w) / float(h);

}



// Window dimensions


int main(int argc, char *argv[])
{
	MedialAxisTrans MAT;
	/*vertex_id v0 = MAT.add_vertex(Point(0, 0, 0));
	vertex_id v1 = MAT.add_vertex(Point(1, 0, 0));
	vertex_id v2 = MAT.add_vertex(Point(1, 1, 0));
	vertex_id v3 = MAT.add_vertex(Point(0, 1, 0));

	MAT.add_face(v0, v1, v2);
	MAT.add_face(v0, v2, v3);

	MAT.vAttributes[v0].radius = 0.5;
	MAT.vAttributes[v1].radius = 0.5;
	MAT.vAttributes[v2].radius = 0.5;
	MAT.vAttributes[v3].radius = 0.5;*/

	MedialAxisTrans::read_ma("Data/chair.ma", MAT);
	//MedialAxisTrans::read_ma("Data/testfile.ma", MAT);

	double k = 1e-5;
	MAT.Initialize(k);
	int fiddd = 83999;



	//for (int i = 0; i < 3; i++)
	//{	
	//	std::cout << "v:"<< MAT.faces[fiddd][i] << MAT.vertices[MAT.faces[fiddd][i]] << " r: " <<
	//		MAT.vAttributes[MAT.faces[fiddd][i]].radius << std::endl;
	//	fprintf(fp, "%g %g %g %g\n", MAT.vertices[MAT.faces[fiddd][i]][0], MAT.vertices[MAT.faces[fiddd][i]][1],
	//		MAT.vertices[MAT.faces[fiddd][i]][2], MAT.vAttributes[MAT.faces[fiddd][i]].radius);
	//}
	//std::cout << fiddd << "  " << MAT.slabNormal1[fiddd] << MAT.slabNormal2[fiddd] << std::endl;

	/*FILE* fp = fopen("slabnormalNoX.txt", "w+");
	for (int i = 0; i < MAT.fN; i++)
	{
		fprintf(fp, "%d (%d %d %d)\n       (%f %f %f) (%f %f %f)\n", i, MAT.faces[i][0], MAT.faces[i][1], MAT.faces[i][2],
			MAT.slabNormal1[i][0], MAT.slabNormal1[i][1], MAT.slabNormal1[i][2],
			MAT.slabNormal2[i][0], MAT.slabNormal2[i][1], MAT.slabNormal2[i][2]);
	}
	fclose(fp);*/

	//int abv = 52529;
	//for (int i = 0; i < MAT.adjacentfaces[abv].size(); i++)
	//{
	//	int fidd = MAT.adjacentfaces[abv][i];
	//	std::cout << fidd << "  " <<MAT.slabNormal1[fidd] << MAT.slabNormal2[fidd] << std::endl;
	//}
	//printf("\n");
	/*FILE* file = fopen("edgecost.txt", "w+");
	while (!MAT.prioQue.empty())
	{
		int eid = MAT.prioQue.top().id;
		fprintf(file, "%d %d %d %g\n", eid, MAT.edges[eid][0], MAT.edges[eid][1], MAT.prioQue.top().mcost);
		MAT.prioQue.pop();
	}
	fclose(file);*/
	//
	//for (int i = 0; i < MAT.vN; i++)
	//{
	//	std::cout << MAT.vAttributes[i].radius << std::endl;
	//}

	////for (auto i = 0; i < MAT.vertices.size(); i++) {
	////	std::cout << MAT.attributes[i].radius << std::endl;
	////}
	//std::cout << "v_num " << MAT.vertices.size() << "  f_num " << MAT.faces.size() <<
	//	"  c_num " << MAT.cones.size() << "  e_num " << MAT.edges.size() << std::endl;

	////for (auto i = MAT.edges.begin(); i != MAT.edges.end(); i++)
	////	std::cout << *i << "  " << (*i).stability << std::endl;

	//for (int i = 0; i < MAT.faces.size(); i++) {
	//	std::cout<< MAT.faces[i] << MAT.slabNormal1[i] << MAT.slabNormal2[i] << std::endl;
	//}

	//for (int i = 0; i < MAT.eAttributes.size(); i++) {
	//	std::cout << MAT.edges[i] <<  "    stability: " << MAT.eAttributes[i].stability << std::endl;
	//	std::cout  << "cost:"<< MAT.eAttributes[i].cost <<" c_g:"  << MAT.eAttributes[i].c_g << std::endl;
	//}

	//while (!MAT.prioQue.empty())
	//{
	//	std::cout<<"edge: " << MAT.prioQue.top().id << " with cost:"<< MAT.prioQue.top().mcost << std::endl;
	//	MAT.prioQue.pop();
	//}
	//::QueryPerformanceFrequency(&freq);
	//::QueryPerformanceCounter(&time1);



	//::QueryPerformanceCounter(&time2);
	//float iter_f = (float)(time2.QuadPart - time1.QuadPart) / freq.QuadPart;
	//std::cout<< "Voxel time:%10.5f\n" << iter_f <<std::endl;

	// Init GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);


	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	TwInit(TW_OPENGL_CORE, NULL);
	TwWindowSize(WIDTH, HEIGHT);
	TwBar * QuaternionGUI = TwNewBar("Quaternion settings");
	//TwSetParam(QuaternionGUI, NULL, "position", TW_PARAM_CSTRING, 1, "808 16");
	TwAddVarRW(QuaternionGUI, "Zoom", TW_TYPE_FLOAT, &Zoom,
		" min=0.01 max=2.5 step=0.01 keyIncr=z keyDecr=Z help='Scale the object (1=original size).' ");
	TwAddVarRW(QuaternionGUI, "Quaternion", TW_TYPE_QUAT4F, &gOrientation, "showval=true open=true ");
	TwAddVarRW(QuaternionGUI, "Contraction", TW_TYPE_BOOL8, &gOriginal, "help='Contracting Outcome ?'");
	// Set GLFW event callbacks. I removed glfwSetWindowSizeCallback for conciseness
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetMouseButtonCallback(window, (GLFWmousebuttonfun)TwEventMouseButtonGLFW); // - Directly redirect GLFW mouse button events to AntTweakBar
	glfwSetCursorPosCallback(window, (GLFWcursorposfun)TwEventMousePosGLFW);          // - Directly redirect GLFW mouse position events to AntTweakBar
	glfwSetScrollCallback(window, (GLFWscrollfun)TwEventMouseWheelGLFW);    // - Directly redirect GLFW mouse wheel events to AntTweakBar
	glfwSetKeyCallback(window, (GLFWkeyfun)TwEventKeyGLFW);                         // - Directly redirect GLFW key events to AntTweakBar
	glfwSetCharCallback(window, (GLFWcharfun)TwEventCharGLFW);
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	framebuffer_size_callback(window, width, height);
	//
//	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);


	//GLvertices = MAT.vertices;
	

	//indexVBO_VN(MAT.vertices, MAT.faces, face_indices, GLvertices, GLnormals);

	printf("Initializing rendering \n\n\n");
	init(MAT);
	printf("Initializing rendering  ..... Done\n\n\n");

	while (!glfwWindowShouldClose(window))
	{

		display();

		TwDraw();

		glfwSwapBuffers(window);
		glfwPollEvents();

	}
	Release();	// Properly de-allocate all resources once they've outlived their purpose

	TwTerminate();
	glfwTerminate();
	return 0;

}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

//GLFWwindowsizefun WindowSizeCB(int width, int height)
//{
//	// Set OpenGL viewport and camera
//	glViewport(0, 0, width, height);
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//	gluPerspective(40, (double)width / height, 1, 10);
//	gluLookAt(-1, 0, 3, 0, 0, 0, 0, 1, 0);
//
//	// Send the new window size to AntTweakBar
//	TwWindowSize(width, height);
//}



