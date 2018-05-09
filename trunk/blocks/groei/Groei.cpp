//
// Created by Jaspreet Singh on 19/04/2018.
//

#include "Groei.h"
#include "../slim/codetable/CCPUCodeTable.h"
#include "../slim/SlimAlgo.h"

/**
 * Constructors
 */
Groei::Groei(CodeTable *ct, HashPolicyType hashPolicy, Config *config)
        : GroeiAlgo(ct, hashPolicy, config) {
    mWriteLogFile = true;
}

CodeTable *Groei::DoeJeDing(const uint64 candidateOffset, const uint32 startSup) {

}

/**
 * Playground for experimenting
 */
void Groei::Playground() {
    uint64 complexities[] = {1, 2, 3};
    uint32 beamWidth = 10;
    uint64 iteration = 0;
    uint64 *maxComplexity = max_element(begin(complexities), end(complexities));
    auto *candidates = new CTSet();
    auto ctAlpha = mCT->Clone();
    candidates->Add(ctAlpha);
    while (iteration < *maxComplexity) {
        CTSet *prevBest = candidates;
        candidates = new CTSet();
        auto *isc = new ItemSetCollection();
        uint64 numIsc = isc->GetNumItemSets();
        for (uint64 curIsc = 0; curIsc < numIsc; curIsc++);
        {
            ItemSet *itemSet = isc->GetNextItemSet();

            prevBest->ResetIterator();
            do {
                CodeTable *curTable = prevBest->NextCodeTable()->Clone();
                curTable->AddAndCommit(itemSet, itemSet->GetUniqueID());
                candidates->Add(curTable);
            } while(!prevBest->IsCurTableNullPtr());
        }
        candidates->SortAndPrune(beamWidth);
        if (begin(complexities), end(complexities), iteration) {
            //TODO SOMETHING
        }
        iteration++;
    }
}
