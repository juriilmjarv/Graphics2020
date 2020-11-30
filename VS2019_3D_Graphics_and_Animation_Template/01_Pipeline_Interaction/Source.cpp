// 3D Graphics and Animation - Main Template
// Visual Studio 2019
// Last Changed 01/10/2019

#pragma comment(linker, "/NODEFAULTLIB:MSVCRT")
#define GLM_ENABLE_EXPERIMENTAL
#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <stack>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
using namespace std;

#include <GL/glew.h>			// Add library to extend OpenGL to newest version
#include <GLFW/glfw3.h>			// Add library to launch a window
#include <GLM/glm.hpp>			// Add helper maths library
#include <GLM/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <stb_image.h>			// Add library to load images for textures

#include "Mesh.h"				// Simplest mesh holder and OBJ loader - can update more - from https://github.com/BennyQBD/ModernOpenGLTutorial


// MAIN FUNCTIONS
void setupRender();
void startup();
void update(GLfloat currentTime);
void render(GLfloat currentTime);
void endProgram();

// HELPER FUNCTIONS OPENGL
void hintsGLFW();
string readShader(string name);
void checkErrorShader(GLuint shader);
void errorCallbackGLFW(int error, const char* description);
void debugGL();
static void APIENTRY openGLDebugCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const GLvoid* userParam);

// CALLBACK FUNCTIONS FOR WINDOW
void onResizeCallback(GLFWwindow* window, int w, int h);
void onKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void onMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void onMouseMoveCallback(GLFWwindow* window, double x, double y);
void onMouseWheelCallback(GLFWwindow* window, double xoffset, double yoffset);


// VARIABLES
GLFWwindow*		window;											// Keep track of the window
int				windowWidth = 1000;				
int				windowHeight = 800;
bool			running = true;									// Are we still running?
glm::mat4		proj_matrix;									// Projection Matrix

//Camera settings
glm::vec3		cameraPosition = glm::vec3(0.0f, 0.0f, 5.0f);	// Week 5 lecture
glm::vec3		cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3		cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
GLfloat			vx = 0;
GLfloat			vy = 0;
GLfloat			vz = 0;

//Lighting
GLfloat			ka = 1.0;
glm::vec3		ia = glm::vec3(0.7f, 0.7f, 0.7f);
GLfloat			kd = 1.0;
glm::vec3		id = glm::vec3(0.5f, 0.5f, 0.5f);
GLfloat			ks = 1.0;
glm::vec3		is = glm::vec3(0.7f, 0.7f, 0.7f);

float           aspect = (float)windowWidth / (float)windowHeight;
float			fovy = 45.0f;
bool			keyStatus[1024];
GLfloat			deltaTime = 0.0f;
GLfloat			lastTime = 0.0f;
GLfloat			model_scale = 0.3f;
GLuint			program;
GLint			proj_location;
glm::vec3		modelPosition;
glm::vec3		modelRotation;
//light position
GLfloat	light_x = 2.5;
GLfloat	light_y = 0;
GLfloat	light_z = 0;

//Lightsource rotation
GLfloat rotate_light = 0;
GLfloat rotate_inc = 0.5;

//Disasemble animation vars
bool disasemble = false;
GLfloat move_parts = 0;
GLfloat velocity = 0.01;
GLfloat move_handslegs = 0;


//Textures and objects
GLuint texSwitch = 0;
Mesh cube;		// Add a cube object
Mesh skybox;	//skybox
Mesh sphere;	// lightsource
Mesh nose;		//nose
Mesh rightsleeve;	//sleeve
Mesh leftsleeve; //sleeve
Mesh righthand;
Mesh lefthand;
Mesh rightlegsleeve;
Mesh leftlegsleeve;
Mesh rightleg;
Mesh leftleg;
Mesh rightboot;
Mesh leftboot;

//texture IDs
GLuint texture1,texture2, texture3,texture4, cubemapTexture, tex_location;


