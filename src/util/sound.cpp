#include "sound.hpp"

Sound::~Sound() {
	if (adc.isStreamRunning())
		adc.stopStream();
}

int Sound::record() {
	RtAudio adc;

	std::vector<unsigned int> deviceIds = adc.getDeviceIds();
	if (deviceIds.size() < 1) {
		std::cout << "\nNo audio devices found!\n";
		exit(0);
	}

	RtAudio::StreamParameters parameters;
	parameters.deviceId = adc.getDefaultInputDevice();

	// for (auto st : adc.getDeviceNames()) {
	// 	cout << st << endl;
	// }

	parameters.nChannels = 2;
	parameters.firstChannel = 0;
	unsigned int sampleRate = 44100;
	unsigned int bufferFrames = 256; // 256 sample frames

	if (adc.openStream(NULL, &parameters, RTAUDIO_FLOAT32, sampleRate,
					   &bufferFrames, static_callback, this)) {
		std::cout << '\n' << adc.getErrorText() << '\n' << std::endl;
		exit(0);
	}
	// Stream is open ... now start it.
	if (adc.startStream()) {
		std::cout << adc.getErrorText() << std::endl;
		if (adc.isStreamOpen())
			adc.closeStream();
	}

	// char input;
	// std::cout << "\nRecording ... press <enter> to quit.\n";
	// std::cin.get(input);

	// Block released ... stop the stream

	return 0;
}

int Sound::callback(void *outputBuffer, void *inputBuffer,
					unsigned int nBufferFrames, double streamTime,
					RtAudioStreamStatus status) {
	if (status)
		std::cout << "Stream overflow detected!" << std::endl;

	// std::cout << nBufferFrames << " ";
	// std::cout << outputBuffer << std::endl;
	float *input = static_cast<float *>(inputBuffer);
	double sum = 0.0;

	for (unsigned int i = 0; i < nBufferFrames; ++i) {
		float sample = input[i];
		sum += sample * sample;
	}

	double rms = std::sqrt(sum / nBufferFrames);
	double db = 20.0 * std::log10(rms + 1e-10);
	cout << db << endl;

	return 0;
}

int Sound::static_callback(void *outputBuffer, void *inputBuffer,
						   unsigned int nBufferFrames, double streamTime,
						   RtAudioStreamStatus status, void *userData) {

	Sound *instance = static_cast<Sound *>(userData);
	return instance->callback(outputBuffer, inputBuffer, nBufferFrames,
							  streamTime, status);
}
