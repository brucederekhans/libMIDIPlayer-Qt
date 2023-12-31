#ifndef QMIDITRACKHEADER_H
#define QMIDITRACKHEADER_H

#include <windows.h>

namespace QMIDI{
    struct QMIDITrackHeader{
        unsigned char isEnabled;
        unsigned char isReadOnce;
        unsigned int length;
        unsigned char * data;
        unsigned char * pData;
        double deltaTime;
        double triggerTime;
        unsigned char lastCommand;
    };

    unsigned int readVLQFromMIDITrackHeader(QMIDITrackHeader * pMIDITrackHeader);
    unsigned char readByteFromMIDITrackHeader(QMIDITrackHeader * pMIDITrackHeader);
    unsigned short readUShortFromMIDITrackHeader(QMIDITrackHeader * pMIDITrackHeader);
}

#endif