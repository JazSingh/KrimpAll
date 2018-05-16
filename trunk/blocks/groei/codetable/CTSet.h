//
// Created by Jaspreet Singh on 19/04/2018.
//

#ifndef FIC_CTSET_H
#define FIC_CTSET_H

class CodeTable;

#include "../../krimp/codetable/CodeTable.h"

typedef std::vector<CodeTable*> ctVec;

class CTSet {
public:
    CTSet();
    // Iterator is reset after modifying the list
    void        Add(CodeTable* codeTable);
    void        PopBack();
    void        Sort();
    void        SortReverse();
    void        SortAndPrune(uint32 numTablesRemain);

    //Stats
    double      AvgCompression();
    uint32      GetNumTables();

    // Iteration stuff
    CodeTable*  NextCodeTable();
    bool        IsCurTableNullPtr();
    void        ResetIterator();

protected:

private:
    ctVec               *codeTables;
    ctVec::iterator     curTable;

    uint32              nTables;
};


#endif //FIC_CTSET_H
