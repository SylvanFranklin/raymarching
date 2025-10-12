#ifndef SOUND_H
#define SOUND_H

#include <RtAudio.h>

#include <atomic>
#include <cmath>
#include <memory>
#include <vector>

class Sound {
 public:
  // simple intrusive forward list, used as a stack
  struct DataBufferNode {
    DataBufferNode *next = nullptr;
    std::vector<float> payload;

    void destroy();
  };

 public:
  static std::unique_ptr<Sound> create();
  ~Sound();

  bool start();            // start capturing
  void stop();             // stop capturing

  // don't forget to call DataBufferNode::destroy() after processing
  DataBufferNode *extractDataBufferList();

 private:
  Sound() = default;

  static int static_callback(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime,
                             RtAudioStreamStatus status, void *userData);

  int callback(void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime,
               RtAudioStreamStatus status);

 private:
  RtAudio adc;
  std::atomic<DataBufferNode *> dataBufferList{nullptr};
  bool running = false;
};

#endif  // SOUND_H
