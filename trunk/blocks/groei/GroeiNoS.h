//
// Created by Jaspreet Singh on 25/05/2018.
//

#ifndef FIC_GROEINOS_H
#define FIC_GROEINOS_H


#include "../krimp/codetable/CodeTable.h"
#include "../slim/SlimAlgo.h"
#include "GroeiAlgo.h"

class GroeiNoS: public GroeiAlgo {
public:
    GroeiNoS(CodeTable* ct, HashPolicyType hashPolicy, Config* config);

    virtual CodeTable*	DoeJeDing(const uint64 candidateOffset=0, const uint32 startSup=0);

private:

    uint32 mMinSup;
};


#endif //FIC_GROEINOS_H
