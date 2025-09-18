#pragma once
#include "juce_audio_utils/juce_audio_utils.h"

class SystemAudioCapture : public juce::AudioIODeviceCallback {
  public:
	SystemAudioCapture() { setupAudioDevices(); }
	~SystemAudioCapture() { deviceManager.closeAudioDevice(); }
	void audioDeviceIOCallback(const float **inputChannelData,
							   int numInputChannels, float **outputChannelData,
							   int numOutputChannels, int numSamples) {

		// Clear output (we're only capturing, not playing)
		for (int i = 0; i < numOutputChannels; ++i) {
			if (outputChannelData[i] != nullptr)
				juce::FloatVectorOperations::clear(outputChannelData[i],
												   numSamples);
		}

		// Process input audio data
		if (numInputChannels > 0 && inputChannelData[0] != nullptr) {
			processAudioData(inputChannelData, numInputChannels, numSamples);
		}
	}

	void audioDeviceAboutToStart(juce::AudioIODevice *device) override {
		sampleRate = device->getCurrentSampleRate();
		bufferSize = device->getCurrentBufferSizeSamples();

		std::cout << ("Audio device started - Sample rate: " +
					  juce::String(sampleRate) +
					  " Buffer size: " + juce::String(bufferSize))
				  << std::endl;
	}

	void audioDeviceStopped() override {
		std::cout << "Audio device stopped" << std::endl;
	}

	void audioDeviceError(const juce::String &errorMessage) override {
		DBG("Audio device error: " + errorMessage);
	}

	float getCurrentLevel() const { return currentLevel.load(); }

	void startCapture() {
		if (auto *device = deviceManager.getCurrentAudioDevice()) {
			device->start(this);
			isCapturing = true;
		}
	}

	void stopCapture() {
		if (auto *device = deviceManager.getCurrentAudioDevice()) {
			device->stop();
			isCapturing = false;
		}
	}

	bool isCaptureActive() const { return isCapturing; }

  private:
	juce::AudioDeviceManager deviceManager;
	double sampleRate = 44100.0;
	int bufferSize = 512;
	std::atomic<float> currentLevel{0.0f};
	bool isCapturing = false;

	void setupAudioDevices() {
		// Get all available audio device types
		juce::OwnedArray<juce::AudioIODeviceType> types;
		deviceManager.createAudioDeviceTypes(types);

		juce::String targetInputDevice;

		for (int i = 0; i < types.size(); ++i) {
			juce::String typeName = types[i]->getTypeName();
			DBG("Found audio driver type: " + typeName);

			types[i]->scanForDevices();
			juce::StringArray deviceNames = types[i]->getDeviceNames();

			for (int j = 0; j < deviceNames.size(); ++j) {
				juce::String deviceName = deviceNames[j];
				DBG("  Device: " + deviceName);

				// Look for system audio capture devices based on platform
				if (isSystemAudioDevice(deviceName)) {
					targetInputDevice = deviceName;
					DBG("Found system audio device: " + targetInputDevice);

					// Set up the device manager with this input device
					juce::AudioDeviceManager::AudioDeviceSetup setup;
					setup.inputDeviceName = targetInputDevice;
					setup.outputDeviceName = ""; // No output needed
					setup.inputChannels.setRange(0, 2, true); // Stereo input
					setup.outputChannels.clear();			  // No output
					setup.sampleRate = 44100;
					setup.bufferSize = 512;
					setup.useDefaultInputChannels = true;
					setup.useDefaultOutputChannels = false;

					juce::String error = deviceManager.initialise(
						2, 0, nullptr, true, targetInputDevice, &setup);
					if (error.isEmpty()) {
						DBG("Successfully initialized audio device");
						return;
					} else {
						DBG("Failed to initialize device: " + error);
					}
				}
			}
		}

		// Fallback: try to initialize with default input device
		if (targetInputDevice.isEmpty()) {
			DBG("No system audio device found, trying default input...");
			juce::String error = deviceManager.initialise(2, 0, nullptr, true);
			if (!error.isEmpty()) {
				DBG("Failed to initialize default audio device: " + error);
			}
		}
	}

