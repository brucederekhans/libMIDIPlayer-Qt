#ifndef QMIDITRACKHEADER_H
#define QMIDITRACKHEADER_H

#include <windows.h>

struct QMIDITrackHeader{
	unsigned char isEnabled;
	unsigned char isReadOnce;
	unsigned int length;
	unsigned char * data;
	unsigned char * pData;
	double tickDuration;
	double triggerTime;
	unsigned char lastCommand;
};

unsigned int readVLQFromMIDITrackHeader(QMIDITrackHeader * pMIDITrackHeader);

#endif