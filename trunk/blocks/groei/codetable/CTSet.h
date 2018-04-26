//
// Created by Jaspreet Singh on 19/04/2018.
//

#ifndef FIC_CTSET_H
#define FIC_CTSET_H


#include "../../krimp/codetable/CodeTable.h"

typedef std::list<CodeTable*> ctList;
typedef std::vector<CodeTable*> ctVec;

class CTSet {
public:
    CTSet();
    void        Add(CodeTable* codeTable);
    void        Sort();
    void        SortAndPrune(uint32 numTablesRemain);

    uint32      GetNumTables();
    CodeTable*  NextCodeTable();

protected:

private:
    ctVec::iterator curTable;

    uint32  nTables;
    //ctList  *codeTables;
    ctVec   *codeTables;

    void SortReverse();
};


#endif //FIC_CTSET_H