	bool isSystemAudioDevice(const juce::String &deviceName) const {
// Platform-specific device name detection
#if JUCE_MAC
		return deviceName.containsIgnoreCase("soundflower") ||
			   deviceName.containsIgnoreCase("blackhole") ||
			   deviceName.containsIgnoreCase("loopback");
#elif JUCE_WINDOWS
		return deviceName.containsIgnoreCase("stereo mix") ||
			   deviceName.containsIgnoreCase("what u hear") ||
			   deviceName.containsIgnoreCase("wave out mix");
#elif JUCE_LINUX
		return deviceName.containsIgnoreCase("monitor") ||
			   deviceName.containsIgnoreCase("analog-stereo");
#else
		return false;
#endif
	}

	void processAudioData(const float **inputChannelData, int numInputChannels,
						  int numSamples) {
		// Calculate RMS level for visualization
		float rms = 0.0f;

		for (int channel = 0; channel < numInputChannels; ++channel) {
			if (inputChannelData[channel] != nullptr) {
				for (int sample = 0; sample < numSamples; ++sample) {
					float sampleValue = inputChannelData[channel][sample];
					rms += sampleValue * sampleValue;
				}
			}
		}

		rms = std::sqrt(rms / (numSamples * numInputChannels));
		currentLevel.store(rms);

		// Add your audio analysis code here!
		// Examples:
		// - FFT analysis for frequency content
		// - Peak detection
		// - Onset detection
		// - Feature extraction

		analyzeAudio(inputChannelData, numInputChannels, numSamples);
	}

	void analyzeAudio(const float **inputChannelData, int numInputChannels,
					  int numSamples) {
		// This is where you'd add your specific analysis
		// For example, basic peak detection:

		static float peakHold = 0.0f;
		float currentPeak = 0.0f;

		for (int channel = 0; channel < numInputChannels; ++channel) {
			if (inputChannelData[channel] != nullptr) {
				for (int sample = 0; sample < numSamples; ++sample) {
					float absValue =
						std::abs(inputChannelData[channel][sample]);
					if (absValue > currentPeak)
						currentPeak = absValue;
				}
			}
		}

		if (currentPeak > peakHold)
			peakHold = currentPeak;
		else
			peakHold *= 0.999f; // Slow decay

		// You could also do FFT analysis here:
		// fft.performFrequencyOnlyForwardTransform(fftData);
	}
};

// Example usage in your main component:
class MainComponent : public juce::Component, public juce::Timer {
  public:
	MainComponent() {
		addAndMakeVisible(startButton);
		addAndMakeVisible(stopButton);
		addAndMakeVisible(levelLabel);

		startButton.onClick = [this] { audioCapture.startCapture(); };
		stopButton.onClick = [this] { audioCapture.stopCapture(); };

		startTimer(50); // Update UI at 20fps
		setSize(400, 200);
	}

	void paint(juce::Graphics &g) override {
		g.fillAll(juce::Colours::darkgrey);
	}

	void resized() override {
		auto bounds = getLocalBounds().reduced(10);
		startButton.setBounds(bounds.removeFromTop(30));
		bounds.removeFromTop(10);
		stopButton.setBounds(bounds.removeFromTop(30));
		bounds.removeFromTop(10);
		levelLabel.setBounds(bounds.removeFromTop(30));
	}

  private:
	SystemAudioCapture audioCapture;
	juce::TextButton startButton{"Start Capture"};
	juce::TextButton stopButton{"Stop Capture"};
	juce::Label levelLabel;

	void timerCallback() override {
		float level = audioCapture.getCurrentLevel();
		levelLabel.setText("Level: " + juce::String(level, 3),
						   juce::dontSendNotification);
	}
};