int main()
{
	if (!glfwInit()) {							// Checking for GLFW
		cout << "Could not initialise GLFW...";
		return 0;
	}

	glfwSetErrorCallback(errorCallbackGLFW);	// Setup a function to catch and display all GLFW errors.

	hintsGLFW();								// Setup glfw with various hints.		

												// Start a window using GLFW
	string title = "Coursework";

	// Fullscreen
	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	// windowWidth = mode->width; windowHeight = mode->height; //fullscreen
	// window = glfwCreateWindow(windowWidth, windowHeight, title.c_str(), glfwGetPrimaryMonitor(), NULL); // fullscreen

	// Window
	window = glfwCreateWindow(windowWidth, windowHeight, title.c_str(), NULL, NULL);
	if (!window) {								// Window or OpenGL context creation failed
		cout << "Could not initialise GLFW...";
		endProgram();
		return 0;
	}

	glfwMakeContextCurrent(window);				// making the OpenGL context current

												// Start GLEW (note: always initialise GLEW after creating your window context.)
	glewExperimental = GL_TRUE;					// hack: catching them all - forcing newest debug callback (glDebugMessageCallback)
	GLenum errGLEW = glewInit();
	if (GLEW_OK != errGLEW) {					// Problems starting GLEW?
		cout << "Could not initialise GLEW...";
		endProgram();
		return 0;
	}

	debugGL();									// Setup callback to catch openGL errors.	

												// Setup all the message loop callbacks.
	glfwSetWindowSizeCallback(window, onResizeCallback);		// Set callback for resize
	glfwSetKeyCallback(window, onKeyCallback);					// Set Callback for keys
	glfwSetMouseButtonCallback(window, onMouseButtonCallback);	// Set callback for mouse click
	glfwSetCursorPosCallback(window, onMouseMoveCallback);		// Set callback for mouse move
	glfwSetScrollCallback(window, onMouseWheelCallback);		// Set callback for mouse wheel.
																//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);	// Set mouse cursor. Fullscreen
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);	// Set mouse cursor FPS fullscreen.

	setupRender();								// setup some render variables.
	startup();									// Setup all necessary information for startup (aka. load texture, shaders, models, etc).

	do {										// run until the window is closed
		GLfloat currentTime = (GLfloat)glfwGetTime();		// retrieve timelapse
		deltaTime = currentTime - lastTime;		// Calculate delta time
		lastTime = currentTime;					// Save for next frame calculations.
		glfwPollEvents();						// poll callbacks
		update(currentTime);					// update (physics, animation, structures, etc)
		render(currentTime);					// call render function.

		glfwSwapBuffers(window);				// swap buffers (avoid flickering and tearing)

		running &= (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE);	// exit if escape key pressed
		running &= (glfwWindowShouldClose(window) != GL_TRUE);
	} while (running);

	endProgram();			// Close and clean everything up...

	cout << "\nPress any key to continue...\n";
	cin.ignore(); cin.get(); // delay closing console to read debugging errors.

	return 0;
}

bool load_texture(const char* filename, GLuint& texID, bool bGenMipmaps)
{
	glGenTextures(1, &texID);

	stbi_set_flip_vertically_on_load(true);

	// local image parameters
	int width, height, nrChannels;

	/* load an image file using stb_image */
	unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);

	// check for an error during the load process
	if (data)
	{
		// Note: this is not a full check of all pixel format types, just the most common two!
		int pixel_format = 0;

		if (nrChannels == 3)
			pixel_format = GL_RGB;
		else
			pixel_format = GL_RGBA;

		// Bind the texture ID before the call to create the texture.
		// texID[i] will now be the identifier for this specific texture
		glBindTexture(GL_TEXTURE_2D, texID);

		// Create the texture, passing in the pointer to the loaded image pixel data
		glTexImage2D(GL_TEXTURE_2D, 0, pixel_format, width, height, 0, pixel_format, GL_UNSIGNED_BYTE, data);

		//glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, width, height);

		//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, pixel_format, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// Set texture filtering parameters (next lecture)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		

		// Generate Mip Maps
		if (bGenMipmaps)
			glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		printf("stb_image  loading error: filename=%s", filename);
		return false;
	}

	stbi_image_free(data);
	return true;
}

