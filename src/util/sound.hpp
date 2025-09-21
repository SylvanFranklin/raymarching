#ifndef Sound_H
#define Sound_H

#include <RtAudio.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
using std::cout, std::endl;

class Sound {
  public:
	RtAudio adc;
	~Sound();
	float level = 0;
	int record();

  private:
	static int static_callback(void *outputBuffer, void *inputBuffer,
				 unsigned int nBufferFrames, double streamTime,
				 RtAudioStreamStatus status, void *userData);

	int callback(void *outputBuffer, void *inputBuffer,
				 unsigned int nBufferFrames, double streamTime,
				 RtAudioStreamStatus status);
};

#endif // Sound_H
