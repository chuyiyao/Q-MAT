// Include GLFW
#include<GLFW\glfw3.h>
extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" declared in tutorialXXX.cpp. This is a hack to keep the tutorials simple. Please avoid this.

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//using namespace glm;

#include "controls.hpp"

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix(){
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix(){
	return ProjectionMatrix;
}


// Initial position : on +Z
glm::vec3 position = glm::vec3( 4, 0, 3 ); 
// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f / 2;
// Initial vertical angle : none
float verticalAngle = 0;
// Initial Field of View
float initialFoV = 45.0f;

float speed = 3.0f; // 3 units / second
float mouseSpeed = 0.001f;



void computeMatricesFromInputs(){

	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// Reset mouse position for next frame
	int wei, hei;
	glfwGetWindowSize(window, &wei, &hei);
	glfwSetCursorPos(window, wei/2, hei/2);

	// Compute new orientation
	horizontalAngle += mouseSpeed * float(wei/2 - xpos );
	verticalAngle   += mouseSpeed * float( hei/2 - ypos );

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle), 
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);
	
	// Right vector
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f/2.0f), 
		0,
		cos(horizontalAngle - 3.14f/2.0f)
	);
	
	// Up vector
	glm::vec3 up = glm::cross( right, direction );

	// Move forward
	if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS){
		position += direction * deltaTime * speed;
	}
	// Move backward
	if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS){
		position -= direction * deltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS){
		position += right * deltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS){
		position -= right * deltaTime * speed;
	}
	//double xf, yf;
	//GLFWscrollfun(window, &xf, &yf);
	float FoV = initialFoV;// -5 * float(xf); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

	// Projection matrix : 45?Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	ViewMatrix       = glm::lookAt(
								position,           // Camera is here
								position+direction, // and looks here : at the same position, plus "direction"
								up                  // Head is up (set to 0,-1,0 to look upside-down)
						   );

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}

void computeMatricesRotateInputs() {

	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// Reset mouse position for next frame
	int wei, hei;
	glfwGetWindowSize(window, &wei, &hei);
	//glfwSetCursorPos(window, wei / 2, hei / 2);

	// Compute new orientation
	horizontalAngle += mouseSpeed * float(wei / 2 - xpos);
	verticalAngle += mouseSpeed * float(hei / 2 - ypos);

	// Direction : Spherical coordinates to Cartesian coordinates conversion

	float FoV = initialFoV;
	ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);

	//glm::vec3 direction(
	//	cos(verticalAngle) * sin(horizontalAngle),
	//	sin(verticalAngle),
	//	cos(verticalAngle) * cos(horizontalAngle)
	//);

	//// Right vector
	//glm::vec3 right = glm::vec3(
	//	sin(horizontalAngle - 3.14f / 2.0f),
	//	0,
	//	cos(horizontalAngle - 3.14f / 2.0f)
	//);

	float radius = 4.0f;
	static float theta = 0.0f;
	static float phi = 0.0f;
	int index = 0;

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		phi +=  deltaTime * speed;
	}
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		phi -=  deltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		theta += deltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		theta -=  deltaTime * speed;

	}


	//phi += deltaTime*sin(horizontalAngle);
	position = glm::vec3(0,0,0) + glm::vec3(radius * cos(theta) * cos(phi), 
		radius * cos(theta) * sin(phi) , radius * sin(theta));
	//glm::vec3 up = glm::cross(-position, glm::vec3(-cos(phi), -sin(phi), 0));
	glm::vec3 up = glm::vec3(-sin(theta)*cos(phi), -sin(theta)*sin(phi), cos(theta));
	// Camera matrix
	ViewMatrix = glm::lookAt(
		position,           // Camera is here
		glm::vec3(0, 0, 0), // and looks here : at the same position, plus "direction"
		up                  // Head is up (set to 0,-1,0 to look upside-down)
	);

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}