//From tutorial:
//https://learnopengl.com/Advanced-OpenGL/Cubemaps
unsigned int loadCubemap(vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	stbi_set_flip_vertically_on_load(false);

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

void errorCallbackGLFW(int error, const char* description) {
	cout << "Error GLFW: " << description << "\n";
}

void hintsGLFW() {
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);			// Create context in debug mode - for debug message callback
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
}

void endProgram() {
	glfwMakeContextCurrent(window);		// destroys window handler
	glfwTerminate();	// destroys all windows and releases resources.
}

void setupRender() {
	glfwSwapInterval(1);	// Ony render when synced (V SYNC)

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 2);
	glfwWindowHint(GLFW_STEREO, GL_FALSE);
}

void startup() {

	//define skybox images
	//from: https://spacedock.info/mod/2572/Triangulum%20Galaxy%20Skybox
	vector<std::string> faces
	{	
		"GalaxyTex_NegativeX.png",
		"GalaxyTex_PositiveX.png",
		"GalaxyTex_NegativeY.png",
		"GalaxyTex_PositiveY.png",
		"GalaxyTex_NegativeZ.png",
		"GalaxyTex_PositiveZ.png"
	};

	//FROM: https://sherly.mobile9.com/download/media/559/spongebob-_Al2c7lEeK3.jpg
	const char* file1 = "spongebobtexture.jpg";

	if (!load_texture(file1, texture1, true)) {
		cout << "error: " << file1 << endl;
		exit(0);
	}

	//FROM: https://www.solarsystemscope.com/textures/
	const char* file2 = "moontex.jpg";

	if (!load_texture(file2, texture2, true)) {
		cout << "error: " << file2 << endl;
		exit(0);
	}

	const char* file3 = "polySurface10_polySurfaceShape10_color_2.jpg";

	if (!load_texture(file3, texture3, true)) {
		cout << "error: " << file3 << endl;
		exit(0); 
	}
	//FROM: https://www.solarsystemscope.com/textures/
	const char* file4 = "2k_neptune.jpg";

	if (!load_texture(file4, texture4, true)) {
		cout << "error: " << file4 << endl;
		exit(0);
	}

	// Load main object model and shaders
	cubemapTexture = loadCubemap(faces);
	skybox.LoadModel("cube_uv.obj");
	sphere.LoadModel("sun.obj");
	cube.LoadModel("SpongebobBody3.obj");
	nose.LoadModel("nose.obj");
	rightsleeve.LoadModel("rightsleeve.obj");
	leftsleeve.LoadModel("leftsleeve.obj");
	righthand.LoadModel("righthand.obj");
	lefthand.LoadModel("lefthand.obj");
	rightlegsleeve.LoadModel("rightlegsleeve.obj");
	leftlegsleeve.LoadModel("leftlegsleeve.obj");
	rightleg.LoadModel("rightleg.obj");
	leftleg.LoadModel("leftleg.obj");
	rightboot.LoadModel("rightboot1.obj");
	leftboot.LoadModel("leftboot.obj");
	
	program = glCreateProgram();


	string vs_text = readShader("vs_model.glsl"); const char* vs_source = vs_text.c_str();
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vs_source, NULL);
	glCompileShader(vs);
	checkErrorShader(vs);
	glAttachShader(program, vs);

	string fs_text = readShader("fs_model.glsl"); const char* fs_source = fs_text.c_str();
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fs_source, NULL);
	glCompileShader(fs);
	checkErrorShader(fs);
	glAttachShader(program, fs);

	glLinkProgram(program);
	glUseProgram(program);


	// Start from the centre
	modelPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	modelRotation = glm::vec3(0.0f, 0.0f, 0.0f);
	
	// A few optimizations.
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// Calculate proj_matrix for the first time.
	aspect = (float)windowWidth / (float)windowHeight;
	proj_matrix = glm::perspective(glm::radians(fovy), aspect, 0.1f, 1000.0f);

	//Instructions to print out to console
	cout << "==================INSTRUCTIONS===================" << endl;
	cout << " KEY_LEFT - Camera +x" << endl;
	cout << " KEY_RIGHT - Camera -x" << endl;
	cout << " KEY_UP - Camera +y" << endl;
	cout << " KEY_DOWN - Camera -y" << endl;
	cout << " Q - Camera z" << endl;
	cout << " W - Camera -z" << endl;
	cout << " A - Zoom in" << endl;
	cout << " S - Zoom out" << endl;
	cout << " K - Disassemble" << endl;
	cout << " L - Assemble" << endl;
	cout << "================================================" << endl;
}

