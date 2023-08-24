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
            QMIDI midi;
            memset(&midi, 0, sizeof(QMIDI));
            QMIDITrackHeader * midiTrackHeaders;
            try
            {
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

                midiTrackHeaders = new QMIDITrackHeader[midi.countTracks];
                try
                {
                    unsigned short iTrack = 0;
                    while(!feof(pMIDIFile))
                    {
                        unsigned char t4Bytes[4];
                        if(fread(t4Bytes, 1, 4, pMIDIFile) == 4)
                        {
                            if(memcmp(t4Bytes, MTrk, 4))
                            {
                                throw -6;
                            }

                            midiTrackHeaders[iTrack].isEnabled = 1;
                            midiTrackHeaders[iTrack].isReadOnce = 0;

                            if(readUIntFromMIDIFile(midiTrackHeaders[iTrack].length, pMIDIFile) != 4)
                            {
                                throw -7;
                            }

                            midiTrackHeaders[iTrack].data = new unsigned char[midiTrackHeaders[iTrack].length];

                            if(fread(midiTrackHeaders[iTrack].data, 1, midiTrackHeaders[iTrack].length, pMIDIFile) != midiTrackHeaders[iTrack].length)
                            {
                                throw -8;
                            }

                            midiTrackHeaders[iTrack].pData = midiTrackHeaders[iTrack].data;
                            midiTrackHeaders[iTrack].tickDuration = 0;
                            midiTrackHeaders[iTrack].triggerTime = 0;
                            midiTrackHeaders[iTrack].lastCommand = 0;

                            iTrack++;
                        }
                    }
                    this->isTrackHeadersValid = true;
                    midi.countTracks = iTrack;
                }
                catch(int errCode)
                {
                    delete []midiTrackHeaders;
                }

                fclose(pMIDIFile);

                HMIDIOUT hMIDIOut;
                if(midiOutOpen(&hMIDIOut, static_cast<unsigned int>(this->selectedOuputDeviceIndex), 0, 0, 0) == MMSYSERR_NOERROR)
                {
                    midi.isPlaying = 1;
                    midi.tempo = 1000;
                    midi.tickLength = midi.tempo * 1.0 / midi.countTicks;

                    double tCurrentTime = getHighResolutionTime();
                    midi.currentTime = tCurrentTime;

                    while(!midi.isStopRequested)
                    {
                        tCurrentTime = getHighResolutionTime();
                    }
                }
            }
            catch(int errCode)
            {
                fclose(pMIDIFile);
            }
        }
    }

    emit this->finished();
}