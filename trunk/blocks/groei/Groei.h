//
// Created by Jaspreet Singh on 19/04/2018.
//

#ifndef FIC_GROEI_H
#define FIC_GROEI_H

#include "codetable/CTSet.h"
#include "../../bass/isc/ItemSetCollection.h"
#include "../../bass/itemstructs/ItemSet.h"
#include "../krimp/codetable/CodeTable.h"
#include "../slim/SlimAlgo.h"

#include "GroeiAlgo.h"

class Groei: public GroeiAlgo {
public:
    Groei(CodeTable* ct, HashPolicyType hashPolicy, Config* config);
    virtual void Playground();

    virtual CodeTable*	DoeJeDing(const uint64 candidateOffset=0, const uint32 startSup=0);

private:

    uint32 mMinSup;
};


#endif //FIC_GROEI_H
