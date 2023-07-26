#ifndef QMIDITRACKHEADER_H
#define QMIDITRACKHEADER_H

#include <windows.h>

struct QMIDITrackHeader{
	unsigned char isEnabled;
	unsigned int length;
	unsigned char * data;
	unsigned char * pData;
	double triggerTime;
};

#endif