#ifndef QMIDIPLAYING_H
#define QMIDIPLAYING_H

#include "qmidi.h"
#include "qmiditrackheader.h"

class QMIDIPlaying : public QObject
{
    Q_OBJECT
public:
    explicit QMIDIPlaying(char * pFilename, unsigned char AVolumePercentage, QObject * parent = nullptr);
    ~QMIDIPlaying();

private:
    char filename[MAX_PATH];
    unsigned char volumePercentage;
    unsigned long long selectedOuputDeviceIndex;
    bool isSelectedOuputDeviceValid;
    bool isTrackHeadersValid;
    HMIDIOUT hMIDIOut;
    QMIDI midi;
    QMIDITrackHeader * pMIDITrackHeaders;
};

signals:
    void finished();

public slots:
    void execute();

#endif