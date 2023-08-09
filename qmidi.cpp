#include "qmidi.h"

const char MThd[5] = "MThd";

unsigned long long readUShortFromMIDIFile(unsigned short * pDst, FILE * pMIDIFile)
{
    if(pMIDIFile != nullptr)
    {
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
    else
    {
        return 0;
    }
}

unsigned long long readUIntFromMIDIFile(unsigned int * pDst, FILE * pMIDIFile)
{
    if(pMIDIFile != nullptr)
    {
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
    else
    {
        return 0;
    }
}