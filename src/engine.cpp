#include "engine.hpp"
#include "util/scene.hpp"
#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw.h"
#include "vendor/imgui/imgui_impl_opengl3.h"
#include <iostream>
#include <memory>

using glm::vec2;
using std::endl, std::cout;
using namespace glm;

Engine::Engine() {
	this->initWindow();
	this->initMatrices();
	this->initShaders();
	this->initScene();
}

unsigned int Engine::initWindow(bool debug) {
	// glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SCALE_TO_MONITOR, true);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_FALSE);
	glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
#endif
	glfwWindowHint(GLFW_RESIZABLE, false);

	GLFWmonitor *primaryMonitor = glfwGetPrimaryMonitor();
	const GLFWvidmode *mode = glfwGetVideoMode(primaryMonitor);
	this->width = mode->width;
	this->height = mode->height;
	this->aspect = (float)width / (float)height;
	window = glfwCreateWindow(width, height, "Raymarcher", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	//	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
	//	glfwSetWindowOpacity(window, 1.0f);
	//	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //disables
	// the cursor
	this->mouse = make_unique<Mouse>(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Failed to initialize GLAD" << endl;
		return -1;
	}

	glViewport(0, 0, mode->width, mode->height);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glfwSwapInterval(1);

	// Setup Dear ImGui context
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();
	return 0;
}

void Engine::initShaders() {
	shaderManager = make_unique<ShaderManager>();
	defaultShader = this->shaderManager->loadShader(
		"../res/shaders/default.vert", "../res/shaders/default.frag", nullptr,
		"default");
	//	defaultShader.use();
	defaultShader.setMatrix4("projection", this->PROJECTION);
}

void Engine::initScene() {
	this->scene = Scene();
	scene.setShader(defaultShader);
	scene.initVAO();
	scene.initVBO();
}

void Engine::initMatrices() { modelLeft = mat4(1.0f); }

void Engine::update() {
	glfwPollEvents();
	this->mouse->update();
	if (!mouse->clicked) {
		this->influences[0] += this->mouse->deltaY;
		this->influences[1] -= this->mouse->deltaX;
	}

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();

	ImGui::NewFrame();
	ImGui::Begin("Dev Tools", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.2f, 0.4f, 0.6f, 0.5f));
	ImGuiIO &io = ImGui::GetIO();
	float fps = io.Framerate;
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Performance");
	ImGui::Separator();
	ImGui::Text("Frame Rate: %.2f FPS", fps);
	ImGui::Spacing();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Terrain Parameters");
	ImGui::Separator();
	ImGui::SliderFloat("Rotation X", &influences[0], 0.0f, 6.5f, "%.3f");
	ImGui::SliderFloat("Rotation Y", &influences[1], 0.0f, 6.5f, "%.3f");
	ImGui::SliderFloat("Rotation Z", &influences[2], 0.0f, 6.5f, "%.3f");
	ImGui::SliderFloat("X Pos", &influences[3], -5.0f, 5.0f, "%.3f");
	ImGui::Spacing();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);
	ImGui::End();

	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	time += deltaTime;
}

void Engine::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	scene.setUniforms(modelLeft, view, projection, vec2(0, 0), aspect,
					  mouse->clicked, time);
	defaultShader.setVector4f("influences", influences);
	defaultShader.use();
	scene.draw();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(window);
}

bool Engine::shouldClose() { return glfwWindowShouldClose(window); }
Engine::~Engine() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
