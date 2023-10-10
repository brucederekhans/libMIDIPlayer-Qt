#include "qmidiplaying.h"

QMIDIPlaying::QMIDIPlaying(char * pFilename, unsigned char AVolumePercentage, QObject * parent) :
    QObject(parent),
    isPaused(0),
    isStopRequested(0),
    volumePercentage(AVolumePercentage),
    selectedOuputDeviceIndex(0),
    isSelectedOuputDeviceValid(false)
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

void QMIDIPlaying::setIsPaused(unsigned char AIsPaused)
{
    if(this->isSelectedOuputDeviceValid)
    {
        this->isPaused = AIsPaused;
    }
}

void QMIDIPlaying::setIsStopRequested(unsigned char AIsStopRequested)
{
    if(this->isSelectedOuputDeviceValid)
    {
        this->isStopRequested = AIsStopRequested;
    }
}

void QMIDIPlaying::setVolumePercentage(unsigned char AVolumePercentage)
{
    if(this->isSelectedOuputDeviceValid)
    {
        this->volumePercentage = AVolumePercentage;
    }
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
                unsigned char t4BytesBufferMThd[4];
                if(fread(t4BytesBufferMThd, 1, 4, pMIDIFile) != 4)
                {
                    throw -1;
                }

                if(memcmp(t4BytesBufferMThd, MThd, 4))
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
                unsigned short iTrack = 0;
                try
                {
                    while(!feof(pMIDIFile))
                    {
                        unsigned char t4BytesBufferMTrk[4];
                        if(fread(t4BytesBufferMTrk, 1, 4, pMIDIFile) == 4)
                        {
                            if(memcmp(t4BytesBufferMTrk, MTrk, 4))
                            {
                                throw -6;
                            }

                            midiTrackHeaders[iTrack].isEnabled = 1;
                            midiTrackHeaders[iTrack].isReadOnce = 0;

                            if(readUIntFromMIDIFile(&midiTrackHeaders[iTrack].length, pMIDIFile) != 4)
                            {
                                throw -7;
                            }

                            midiTrackHeaders[iTrack].data = new unsigned char[midiTrackHeaders[iTrack].length];

                            if(fread(midiTrackHeaders[iTrack].data, 1, midiTrackHeaders[iTrack].length, pMIDIFile) != midiTrackHeaders[iTrack].length)
                            {
                                throw -8;
                            }

                            midiTrackHeaders[iTrack].pData = midiTrackHeaders[iTrack].data;
                            midiTrackHeaders[iTrack].deltaTime = 0;
                            midiTrackHeaders[iTrack].triggerTime = 0;
                            midiTrackHeaders[iTrack].lastCommand = 0;

                            iTrack++;
                        }
                    }
                    midi.countTracks = iTrack;

                    fclose(pMIDIFile);

                    HMIDIOUT hMIDIOut;
                    if(midiOutOpen(&hMIDIOut, static_cast<unsigned int>(this->selectedOuputDeviceIndex), 0, 0, 0) == MMSYSERR_NOERROR)
                    {
                        midi.tempo = 1000;
                        midi.tickLength = midi.tempo * 1.0 / midi.countTicks;

                        midi.currentTime = getHighResolutionTime();

                        double tCurrentTime;
                        while(!this->isStopRequested)
                        {
                            tCurrentTime = getHighResolutionTime();
                            double nextTriggerTime = tCurrentTime + 10;
                            unsigned char isAnyTrackEnabled = 0;
                            unsigned short jTrack;
                            for(jTrack = 0; jTrack < midi.countTracks; jTrack++)
                            {
                                if(midiTrackHeaders[jTrack].isEnabled)
                                {
                                    isAnyTrackEnabled = 1;
                                    if(midiTrackHeaders[jTrack].pData < (midiTrackHeaders[jTrack].data + midiTrackHeaders[jTrack].length))
                                    {
                                        if(midiTrackHeaders[jTrack].isReadOnce == 0)
                                        {
                                            midiTrackHeaders[jTrack].isReadOnce = 1;
                                            midiTrackHeaders[jTrack].deltaTime = readVLQFromMIDITrackHeader(&midiTrackHeaders[jTrack]) * midi.tickLength;
                                            midiTrackHeaders[jTrack].triggerTime = tCurrentTime + midiTrackHeaders[jTrack].deltaTime;
                                        }

                                        while(tCurrentTime >= midiTrackHeaders[jTrack].triggerTime)
                                        {
                                            midi.currentTime = tCurrentTime;

                                            unsigned char command = readByteFromMIDITrackHeader(&midiTrackHeaders[jTrack]);
                                            if(command < 128)
                                            {
                                                midiTrackHeaders[jTrack].pData--;
                                                command = midiTrackHeaders[jTrack].lastCommand;
                                            }
                                            else
                                            {
                                                midiTrackHeaders[jTrack].lastCommand = command;
                                            }

                                            if(command == 0xFF)
                                            {
                                                command = readByteFromMIDITrackHeader(&midiTrackHeaders[jTrack]);
                                                unsigned int tLength = readVLQFromMIDITrackHeader(&midiTrackHeaders[jTrack]);
                                                if(tLength <= (midiTrackHeaders[jTrack].pData - midiTrackHeaders[jTrack].data + midiTrackHeaders[jTrack].length))
                                                {
                                                    if(command == 0x0)
                                                    {
                                                        midiTrackHeaders[jTrack].pData += 2;
                                                    }
                                                    else if(command <= 0x07)
                                                    {
                                                        midiTrackHeaders[jTrack].pData += tLength;
                                                    }
                                                    else if(command == 0x20)
                                                    {
                                                        midiTrackHeaders[jTrack].pData++;
                                                    }
                                                    else if(command == 0x21)
                                                    {
                                                        midiTrackHeaders[jTrack].pData++;
                                                    }
                                                    else if(command == 0x2F)
                                                    {
                                                        midiTrackHeaders[jTrack].isEnabled = 0;
                                                    }
                                                    else if(command == 0x51)
                                                    {
                                                        unsigned char t3Bytes[3];
                                                        memcpy(t3Bytes, midiTrackHeaders[jTrack].pData, 3);
                                                        midiTrackHeaders[jTrack].pData += 3;
                                                        double newTempo = ((t3Bytes[0] << 16) + (t3Bytes[1] << 8) + t3Bytes[2]) / 1000;
                                                        double orgTickLength = midi.tickLength;
                                                        midi.tickLength = newTempo / midi.countTicks;

                                                        unsigned short kTrack;
                                                        for(kTrack = 0; kTrack < midi.countTracks; kTrack++)
                                                        {
                                                            if(midiTrackHeaders[kTrack].isEnabled)
                                                            {
                                                                midiTrackHeaders[kTrack].deltaTime = (midiTrackHeaders[kTrack].triggerTime - midi.currentTime) / orgTickLength * midi.tickLength;
                                                                midiTrackHeaders[kTrack].triggerTime = midi.currentTime + midiTrackHeaders[kTrack].deltaTime;
                                                            }
                                                        }
                                                    }
                                                    else if(command == 0x54)
                                                    {
                                                        midiTrackHeaders[jTrack].pData += 5;
                                                    }
                                                    else if(command == 0x58)
                                                    {
                                                        midiTrackHeaders[jTrack].pData += 4;
                                                    }
                                                    else if(command == 0x59)
                                                    {
                                                        midiTrackHeaders[jTrack].pData += 2;
                                                    }
                                                    else if(command == 0x0F)
                                                    {
                                                        midiTrackHeaders[jTrack].pData += tLength;
                                                    }
                                                    else
                                                    {
                                                        midiTrackHeaders[jTrack].pData += tLength;
                                                    }
                                                }
                                            }
                                            else
                                            {
                                                unsigned char hiNybble = HI_NYBBLE(command);
                                                unsigned char loNybble = LO_NYBBLE(command);
                                                if(hiNybble == 0x08)
                                                {
                                                    unsigned char key = readByteFromMIDITrackHeader(&midiTrackHeaders[jTrack]);
                                                    unsigned char velocity = readByteFromMIDITrackHeader(&midiTrackHeaders[jTrack]);
                                                    setNoteOnOff(0, key, velocity, this->volumePercentage, loNybble, &midi, &hMIDIOut);
                                                }
                                                else if(hiNybble == 0x09)
                                                {
                                                    unsigned char key = readByteFromMIDITrackHeader(&midiTrackHeaders[jTrack]);
                                                    unsigned char velocity = readByteFromMIDITrackHeader(&midiTrackHeaders[jTrack]);
                                                    setNoteOnOff(1, key, velocity, this->volumePercentage, loNybble, &midi, &hMIDIOut);
                                                }
                                                else if(hiNybble == 0x0A)
                                                {
                                                    unsigned char key = readByteFromMIDITrackHeader(&midiTrackHeaders[jTrack]);
                                                    unsigned char touch = readByteFromMIDITrackHeader(&midiTrackHeaders[jTrack]);
                                                    midiOutShortMsg(hMIDIOut, static_cast<DWORD>(MAKELONG(MAKEWORD(command, key), MAKEWORD(touch, 0))));
                                                }
                                                else if(hiNybble == 0x0B)
                                                {
                                                    unsigned char controller = readByteFromMIDITrackHeader(&midiTrackHeaders[jTrack]);
                                                    unsigned char controllerValue = readByteFromMIDITrackHeader(&midiTrackHeaders[jTrack]);
                                                    midiOutShortMsg(hMIDIOut, static_cast<DWORD>(MAKELONG(MAKEWORD(command, controller), MAKEWORD(controllerValue, 0))));
                                                }
                                                else if(hiNybble == 0x0C)
                                                {
                                                    unsigned char instrument = readByteFromMIDITrackHeader(&midiTrackHeaders[jTrack]);
                                                    midiOutShortMsg(hMIDIOut, static_cast<DWORD>(MAKELONG(MAKEWORD(command, instrument), MAKEWORD(0, 0))));
                                                }
                                                else if(hiNybble == 0x0D)
                                                {
                                                    unsigned char pressure = readByteFromMIDITrackHeader(&midiTrackHeaders[jTrack]);
                                                    midiOutShortMsg(hMIDIOut, static_cast<DWORD>(MAKELONG(MAKEWORD(command, pressure), MAKEWORD(0, 0))));
                                                }
                                                else if(hiNybble == 0x0E)
                                                {
                                                    unsigned short tUShort = readUShortFromMIDITrackHeader(&midiTrackHeaders[jTrack]);
                                                    unsigned char hiByte = HIBYTE(tUShort);
                                                    unsigned char loByte = LOBYTE(tUShort);
                                                    midiOutShortMsg(hMIDIOut, static_cast<DWORD>(MAKELONG(MAKEWORD(command, hiByte), MAKEWORD(loByte, 0))));
                                                }
                                                else if(hiNybble == 0x0F)
                                                {
                                                    if(loNybble == 0x02)
                                                    {
                                                        midiTrackHeaders[jTrack].pData += 2;
                                                    }
                                                    else if(loNybble == 0x03)
                                                    {
                                                        midiTrackHeaders[jTrack].pData++;
                                                    }
                                                    else if( (loNybble == 0x00) || (loNybble == 0x07))
                                                    {
                                                        unsigned int tLength = readVLQFromMIDITrackHeader(&midiTrackHeaders[jTrack]);
                                                        midiTrackHeaders[jTrack].pData += tLength;
                                                    }
                                                }
                                                else
                                                {
                                                    midiTrackHeaders[jTrack].pData++;
                                                }
                                            }

                                            if(!midiTrackHeaders[jTrack].isEnabled)
                                            {
                                                break;
                                            }

                                            midiTrackHeaders[jTrack].deltaTime = readVLQFromMIDITrackHeader(&midiTrackHeaders[jTrack]) * midi.tickLength;
                                            midiTrackHeaders[jTrack].triggerTime += midiTrackHeaders[jTrack].deltaTime;
                                        }

                                        if(midiTrackHeaders[jTrack].isEnabled)
                                        {
                                            if(midiTrackHeaders[jTrack].triggerTime < nextTriggerTime)
                                            {
                                                nextTriggerTime = midiTrackHeaders[jTrack].triggerTime;
                                            }
                                        }
                                    }
                                    else
                                    {
                                        midiTrackHeaders[jTrack].isEnabled = 0;
                                    }
                                }
                            }
                            tCurrentTime = getHighResolutionTime();

                            if(this->isPaused)
                            {
                                setAllNotesOff(&midi, &hMIDIOut);
                                double tPausedTime = tCurrentTime;
                                while(true)
                                {
                                    Sleep(10);
                                    if( (!this->isPaused) || this->isStopRequested )
                                    {
                                        break;
                                    }
                                }

                                if(this->isStopRequested)
                                {
                                    break;
                                }

                                tCurrentTime = getHighResolutionTime();
                                nextTriggerTime += (tCurrentTime - tPausedTime);

                                for(jTrack = 0; jTrack < midi.countTracks; jTrack++)
                                {
                                    midiTrackHeaders[jTrack].triggerTime += (tCurrentTime - tPausedTime);
                                }
                            }

                            while( (!this->isStopRequested) && (tCurrentTime < nextTriggerTime) )
                            {
                                Sleep(static_cast<DWORD>(nextTriggerTime - tCurrentTime));
                                tCurrentTime = getHighResolutionTime();
                            }

                            if(!isAnyTrackEnabled)
                            {
                                break;
                            }
                        }

                        setAllNotesOff(&midi, &hMIDIOut);
                        Sleep(10);
                        midiOutReset(hMIDIOut);
                        unsigned short iTryClosing = 0;
                        while( (midiOutClose(hMIDIOut) != MMSYSERR_NOERROR) && (iTryClosing < 100) )
                        {
                            Sleep(10);
                            iTryClosing++;
                        }
                        hMIDIOut = nullptr;
                    }

                    unsigned short jTrack;
                    for(jTrack = 0; jTrack < midi.countTracks; jTrack++)
                    {
                        midiTrackHeaders[jTrack].pData = nullptr;
                        if(midiTrackHeaders[jTrack].data)
                        {
                            delete [](midiTrackHeaders[jTrack].data);
                        }
                    }
                    delete []midiTrackHeaders;
                }
                catch(int errCode)
                {
                    if(errCode == -8)
                    {
                        unsigned short jTrack;
                        for(jTrack = 0; jTrack <= iTrack; jTrack++)
                        {
                            delete [](midiTrackHeaders[jTrack].data);
                        }
                    }
                    delete []midiTrackHeaders;
                    throw errCode;
                }
            }
            catch(int errCode)
            {
                Q_UNUSED(errCode)
                fclose(pMIDIFile);
            }
        }
    }

    emit this->finished();
}