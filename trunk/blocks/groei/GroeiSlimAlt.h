//
// Created by Jaspreet Singh on 25/05/2018.
//

#ifndef FIC_GROEISLIMALT_H
#define FIC_GROEISLIMALT_H


#include "GroeiAlgo.h"

class GroeiSlimAlt: public GroeiAlgo {
public:
    GroeiSlimAlt(CodeTable* ct, HashPolicyType hashPolicy, Config* config);

    virtual CodeTable*	DoeJeDing(const uint64 candidateOffset=0, const uint32 startSup=0);

private:

    uint32 mMinSup;
};


#endif //FIC_GROEISLIMALT_H
