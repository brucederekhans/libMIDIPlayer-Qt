#ifndef QMIDIPLAYING_H
#define QMIDIPLAYING_H

#include "qmidi.h"
#include "qmiditrackheader.h"

class QMIDIPlaying : public QObject
{
    Q_OBJECT
public:
    explicit QMIDIPlaying(QObject * parent = nullptr);
    ~QMIDIPlaying();

private:
    char filename[MAX_PATH];
};

#endif