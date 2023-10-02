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
    void setIsPaused(unsigned char AIsPaused);

private:
    char filename[MAX_PATH];
    unsigned char isPaused;
    unsigned char isStopRequested;
    unsigned char volumePercentage;
    unsigned long long selectedOuputDeviceIndex;
    bool isSelectedOuputDeviceValid;

signals:
    void finished();

public slots:
    void execute();
};

#endif