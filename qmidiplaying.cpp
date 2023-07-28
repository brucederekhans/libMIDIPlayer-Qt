#include "qmidiplaying.h"

QMIDIPlaying::QMIDIPlaying(char * pFilename, unsigned char AVolumePercentage, QObject * parent) :
    QObject(parent),
    volumePercentage(AVolumePercentage)
{
    strcpy(this->filename, pFilename);
}

QMIDIPlaying::~QMIDIPlaying()
{
    //
}