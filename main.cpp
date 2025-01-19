//
//  main.cpp
//  OpenGL Advances Lighting
//
//  Created by CGIS on 28/11/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

#if defined (__APPLE__)
#define GLFW_INCLUDE_GLCOREARB
#define GL_SILENCE_DEPRECATION
#else
#define GLEW_STATIC
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.hpp"
#include "Model3D.hpp"
#include "Camera.hpp"
#include "skybox.hpp"



#include <iostream>

int glWindowWidth = 800;
int glWindowHeight = 600;
int retina_width, retina_height;
bool firstMouse = true;
float lastX = glWindowWidth / 2;
float lastY = glWindowHeight / 2;
float yaw = -90.0f, pitch = 0.0f;
const float sensitivity = 0.3f;
bool automove = false;
GLFWwindow* glWindow = NULL;
bool pointLightsEnabled = false;
const unsigned int SHADOW_WIDTH = 2048 * 6;
const unsigned int SHADOW_HEIGHT = 2048 * 6;
bool isLightning = false;
glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;
glm::mat4 lightRotation;

glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;

enum LightingType {
	DIRECTIONAL_LIGHT,
	POINT_LIGHT
};
int fogOn = 0;
bool flashActive = false;
float flashStartTime = 0.0f;
float flashDuration = 0.2f; // Duration of the flash in seconds


LightingType currentLightingType = DIRECTIONAL_LIGHT;
bool isTourActive = false;          // To track whether the tour is active
float tourProgress = 0.0f;          // To track the progress of the tour
int currentTourIndex = 0;           // To track the current point in the tour
float tourSpeed = 0.05f;            // Speed at which the tour progresses
bool useCircularTour = false;
float circularTourRadius = 25.0f; // Radius of the circular tour
float circularTourHeight = 4.0f; // Height of the camera (closer to the ground)
float angularSpeed = 0.002f;     // Speed of the camera along the circular path (smaller values slow it down)
float currentAngle = 0.0f;      // Current angle for the circular path


std::vector<glm::vec3> tourPositions = {
	glm::vec3(0.0f, 2.0f, 5.5f),
	glm::vec3(5.0f, 3.0f, 3.0f),
	glm::vec3(-5.0f, 3.0f, -3.0f),
	glm::vec3(0.0f, 3.0f, -5.0f),
	glm::vec3(2.0f, 2.0f, 6.0f),
	glm::vec3(5.0f, 4.0f, 7.0f),   // New position 1
	glm::vec3(7.0f, 4.5f, -2.0f),  // New position 2
	glm::vec3(4.0f, 3.0f, -6.0f),  // New position 3
	glm::vec3(-3.0f, 2.0f, -7.0f), // New position 4
	glm::vec3(1.0f, 2.5f, 4.0f)    // New position 5
};

std::vector<glm::vec3> tourTargets = {
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(1.0f, 1.0f, 0.0f),
	glm::vec3(-1.0f, 0.5f, -1.0f),
	glm::vec3(0.0f, 0.0f, -1.0f),
	glm::vec3(2.0f, 1.0f, 0.0f),
	glm::vec3(6.0f, 3.0f, 3.0f),   // New target 1
	glm::vec3(8.0f, 3.5f, -1.0f),  // New target 2
	glm::vec3(3.0f, 3.0f, -5.0f),  // New target 3
	glm::vec3(-4.0f, 1.5f, -6.0f), // New target 4
	glm::vec3(2.0f, 2.0f, 3.0f)    // New target 5




};



std::vector<glm::vec3> rainPositions; // Store the positions of raindrops
bool isRaining = false;               // To toggle the rain
const int numRaindrops = 1000;        // Number of raindrops
float rainArea = 20.0f;               // Width/Length of the rain area
float rainHeight = 10.0f;             // Maximum height of the rain



gps::Camera myCamera(
	glm::vec3(0.0f, 2.0f, 5.5f),
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 1.0f, 0.0f));
float cameraSpeed = 0.11f;

bool pressedKeys[1024];
float angleY = 0.0f;
GLfloat lightAngle;

