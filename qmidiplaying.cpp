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

                if(memcmp(t4Bytes, MThd, 4))
                {
                    throw -2;
                }

                if(fseek(pMIDIFile, 6, SEEK_CUR) != 0)
                {
                    throw -3;
                }

                if(readUShortFromMIDIFile(&midi.countTracks, pMIDIFile) != 2)
                {
                    throw -4;
                }

                if(readUShortFromMIDIFile(&midi.countTicks, pMIDIFile) != 2)
                {
                    throw -5;
                }

                unsigned short iTrack = 0;
                while(!feof(pMIDIFile))
                {
                    unsigned char t4Bytes[4];
                    if(fread(t4Bytes, 1, 4, pMIDIFile) != 4)
                    {
                        throw -6;
                    }
                }

                fclose(pMIDIFile);
            }
            catch(int errCode)
            {
                fclose(pMIDIFile);
            }
        }
    }

    emit this->finished();
}