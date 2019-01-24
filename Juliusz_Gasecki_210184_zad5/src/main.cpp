

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader.h>
#include <camera.h>
#include <model.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);
unsigned int loadCubemap(vector<std::string> faces);

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// camera
Camera camera(glm::vec3(-1.2f, 0.0f, -0.8f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

void camToTram(Camera &camera, glm::vec3 position) {
	if (camera.Position.x != (position.x / 100) + 1.2  || camera.Position.y != (position.y / 100) + 0 || camera.Position.z != (position.z / 100) + 0.8)
	{
		camera.Position.x = position.x/100;
		camera.Position.y = position.y/100;
		camera.Position.z = position.z/100;
		camera.Position += glm::vec3(-1.2f, 0.0f, -0.8f);
	}
			
}

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

														 // glfw window creation
														 // --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	// -------------------------
	Shader buildingShader("res/shaders/budynki.vs", "res/shaders/budynki.fs");
	Shader planeShader("res/shaders/normalCubeShader.vs", "res/shaders/normalCubeShader.fs");
	Shader shader("res/shaders/cubemap1.vs", "res/shaders/cubemap2.fs");
	Shader shader2("res/shaders/cubemap1.vs", "res/shaders/cubemap1.fs");
	Shader skyboxShader("res/shaders/skybox.vs", "res/shaders/skybox.fs");

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float cubeVertices[] = {
		// positions          // normals
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};
	float verticesPlane [] {
		-1.0f, 0.0f, -1.0f, 0.4f, 0.50f, 0.5f,
		1.0f, 0.0f, -1.0f, 0.4f, 0.50f, 0.5f,
		1.0f, 0.0f, 1.0f, 0.40f, 0.50f, 0.5f,
		1.0f, 0.0f, 1.0f, 0.40f, 0.50f, 0.5f,
		-1.0f, 0.0f, 1.0f, 0.40f, 0.50f, 0.5f,
		-1.0f, 0.0f, -1.0f, 0.40f, 0.50f, 0.5f,
	};
	float verticesBuildings[] = {
		-0.5f, -0.5f, -0.5f,  0.6f, 0.30f, 0.1f,
		0.5f, -0.5f, -0.5f,  0.6f, 0.30f, 0.1f,
		0.5f,  0.5f, -0.5f,  0.6f, 0.30f, 0.1f,
		0.5f,  0.5f, -0.5f,  0.6f, 0.30f, 0.1f,
		-0.5f,  0.5f, -0.5f,  0.6f, 0.30f, 0.1f,
		-0.5f, -0.5f, -0.5f,  0.6f, 0.30f, 0.1f,

		-0.5f, -0.5f,  0.5f,  0.6f, 0.30f, 0.1f,
		0.5f, -0.5f,  0.5f,  0.6f, 0.30f, 0.1f,
		0.5f,  0.5f,  0.5f,  0.6f, 0.30f, 0.1f,
		0.5f,  0.5f,  0.5f,  0.6f, 0.30f, 0.1f,
		-0.5f,  0.5f,  0.5f,  0.6f, 0.30f, 0.1f,
		-0.5f, -0.5f,  0.5f,  0.6f, 0.30f, 0.1f,

		-0.5f,  0.5f,  0.5f, 0.6f, 0.30f, 0.1f,
		-0.5f,  0.5f, -0.5f, 0.6f, 0.30f, 0.1f,
		-0.5f, -0.5f, -0.5f, 0.6f, 0.30f, 0.1f,
		-0.5f, -0.5f, -0.5f, 0.6f, 0.30f, 0.1f,
		-0.5f, -0.5f,  0.5f, 0.6f, 0.30f, 0.1f,
		-0.5f,  0.5f,  0.5f, 0.6f, 0.30f, 0.1f,

		0.5f,  0.5f,  0.5f,  0.6f, 0.30f, 0.1f,
		0.5f,  0.5f, -0.5f,  0.6f, 0.30f, 0.1f,
		0.5f, -0.5f, -0.5f,  0.6f, 0.30f, 0.1f,
		0.5f, -0.5f, -0.5f,  0.6f, 0.30f, 0.1f,
		0.5f, -0.5f,  0.5f,  0.6f, 0.30f, 0.1f,
		0.5f,  0.5f,  0.5f,  0.6f, 0.30f, 0.1f,

		-0.5f, -0.5f, -0.5f,  0.6f, 0.30f, 0.1f,
		0.5f, -0.5f, -0.5f,  0.6f, 0.30f, 0.1f,
		0.5f, -0.5f,  0.5f,  0.6f, 0.30f, 0.1f,
		0.5f, -0.5f,  0.5f,  0.6f, 0.30f, 0.1f,
		-0.5f, -0.5f,  0.5f,  0.6f, 0.30f, 0.1f,
		-0.5f, -0.5f, -0.5f,  0.6f, 0.30f, 0.1f,

		-0.5f,  0.5f, -0.5f, 0.6f, 0.30f, 0.1f,
		0.5f,  0.5f, -0.5f,  0.6f, 0.30f, 0.1f,
		0.5f,  0.5f,  0.5f,  0.6f, 0.30f, 0.1f,
		0.5f,  0.5f,  0.5f,  0.6f, 0.30f, 0.1f,
		-0.5f,  0.5f,  0.5f,  0.6f, 0.30f, 0.1f,
		-0.5f,  0.5f, -0.5f,  0.6f, 0.30f, 0.1f
	};

	GLint modelLoc = glGetUniformLocation(shader.ID, "model");
	GLint viewLoc = glGetUniformLocation(shader.ID, "view");
	GLint projectionLoc = glGetUniformLocation(shader.ID, "projection");

	// cube VAO
	unsigned int cubeVAO, cubeVBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	// plane VAO
	unsigned int planeVBO, planeVAO;
	glGenBuffers(1, &planeVBO);
	glGenVertexArrays(1, &planeVAO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesPlane), &verticesPlane, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//Tramwaj :3
	unsigned int tramwajVAO, tramwajVBO;
	glGenVertexArrays(1, &tramwajVAO);
	glGenBuffers(1, &tramwajVBO);
	glBindVertexArray(tramwajVAO);
	glBindBuffer(GL_ARRAY_BUFFER, tramwajVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));


	unsigned int VBO, VAO;
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesBuildings), &verticesBuildings, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(VAO);
	glm::vec3 translations[20];
	int index = 0;
	float offset = 1;
	for (int y = -3; y < 2; y+= 3)
	{
		for (int x = -10; x < 10; x += 2)
		{
			glm::vec3 translation;
			translation.x = (float)x + offset;
			translation.y = 0.0f;
			translation.z = (float)y + offset;
			translations[index++] = translation;

		}
	}
	// buildings VAo
	unsigned int instanceVBO;
	glGenBuffers(1, &instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 20, &translations[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glVertexAttribDivisor(2, 1);


	// skybox VAO
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	// load textures
	// -------------
	vector<std::string> faces
	{
		"res/textures/land_bk.jpg",
		"res/textures/land_ft.jpg",
		"res/textures/land_up.jpg",
		"res/textures/land_dn.jpg",
		"res/textures/land_lf.jpg",
		"res/textures/land_rt.jpg"
	};
	unsigned int cubemapTexture = loadCubemap(faces);

	// shader configuration
	// --------------------
	shader.use();
	shader.setInt("skybox", 0);

	shader2.use();
	shader2.setInt("skybox", 0);

	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);


	glm::mat4 localTransform(1);

	//DrawCam * camModel = new DrawCam(new Model("res/models/drzwi.obj"), shader2.ID);
	//GraphNode camNode(localTransform, camModel, modelLoc, shader.ID);

	DrawModel * tramwaj = new DrawModel(new Model("res/models/tramwaj.obj"), shader.ID);
	localTransform = glm::scale(localTransform, glm::vec3(0.001f, 0.001f, 0.001f));
	GraphNode tramwajNode(localTransform, tramwaj, modelLoc, shader.ID);

	DrawModel * drzwi = new DrawModel(new Model("res/models/drzwi.obj"), shader2.ID);
	localTransform = glm::scale(localTransform, glm::vec3(0.001f, 0.001f, 0.001f));
	GraphNode drzwi2Node(localTransform, drzwi, modelLoc, shader2.ID);

	DrawModel * drzwi2 = new DrawModel(new Model("res/models/drzwi.obj"), shader2.ID);
	localTransform = glm::scale(localTransform, glm::vec3(0.001f, 0.001f, 0.001f));
	GraphNode drzwi2Node2(localTransform, drzwi2, modelLoc, shader2.ID);

	DrawModel * drzwi3 = new DrawModel(new Model("res/models/drzwi.obj"), shader2.ID);
	localTransform = glm::scale(localTransform, glm::vec3(0.001f, 0.001f, 0.001f));
	GraphNode drzwi2Node3(localTransform, drzwi3, modelLoc, shader2.ID);

	DrawModel * drzwi4 = new DrawModel(new Model("res/models/drzwi.obj"), shader2.ID);
	localTransform = glm::scale(localTransform, glm::vec3(0.001f, 0.001f, 0.001f));
	GraphNode drzwi2Node4(localTransform, drzwi4, modelLoc, shader2.ID);

	DrawModel * drzwi5 = new DrawModel(new Model("res/models/drzwi.obj"), shader2.ID);
	localTransform = glm::scale(localTransform, glm::vec3(0.001f, 0.001f, 0.001f));
	GraphNode drzwi2Node5(localTransform, drzwi5, modelLoc, shader2.ID);
	
	DrawModel * drzwi6 = new DrawModel(new Model("res/models/drzwi.obj"), shader2.ID);
	localTransform = glm::scale(localTransform, glm::vec3(0.001f, 0.001f, 0.001f));
	GraphNode drzwi2Node6(localTransform, drzwi6, modelLoc, shader2.ID);

	DrawModel * drzwi7 = new DrawModel(new Model("res/models/drzwi.obj"), shader2.ID);
	localTransform = glm::scale(localTransform, glm::vec3(0.001f, 0.001f, 0.001f));
	GraphNode drzwi2Node7(localTransform, drzwi7, modelLoc, shader2.ID);

	DrawModel * drzwi8 = new DrawModel(new Model("res/models/drzwi.obj"), shader2.ID);
	localTransform = glm::scale(localTransform, glm::vec3(0.001f, 0.001f, 0.001f));
	GraphNode drzwi2Node8(localTransform, drzwi8, modelLoc, shader2.ID);


	//tramwajNode.addChildren(&camNode);
	tramwajNode.addChildren(&drzwi2Node);
	tramwajNode.addChildren(&drzwi2Node2);
	tramwajNode.addChildren(&drzwi2Node3);
	tramwajNode.addChildren(&drzwi2Node4);
	tramwajNode.addChildren(&drzwi2Node5);
	tramwajNode.addChildren(&drzwi2Node6);
	tramwajNode.addChildren(&drzwi2Node7);
	tramwajNode.addChildren(&drzwi2Node8);

	//tramwajNode.addChildren(&lolNode);
	glm::vec3 tramwajPosition(1);
	//glm::vec3 doorPosition1 = glm::vec3(31.0f, 2.0f, -20.0f);
	//glm::vec3 doorPosition2 = glm::vec3(26.0f, 2.0f, -20.0f);
	glm::vec3 doorPosition1 = glm::vec3(31.0f, 2.0f, -20.0f);
	glm::vec3 doorPosition2 = glm::vec3(29.0f, 2.0f, -20.0f);
	glm::vec3 doorPosition7 = glm::vec3(27.0f, 2.0f, -20.0f);
	glm::vec3 doorPosition8 = glm::vec3(25.0f, 2.0f, -20.0f);
	glm::vec3 doorPosition3 = glm::vec3(-2.0f, 2.0f, -20.0f);
	glm::vec3 doorPosition4 = glm::vec3(-7.0f, 2.0f, -20.0f);
	glm::vec3 doorPosition5 = glm::vec3(-33.0f, 2.0f, -20.0f);
	glm::vec3 doorPosition6 = glm::vec3(-38.0f, 2.0f, -20.0f);


	float doorsRotate1 = 0;
	float doorsRotate2 = 0;
	float doorsRotate3 = 0;
	float doorsRotate4 = 0;

	float doorsRotate5 = 0;
	float doorsRotate6 = 0;
	float doorsRotate7 = 0;
	float doorsRotate8 = 0;

	// render loop
	// -----------
	float doorsStatez1 = 0;
	float doorsStatex1 = 0;
	float doorsStatez2 = 0;
	float doorsStatex2 = 0;
	float doorsStatez3 = 0;
	float doorsStatex3 = 0;
	bool doorsOpen = false;
	bool followTram = true;
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// draw scene as normal
		shader.use();
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		shader.setMat4("model", model);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		shader.setVec3("cameraPos", camera.Position);
		// cubes
		/*glBindVertexArray(cubeVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);*/

		//Rysowanie tramwaju

		shader2.use();
		shader.setMat4("model", model);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		shader.setVec3("cameraPos", camera.Position);
		shader.use();
		localTransform = glm::mat4(1);
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(tramwajPosition));
		//glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		//glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
			camera.Position.x = tramwajPosition.x/100;
			camera.Position.y = tramwajPosition.y/100;
			camera.Position.z = tramwajPosition.z/100;
			camera.Position += glm::vec3(-1.2f, 0.0f, -0.8f);

		}
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
			followTram = !followTram;

		}

		//model = glm::scale(model, glm::vec3(2, 0.3f, 1));
		model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
			if (followTram)
			{
				camToTram(camera, tramwajPosition);
				//camera.Position += glm::vec3(0.01f, 0.0f, -0.0f);
			}	
			tramwajPosition += glm::vec3(1.0f, 0.0f, -0.0f);
			
			
			
		}
		else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			if (followTram)
			{
				camToTram(camera, tramwajPosition);
				//camera.Position += glm::vec3(-0.01f, 0.0f, -0.0f);
			}	
			tramwajPosition += glm::vec3(-1.0f, 0.0f, -0.0f);
			
		}
		model = glm::translate(model, tramwajPosition);
		shader.setMat4("model", model);
		tramwajNode.setLocalTransform(model);
		
		////////////////////////////////////////////////////////////////////////////

		localTransform = glm::mat4(1);
		localTransform = glm::translate(localTransform, doorPosition1);
		localTransform = glm::rotate(localTransform, doorsRotate5, glm::vec3(0, 1, 0));
		localTransform = glm::scale(localTransform, glm::vec3(0.05f, 0.23f, 0.1f));

		//localTransform = glm::scale(localTransform, glm::vec3(0.1f, 0.23f, 0.1f));
		drzwi2Node.setLocalTransform(localTransform);

		localTransform = glm::mat4(1);
		localTransform = glm::translate(localTransform, doorPosition2);
		localTransform = glm::rotate(localTransform, doorsRotate6, glm::vec3(0, 1, 0));
		localTransform = glm::scale(localTransform, glm::vec3(0.05f, 0.23f, 0.1f));
		//localTransform = glm::scale(localTransform, glm::vec3(0.1f, 0.23f, 0.1f));
		drzwi2Node2.setLocalTransform(localTransform);

		localTransform = glm::mat4(1);
		localTransform = glm::translate(localTransform, doorPosition7);
		localTransform = glm::rotate(localTransform, doorsRotate7, glm::vec3(0, 1, 0));
		localTransform = glm::scale(localTransform, glm::vec3(0.05f, 0.23f, 0.1f));
		drzwi2Node7.setLocalTransform(localTransform);

		localTransform = glm::mat4(1);
		localTransform = glm::translate(localTransform, doorPosition8);
		localTransform = glm::rotate(localTransform, doorsRotate8, glm::vec3(0, 1, 0));
		localTransform = glm::scale(localTransform, glm::vec3(0.05f, 0.23f, 0.1f));
		drzwi2Node8.setLocalTransform(localTransform);

		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			doorsOpen = true;
			localTransform = glm::mat4(1);
			if (doorsStatex1 < 15) {
				localTransform = glm::mat4(1);
				doorPosition1 += glm::vec3(0.2f, 0.0f, 0.33f );
				doorPosition2 += glm::vec3(-0.33f , 0.0f, 0.33f);
				doorPosition7 += glm::vec3(0.33f , 0.0f, 0.33f);
				doorPosition8 += glm::vec3(-0.2f , 0.0f, 0.33f);
				doorsRotate5 += 0.1 ;
				doorsRotate6 += 0.05 ;
				doorsRotate7 -= 0.05 ;
				doorsRotate8 -= 0.1 ;
				doorsStatex1 += 1;
				localTransform = glm::translate(localTransform, doorPosition1);
				localTransform = glm::rotate(localTransform, doorsRotate5, glm::vec3(0, 1, 0));
				localTransform = glm::scale(localTransform, glm::vec3(0.05f, 0.23f, 0.1f));
				drzwi2Node.setLocalTransform(localTransform);

				localTransform = glm::translate(localTransform, doorPosition2);
				localTransform = glm::rotate(localTransform, doorsRotate6, glm::vec3(0, 1, 0));
				localTransform = glm::scale(localTransform, glm::vec3(0.05f, 0.23f, 0.1f));
				drzwi2Node2.setLocalTransform(localTransform);

				localTransform = glm::translate(localTransform, doorPosition7);
				localTransform = glm::rotate(localTransform, doorsRotate7, glm::vec3(0, 1, 0));
				localTransform = glm::scale(localTransform, glm::vec3(0.05f, 0.23f, 0.1f));
				drzwi2Node7.setLocalTransform(localTransform);

				localTransform = glm::mat4(1);
				localTransform = glm::translate(localTransform, doorPosition8);
				localTransform = glm::rotate(localTransform, doorsRotate8, glm::vec3(0, 1, 0));
				localTransform = glm::scale(localTransform, glm::vec3(0.05f, 0.23f, 0.1f));
				drzwi2Node8.setLocalTransform(localTransform);
			}
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			doorsOpen = true;
			localTransform = glm::mat4(1);
			if (doorsStatex1 > 0) {
				localTransform = glm::mat4(1);
				doorPosition1 += glm::vec3(-0.2f, 0.0f, -0.33f);
				doorPosition2 += glm::vec3(0.33f, 0.0f, -0.33f);
				doorPosition7 += glm::vec3(-0.33f, 0.0f, -0.33f);
				doorPosition8 += glm::vec3(0.2f, 0.0f, -0.33f);
				doorsRotate5 -= 0.1;
				doorsRotate6 -= 0.05;
				doorsRotate7 += 0.05;
				doorsRotate8 += 0.1;
				doorsStatex1 -= 1;
				localTransform = glm::translate(localTransform, doorPosition1);
				localTransform = glm::rotate(localTransform, doorsRotate5, glm::vec3(0, 1, 0));
				localTransform = glm::scale(localTransform, glm::vec3(0.05f, 0.23f, 0.1f));
				drzwi2Node.setLocalTransform(localTransform);

				localTransform = glm::translate(localTransform, doorPosition2);
				localTransform = glm::rotate(localTransform, doorsRotate6, glm::vec3(0, 1, 0));
				localTransform = glm::scale(localTransform, glm::vec3(0.05f, 0.23f, 0.1f));
				drzwi2Node2.setLocalTransform(localTransform);

				localTransform = glm::translate(localTransform, doorPosition7);
				localTransform = glm::rotate(localTransform, doorsRotate7, glm::vec3(0, 1, 0));
				localTransform = glm::scale(localTransform, glm::vec3(0.05f, 0.23f, 0.1f));
				drzwi2Node7.setLocalTransform(localTransform);

				localTransform = glm::mat4(1);
				localTransform = glm::translate(localTransform, doorPosition8);
				localTransform = glm::rotate(localTransform, doorsRotate8, glm::vec3(0, 1, 0));
				localTransform = glm::scale(localTransform, glm::vec3(0.05f, 0.23f, 0.1f));
				drzwi2Node8.setLocalTransform(localTransform);
			}

		}


		/*
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			localTransform = glm::mat4(1);
			if (doorsStatez1 >= 0 && doorsStatez1 < 5)
			{
				localTransform = glm::mat4(1);
				doorPosition1 += glm::vec3(0.0f, 0.0f, -1.0f);
				doorPosition2 += glm::vec3(0.0f, 0.0f, -1.0f);
				doorsStatez1 += 1;
				localTransform = glm::translate(localTransform, doorPosition2);
				localTransform = glm::scale(localTransform, glm::vec3(0.1f, 0.23f, 0.1f));
				drzwi2Node2.setLocalTransform(localTransform);
				localTransform = glm::mat4(1);
				localTransform = glm::translate(localTransform, doorPosition1);
				localTransform = glm::scale(localTransform, glm::vec3(0.1f, 0.23f, 0.1f));
				drzwi2Node.setLocalTransform(localTransform);
			}
			else if (doorsStatez1 >= 5 && doorsStatex1 <=2)
			{
				localTransform = glm::mat4(1);
				doorPosition1 += glm::vec3(1.0f, 0.0f, 0.0f);
				doorPosition2 += glm::vec3(-1.0f, 0.0f, 0.0f);
				doorsStatex1 += 1;
				localTransform = glm::translate(localTransform, doorPosition2);
				localTransform = glm::scale(localTransform, glm::vec3(0.1f, 0.23f, 0.1f));
				drzwi2Node2.setLocalTransform(localTransform);
				localTransform = glm::mat4(1);
				localTransform = glm::translate(localTransform, doorPosition1);
				localTransform = glm::scale(localTransform, glm::vec3(0.1f, 0.23f, 0.1f));
				drzwi2Node.setLocalTransform(localTransform);
			}
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			localTransform = glm::mat4(1);
			if (doorsStatez1 == 5 && doorsStatex1 > 0)
			{
				localTransform = glm::mat4(1);
				doorPosition1 += glm::vec3(-1.0f, 0.0f, .0f);
				doorPosition2 += glm::vec3(1.0f, 0.0f, .0f);
				doorsStatex1 -= 1;
				localTransform = glm::translate(localTransform, doorPosition2);
				localTransform = glm::scale(localTransform, glm::vec3(0.1f, 0.23f, 0.1f));
				drzwi2Node2.setLocalTransform(localTransform);
				localTransform = glm::mat4(1);
				localTransform = glm::translate(localTransform, doorPosition1);
				localTransform = glm::scale(localTransform, glm::vec3(0.1f, 0.23f, 0.1f));
				drzwi2Node.setLocalTransform(localTransform);
			}
			else if (doorsStatez1 > 0 && doorsStatex1 ==0)
			{
				localTransform = glm::mat4(1);
				doorPosition1 += glm::vec3(.0f, 0.0f, 1.0f);
				doorPosition2 += glm::vec3(.0f, 0.0f, 1.0f);
				doorsStatez1 -= 1;
				localTransform = glm::translate(localTransform, doorPosition2);
				localTransform = glm::scale(localTransform, glm::vec3(0.1f, 0.23f, 0.1f));
				drzwi2Node2.setLocalTransform(localTransform);
				localTransform = glm::mat4(1);
				localTransform = glm::translate(localTransform, doorPosition1);
				localTransform = glm::scale(localTransform, glm::vec3(0.1f, 0.23f, 0.1f));
				drzwi2Node.setLocalTransform(localTransform);
			}
		}
		*/
		
		
		//////////////////////////////////////////////////////////////////////////////

		localTransform = glm::mat4(1);
		localTransform = glm::translate(localTransform, doorPosition3);
		localTransform = glm::rotate(localTransform, doorsRotate1, glm::vec3(0,1,0));
		localTransform = glm::scale(localTransform, glm::vec3(0.1f, 0.23f, 0.1f));
		drzwi2Node3.setLocalTransform(localTransform);

		localTransform = glm::mat4(1);
		localTransform = glm::translate(localTransform, doorPosition4);
		localTransform = glm::rotate(localTransform , doorsRotate2, glm::vec3(0, 1, 0));
		localTransform = glm::scale(localTransform, glm::vec3(0.1f, 0.23f, 0.1f));
		drzwi2Node4.setLocalTransform(localTransform);

		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			doorsOpen = true;
			localTransform = glm::mat4(1);
			if (doorsStatex2 < 5) {
				localTransform = glm::mat4(1);
				doorPosition3 += glm::vec3(-0.6f, 0.0f, 1.0f);
				doorPosition4 += glm::vec3(0.6f, 0.0f, 1.0f);
				doorsRotate1 += 0.3;
				doorsRotate2 -= 0.3;
				doorsStatex2 += 1;
				localTransform = glm::translate(localTransform, doorPosition3);
				localTransform = glm::rotate(localTransform, doorsRotate1, glm::vec3(0, 1, 0));
				localTransform = glm::scale(localTransform, glm::vec3(0.1f, 0.23f, 0.1f));
				
				drzwi2Node3.setLocalTransform(localTransform);
				localTransform = glm::mat4(1);
				localTransform = glm::translate(localTransform, doorPosition4);
				localTransform = glm::rotate(localTransform, doorsRotate2, glm::vec3(0, 1, 0));
				localTransform = glm::scale(localTransform, glm::vec3(0.1f, 0.23f, 0.1f));
				drzwi2Node4.setLocalTransform(localTransform);
			}
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			doorsOpen = true;
			localTransform = glm::mat4(1);
			if (doorsStatex2 > 0) {
				localTransform = glm::mat4(1);
				doorPosition3 += glm::vec3(0.6f, 0.0f, -1.0f);
				doorPosition4 += glm::vec3(-0.6f, 0.0f, -1.0f);
				doorsRotate1 -= 0.3;
				doorsRotate2 += 0.3;
				doorsStatex2 -= 1;
				localTransform = glm::translate(localTransform, doorPosition3);
				localTransform = glm::rotate(localTransform, doorsRotate1, glm::vec3(0, 1, 0));
				localTransform = glm::scale(localTransform, glm::vec3(0.1f, 0.23f, 0.1f));

				drzwi2Node3.setLocalTransform(localTransform);
				localTransform = glm::mat4(1);
				localTransform = glm::translate(localTransform, doorPosition4);
				localTransform = glm::rotate(localTransform, doorsRotate2, glm::vec3(0, 1, 0));
				localTransform = glm::scale(localTransform, glm::vec3(0.1f, 0.23f, 0.1f));
				drzwi2Node4.setLocalTransform(localTransform);
			}

		}

		//////////////////////////////////////////////////////////////////////////////

		localTransform = glm::mat4(1);
		localTransform = glm::translate(localTransform, doorPosition5);
		localTransform = glm::rotate(localTransform, doorsRotate3, glm::vec3(0, 1, 0));
		localTransform = glm::scale(localTransform, glm::vec3(0.1f, 0.23f, 0.1f));
		drzwi2Node5.setLocalTransform(localTransform);

		localTransform = glm::mat4(1);
		localTransform = glm::translate(localTransform, doorPosition6);
		localTransform = glm::rotate(localTransform, doorsRotate4, glm::vec3(0, 1, 0));
		localTransform = glm::scale(localTransform, glm::vec3(0.1f, 0.23f, 0.1f));
		drzwi2Node6.setLocalTransform(localTransform);

		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			doorsOpen = true;
			localTransform = glm::mat4(1);
			if (doorsStatex3 < 10) {
				localTransform = glm::mat4(1);
				doorPosition5 += glm::vec3(0.05f, 0.0f, 0.5f);
				doorPosition6 += glm::vec3(-0.05f, 0.0f, 0.5f);
				doorsRotate3 += 0.15;
				doorsRotate4 -= 0.15;
				doorsStatex3 += 1;
				localTransform = glm::translate(localTransform, doorPosition5);
				localTransform = glm::rotate(localTransform, doorsRotate3, glm::vec3(0, 1, 0));
				localTransform = glm::scale(localTransform, glm::vec3(0.1f, 0.23f, 0.1f));

				drzwi2Node5.setLocalTransform(localTransform);
				localTransform = glm::mat4(1);
				localTransform = glm::translate(localTransform, doorPosition6);
				localTransform = glm::rotate(localTransform, doorsRotate4, glm::vec3(0, 1, 0));
				localTransform = glm::scale(localTransform, glm::vec3(0.1f, 0.23f, 0.1f));
				drzwi2Node6.setLocalTransform(localTransform);
			}
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			doorsOpen = true;
			localTransform = glm::mat4(1);
			if (doorsStatex3 > 0) {
				localTransform = glm::mat4(1);
				doorPosition5 += glm::vec3(-0.05f, 0.0f, -0.5f);
				doorPosition6 += glm::vec3(0.05f, 0.0f, -0.5f);
				doorsRotate3 -= 0.15;
				doorsRotate4 += 0.15;
				doorsStatex3 -= 1;
				localTransform = glm::translate(localTransform, doorPosition5);
				localTransform = glm::rotate(localTransform, doorsRotate3, glm::vec3(0, 1, 0));
				localTransform = glm::scale(localTransform, glm::vec3(0.1f, 0.23f, 0.1f));

				drzwi2Node5.setLocalTransform(localTransform);
				localTransform = glm::mat4(1);
				localTransform = glm::translate(localTransform, doorPosition6);
				localTransform = glm::rotate(localTransform, doorsRotate4, glm::vec3(0, 1, 0));
				localTransform = glm::scale(localTransform, glm::vec3(0.1f, 0.23f, 0.1f));
				drzwi2Node6.setLocalTransform(localTransform);
			}

		}
		/*
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			localTransform = glm::mat4(1);
			if (doorsStatez3 >= 0 && doorsStatez3 < 5)
			{
				localTransform = glm::mat4(1);
				doorPosition5 += glm::vec3(0.0f, 0.0f, -1.0f);
				doorPosition6 += glm::vec3(0.0f, 0.0f, -1.0f);
				doorsStatez3 += 1;
				localTransform = glm::translate(localTransform, doorPosition6);
				localTransform = glm::scale(localTransform, glm::vec3(0.1f, 0.23f, 0.1f));
				drzwi2Node5.setLocalTransform(localTransform);
				localTransform = glm::mat4(1);
				localTransform = glm::translate(localTransform, doorPosition5);
				localTransform = glm::scale(localTransform, glm::vec3(0.1f, 0.23f, 0.1f));
				drzwi2Node6.setLocalTransform(localTransform);
			}
			else if (doorsStatez3 >= 5 && doorsStatex3 <= 2)
			{
				localTransform = glm::mat4(1);
				doorPosition5 += glm::vec3(1.0f, 0.0f, 0.0f);
				doorPosition6 += glm::vec3(-1.0f, 0.0f, 0.0f);
				doorsStatex3 += 1;
				localTransform = glm::translate(localTransform, doorPosition6);
				localTransform = glm::scale(localTransform, glm::vec3(0.1f, 0.23f, 0.1f));
				drzwi2Node6.setLocalTransform(localTransform);
				localTransform = glm::mat4(1);
				localTransform = glm::translate(localTransform, doorPosition5);
				localTransform = glm::scale(localTransform, glm::vec3(0.1f, 0.23f, 0.1f));
				drzwi2Node5.setLocalTransform(localTransform);
			}
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			localTransform = glm::mat4(1);
			if (doorsStatez3 == 5 && doorsStatex3 > 0)
			{
				localTransform = glm::mat4(1);
				doorPosition5 += glm::vec3(-1.0f, 0.0f, .0f);
				doorPosition6 += glm::vec3(1.0f, 0.0f, .0f);
				doorsStatex3 -= 1;
				localTransform = glm::translate(localTransform, doorPosition6);
				localTransform = glm::scale(localTransform, glm::vec3(0.1f, 0.23f, 0.1f));
				drzwi2Node6.setLocalTransform(localTransform);
				localTransform = glm::mat4(1);
				localTransform = glm::translate(localTransform, doorPosition5);
				localTransform = glm::scale(localTransform, glm::vec3(0.1f, 0.23f, 0.1f));
				drzwi2Node5.setLocalTransform(localTransform);
			}
			else if (doorsStatez3 > 0 && doorsStatex3 == 0)
			{
				localTransform = glm::mat4(1);
				doorPosition5 += glm::vec3(.0f, 0.0f, 1.0f);
				doorPosition6 += glm::vec3(.0f, 0.0f, 1.0f);
				doorsStatez3 -= 1;
				localTransform = glm::translate(localTransform, doorPosition6);
				localTransform = glm::scale(localTransform, glm::vec3(0.1f, 0.23f, 0.1f));
				drzwi2Node6.setLocalTransform(localTransform);
				localTransform = glm::mat4(1);
				localTransform = glm::translate(localTransform, doorPosition5);
				localTransform = glm::scale(localTransform, glm::vec3(0.1f, 0.23f, 0.1f));
				drzwi2Node5.setLocalTransform(localTransform);
			}
		}
		*/
		/////////////////////////////////////////////////////////////////////////////
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(tramwaj.getTransform()));
		glBindVertexArray(tramwajVAO);
		tramwajNode.draw();
		glDrawArrays(GL_TRIANGLES, 0, 36 * 3);

		//cellingNode.draw();
		//lolNode.draw();
		//draw plane

		planeShader.use();
		planeShader.setMat4("projection", projection);
		planeShader.setMat4("view", view);
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(1, -0.5f, 1));
		model = glm::scale(model, glm::vec3(60.0f, 0.0f, 60.0f)); //
		planeShader.setMat4("model", model);
		glBindVertexArray(planeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		//draw buildings
		buildingShader.use();
		buildingShader.setMat4("projection", projection);
		buildingShader.setMat4("view", view);
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f)); 
		buildingShader.setMat4("model", model);
		glBindVertexArray(VAO);
		glDrawArraysInstanced(GL_TRIANGLES, 0, sizeof(verticesBuildings), 20); // 100 triangles of 6 vertices each
		glBindVertexArray(0);

		// draw skybox as last
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		skyboxShader.use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
		skyboxShader.setMat4("view", view);
		skyboxShader.setMat4("projection", projection);
		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // set depth function back to default


		
							  // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
							  // -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &cubeVBO);
	glDeleteBuffers(1, &skyboxVAO);

	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
// -------------------------------------------------------
unsigned int loadCubemap(vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrComponents;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
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