gps::Model3D untitled;
gps::Model3D water;
gps::Model3D lightCube;
gps::Model3D screenQuad;
gps::Model3D iceLake;
gps::Model3D lowPolyTree;
gps::Model3D scene;
gps::Model3D justground;
gps::Model3D lake;
gps::Model3D fish;

//skybox 
gps::SkyBox mySkyBox;
gps::Shader skyboxShader;
gps::Shader depthMapShader;
gps::Shader myCustomShader;
gps::Shader lightShader;
gps::Shader screenQuadShader;
extern gps::Camera myCamera;
GLuint shadowMapFBO;
GLuint depthMapTexture;

bool showDepthMap;

GLenum glCheckError_(const char* file, int line) {
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	//TODO
	glWindowWidth = width;
	glWindowHeight = height;
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);
	glViewport(0, 0, retina_width, retina_height);
		projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
		projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
		// set the projection matrix in the light shader
		lightShader.useShaderProgram();
		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		// set the projection matrix in the screen quad shader
			screenQuadShader.useShaderProgram();
			glUniformMatrix4fv(glGetUniformLocation(screenQuadShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));



}

GLenum renderMode = GL_FILL;

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_M && action == GLFW_PRESS)
		showDepthMap = !showDepthMap;

	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS) {
			pressedKeys[key] = true;

			// Toggle render mode
			if (key == GLFW_KEY_1) { // Press '1' for solid view

				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
			if (key == GLFW_KEY_2) { // Press '2' for wireframe view

				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			if (key == GLFW_KEY_3) { // Press '3' for point view

				glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
			}
			if (pressedKeys[GLFW_KEY_F])
			{
				fogOn = !fogOn;
				myCustomShader.useShaderProgram();
			
				glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "fogOn"), fogOn);
				if (fogOn) {
					std::cout << "Fog on" << std::endl;
					GLint fogColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "fogColor");
					glUniform4f(fogColorLoc, 0.5f, 0.5f, 0.5f, 1.0f); // Gray fog
				}
				else {
					std::cout << "Fog off" << std::endl;
					//GLint fogDensityLoc = glGetUniformLocation(myCustomShader.shaderProgram, "fogDensity");
					//glUniform1f(fogDensityLoc, 0.003f); // Example fog density
				}
				
			}

		}
		else if (action == GLFW_RELEASE) {
			pressedKeys[key] = false;
		}
		if (key == GLFW_KEY_L && action == GLFW_PRESS) {
			if (currentLightingType == DIRECTIONAL_LIGHT) {
				currentLightingType = POINT_LIGHT;
				std::cout << "Switched to Point Light" << std::endl;
			}
			else {
				currentLightingType = DIRECTIONAL_LIGHT;
				std::cout << "Switched to Directional Light" << std::endl;
			}
		}
		if (key == GLFW_KEY_T && action == GLFW_PRESS) {
			isTourActive = !isTourActive;
			if (isTourActive) {
				std::cout << "Automated tour started!" << std::endl;
				currentTourIndex = 0;
				tourProgress = 0.0f;
			}
			else {
				std::cout << "Automated tour stopped!" << std::endl;
			}
		}
		if (key == GLFW_KEY_C && action == GLFW_PRESS) {
			// Toggle between circular tour and predefined tour
			useCircularTour = !useCircularTour;
			if (useCircularTour) {
				std::cout << "Switched to Circular Tour" << std::endl;
			}
			else {
				std::cout << "Switched to Interpolated Tour" << std::endl;
			}
		}
		if (key == GLFW_KEY_R && action == GLFW_PRESS) { // Press 'R' to toggle rain
			isRaining = !isRaining;
			if (isRaining) {
				std::cout << "Rain started!" << std::endl;
			}
			else {
				std::cout << "Rain stopped!" << std::endl;
			}
		}
		if (key == GLFW_KEY_K && action == GLFW_PRESS) { // Toggle point lights with 'P'
			pointLightsEnabled = !pointLightsEnabled;

			myCustomShader.useShaderProgram();
			GLint pointLightsEnabledLoc = (glGetUniformLocation(myCustomShader.shaderProgram, "pointLightsEnabled"));
			glUniform1i(pointLightsEnabledLoc, pointLightsEnabled);

			std::cout << "Point lights " << (pointLightsEnabled ? "enabled" : "disabled") << "!" << std::endl;
		}
		if (key == GLFW_KEY_B && action == GLFW_PRESS) {
			flashActive = true;
			flashStartTime = glfwGetTime(); // Record the time when the flash starts
			std::cout << "Flash triggered!" << std::endl;
		}



	}
}

