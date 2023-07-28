#include "qmidiplaying.h"

QMIDIPlaying::QMIDIPlaying(char * pFilename, QObject * parent) :
    QObject(parent)
{
    strcpy(this->filename, pFilename);
}

QMIDIPlaying::~QMIDIPlaying()
{
    //
}