#include "qmidiplaying.h"

QMIDIPlaying::QMIDIPlaying(char * pFilename, unsigned char AVolumePercentage, QObject * parent) :
    QObject(parent),
    volumePercentage(AVolumePercentage),
    selectedOuputDeviceIndex(0),
    isSelectedOuputDeviceValid(false),
    isTrackHeadersValid(false)
{
    strcpy(this->filename, pFilename);
}

QMIDIPlaying::~QMIDIPlaying()
{
    //
}