#include "sound.hpp"

std::unique_ptr<Sound> Sound::create() { return std::unique_ptr<Sound>{new Sound}; }

Sound::~Sound() { stop(); }

bool Sound::start() {
  if (adc.getDeviceCount() < 1) {
    std::cerr << "No audio devices found!" << std::endl;
    return false;
  }

  RtAudio::StreamParameters parameters;
  parameters.deviceId = adc.getDefaultInputDevice();
  parameters.nChannels = 1;  // mono for microphone
  parameters.firstChannel = 0;

  unsigned int sampleRate = 44100;
  unsigned int bufferFrames = 256;

  try {
    adc.openStream(nullptr, &parameters, RTAUDIO_FLOAT32, sampleRate, &bufferFrames, &Sound::static_callback, this);
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

Sound::DataBufferNode *Sound::extractDataBufferList() {
  // the following is a stack "extraction" operation for external use
  // leaves current stack head as nullptr, ready to be filled with data again
  DataBufferNode *oldList = dataBufferList.load(std::memory_order_relaxed);

  while (oldList != nullptr && !dataBufferList.compare_exchange_weak(oldList, nullptr, std::memory_order_acquire,
                                                                     std::memory_order_relaxed)) {
  }

  return oldList;
}

int Sound::callback(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime,
                    RtAudioStreamStatus status) {
  if (status) std::cerr << "Stream overflow detected!" << std::endl;

  float *input = static_cast<float *>(inputBuffer);

  // first alloc for Node
  auto *bufferNode = new DataBufferNode{
      /*next=*/nullptr,
      /*payload=*/std::vector<float>(),
  };
  // another alloc for a vector of nBufferFrames
  bufferNode->payload.assign(input, input + nBufferFrames);
  // could be reduced to a single alloc, but would introduce funny type erasure

  // following is a lock-free stack push impl
  bufferNode->next = dataBufferList.load(std::memory_order_relaxed);
  while (!dataBufferList.compare_exchange_weak(bufferNode->next, bufferNode, std::memory_order_release,
                                               std::memory_order_relaxed)) {
  }

  return 0;
}

int Sound::static_callback(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime,
                           RtAudioStreamStatus status, void *userData) {
  Sound *instance = static_cast<Sound *>(userData);
  return instance->callback(outputBuffer, inputBuffer, nBufferFrames, streamTime, status);
}

void Sound::DataBufferNode::destroy() {
  if (next != nullptr) {
    next->destroy();
  }
  delete this;
}
