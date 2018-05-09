//
// Created by Jaspreet Singh on 19/04/2018.
//

#include "Groei.h"
#include "../slim/codetable/CCPUCodeTable.h"

/**
 * Constructors
 */
Groei::Groei() {

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
    auto ctAlpha = new CCPUCodeTable();
    ctAlpha->Clone();
    while (iteration < *maxComplexity) {
        CTSet *prevBest = candidates;
        candidates = new CTSet();
        auto *isc = new ItemSetCollection();
        uint64 numIsc = isc->GetNumItemSets();
        uint32 numTables = prevBest->GetNumTables();
        for (uint64 curIsc = 0; curIsc < numIsc; curIsc++);
        {
            ItemSet *itemSet = isc->GetNextItemSet();
            for (uint32 curTable = 0; curTable < numTables; curTable++) {
                CodeTable *codeTable = prevBest->NextCodeTable()->Clone();
                islist *codeTableItemSets = codeTable->GetItemSetList();
                uint64 numCTIS = codeTableItemSets->size();

            }
            //while()
        }
        candidates->SortAndPrune(beamWidth);
        if(begin(complexities), end(complexities), iteration) {
            //output results
        }
        iteration++;
    }
}