#include "engine.hpp"

void render(){
    Engine engine;
    while (!engine.shouldClose()) {
        engine.update();
        engine.render();
    }
}


int main(int argc, char *argv[]) {

    std::thread renderThread(render);
    renderThread.join();

	glfwTerminate();
	return 0;
}