void update(GLfloat currentTime) {
	if (keyStatus[GLFW_KEY_LEFT])			vy += 1.f;
	if (keyStatus[GLFW_KEY_RIGHT])			vy -= 1.f;
	if (keyStatus[GLFW_KEY_UP])				vx += 1.f;
	if (keyStatus[GLFW_KEY_DOWN])			vx -= 1.f;
	if (keyStatus[GLFW_KEY_Q])				vz += 1.f;
	if (keyStatus[GLFW_KEY_W])				vz -= 1.f;
	if (keyStatus[GLFW_KEY_A])				model_scale -= 0.01f;
	if (keyStatus[GLFW_KEY_S])				model_scale += 0.01f;
	if (keyStatus[GLFW_KEY_K])				disasemble = true;
	if (keyStatus[GLFW_KEY_L])				disasemble = false;


}

void render(GLfloat currentTime) {
	glViewport(0, 0, windowWidth, windowHeight);

	// Clear colour buffer
	glm::vec4 backgroundColor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f); glClearBufferfv(GL_COLOR, 0, &backgroundColor[0]);

	// Clear deep buffer
	static const GLfloat one = 1.0f; glClearBufferfv(GL_DEPTH, 0, &one);

	// Enable blend
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Use our shader programs
	glUseProgram(program);

	// Setup camera
	glm::mat4 viewMatrix = glm::lookAt(cameraPosition,					// eye
		cameraPosition + cameraFront,	// centre
		cameraUp);						// up

	//camera rotation
	viewMatrix = glm::rotate(viewMatrix, -glm::radians(vx), glm::vec3(1, 0, 0)); //rotating in clockwise direction around x-axis
	viewMatrix = glm::rotate(viewMatrix, -glm::radians(vy), glm::vec3(0, 1, 0)); //rotating in clockwise direction around y-axis
	viewMatrix = glm::rotate(viewMatrix, -glm::radians(vz), glm::vec3(0, 0, 1));


	glUniformMatrix4fv(glGetUniformLocation(program, "view_matrix"), 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "proj_matrix"), 1, GL_FALSE, &proj_matrix[0][0]);
	glUniform4f(glGetUniformLocation(program, "viewPosition"), cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0);
	glUniform4f(glGetUniformLocation(program, "lightPosition"), light_x,light_y, light_z, 1.0);
	glUniform4f(glGetUniformLocation(program, "ia"), ia.r, ia.g, ia.b, 1.0);
	glUniform1f(glGetUniformLocation(program, "ka"), ka);
	glUniform4f(glGetUniformLocation(program, "id"), id.r, id.g, id.b, 1.0);
	glUniform1f(glGetUniformLocation(program, "kd"), kd);
	glUniform4f(glGetUniformLocation(program, "is"), is.r, is.g, is.b, 1.0);
	glUniform1f(glGetUniformLocation(program, "ks"), ks);
	glUniform1f(glGetUniformLocation(program, "shininess"), 32.0f);
	glUniform1f(glGetUniformLocation(program,"lightConstant"), 0.5f);
	glUniform1f(glGetUniformLocation(program,"lightLinear"), 0.5f);
	glUniform1f(glGetUniformLocation(program,"lightQuadratic"), 0.2f);
	glUniform4f(glGetUniformLocation(program, "objcolor"), 0.965, 0.910, 0.004, 1.0);

	//Use stack data structure to draw multiple objects
	stack<glm::mat4> model;
	model.push(glm::mat4(1.0f));

	model.top() = glm::scale(model.top(), glm::vec3(model_scale, model_scale, model_scale));

	texSwitch = 1; //turn texture on
	glUniform1ui(glGetUniformLocation(program, "texSwitch"), texSwitch); //update value and send to shader
	//Moon texture for lightsource
	glBindTexture(GL_TEXTURE_2D, texture2); //take moon texture

	//=============================================================LIGHTSOURCE==============================================================================

	//draw moon sphere as same position as lightsource
	model.push(model.top());
	{
		// Define the model transformations for the moon
		model.top() = glm::rotate(model.top(), rotate_light / 30, glm::vec3(0, 1, 0));
		model.top() = glm::translate(model.top(), glm::vec3(8.0, 0, 0));
		model.top() = glm::rotate(model.top(), rotate_light / 30, glm::vec3(0, 1, 0));
		model.top() = glm::translate(model.top(), glm::vec3(-light_x, light_y, light_z));
		model.top() = glm::scale(model.top(), glm::vec3(0.5f, 0.5f, 0.5f));

		//make light position same as model position by extracting matrix values
		light_x = model.top()[3][0];
		light_y = model.top()[3][1];
		light_z = model.top()[3][2];

		//cout << "X: " << light_x << "Y: " << light_y << "Z: " << light_z << endl;
		// Send the model uniform to the currently bound shader,
		glUniformMatrix4fv(glGetUniformLocation(program, "model_matrix"), 1, GL_FALSE, &(model.top()[0][0]));

		//Draw Sphere
		sphere.Draw();
	}
	model.pop();

	//=============================================================END LIGHTSOURCE==============================================================================

	texSwitch = 2; //turn skybox texture on
	glUniform1ui(glGetUniformLocation(program, "texSwitch"), texSwitch); //update value and send to shader

	//=============================================================SKYBOX==============================================================================

	//apply skybox texture to a cube
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	//Show on both sides of cube
	glDisable(GL_CULL_FACE);

	//SKYBOX
	model.push(model.top());
	{
		// Define the model transformations for the skybox
		model.top() = glm::translate(model.top(), glm::vec3(0, 4.0f, 0));
		model.top() = glm::scale(model.top(), glm::vec3(20.0f, 20.0f, 20.0f));
		// Send the model uniform and normal matrix to the currently bound shader,
		glUniformMatrix4fv(glGetUniformLocation(program, "model_matrix"), 1, GL_FALSE, &(model.top()[0][0]));
		//Draw skybox
		skybox.Draw();
	}
	model.pop();

	//=============================================================END OF SKYBOX==============================================================================


	texSwitch = 1; //turn texture on for regular texture
	glUniform1ui(glGetUniformLocation(program, "texSwitch"), texSwitch); //update value and send to shader

	//=============================================================SPONGEBOB BODY==============================================================================

	//Spongebob body texture
	glBindTexture(GL_TEXTURE_2D, texture1);

	//draw spongebob body object
	model.push(model.top());
	{
		model.top() = glm::scale(model.top(), glm::vec3(0.5f, 0.5f, 0.5f));//scale equally in all axis
		glUniformMatrix4fv(glGetUniformLocation(program, "model_matrix"), 1, GL_FALSE, &(model.top()[0][0]));
		/* Draw our second object */
		cube.Draw();
	}
	model.pop();
	//=============================================================END SPONGEBOB BODY==============================================================================

	//=============================================================NOSE==============================================================================
	texSwitch = 0; //turn texture off and use regular color
	glUniform1ui(glGetUniformLocation(program, "texSwitch"), texSwitch); //update value and send to shader

	//draw spongebob nose object
	model.push(model.top());
	{
		model.top() = glm::translate(model.top(), glm::vec3(0, 0, move_parts));
		model.top() = glm::scale(model.top(), glm::vec3(0.5f, 0.5f, 0.5f));//scale equally in all axis
		glUniformMatrix4fv(glGetUniformLocation(program, "model_matrix"), 1, GL_FALSE, &(model.top()[0][0]));
		//update color
		glUniform4f(glGetUniformLocation(program, "objcolor"), 0.965, 0.910, 0.004, 1.0);
		//draw nose
		nose.Draw();
	}
	model.pop();
	//=============================================================END NOSE==============================================================================

	//=============================================================RIGHT HAND==============================================================================
	//draw spongebob right sleeve object
	model.push(model.top());
	{
		model.top() = glm::translate(model.top(), glm::vec3(-(move_parts), 0, 0));
		model.top() = glm::scale(model.top(), glm::vec3(0.5f, 0.5f, 0.5f));//scale equally in all axis
		glUniformMatrix4fv(glGetUniformLocation(program, "model_matrix"), 1, GL_FALSE, &(model.top()[0][0]));
		//update color
		glUniform4f(glGetUniformLocation(program, "objcolor"), 1, 1, 0.879, 1.0);
		//draw nose
		rightsleeve.Draw();
	}
	model.pop();

	//draw spongebob right hand
	model.push(model.top());
	{
		model.top() = glm::translate(model.top(), glm::vec3(-(move_handslegs), 0, 0));
		model.top() = glm::scale(model.top(), glm::vec3(0.5f, 0.5f, 0.5f));//scale equally in all axis
		glUniformMatrix4fv(glGetUniformLocation(program, "model_matrix"), 1, GL_FALSE, &(model.top()[0][0]));
		//update color
		glUniform4f(glGetUniformLocation(program, "objcolor"), 0.965, 0.910, 0.04, 1.0);
		//draw right hand
		righthand.Draw();
	}
	model.pop();
	//=============================================================END RIGH HAND==============================================================================

	//=============================================================LEFT HAND==============================================================================
	//draw spongebob left sleeve object
	model.push(model.top());
	{
		model.top() = glm::translate(model.top(), glm::vec3(move_parts, 0, 0));
		model.top() = glm::scale(model.top(), glm::vec3(0.5f, 0.5f, 0.5f));//scale equally in all axis
		glUniformMatrix4fv(glGetUniformLocation(program, "model_matrix"), 1, GL_FALSE, &(model.top()[0][0]));
		//update color
		glUniform4f(glGetUniformLocation(program, "objcolor"), 1, 1, 0.879, 1.0);
		//draw sleeve
		leftsleeve.Draw();
	}
	model.pop();

	//draw spongebob left hand
	model.push(model.top());
	{
		model.top() = glm::translate(model.top(), glm::vec3(move_handslegs, 0, 0));
		model.top() = glm::scale(model.top(), glm::vec3(0.5f, 0.5f, 0.5f));//scale equally in all axis
		glUniformMatrix4fv(glGetUniformLocation(program, "model_matrix"), 1, GL_FALSE, &(model.top()[0][0]));
		//update color
		glUniform4f(glGetUniformLocation(program, "objcolor"), 0.965, 0.910, 0.04, 1.0);
		//draw right hand
		lefthand.Draw();
	}
	model.pop();
	//=============================================================END LEFT HAND==============================================================================

	//=============================================================RIGHT LEG==============================================================================
	model.push(model.top());
	{
		model.top() = glm::translate(model.top(), glm::vec3(0, -(move_parts), 0));
		model.top() = glm::scale(model.top(), glm::vec3(0.5f, 0.5f, 0.5f));//scale equally in all axis
		glUniformMatrix4fv(glGetUniformLocation(program, "model_matrix"), 1, GL_FALSE, &(model.top()[0][0]));
		//update color
		glUniform4f(glGetUniformLocation(program, "objcolor"), 1, 1, 0.879, 1.0);
		//draw right leg sleeve
		rightlegsleeve.Draw();
	}
	model.pop();

	//draw spongebob right leg
	model.push(model.top());
	{
		model.top() = glm::translate(model.top(), glm::vec3(0, -(move_handslegs), 0));
		model.top() = glm::scale(model.top(), glm::vec3(0.5f, 0.5f, 0.5f));//scale equally in all axis
		glUniformMatrix4fv(glGetUniformLocation(program, "model_matrix"), 1, GL_FALSE, &(model.top()[0][0]));
		//update color
		glUniform4f(glGetUniformLocation(program, "objcolor"), 0.965, 0.910, 0.04, 1.0);
		//draw right leg
		rightleg.Draw();
	}
	model.pop();

	texSwitch = 1; //turn texture on
	glUniform1ui(glGetUniformLocation(program, "texSwitch"), texSwitch); //update value and send to shader
	//boot texture for right boot
	glBindTexture(GL_TEXTURE_2D, texture3); //take moon texture

	//draw spongebob right leg
	model.push(model.top());
	{
		model.top() = glm::translate(model.top(), glm::vec3(0, -(move_handslegs), 0));
		model.top() = glm::scale(model.top(), glm::vec3(0.5f, 0.5f, 0.5f));//scale equally in all axis
		glUniformMatrix4fv(glGetUniformLocation(program, "model_matrix"), 1, GL_FALSE, &(model.top()[0][0]));
		//draw right boot
		rightboot.Draw();
	}
	model.pop();

	texSwitch = 0; //turn texture on
	glUniform1ui(glGetUniformLocation(program, "texSwitch"), texSwitch); //update value and send to shader

	//=============================================================END RIGHT LEG==============================================================================

	//=============================================================LEFT LEG==============================================================================
	model.push(model.top());
	{
		model.top() = glm::translate(model.top(), glm::vec3(0, -(move_parts), 0));
		model.top() = glm::scale(model.top(), glm::vec3(0.5f, 0.5f, 0.5f));//scale equally in all axis
		glUniformMatrix4fv(glGetUniformLocation(program, "model_matrix"), 1, GL_FALSE, &(model.top()[0][0]));
		//update color
		glUniform4f(glGetUniformLocation(program, "objcolor"), 1, 1, 0.879, 1.0);
		//draw left leg
		leftlegsleeve.Draw();
	}
	model.pop();

	//draw spongebob left leg
	model.push(model.top());
	{
		model.top() = glm::translate(model.top(), glm::vec3(0, -(move_handslegs), 0));
		model.top() = glm::scale(model.top(), glm::vec3(0.5f, 0.5f, 0.5f));//scale equally in all axis
		glUniformMatrix4fv(glGetUniformLocation(program, "model_matrix"), 1, GL_FALSE, &(model.top()[0][0]));
		//update color
		glUniform4f(glGetUniformLocation(program, "objcolor"), 0.965, 0.910, 0.04, 1.0);
		//draw left leg
		leftleg.Draw();
	}
	model.pop();

	texSwitch = 1; //turn texture on
	glUniform1ui(glGetUniformLocation(program, "texSwitch"), texSwitch); //update value and send to shader
	//boot texture for left boot
	glBindTexture(GL_TEXTURE_2D, texture3); //take moon texture

	//draw spongebob left boot
	model.push(model.top());
	{
		model.top() = glm::translate(model.top(), glm::vec3(0, -(move_handslegs), 0));
		model.top() = glm::scale(model.top(), glm::vec3(0.5f, 0.5f, 0.5f));//scale equally in all axis
		glUniformMatrix4fv(glGetUniformLocation(program, "model_matrix"), 1, GL_FALSE, &(model.top()[0][0]));
		//draw left boot
		leftboot.Draw();
	}
	model.pop();

	texSwitch = 0; //turn texture on
	glUniform1ui(glGetUniformLocation(program, "texSwitch"), texSwitch); //update value and send to shader


	//=============================================================END LEFT LEG==============================================================================

	//============================================================ANIMATION==============================================================================
	//Disasemble the parts by moving back and forth
	if (disasemble == true) {
		if (move_parts <= 0.5) {
			move_parts += velocity;
		}
		else if(move_parts == 0.5) {
			move_parts = 1.0;
		}

		if (move_handslegs <= 1.0) {
			move_handslegs += velocity;
		}
		else if (move_handslegs == 1.0) {
			move_handslegs = 1.0;
		}
	}
	else {
		if (move_parts <= 0.0) {	
			move_parts = 0.0;
		}
		else {
			move_parts -= velocity;
		}

		if (move_handslegs <= 0) {
			move_handslegs = 0;
		}
		else {
			move_handslegs -= velocity;
		}
	}

	//rotate light
	rotate_light += rotate_inc;
}

