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

signals:
    void finished();

public slots:
    void execute();
};

#define HI_NYBBLE(byte) (((byte) & 0xF0) >> 4)
#define LO_NYBBLE(byte) ((byte) & 0x0F)

#endif