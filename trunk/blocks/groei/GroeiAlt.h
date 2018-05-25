//
// Created by Jaspreet Singh on 25/05/2018.
//

#ifndef FIC_GROEIALT_H
#define FIC_GROEIALT_H


#include "GroeiAlgo.h"

class GroeiAlt: public GroeiAlgo {
public:
    GroeiAlt(CodeTable* ct, HashPolicyType hashPolicy, Config* config);

    virtual CodeTable* DoeJeDing(const uint64 candidateOffset=0, const uint32 startSup=0);

private:

    uint32 mMinSup;
};


#endif //FIC_GROEIALT_H