int maxRainDroplets = 100;           // Maximum number of droplets
float rainAreaSize = 20.0f;          // Size of the rain area

void initializeRain() {
	// Update rain area to cover the entire scene
	float sceneWidth = 40.0f;  // Width of the scene (adjust as needed)
	float sceneLength = 40.0f; // Length of the scene (adjust as needed)
	float rainHeight = 20.0f;  // Starting height of raindrops

	// Generate raindrops
	for (int i = 0; i < numRaindrops; ++i) {
		float x = static_cast<float>(rand()) / RAND_MAX * sceneWidth - (sceneWidth / 2);
		float y = static_cast<float>(rand()) / RAND_MAX * rainHeight + rainHeight; // Start above the ground
		float z = static_cast<float>(rand()) / RAND_MAX * sceneLength - (sceneLength / 2);
		rainPositions.push_back(glm::vec3(x, y, z));
	}
}

void updateRain() {
	if (isRaining) {
		float sceneWidth = 40.0f;  // Width of the scene (adjust as needed)
		float sceneLength = 40.0f; // Length of the scene (adjust as needed)
		float rainHeight = 20.0f;  // Height to reset raindrops

		for (auto& position : rainPositions) {
			position.y -= 0.1f; // Droplet falls down
			if (position.y < 0.0f) { // Reset droplet if it hits the ground
				position.y = rainHeight + static_cast<float>(rand()) / RAND_MAX * rainHeight;
				position.x = static_cast<float>(rand()) / RAND_MAX * sceneWidth - (sceneWidth / 2);
				position.z = static_cast<float>(rand()) / RAND_MAX * sceneLength - (sceneLength / 2);
			}
		}
	}
}



void handleFlashEffect() {
	if (flashActive) {
		float currentTime = glfwGetTime();
		if (currentTime - flashStartTime < flashDuration) {
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Bright white flash
		}
		else {
			glClearColor(0.3f, 0.3f, 0.3f, 1.0f); // Reset to normal background color
			flashActive = false; // End the flash
		}
	}
}


float smoothStep(float t) {
	return t * t * (3.0f - 1.7f * t);
}

void updateCameraForTour() {
	static bool flipOccurred = false;  // Static flag to ensure it persists between function calls

	if (isTourActive) {
		if (useCircularTour) {
			// Circular tour logic: Move the camera in a circular path
			float x = circularTourRadius * cos(currentAngle);  // X position in the circle
			float z = circularTourRadius * sin(currentAngle);  // Z position in the circle
			float y = circularTourHeight;                      // Constant height for the tour

			// Set camera position and target (Look at the center of the scene)
			myCamera.cameraPosition = glm::vec3(x, y, z);
			myCamera.cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);  // Looking at the center

			// Update progress of the angle for the circular motion (slower)
			currentAngle += angularSpeed;
			if (currentAngle >= glm::two_pi<float>()) {
				currentAngle -= glm::two_pi<float>();  // Reset the angle after a full circle
			}
		}
		else {
			// Interpolated tour logic with flips
			if (currentTourIndex < tourPositions.size() - 1) {
				glm::vec3 startPosition = tourPositions[currentTourIndex];
				glm::vec3 endPosition = tourPositions[currentTourIndex + 1];
				glm::vec3 startTarget = tourTargets[currentTourIndex];
				glm::vec3 endTarget = tourTargets[currentTourIndex + 1];

				// Interpolate position and target
				glm::vec3 interpolatedPosition = glm::mix(startPosition, endPosition, tourProgress);
				glm::vec3 interpolatedTarget = glm::mix(startTarget, endTarget, tourProgress);

				// Check for flip effect halfway through the segment
				if (tourProgress > 0.5f && !flipOccurred) {
					// Flip the target direction (flip the target along the y-axis)
					//interpolatedTarget.y = -interpolatedTarget.y;  // Flip vertically along y-axis

					// Uncomment the following line for a more dramatic 180-degree flip around the Y-axis
					interpolatedTarget = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(interpolatedTarget, 1.0f);

					flipOccurred = true;  // Flag to ensure only one flip per segment
				}
				else if (tourProgress < 0.5f) {
					flipOccurred = false;  // Reset flip flag to allow flip in the next half of the segment
				}

				// Set the camera position and target
				myCamera.cameraPosition = interpolatedPosition;
				myCamera.cameraTarget = interpolatedTarget;

				// Update progress
				tourProgress += tourSpeed;

				// If the tourProgress is complete, move to the next point
				if (tourProgress >= 1.0f) {
					tourProgress = 0.0f;  // Reset progress for the next segment
					currentTourIndex++;
				}
			}
		}
	}
}



