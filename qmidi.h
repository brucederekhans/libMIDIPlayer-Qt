#ifndef QMIDI_H
#define QMIDI_H

#include <windows.h>
#include <mmsystem.h>

struct QMIDI{
	unsigned char isPlaying;
	unsigned short countTracks;
	unsigned short countTicks;
	unsigned int tempo;
	double tickLength;
	unsigned char channels[16][128];
	double currentTime;
};

#endif