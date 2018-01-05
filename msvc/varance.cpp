//#include "Rendering.h"
#include "specializedMesh.h"
#include <algorithm>
#include <Vec.h>
#include <set>
#include <map>

#define GLEW_STATIC
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include "shader.hpp"
#include "SLrendering.h"

typedef Vec<3, float> Point;
GLFWwindow* window;


//LARGE_INTEGER time1, time2, freq;

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Window dimensions
const GLuint WIDTH = 1024, HEIGHT = 768;


int main(int argc, char *argv[])
{
	MedialAxisTrans MAT;
	vertex_id v0 = MAT.add_vertex(Point(0, 0, 0));
	vertex_id v1 = MAT.add_vertex(Point(2, 0, 0));
	vertex_id v2 = MAT.add_vertex(Point(2, 3, 0));
	vertex_id v3 = MAT.add_vertex(Point(0, 1, 1));
	vertex_id v4 = MAT.add_vertex(Point(-1, 0, 0));
	vertex_id v5 = MAT.add_vertex(Point(0, 0, 1));
	vertex_id v6 = MAT.add_vertex(Point(0, -1, 0));
	vertex_id v7 = MAT.add_vertex(Point(0, 1, 0));

	face_id f = MAT.add_face(v0, v1, v2);
	MAT.add_face(v0, v2, v3);
	MAT.add_face(v0, v3, v4);
	MAT.add_face(v0, v4, v5);
	MAT.add_face(v0, v5, v6);
	MAT.add_face(v0, v2, v7);
	edge_id e = MAT.add_cone(v6, v1);

	MAT.vAttributes[v0].radius = 1;
	MAT.vAttributes[v1].radius = 0.5;
	MAT.vAttributes[v2].radius = 0.5;
	MAT.vAttributes[v7].radius = 0.5;

	double k = 1e-5;
	MAT.Initialize(k);

	for (int i = 0; i < MAT.vN; i++)
	{
		std::cout << MAT.vertices[i] << std::endl;
	}

	//for (auto i = 0; i < MAT.vertices.size(); i++) {
	//	std::cout << MAT.attributes[i].radius << std::endl;
	//}
	std::cout << "v_num " << MAT.vertices.size() << "  f_num " << MAT.faces.size() <<
		"  c_num " << MAT.cones.size() << "  e_num " << MAT.edges.size() << std::endl;

	//for (auto i = MAT.edges.begin(); i != MAT.edges.end(); i++)
	//	std::cout << *i << "  " << (*i).stability << std::endl;

	for (int i = 0; i < MAT.faces.size(); i++) {
		std::cout<< MAT.faces[i] << MAT.slabNormal1[i] << MAT.slabNormal2[i] << std::endl;
	}

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
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
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
	TwSetParam(QuaternionGUI, NULL, "position", TW_PARAM_CSTRING, 1, "808 16");
	TwAddVarRW(QuaternionGUI, "Quaternion", TW_TYPE_QUAT4F, &gOrientation, "showval=true open=true ");
	TwAddVarRW(QuaternionGUI, "Use LookAt", TW_TYPE_BOOL8, &gLookAtOther, "help='Look at the other monkey ?'");
	// Set GLFW event callbacks. I removed glfwSetWindowSizeCallback for conciseness
	glfwSetMouseButtonCallback(window, (GLFWmousebuttonfun)TwEventMouseButtonGLFW); // - Directly redirect GLFW mouse button events to AntTweakBar
	//glfwSetCursorPosCallback(window, (GLFWcursorposfun)TwEventMousePosGLFW);          // - Directly redirect GLFW mouse position events to AntTweakBar
	//glfwSetScrollCallback(window, (GLFWscrollfun)TwEventMouseWheelGLFW);    // - Directly redirect GLFW mouse wheel events to AntTweakBar
	//glfwSetKeyCallback(window, (GLFWkeyfun)TwEventKeyGLFW);                         // - Directly redirect GLFW key events to AntTweakBar
	//glfwSetCharCallback(window, (GLFWcharfun)TwEventCharGLFW);
//	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Define the viewport dimensions
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	//GLvertices = MAT.vertices;
	

	//indexVBO_VN(MAT.vertices, MAT.faces, face_indices, GLvertices, GLnormals);

	init(MAT);

	//std::vector<Face>::iterator iter1 = GLtriangles->begin();
	//std::vector<Face>::iterator iter2 = iter1 + 1;
	//std::cout << "两个迭代器之间的距离（差值）" << iter2 - iter1 << std::endl;
	//std::cout << "vector占用内存大小:" << sizeof(std::vector<Face>) << std::endl;

	//
	//for (int i = 0; i<GLtriangles->size(); i++)//输出其地址和值  
	//{
	//	for (int j = 0; j<3; j++)
	//		std::cout << &GLtriangles[i][j] << ":" << GLtriangles[i][j] << " ";
	//	std::cout << std::endl;
	//}


	while (!glfwWindowShouldClose(window))
	{
		display();
		
		glfwSwapBuffers(window);
		glfwPollEvents();

	}
	Release();	// Properly de-allocate all resources once they've outlived their purpose

	glfwTerminate();
	return 0;

}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}






