#include "qmidi.h"

const char MIDI::MThd[5] = "MThd";
const char MIDI::MTrk[5] = "MTrk";

unsigned long long MIDI::readUShortFromMIDIFile(unsigned short * pDst, FILE * pMIDIFile)
{
    if(pMIDIFile == nullptr)
    {
        return 0;
    }

    int fgetcRet = fgetc(pMIDIFile);
    if(fgetcRet == EOF)
    {
        return 0;
    }
    unsigned char hiByte = static_cast<unsigned char>(fgetcRet);

    fgetcRet = fgetc(pMIDIFile);
    if(fgetcRet == EOF)
    {
        return 0;
    }
    unsigned char loByte = static_cast<unsigned char>(fgetcRet);

    (*pDst) = static_cast<unsigned short>(MAKEWORD(loByte, hiByte));
    return 2;
}

unsigned long long MIDI::readUIntFromMIDIFile(unsigned int * pDst, FILE * pMIDIFile)
{
    if(pMIDIFile == nullptr)
    {
        return 0;
    }

    unsigned short hi2Bytes;
    if(readUShortFromMIDIFile(&hi2Bytes, pMIDIFile) == 0)
    {
        return 0;
    }

    unsigned short lo2Bytes;
    if(readUShortFromMIDIFile(&lo2Bytes, pMIDIFile) == 0)
    {
        return 0;
    }

    (*pDst) = static_cast<unsigned int>((hi2Bytes << 16) + lo2Bytes);
    return 4;
}

double MIDI::getHighResolutionTime()
{
    timeBeginPeriod(1);
    double highResolutionTime = static_cast<double>(timeGetTime());
    timeEndPeriod(1);
    return highResolutionTime;
}

void MIDI::setNoteOnOff(unsigned char isOn, unsigned char note, unsigned char velocity, unsigned char velocityPercentage, unsigned char channelIndex, QMIDI * pMIDI, HMIDIOUT * pHMIDIOut)
{
    velocity = static_cast<unsigned char>(velocity * velocityPercentage / 100.0);
    pMIDI->channels[channelIndex][note] = (isOn ? velocity : 0);

    if(*pHMIDIOut)
    {
        midiOutShortMsg(*pHMIDIOut, static_cast<DWORD>(MAKELONG(MAKEWORD(MAKEBYTE(channelIndex, (isOn ? 9 : 8)), note), MAKEWORD(velocity, 0))));
    }
}

void MIDI::setAllNotesOff(QMIDI * pMIDI, HMIDIOUT * pHMIDIOut)
{
    unsigned short channel;
    for(channel = 0; channel < 16; channel++)
    {
        memset(&(pMIDI->channels[channel]), 0, sizeof(pMIDI->channels[channel]));

        if(*pHMIDIOut)
        {
            midiOutShortMsg(*pHMIDIOut, static_cast<DWORD>(MAKELONG(MAKEWORD(0xB0 + channel, 120), MAKEWORD(0, 0))));
            midiOutShortMsg(*pHMIDIOut, static_cast<DWORD>(MAKELONG(MAKEWORD(0xB0 + channel, 121), MAKEWORD(0, 0))));
            midiOutShortMsg(*pHMIDIOut, static_cast<DWORD>(MAKELONG(MAKEWORD(0xB0 + channel, 123), MAKEWORD(0, 0))));
        }
    }
}
