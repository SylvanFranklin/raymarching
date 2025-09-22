#include "engine.hpp"

// void render() {}

int main(int argc, char *argv[]) {

	// std::thread renderThread(render);
	// renderThread.join();
	Engine engine;
	while (!engine.shouldClose()) {
		engine.update();
		engine.render();
	}

	glfwTerminate();
	return 0;
}
