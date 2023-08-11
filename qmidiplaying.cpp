#include "qmidiplaying.h"

QMIDIPlaying::QMIDIPlaying(char * pFilename, unsigned char AVolumePercentage, QObject * parent) :
    QObject(parent),
    volumePercentage(AVolumePercentage),
    selectedOuputDeviceIndex(0),
    isSelectedOuputDeviceValid(false),
    isTrackHeadersValid(false)
{
    strcpy(this->filename, pFilename);
    unsigned long long i, countMIDIOutDevices = midiOutGetNumDevs();
    for(i = 0; i < countMIDIOutDevices; i++)
    {
        MIDIOUTCAPS midiOutCaps;
        if(midiOutGetDevCaps(i, &midiOutCaps, sizeof(MIDIOUTCAPS)) == MMSYSERR_NOERROR)
        {
            this->selectedOuputDeviceIndex = i;
            this->isSelectedOuputDeviceValid = true;
            break;
        }
    }
}

QMIDIPlaying::~QMIDIPlaying()
{
    //
}

void QMIDIPlaying::execute()
{
    if(this->isSelectedOuputDeviceValid)
    {
        FILE * pMIDIFile;
        fopen_s(&pMIDIFile, this->filename, "rb");
        if(pMIDIFile != nullptr)
        {
            try
            {
                QMIDI midi;
                memset(&midi, 0, sizeof(QMIDI));
                unsigned char t4Bytes[4];
                if(fread(t4Bytes, 1, 4, pMIDIFile) != 4)
                {
                    throw -1;
                }

                if(!memcmp(t4Bytes, MThd, 4))
                {
                    fseek(pMIDIFile, 6, SEEK_CUR);
                    readUShortFromMIDIFile(&midi.countTracks, pMIDIFile);
                }

                fclose(pMIDIFile);
            }
            catch(int errCode)
            {
                //
            }
        }
    }

    emit this->finished();
}