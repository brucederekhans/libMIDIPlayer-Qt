#ifndef QMIDIPLAYING_H
#define QMIDIPLAYING_H

#include <QObject>
#include "qmidi.h"
#include "qmiditrackheader.h"

namespace QMIDI{
    class QMIDIPlaying : public QObject
    {
        Q_OBJECT
    public:
        explicit QMIDIPlaying(const wchar_t * pFilename, unsigned char AVolumePercentage, unsigned char AIsLoop, QObject * parent = nullptr);
        ~QMIDIPlaying();
        void setIsLoop(unsigned char AIsLoop);
        unsigned char getIsPaused();
        void setIsPaused(unsigned char AIsPaused);
        unsigned char getIsStopRequested();
        void setIsStopRequested(unsigned char AIsStopRequested);
        unsigned char getVolumePercentage();
        void setVolumePercentage(unsigned char AVolumePercentage);

    private:
        wchar_t filename[MAX_PATH];
        unsigned char isLoop;
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
}

#endif