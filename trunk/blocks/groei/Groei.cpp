//
// Created by Jaspreet Singh on 19/04/2018.
//

#include "Groei.h"
#include "../slim/codetable/CCPUCodeTable.h"
#include "../slim/SlimAlgo.h"
#include <StringUtils.h>
#include <boost/tokenizer.hpp>


using namespace boost;

/**
 * Constructors
 */
Groei::Groei(CodeTable *ct, HashPolicyType hashPolicy, Config *config)
        : GroeiAlgo(ct, hashPolicy, config) {
    mWriteLogFile = true;
}

CodeTable *Groei::DoeJeDing(const uint64 candidateOffset, const uint32 startSup) {
    // Read properties from config
    uint32 beamWidth        = mConfig->Read<uint32>("beamWidth");
    uint32 numComplexities  = mConfig->Read<uint32>("numComplexities");
    string sComplexities    = mConfig->Read<string>("complexities", "");
    uint32 *complexities    = StringUtils::TokenizeUint32(sComplexities, numComplexities);
    uint32 *maxComplexity   = std::max_element(complexities, complexities+numComplexities);

    // Initialize
    uint32 complexityLvl    = 0;
    uint32 iteration        = 0;
    auto *candidates        = new CTSet();
    auto ctAlpha            = mCT->Clone();
    candidates->Add(ctAlpha);

    // Start: for all complexity levels from [0..max]:
    while (iteration <= *maxComplexity) {
        CTSet *prevBest  = candidates;
        candidates       = new CTSet();
        uint64 numIsc    = mISC->GetNumItemSets();
        // For all item sets in the item set collection:
        for (uint64 curIsc = 0; curIsc < numIsc; curIsc++);
        {
            ItemSet *itemSet = mISC->GetNextItemSet();

            // Add to all clones of all code tables
            prevBest->ResetIterator();
            do {
                CodeTable *curTable = prevBest->NextCodeTable()->Clone();
                curTable->AddAndCommit(itemSet, itemSet->GetUniqueID());
                candidates->Add(curTable);
            } while(!prevBest->IsCurTableNullPtr());
        }
        candidates->SortAndPrune(beamWidth);
        if(iteration == *(complexities + complexityLvl)) {
            //TODO something interesting
            complexityLvl++;
        }
        iteration++;
    }
    ctAlpha->SetCodeTableSet(candidates);
    return ctAlpha;
}
