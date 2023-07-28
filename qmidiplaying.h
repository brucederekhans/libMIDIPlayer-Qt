#ifndef QMIDIPLAYING_H
#define QMIDIPLAYING_H

#include "qmidi.h"
#include "qmiditrackheader.h"

class QMIDIPlaying : public QObject
{
    Q_OBJECT
public:
    explicit QMIDIPlaying(char * pFilename, QObject * parent = nullptr);
    ~QMIDIPlaying();

private:
    char filename[MAX_PATH];
    unsigned char volumePercentage;
};

#endif