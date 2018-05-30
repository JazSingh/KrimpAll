//
// Created by Jaspreet Singh on 25/05/2018.
//

#ifndef FIC_GROEISLIMNOS_H
#define FIC_GROEISLIMNOS_H


#include "../krimp/codetable/CodeTable.h"
#include "../slim/SlimAlgo.h"
#include "GroeiAlgo.h"

class GroeiSlimNoS: public GroeiAlgo {
public:
    GroeiSlimNoS(CodeTable* ct, HashPolicyType hashPolicy, Config* config);

    virtual CodeTable*	DoeJeDing(const uint64 candidateOffset=0, const uint32 startSup=0);

private:

    uint32 mMinSup;
};


#endif //FIC_GROEISLIMNOS_H
