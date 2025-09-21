// #include "engine.hpp"
//
// int main(int argc, char *argv[]) {
//
// 	Engine engine;
// 	while (!engine.shouldClose()) {
// 		engine.update();
// 		engine.render();
// 	}
//
// 	glfwTerminate();
// 	return 0;
// }
//
//
#include "vendor/rtaudio/RtAudio.h"
#include <cstdlib>
#include <cstring>
#include <iostream>

int record(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
		   double streamTime, RtAudioStreamStatus status, void *userData) {
	if (status)
		std::cout << "Stream overflow detected!" << std::endl;

	// Do something with the data in the "inputBuffer" buffer.

	return 0;
}

int main() {
	RtAudio adc;
	std::vector<unsigned int> deviceIds = adc.getDeviceIds();
	if (deviceIds.size() < 1) {
		std::cout << "\nNo audio devices found!\n";
		exit(0);
	}

	RtAudio::StreamParameters parameters;
	parameters.deviceId = adc.getDefaultInputDevice();
	parameters.nChannels = 2;
	parameters.firstChannel = 0;
	unsigned int sampleRate = 44100;
	unsigned int bufferFrames = 256; // 256 sample frames

	if (adc.openStream(NULL, &parameters, RTAUDIO_SINT16, sampleRate,
					   &bufferFrames, &record)) {
		std::cout << '\n' << adc.getErrorText() << '\n' << std::endl;
		exit(0); // problem with device settings
	}

	// Stream is open ... now start it.
	if (adc.startStream()) {
		std::cout << adc.getErrorText() << std::endl;
		goto cleanup;
	}

	char input;
	std::cout << "\nRecording ... press <enter> to quit.\n";
	std::cin.get(input);

	// Block released ... stop the stream
	if (adc.isStreamRunning())
		adc.stopStream(); // or could call adc.abortStream();

cleanup:
	if (adc.isStreamOpen())
		adc.closeStream();

	return 0;
}
