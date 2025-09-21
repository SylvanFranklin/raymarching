#include "engine.hpp"
#include "juce_audio_devices/juce_audio_devices.h"

int main(int argc, char *argv[]) {

	juce::AudioDeviceManager audio = juce::AudioDeviceManager();
	Engine engine;
	while (!engine.shouldClose()) {
		engine.update();
		engine.render();
	}

	glfwTerminate();
	return 0;
}
