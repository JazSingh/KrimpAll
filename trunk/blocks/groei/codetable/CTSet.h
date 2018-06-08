//
// Created by Jaspreet Singh on 19/04/2018.
//

#ifndef FIC_CTSET_H
#define FIC_CTSET_H

class CodeTable;

#include "../../krimp/codetable/CodeTable.h"

struct CoverStats;

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

    bool ContainsCodeTable(CodeTable *ct);

    bool ContainsItemSet(CodeTable *ct, ItemSet *is);

    //Stats
    double AvgCompression();

    double* CalcProbs(double *encLengths);

    void CalcProbs(Database *db);

    double* SummarizeProbs(Database *db);

    double* CalcEncLengths(ItemSet* is);

    void CalcEncLengths(Database *db);

    void CalcEntropy(Database *db);

    uint64 GetNumTables();

    void PrintStats();

    CoverStats &GetBestStats();

    CoverStats &GetWorstStats();

    CodeTable *GetBestTable();

    CodeTable *GetWorstTable();

    void Dissimilarity(Database *db);

    // Iteration stuff
    CodeTable *NextCodeTable();

    bool IsIteratorEnd();

    void ResetIterator();

    double **GetProbs() {return probs;}

    double **GetEncLengths() {return encLengths;}

    double *GetEntropies() {return entropies;}

    double GetTotalEntropy() {return totalEntropy;}

    double GetAvgEncLength(ItemSet *is);

    virtual void ReadFromDisk(const string &filename, vector<string> *tables, const bool needfreqs);

    static CTSet* CreateCTForClassification(const string &name, ItemSetType dataType, uint64 numTables);

    void UseThisStuff(Database *db, ItemSetType type, uint32 maxCTElemLength=0, uint32 toMinSup = 0);

    void AddOneToEachUsageCount();

    void SetAlphabetCount(uint32 item, uint32 count);

    void UpdateUsageCountSums(uint32 delta);

    double BerekenAfstandTussen(Database *db1, Database *db2, CodeTable *ct1, CodeTable *ct2);

    void PushBack(CodeTable *codeTable);

    void DissimilarityFile(Database *db, FILE *file);

    void PrintStatsFile(FILE *pFILE);

protected:

private:
    ctVec *GetCodeTables();

    ctVec *codeTables;
    ctVec::iterator curTable;

    uint64 maxTables;

    double **probs;
    double **encLengths;
    double *entropies;
    double totalEntropy;
};


#endif //FIC_CTSET_H