void mouseCallback(GLFWwindow* window, double xpos, double ypos) {

	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
	lastX = xpos;
	lastY = ypos;


	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 88.0f) pitch = 88.0f;
	if (pitch < -88.0f) pitch = -88.0f;

	myCamera.rotate(pitch, yaw);

}

void processMovement()
{
	if (isTourActive) {
		updateCameraForTour();
	}

	// Camera controls
	else {

		if (pressedKeys[GLFW_KEY_U]) {
			lightAngle += 4.0f;
		}
		if (pressedKeys[GLFW_KEY_J]) {
			lightAngle -= 4.0f;
		}
		if (pressedKeys[GLFW_KEY_W]) {
			myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
		}
		if (pressedKeys[GLFW_KEY_S]) {
			myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
		}
		if (pressedKeys[GLFW_KEY_A]) {
			myCamera.move(gps::MOVE_LEFT, cameraSpeed);
		}
		if (pressedKeys[GLFW_KEY_D]) {
			myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
		}
		if (pressedKeys[GLFW_KEY_Z]) {
			myCamera.move(gps::MOVE_UP, cameraSpeed);
		}
		if (pressedKeys[GLFW_KEY_X]) {
			myCamera.move(gps::MOVE_DOWN, cameraSpeed);
		}
		if (pressedKeys[GLFW_KEY_P]) {
			automove = !automove;
		}
		if (automove) {
			lightAngle += 0.1f;
			myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
		}

	}
}

bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	//window scaling for HiDPI displays
	glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

	//for sRBG framebuffer
	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

	//for antialising
	glfwWindowHint(GLFW_SAMPLES, 4);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);
	//glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwMakeContextCurrent(glWindow);

	glfwSwapInterval(1);

#if not defined (__APPLE__)
	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();
#endif

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	return true;
}

void initOpenGLState()
{
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glViewport(0, 0, retina_width, retina_height);

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glDisable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_FRAMEBUFFER_SRGB);
}

void initObjects() {
	lightCube.LoadModel("models/ground/ground.obj");
	screenQuad.LoadModel("models/quad/quad.obj");
	untitled.LoadModel("models/untitled.obj");
	water.LoadModel("models/water.obj");
	scene.LoadModel("models/scene.obj");
	lake.LoadModel("models/lake.obj");
	fish.LoadModel("models/fish.obj");
	justground.LoadModel("models/justground.obj");
	std::vector<const GLchar*> faces = {
	"models/sky/Tantolunden5/posx.jpg",
	"models/sky/Tantolunden5/negx.jpg",
	"models/sky/Tantolunden5/posy.jpg",
	"models/sky/Tantolunden5/negy.jpg",
	"models/sky/Tantolunden5/posz.jpg",
	"models/sky/Tantolunden5/negz.jpg",
	};
	mySkyBox.Load(faces);
	
}

