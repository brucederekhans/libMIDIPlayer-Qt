#ifndef QMIDITRACKHEADER_H
#define QMIDITRACKHEADER_H

#include <windows.h>

struct QMIDITrackHeader{
	unsigned int length;
	unsigned char * data;
	unsigned char * pData;
};

#endif