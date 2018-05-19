//
// Created by Jaspreet Singh on 19/04/2018.
//

#ifndef FIC_CTSET_H
#define FIC_CTSET_H

class CodeTable;

struct CoverStats;

#include "../../krimp/codetable/CodeTable.h"

typedef std::vector<CodeTable *> ctVec;

class CTSet {
public:
    CTSet();

    CTSet(uint64 maxTables);

    ~CTSet();

    // Iterator is reset after modifying the list
    void Add(CodeTable *codeTable);

    void AddLim(CodeTable *codeTable, CoverStats &prevBestStats);

    void PopBack();

    void Sort();

    void SortReverse();

    void SortAndPrune(uint32 numTablesRemain);

    bool ContainsItemSet(CodeTable *ct, ItemSet *is);

    //Stats
    double AvgCompression();

    uint64 GetNumTables();

    void PrintStats();

    CoverStats &GetBestStats();

    CoverStats &GetWorstStats();

    CodeTable *GetBestTable();

    CodeTable *GetWorstTable();

    // Iteration stuff
    CodeTable *NextCodeTable();

    bool IsIteratorEnd();

    void ResetIterator();


protected:

private:
    ctVec *GetCodeTables();

    ctVec *codeTables;
    ctVec::iterator curTable;

    uint64 maxTables;
};


#endif //FIC_CTSET_H
