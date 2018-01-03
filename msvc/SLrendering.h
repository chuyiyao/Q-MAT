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
using namespace glm;
#include "controls.hpp"



enum Attrib_IDs { vPosition = 0 };

GLuint VAO;
GLuint VBO, NormalBuffer;
GLuint elementbuffer[2];

GLuint NumVertices;
GLuint NumFaces;
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


	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);


	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, NumVertices * sizeof(point), &MAT.vertices[0][0], GL_STATIC_DRAW);
	

	glGenBuffers(1, &NormalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, NormalBuffer);
	glBufferData(GL_ARRAY_BUFFER, NumVertices * sizeof(vec), &MAT.normals[0], GL_STATIC_DRAW);

	glGenBuffers(2, elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, NumFaces * sizeof(Face), &MAT.faces[0], GL_STATIC_DRAW);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer[1]);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, cone_indices.size() * sizeof(unsigned short), &cone_indices[0], GL_STATIC_DRAW);

	programID = LoadShaders("vertex.vert","fragment.frag");
	glUseProgram(programID);

	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(vPosition);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	//glEnable(GL_CULL_FACE);

	MatrixID = glGetUniformLocation(programID, "MVP");
	VID = glGetUniformLocation(programID, "V");
	MID = glGetUniformLocation(programID, "M");
	lightID = glGetUniformLocation(programID, "lightPosition_w");

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


	computeMatricesRotateInputs();
	glm::mat4 ProjectionMatrix = getProjectionMatrix();
	glm::mat4 ViewMatrix = getViewMatrix();
	glm::mat4 ModelMatrix = glm::mat4(1.0);
	glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(MID, 1, GL_FALSE, &ModelMatrix[0][0]);
	glUniformMatrix4fv(VID, 1, GL_FALSE, &ViewMatrix[0][0]);

	glm::vec3 lightPos = glm::vec3(4, 4, 4);
	glUniform3f(lightID, lightPos.x, lightPos.y, lightPos.z);
	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(
		0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, NormalBuffer);
	glVertexAttribPointer(
		1,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	//glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer[0]);
	glDrawElements(
		GL_TRIANGLES,
		NumFaces*sizeof(Face),    // count
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

}


