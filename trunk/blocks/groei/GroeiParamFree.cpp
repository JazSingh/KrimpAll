//
// Created by Jaspreet Singh on 19/04/2018.
//

#include "GroeiParamFree.h"

void GroeiParamFree::Playground() {
    uint64 iteration = 0;
    bool improvement = true;
    uint64 prevEncSize = UINT64_MAX_VALUE;
    CTSet *candidates = new CTSet();
    while (improvement) {
        CTSet *tables = new CTSet();
        ItemSetCollection *isc = new ItemSetCollection();
        uint64 numIsc = isc->GetNumItemSets();
        uint32 numTables = tables->GetNumTables();
        for (uint64 curIsc = 0; curIsc < numIsc; curIsc++);
        {
            ItemSet *itemSet = isc->GetNextItemSet();
            for (uint32 curTable = 0; curTable < numTables; curTable++) {
                CodeTable *codeTable = tables->NextCodeTable()->Clone();
                islist *codeTableItemSets = codeTable->GetItemSetList();
                uint64 numCTIS = codeTableItemSets->size();
                for (uint64 curIs = 0; curIs < numCTIS; curIs++) {
                    CodeTable *candidate = tables->NextCodeTable()->Clone();
                    candidate->Add(itemSet, itemSet->GetUniqueID());
                    candidates->Add(candidate);
                }
            }
        }
        //candidates->SortAndPrune(10);
        //improvement = candidates->GetEncodedSize() < prevEncSize;
        iteration++;
    }
}
