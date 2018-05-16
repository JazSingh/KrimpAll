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
}

void CTSet::PopBack() {
    codeTables->pop_back();
    ResetIterator();
}

void CTSet::Sort() {
    std::sort(codeTables->begin(), codeTables->end(), SortingMethod);
    ResetIterator();
}

void CTSet::SortReverse() {
    std::sort(codeTables->begin(), codeTables->end(), SortingMethodReverse);
    ResetIterator();
}

void CTSet::SortAndPrune(uint32 numTablesRemain) {
    SortReverse();
    auto *codeTablesNew = new ctVec();
    for(int i = 0; i < numTablesRemain; i++) {
        codeTablesNew->push_back(codeTables->front());
        codeTables->pop_back();
    }
    delete codeTables;
    codeTables = codeTablesNew;
    ResetIterator();
}

/**
 * Works like an iterator (linked-list like behavior)
 *  If the iterator is a nullptr, the first element of the set is returned.
 *  If the iterator is at the end of the set, the next element will be a nullptr.
 **/
CodeTable *CTSet::NextCodeTable() {
    if(*curTable == nullptr) {
        curTable = codeTables->begin();
    }
    if(curTable == codeTables->end()) {
        *curTable = nullptr;
        return *codeTables->end();
    }
    return *curTable++;
}

double CTSet::AvgCompression() {
    uint64 numTab = GetNumTables();
    double sumCompression = 0;
    ctVec::iterator iter;
    for(iter = codeTables->begin(); iter != codeTables->end(); ++iter) {
        CodeTable* curTab = *iter;
        sumCompression += curTab->GetCurStats().encSize;
    }
    double avgCompression = sumCompression/numTab;
    return avgCompression;
}

bool CTSet::IsCurTableNullPtr() {
    return *curTable == nullptr;
}

void CTSet::ResetIterator() {
    curTable = codeTables->begin();
}