void initShaders() {
    myCustomShader.loadShader("shaders/basic.vert", "shaders/basic.frag");
	myCustomShader.useShaderProgram();

	skyboxShader.loadShader("shaders/skybox.vert", "shaders/skybox.frag");
	skyboxShader.useShaderProgram();

	depthMapShader.loadShader("shaders/depthMapShader.vert", "shaders/depthMapShader.frag");
	depthMapShader.useShaderProgram();


}

void initUniforms() {
    myCustomShader.useShaderProgram();

    // Set model matrix
    model = glm::mat4(1.0f);
    modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    // Set view matrix
    view = myCamera.getViewMatrix();
    viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // Set normal matrix
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    // Set projection matrix
    projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
    projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Directional Light
    lightDir = glm::vec3(1.0f, 1.0f, 1.0f); // Direction pointing downwards and towards the scene
    lightColor = glm::vec3(1.0f, 1.0f, 1.0f); // White light
    lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
    lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));


    // Point Light
    glm::vec3 pointLightPos = glm::vec3(2.0f, 2.0f, 2.0f); // Position of the point light
    //glm::vec3 pointLightColor = glm::vec3(1.0f, 0.8f, 1.6f); // Warm white light
	glm::vec3 pointLightColor = glm::vec3(1.0f, 0.8f, 1.0); // Warm white light
    GLuint pointLightPosLoc = glGetUniformLocation(myCustomShader.shaderProgram, "pointLightPos");
    GLuint pointLightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "pointLightColor");
    glUniform3fv(pointLightPosLoc, 1, glm::value_ptr(pointLightPos));
    glUniform3fv(pointLightColorLoc, 1, glm::value_ptr(pointLightColor));



    // Point Light Attenuation
    glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "constant"), 1.0f);
    glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "linear"), 0.09f);
    glUniform1f(glGetUniformLocation(myCustomShader.shaderProgram, "quadratic"), 0.032f);

	// Point Light 1
	glm::vec3 point1Pos = glm::vec3(18.0f, 20.0f, 0.0f);
	glm::vec3 point1Color = glm::vec3(10.0f, 10.0f, 0.0f); // yellow
	glUniform3fv(glGetUniformLocation(myCustomShader.shaderProgram, "pointLight1Position"), 1, glm::value_ptr(point1Pos));
	glUniform3fv(glGetUniformLocation(myCustomShader.shaderProgram, "pointLight1Color"), 1, glm::value_ptr(point1Color));

	// Point Light 2
	glm::vec3 point2Pos = glm::vec3(-18.0f, 20.0f, 18.0f);
	glm::vec3 point2Color = glm::vec3(0.0f, 0.0f, 10.0f); // blue
	glUniform3fv(glGetUniformLocation(myCustomShader.shaderProgram, "pointLight2Position"), 1, glm::value_ptr(point2Pos));
	glUniform3fv(glGetUniformLocation(myCustomShader.shaderProgram, "pointLight2Color"), 1, glm::value_ptr(point2Color));

	// Point Light 3
	glm::vec3 point3Pos = glm::vec3(18.0f, 20.0f, -18.0f);
	glm::vec3 point3Color = glm::vec3(10.0f, 0.0f, 0.0f); // red
	glUniform3fv(glGetUniformLocation(myCustomShader.shaderProgram, "pointLight3Position"), 1, glm::value_ptr(point3Pos));
	glUniform3fv(glGetUniformLocation(myCustomShader.shaderProgram, "pointLight3Color"), 1, glm::value_ptr(point3Color));

	// Point Light 4
	glm::vec3 point4Pos = glm::vec3(-18.0f, 25.0f, 0.0f);
	glm::vec3 point4Color = glm::vec3(5.0f, 10.0f, 5.0f); // light green
	glUniform3fv(glGetUniformLocation(myCustomShader.shaderProgram, "pointLight4Position"), 1, glm::value_ptr(point4Pos));
	glUniform3fv(glGetUniformLocation(myCustomShader.shaderProgram, "pointLight4Color"), 1, glm::value_ptr(point4Color));

	// Point Light 5
	glm::vec3 point5Pos = glm::vec3(18.0f, 25.0f, 0.0f);
	glm::vec3 point5Color = glm::vec3(0.0f, 5.0f, 0.0f); // dark green
	glUniform3fv(glGetUniformLocation(myCustomShader.shaderProgram, "pointLight5Position"), 1, glm::value_ptr(point5Pos));
	glUniform3fv(glGetUniformLocation(myCustomShader.shaderProgram, "pointLight5Color"), 1, glm::value_ptr(point5Color));

	GLint pointLightsEnabledLoc = glGetUniformLocation(myCustomShader.shaderProgram, "pointLightsEnabled");	
	glUniform1i(pointLightsEnabledLoc, GL_FALSE);

	//fog
	lightShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}


