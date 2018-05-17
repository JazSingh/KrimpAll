//
// Created by Jaspreet Singh on 19/04/2018.
//

#include "CTSet.h"

using namespace std;

bool SortingMethod(CodeTable *ct1, CodeTable *ct2) {
    return ct1->GetCurStats().encSize < ct2->GetCurStats().encSize;
};

bool SortingMethodReverse(CodeTable *ct1, CodeTable *ct2) {
    return ct1->GetCurStats().encSize > ct2->GetCurStats().encSize;
};

CTSet::CTSet() {
    codeTables = new ctVec();
    nTables = 0;
}

uint32 CTSet::GetNumTables() {
    return nTables;
}

void CTSet::Add(CodeTable *codeTable) {
    codeTables->push_back(codeTable);
    ResetIterator();
    nTables++;
}

void CTSet::PopBack() {
    codeTables->pop_back();
    ResetIterator();
    nTables--;
}

void CTSet::Sort() {
    //Cover();
    std::sort(codeTables->begin(), codeTables->end(), SortingMethod);
    ResetIterator();
}

void CTSet::SortReverse() {
    //Cover();
    std::sort(codeTables->begin(), codeTables->end(), SortingMethodReverse);
    ResetIterator();
}

void CTSet::SortAndPrune(uint32 numTablesRemain) {
    SortReverse();
    if(nTables <= numTablesRemain) { return; }
    auto *codeTablesNew = new ctVec();
    for(int i = 0; i < numTablesRemain; i++) {
        codeTablesNew->push_back(codeTables->back());
        PopBack();
    }
    delete codeTables;
    codeTables = codeTablesNew;
    nTables = numTablesRemain;
    ResetIterator();
}

CodeTable *CTSet::NextCodeTable() {
    return *curTable++;
}

double CTSet::AvgCompression() {
    uint64 numTab = GetNumTables();
    double sumCompression = 0;
    ctVec::iterator iter;
    uint64 it = 0;
    for(iter = codeTables->begin(); iter != codeTables->end(); ++iter) {
        CodeTable* curTab = *iter;
        sumCompression += curTab->GetCurStats().encSize;
        it++;
    }
    if(isnan(sumCompression)) {
        return -1; // WILL THROW ERROR
    }
    double avgCompression = sumCompression/numTab;
    return avgCompression;
}

bool CTSet::IsIteratorEnd() {
    return curTable == codeTables->end();
}

void CTSet::ResetIterator() {
    curTable = codeTables->begin();
}

void CTSet::Cover() {
    ctVec::iterator it;
    for(it = codeTables->begin(); it != codeTables->end(); it++) {
        CodeTable* ct = *it;
        ct->CoverDB(ct->GetCurStats());
    }
}
