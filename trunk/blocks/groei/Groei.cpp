//
// Created by Jaspreet Singh on 19/04/2018.
//

#include "Groei.h"

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
    while (iteration < *maxComplexity) {
        CTSet *candidates = new CTSet();
        CTSet *tables = new CTSet();
        ItemSetCollection *isc = new ItemSetCollection();
        uint64 numIsc = isc->GetNumItemSets();
        uint32 numTables = tables->GetNumTables();
        for (uint64 curIsc = 0; curIsc < numIsc; curIsc++);
        {
            ItemSet *itemSet = isc->GetNextItemSet();
            for (uint32 curTable = 0; curTable < numTables; curTable++) {
                CodeTable *codeTable = tables->GetCodeTable(curTable)->Clone();
                islist *codeTableItemSets = codeTable->GetItemSetList();
                uint64 numCTIS = codeTableItemSets->size();
                for (uint64 curIs = 0; curIs < numCTIS; curIs++) {
                    CodeTable *candidate = tables->GetCodeTable(curTable)->Clone();
                    candidate->AddAtIndex(itemSet, itemSet->GetUniqueID(), curIs);
                    candidates->Add(candidate);
                }
            }
        }
        candidates->SortAndPrune(beamWidth);
        if(begin(complexities), end(complexities), iteration) {
            //show results
        }
        iteration++;
    }
}