void initFBO() {
	//TODO - Create the FBO, the depth texture and attach the depth texture to the FBO
	glGenFramebuffers(1, &shadowMapFBO);
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 computeLightSpaceTrMatrix() {
	//TODO - Return the light-space transformation matrix
	lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 lightView = glm::lookAt(glm::vec3(lightRotation * glm::vec4(lightDir, 10.0f)), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	const GLfloat near_plane = 0.1f, far_plane = 100.0f;
	glm::mat4 lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane, far_plane);
	glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;

	return lightSpaceTrMatrix;
}

bool isLake = false;

// Variables for fish animation
glm::vec3 fishCenter = glm::vec3(-2.0f, -2.5f, 9.0f); // Center of the elliptical path
float fishRadiusX = 5.0f;                             // Horizontal radius of the ellipse
float fishRadiusZ = 3.0f;                             // Vertical radius of the ellipse (on the Z-axis)
float fishAngle = 0.0f;                               // Current angle of the fish along the path
float fishSpeed = 0.02f;                              // Speed of the fish's movement along the path
float fishWaveAmplitude = 0.3f;                       // Amplitude of the vertical wave (how high/low the fish moves)
float fishWaveFrequency = 1.0f;                       // Frequency of the wave motion

// Update fish position and orientation
void updateFishAnimation() {
	// Increment the angle for the fish to move along the elliptical path
	fishAngle += fishSpeed;
	if (fishAngle > glm::two_pi<float>()) {
		fishAngle -= glm::two_pi<float>(); // Keep the angle within 0 to 2π
	}
}

void drawObjects(gps::Shader shader, bool depthPass) {

	shader.useShaderProgram();

	//draw ground
	glm::mat4 groundModel = glm::mat4(1.0f);
	groundModel = glm::translate(groundModel, glm::vec3(0.0f, -1.5f, 0.0f)); // Position the ground
	//groundModel = glm::scale(groundModel, glm::vec3(10.0f, 10.0f, 10.0f));    // Scale the ground
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(groundModel));


	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	//nanosuit.Draw(shader);
	//untitled.Draw(shader);
	justground.Draw(shader);


	//draw scene
	glm::mat4 sceneModel = glm::mat4(1.0f);
	sceneModel = glm::translate(sceneModel, glm::vec3(0.0f, -1.5f, 0.0f)); // Position the scene
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(sceneModel));


	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	//nanosuit.Draw(shader);
	scene.Draw(shader);


	/*//draw fish
	glm::mat4 fishModel = glm::mat4(1.0f);
	fishModel = glm::translate(fishModel, glm::vec3(-2.0f, -2.5f, 8.0f)); // Position the fish
	fishModel = glm::rotate(fishModel, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate the fish
	fishModel = glm::scale(fishModel, glm::vec3(0.5f, 0.5f, 0.5f));    // Scale the fish
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(fishModel));


	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	fish.Draw(shader);*/

	updateFishAnimation();

	// Compute fish position along the elliptical path
	glm::vec3 fishPosition;
	fishPosition.x = fishCenter.x + fishRadiusX * cos(fishAngle); // X-coordinate for elliptical path
	fishPosition.z = fishCenter.z + fishRadiusZ * sin(fishAngle); // Z-coordinate for elliptical path
	fishPosition.y = fishCenter.y + fishWaveAmplitude * sin(fishAngle * fishWaveFrequency); // Y-coordinate with wave motion

	// Draw fish
	glm::mat4 fishModel = glm::mat4(1.0f);
	fishModel = glm::translate(fishModel, fishPosition); // Position the fish
	fishModel = glm::scale(fishModel, glm::vec3(0.5f, 0.5f, 0.5f)); // Scale the fish
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(fishModel));

	// Do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * fishModel));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	fish.Draw(shader);


	//draw lake
	glm::mat4 lakeModel = glm::mat4(1.0f);
	lakeModel = glm::translate(lakeModel, glm::vec3(0.0f, -1.5f, 0.0f)); // Position the lake
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(lakeModel));


	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		isLake = !isLake;
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
		glUniform1i(glGetUniformLocation(shader.shaderProgram, "isLake"), isLake);
		lake.Draw(shader); //draw here so it wont cast shadow
		isLake = !isLake;
		glUniform1i(glGetUniformLocation(shader.shaderProgram, "isLake"), isLake);
	}

	//draw skybox
	glm::mat4 skyboxModel = glm::mat4(1.0f);
	skyboxModel = glm::translate(skyboxModel, glm::vec3(0.0f, -1.5f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(skyboxModel));

	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	mySkyBox.Draw(skyboxShader, view, projection);


}


