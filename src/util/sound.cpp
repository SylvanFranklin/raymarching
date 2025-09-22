#include "sound.hpp"

Sound::Sound() {}

Sound::~Sound() { stop(); }

bool Sound::start() {
	if (adc.getDeviceCount() < 1) {
		std::cerr << "No audio devices found!" << std::endl;
		return false;
	}

	RtAudio::StreamParameters parameters;
	parameters.deviceId = adc.getDefaultInputDevice();
	parameters.nChannels = 1; // mono for microphone
	parameters.firstChannel = 0;

	unsigned int sampleRate = 44100;
	unsigned int bufferFrames = 256;

	try {
		adc.openStream(nullptr, &parameters, RTAUDIO_FLOAT32, sampleRate,
					   &bufferFrames, &Sound::static_callback, this);
		adc.startStream();
		running = true;
	} catch (int &e) {
		std::cerr << "RtAudio error: " << e << std::endl;
		return false;
	}

	return true;
}

void Sound::stop() {
	if (adc.isStreamRunning()) {
		try {
			adc.stopStream();
		} catch (int &e) {
			std::cerr << "RtAudio error stopping: " << e << std::endl;
		}
	}
	if (adc.isStreamOpen()) {
		adc.closeStream();
	}
	running = false;
}

float Sound::getLevel() const { return current_dB.load(); }

int Sound::callback(void *outputBuffer, void *inputBuffer,
					unsigned int nBufferFrames, double streamTime,
					RtAudioStreamStatus status) {
	if (status)
		std::cerr << "Stream overflow detected!" << std::endl;

	float *input = static_cast<float *>(inputBuffer);
	double sum = 0.0;

	for (unsigned int i = 0; i < nBufferFrames; ++i) {
		float sample = input[i];
		sum += sample * sample;
	}

	double rms = std::sqrt(sum / nBufferFrames);
	double db = 20.0 * std::log10(rms + 1e-10);
	current_dB.store(static_cast<float>(db));

	return 0;
}

int Sound::static_callback(void *outputBuffer, void *inputBuffer,
						   unsigned int nBufferFrames, double streamTime,
						   RtAudioStreamStatus status, void *userData) {
	Sound *instance = static_cast<Sound *>(userData);
	return instance->callback(outputBuffer, inputBuffer, nBufferFrames,
							  streamTime, status);
}
