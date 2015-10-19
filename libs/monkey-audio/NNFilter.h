#ifndef APE_NNFILTER_H
#define APE_NNFILTER_H

#include "RollBuffer.h"
#define NN_WINDOW_ELEMENTS    512
//#define NN_TEST_MMX

class CNNFilter
{
public:

    CNNFilter(int nOrder, int nShift, int nVersion);
    ~CNNFilter();

    int Compress(int nInput);
    int Decompress(int nInput);
    void Flush();

private:

    int m_nOrder;
    int m_nShift;
    int m_nVersion;
    int m_nRunningAverage;

    CRollBuffer<short> m_rbInput;
    CRollBuffer<short> m_rbDeltaM;

    short * m_paryM;

    inline short GetSaturatedShortFromInt(int nValue) const
    {
        return short((nValue == short(nValue)) ? nValue : (nValue >> 31) ^ 0x7FFF);
    }

};

#endif // #ifndef APE_NNFILTER_H