void renderScene() {
	// Depth maps creation pass
	depthMapShader.useShaderProgram();
	// Pass the light-space transformation matrix to the depth map shader


	handleFlashEffect();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"), 1, GL_FALSE, glm::value_ptr(computeLightSpaceTrMatrix()));

	// Set viewport to shadow map resolution and bind the shadow map framebuffer
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	// Draw objects for shadow map generation
	drawObjects(depthMapShader, true);

	// Unbind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Render depth map on screen (for debugging, toggled with the 'M' key)
	if (showDepthMap) {
		glViewport(0, 0, retina_width, retina_height);
		glClear(GL_COLOR_BUFFER_BIT);
		screenQuadShader.useShaderProgram();

		// Bind and display the depth map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(screenQuadShader.shaderProgram, "depthMap"), 0);

		glDisable(GL_DEPTH_TEST);
		screenQuad.Draw(screenQuadShader);
		glEnable(GL_DEPTH_TEST);
	}
	else {
		// Final scene rendering pass (with shadows)
		glViewport(0, 0, retina_width, retina_height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		myCustomShader.useShaderProgram();

		// Pass view matrix
		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		// Pass updated light direction (in view space)
		lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

		// Bind shadow map texture
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);

		// Pass light-space transformation matrix to the fragment shader
		glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"), 1, GL_FALSE, glm::value_ptr(computeLightSpaceTrMatrix()));

		// Draw objects with shadows, fog, and lighting
		drawObjects(myCustomShader, false);

		// Render rain droplets if enabled
		if (isRaining) {
			updateRain();

			for (const auto& position : rainPositions) {
				glm::mat4 rainModel = glm::mat4(1.0f);
				rainModel = glm::translate(rainModel, position); // Position the droplet
				rainModel = glm::scale(rainModel, glm::vec3(0.02f)); // Scale appropriately

				// Send the rain model matrix
				glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(rainModel));

				// Compute and send the normal matrix
				normalMatrix = glm::mat3(glm::inverseTranspose(view * rainModel));
				glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

				// Render the droplet (ensure 'water' is your droplet model)
				water.Draw(myCustomShader);
			}
		}
	
	}
}


void cleanup() {
	glDeleteTextures(1, &depthMapTexture);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &shadowMapFBO);
	glfwDestroyWindow(glWindow);
	//close GL context and any other GLFW resources
	glfwTerminate();
}

int main(int argc, const char* argv[]) {

	if (!initOpenGLWindow()) {
		glfwTerminate();
		return 1;
	}

	initOpenGLState();
	initObjects();
	initShaders();
	initUniforms();
	initFBO();
	initializeRain();

	glCheckError();

	while (!glfwWindowShouldClose(glWindow)) {
		processMovement();
		renderScene();

		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}

	cleanup();

	return 0;
}
