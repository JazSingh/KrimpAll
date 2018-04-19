//
// Created by Jaspreet Singh on 19/04/2018.
//

#include "GroeiFast.h"

void GroeiFast::Playground() {
    uint64 complexities[] = {1, 2, 3};
    uint32 beamWidth = 10;
    uint64 iteration = 0;
    uint64 *maxComplexity = max_element(begin(complexities), end(complexities));
    while (iteration < *maxComplexity) {
        auto *candidates = new CTSet();
        auto *tables = new CTSet();
        auto *isc = new ItemSetCollection();
        uint64 numIsc = isc->GetNumItemSets();
        uint32 numTables = tables->GetNumTables();
        for (uint64 curIsc = 0; curIsc < numIsc; curIsc++);
        {
            ItemSet *itemSet = isc->GetNextItemSet();
            for (uint32 curTable = 0; curTable < numTables; curTable++) {
                CodeTable *candidate = tables->GetCodeTable(curTable)->Clone();
                candidate->AddAndCommit(itemSet, itemSet->GetUniqueID());
                candidates->Add(candidate);
        }
        candidates->SortAndPrune(beamWidth);
        if(begin(complexities), end(complexities), iteration) {
            //output results
        }
        iteration++;
    }
}
