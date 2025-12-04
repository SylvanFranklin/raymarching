#include "engine.hpp"
using namespace std;

#include <fstream>
#include <iostream>
#include <memory>
#include <numeric>

#include "kiss_fft.h"
#include "util/scene.hpp"
#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw.h"
#include "vendor/imgui/imgui_impl_opengl3.h"

kiss_fft_cfg fwd = kiss_fft_alloc(256, 0, NULL, NULL);
const int nfft = 256;

Engine::Engine() : sound{Sound::create()} {
  this->initWindow();
  this->initMatrices();
  this->initShaders();
  this->initScene();
  if (!sound->start()) {
    std::cerr << "Failed to start audio capture" << std::endl;
  }

  audioState.fftOutput.assign(nfft, std::complex<float>{});
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
  this->mouse = std::make_unique<Mouse>(window);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
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
  shaderManager = std::make_unique<ShaderManager>();
  defaultShader =
      this->shaderManager->loadShader("../res/shaders/default.vert", "../res/shaders/default.frag", nullptr, "default");
  //	defaultShader.use();
  defaultShader.setMatrix4("projection", this->PROJECTION);
}

void Engine::initScene() {
  this->scene = Scene();
  scene.setShader(defaultShader);
  scene.initVAO();
  scene.initVBO();
}

void Engine::initMatrices() { modelLeft = glm::mat4(1.0f); }

void Engine::update() {
  if (pulseUp) {
    pulse += deltaTime;
    if (pulse >= 1) {
      pulseUp = false;
    }
  } else {
    pulse -= deltaTime;
    if (pulse <= 0) {
      pulseUp = true;
    }
  }

  glfwPollEvents();
  this->mouse->update();
  if (!mouse->clicked) {
    this->influences[0] += this->mouse->deltaY;
    this->influences[1] -= this->mouse->deltaX;
  }

  // sound processing
  if (auto *soundBufferList = sound->extractDataBufferList()) {
    constexpr auto reverse = [](Sound::DataBufferNode *head) {
      Sound::DataBufferNode *prev = nullptr;
      Sound::DataBufferNode *node = head;
      while (node != nullptr) {
        auto *next = node->next;
        node->next = prev;
        prev = node;
        node = next;
      }
      return prev;
    };

    constexpr auto forEach = [](Sound::DataBufferNode *node, auto f) {
      while (node != nullptr) {
        auto *next = node->next;
        f(*node);
        node = next;
      }
    };

    forEach(reverse(soundBufferList), [&](Sound::DataBufferNode &node) {
      if (audioState.buffer.empty()) {
        audioState.buffer = std::move(node.payload);
      } else {
        audioState.buffer.insert(audioState.buffer.end(), node.payload.begin(), node.payload.end());
      }
    });
    soundBufferList->destroy();

    const size_t requiredSize = nfft * 2;
    const size_t countToErase = audioState.buffer.size() > requiredSize ? audioState.buffer.size() - requiredSize : 0;
    audioState.buffer.erase(audioState.buffer.begin(), std::next(audioState.buffer.begin(), countToErase));

    if (!audioState.buffer.empty()) {
      const float audioBufferSum = std::accumulate(audioState.buffer.begin(), audioState.buffer.end(), 0.f,
                                                   [](float acc, float x) { return acc + (x * x); });
      const float rms = std::sqrt(audioBufferSum / audioState.buffer.size());
      const float db = 20.0 * std::log10(rms + 1e-10f);
      audioState.level = db;
    }
  }

  if (audioState.buffer.size() >= nfft * 2) {
    kiss_fft(fwd, (kiss_fft_cpx *)audioState.buffer.data(), (kiss_fft_cpx *)audioState.fftOutput.data());
    for (int k = 0; k < nfft; ++k) {
      audioState.fftOutput[k] = audioState.fftOutput[k] * conj(audioState.fftOutput[k]);
      audioState.fftOutput[k] *= 1. / nfft;
    }
    glm::vec4 smaller_buckets = glm::vec4(0.0);
    const int buckets = nfft / 64;

    for (int i = 0; i < buckets; i++) {
      int start = (64 * i);
      if (i == 0) start = 1;  // Skip DC

      for (int j = start; j < (64 * (i + 1)); j++) {
        smaller_buckets[i] += audioState.fftOutput[j].real();
      }
      // Log scale
      smaller_buckets[i] = std::log10(smaller_buckets[i] + 1e-10f);
    }
    this->frequencies = smaller_buckets;
    // for (int i = 0; i < 4; ++i) {
    //   cout << frequencies[i] << " ";
    // }
    // cout << endl;
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
  ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Dope Parameters");
  ImGui::Separator();
  ImGui::SliderFloat("Inactive1", &influences[0], 0.0f, 6.5f, "%.3f");
  ImGui::SliderFloat("Inactive2", &influences[1], 0.0f, 6.5f, "%.3f");
  ImGui::SliderFloat("Mirror Balls Location", &influences[2], 0.0f, 6.5f, "%.3f");
  ImGui::SliderFloat("Mirror Balls Size", &influences[3], 0.0f, 5.0f, "%.3f");

  if (ImGui::Button("SAVE")) {
    this->save();
  }

  ImGui::Checkbox("AUDIO DEBUG VIEW", &audioDebugComponent.isShown);
  if (audioDebugComponent.isShown) {
    ImGui::PlotLines("waveform", audioState.buffer.data(), static_cast<int>(audioState.buffer.size()), 0, nullptr, 0,
                     audioDebugComponent.scale, ImVec2(0, 100));
    ImGui::PlotLines("fft real", (float *)audioState.fftOutput.data(), static_cast<int>(audioState.fftOutput.size()),
                     /*values_offset=*/0, nullptr, 0, audioDebugComponent.scale, ImVec2(0, 100),
                     /*stride=*/sizeof(float) * 2);
    ImGui::PlotLines("fft imag", (float *)audioState.fftOutput.data(), static_cast<int>(audioState.fftOutput.size()),
                     /*values_offset=*/1, nullptr, 0, audioDebugComponent.scale, ImVec2(0, 100),
                     /*stride=*/sizeof(float) * 2);
    ImGui::SliderFloat("scale", &audioDebugComponent.scale, 0.0f, 1.0f);
  }

  ImGui::Spacing();
  ImGui::PopStyleColor();
  ImGui::PopStyleVar(2);
  ImGui::End();

  float currentFrame = glfwGetTime();
  deltaTime = currentFrame - lastFrame;
  lastFrame = currentFrame;
  time += deltaTime;

  //    std::cout << "Live dB: " << sound->getLevel() << std::endl;
}

void Engine::render() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  scene.setUniforms(modelLeft, view, projection, glm::vec2(0, 0), aspect, mouse->clicked, time, pulse, audioState.level,
                    this->frequencies);
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

  if (audioThread.joinable()) audioThread.join();
}

void Engine::save() {
  std::cout << "saved" << std::endl;
  std::ofstream fwriter("../saves/this.frag");
  std::ofstream vwriter("../saves/this.vert");
  std::ofstream uwriter("../saves/uniforms.text");
  fwriter << this->shaderManager->fragmentCode;
  vwriter << this->shaderManager->vertexCode;
  fwriter.close();
  vwriter.close();
}
