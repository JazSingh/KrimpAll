//
// Created by Jaspreet Singh on 19/04/2018.
//

#ifndef FIC_GROEISLIM_H
#define FIC_GROEISLIM_H


#include "GroeiAlgo.h"

class GroeiSlim: public GroeiAlgo {
public:
    GroeiSlim(CodeTable* ct, HashPolicyType hashPolicy, Config* config);

    virtual CodeTable*	DoeJeDing(const uint64 candidateOffset=0, const uint32 startSup=0);

private:

    uint32 mMinSup;
};


#endif //FIC_GROEISLIM_H
