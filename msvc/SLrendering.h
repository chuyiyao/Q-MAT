//////////////////////////////////////////////////////////////////////////////
//
//  Triangles.cpp
//
//////////////////////////////////////////////////////////////////////////////

#include <TriMesh.h>
#include "shader.hpp"
//#include 
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\quaternion.hpp>
#include <glm\gtx\quaternion.hpp>
#include <glm\gtx\euler_angles.hpp>
#include <glm\gtx\norm.hpp>
using namespace glm;
#include "controls.hpp"
#include "quaternion_utils.hpp"

#include <AntTweakBar.h>

GLFWwindow* window;
enum Attrib_IDs { vPosition = 0 };

GLuint VAO;
GLuint VBO, NormalBuffer;
GLuint elementbuffer[2];

GLuint NumVertices;
GLuint NumFaces;
GLuint NumCones;
GLuint NumEdges;

GLint WIDTH = 1024, HEIGHT = 768;
GLfloat ratio = WIDTH / HEIGHT;
vec3 gPosition(0.0f, 0.0f, 0.0f);
quat gOrientation;
float Zoom = 1.0f;
float MatDiffuse[] = { 1.0f, 1.0f, 0.0f, 1.0f };


bool gLookAtOther = true;

//----------------------------------------------------------------------------
//
// init
//
std::vector<unsigned short> face_indices;
std::vector<unsigned short> cone_indices;
std::vector<point> GLvertices;
std::vector<vec> GLnormals;

//uniform
GLuint MatrixID, VID, MID, lightID, isTri;
GLuint programID;

void
init(MedialAxisTrans &MAT)
{
	NumVertices = MAT.vertices.size();
	NumFaces = MAT.faces.size();
	NumCones = MAT.cones.size();
	NumEdges = MAT.edges.size();
	

	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	//glEnable(GL_CULL_FACE);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);


	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, NumVertices * sizeof(point), &MAT.vertices[0][0], GL_STATIC_DRAW);
	

	glGenBuffers(1, &NormalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, NormalBuffer);
	glBufferData(GL_ARRAY_BUFFER, NumVertices * sizeof(vec), &MAT.normals[0], GL_STATIC_DRAW);

	glGenBuffers(2, elementbuffer);
	if (NumFaces > 0) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer[0]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, NumFaces * sizeof(Face), &MAT.faces[0], GL_STATIC_DRAW);
	}
	

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, NumEdges * sizeof(Edge), &MAT.edges[0], GL_STATIC_DRAW);

	programID = LoadShaders("vertex.vert","fragment.frag");
	glUseProgram(programID);

	//glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	//glEnableVertexAttribArray(vPosition);


	MatrixID = glGetUniformLocation(programID, "MVP");
	VID = glGetUniformLocation(programID, "V");
	MID = glGetUniformLocation(programID, "M");
	lightID = glGetUniformLocation(programID, "lightPosition_w");
	//GLuint vertexNormal_modelspaceID = glGetAttribLocation(programID, "vertexNormal_modelspace");

}

//----------------------------------------------------------------------------
//
// display
//

void
display(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(programID);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, NormalBuffer);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glm::vec3 lightPos = glm::vec3(4, 4, 4);
	glUniform3f(lightID, lightPos.x, lightPos.y, lightPos.z);

	glm::mat4 ProjectionMatrix = glm::perspective(glm::radians(45.0f), ratio, 0.1f, 100.0f);
	glm::mat4 ViewMatrix = glm::lookAt(
		glm::vec3(0, 0, 7), // Camera is here
		glm::vec3(0, 0, 0), // and looks here
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		Zoom += 0.02f;
	}
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		Zoom -= 0.02f;
	}

	glm::mat4 RotationMatrix = glm::mat4_cast(gOrientation);
	glm::mat4 TranslationMatrix = glm::translate(mat4(), gPosition); // A bit to the right
	glm::mat4 ScalingMatrix = scale(mat4(), glm::vec3(Zoom, Zoom, Zoom));
	glm::mat4 ModelMatrix = TranslationMatrix * RotationMatrix * ScalingMatrix;

	glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(MID, 1, GL_FALSE, &ModelMatrix[0][0]);
	glUniformMatrix4fv(VID, 1, GL_FALSE, &ViewMatrix[0][0]);

	//glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	if (NumFaces > 0){
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer[0]);
		glDrawElements(
			GL_TRIANGLES,
			NumFaces*sizeof(Face),    // count
			GL_UNSIGNED_INT,   // type
			(void*)0           // element array buffer offset
		);
	}
	

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer[1]);
	glDrawElements(
		GL_LINES,
		NumEdges * 2,//sizeof(Cone),    // count
		GL_UNSIGNED_INT,   // type
		(void*)0           // element array buffer offset
	);
	//glUniform1d(isTri, 0);
	//glDrawElements(
	//	GL_LINES,
	//	NumFaces * sizeof(Edge),    // count
	//	GL_UNSIGNED_INT,   // type
	//	(void*)0           // element array buffer offset
	//);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

}



void Release() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &NormalBuffer);
	glDeleteBuffers(2, elementbuffer);
	glDeleteProgram(programID);

}


