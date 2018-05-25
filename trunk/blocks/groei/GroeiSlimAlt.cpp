//
// Created by Jaspreet Singh on 25/05/2018.
//

#include "GroeiSlimAlt.h"

GroeiSlimAlt::GroeiSlimAlt(CodeTable *ct, HashPolicyType hashPolicy, Config *config) : GroeiAlgo(ct, hashPolicy,
                                                                                                 config) {

}

CodeTable *GroeiSlimAlt::DoeJeDing(const uint64 candidateOffset, const uint32 startSup) {
    return KrimpAlgo::DoeJeDing(candidateOffset, startSup);
}