void onResizeCallback(GLFWwindow* window, int w, int h) {
	windowWidth = w;
	windowHeight = h;

	aspect = (float)w / (float)h;
	proj_matrix = glm::perspective(glm::radians(fovy), aspect, 0.1f, 1000.0f);
}

void onKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) keyStatus[key] = true;
	else if (action == GLFW_RELEASE) keyStatus[key] = false;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void onMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {

}

void onMouseMoveCallback(GLFWwindow* window, double x, double y) {
	int mouseX = static_cast<int>(x);
	int mouseY = static_cast<int>(y);
}

static void onMouseWheelCallback(GLFWwindow* window, double xoffset, double yoffset) {
	int yoffsetInt = static_cast<int>(yoffset);
}

void debugGL() {
	//Output some debugging information
	cout << "VENDOR: " << (char *)glGetString(GL_VENDOR) << endl;
	cout << "VERSION: " << (char *)glGetString(GL_VERSION) << endl;
	cout << "RENDERER: " << (char *)glGetString(GL_RENDERER) << endl;

	// Enable Opengl Debug
	//glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback((GLDEBUGPROC)openGLDebugCallback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, true);
}

static void APIENTRY openGLDebugCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const GLvoid* userParam) {

	cout << "---------------------opengl-callback------------" << endl;
	cout << "Message: " << message << endl;
	cout << "type: ";
	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		cout << "ERROR";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		cout << "DEPRECATED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		cout << "UNDEFINED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		cout << "PORTABILITY";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		cout << "PERFORMANCE";
		break;
	case GL_DEBUG_TYPE_OTHER:
		cout << "OTHER";
		break;
	}
	cout << " --- ";

	cout << "id: " << id << " --- ";
	cout << "severity: ";
	switch (severity) {
	case GL_DEBUG_SEVERITY_LOW:
		cout << "LOW";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		cout << "MEDIUM";
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		cout << "HIGH";
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		cout << "NOTIFICATION";
	}
	cout << endl;
	cout << "-----------------------------------------" << endl;
}

string readShader(string name) {
	string vs_text;
	std::ifstream vs_file(name);

	string vs_line;
	if (vs_file.is_open()) {

		while (getline(vs_file, vs_line)) {
			vs_text += vs_line;
			vs_text += '\n';
		}
		vs_file.close();
	}
	return vs_text;
}

void  checkErrorShader(GLuint shader) {
	// Get log lenght
	GLint maxLength;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

	// Init a string for it
	std::vector<GLchar> errorLog(maxLength);

	if (maxLength > 1) {
		// Get the log file
		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

		cout << "--------------Shader compilation error-------------\n";
		cout << errorLog.data();
	}
}
