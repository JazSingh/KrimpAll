//
// Created by Jaspreet Singh on 19/04/2018.
//

#ifndef FIC_CTSET_H
#define FIC_CTSET_H


#include "../../krimp/codetable/CodeTable.h"

class CTSet {
public:
    CTSet();
    void        Add(CodeTable* codeTable);
    void        Sort();
    void        Prune(uint32 numTablesRemain);
    void        SortAndPrune(uint32 numTablesRemain);

    uint32      GetNumTables();
    CodeTable*  GetCodeTable(uint32 index);

protected:

private:
    uint32 nTables;
};


#endif //FIC_CTSET_H
