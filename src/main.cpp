#include "engine.hpp"
#include <JuceHeader.h>

int main(int argc, char *argv[]) {

	Engine engine;
	while (!engine.shouldClose()) {
		engine.update();
		engine.render();
	}

	glfwTerminate();
	return 0;
}
