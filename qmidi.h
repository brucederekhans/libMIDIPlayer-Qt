#ifndef QMIDI_H
#define QMIDI_H

#include <windows.h>
#include <mmsystem.h>

struct QMIDI{
    unsigned char isPlaying;
    unsigned char isPaused;
    unsigned char isStopRequested;
    unsigned short countTracks;
    unsigned short countTicks;
    unsigned int tempo;
    double tickLength;
    unsigned char channels[16][128];
    double currentTime;
};

extern const char MThd[5];

unsigned short readUShortFromMIDIFile(FILE * pMIDIFile);

#endif