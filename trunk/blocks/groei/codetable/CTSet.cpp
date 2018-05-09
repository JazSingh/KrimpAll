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
}

void CTSet::Sort() {
    std::sort(codeTables->begin(), codeTables->end(), SortingMethod);
}

void CTSet::SortReverse() {
    std::sort(codeTables->begin(), codeTables->end(), SortingMethodReverse);
}

void CTSet::SortAndPrune(uint32 numTablesRemain) {
    SortReverse();
    ctVec *codeTablesNew = new ctVec();
    for(int i = 0; i < numTablesRemain; i++) {
        codeTablesNew->push_back(codeTables->front());
        codeTables->pop_back();
    }
    delete codeTables;
    codeTables = codeTablesNew;
}

CodeTable *CTSet::NextCodeTable() {
    if(*curTable == nullptr) {
        curTable = codeTables->begin();
    }
    if(curTable == codeTables->end()) {
        curTable = codeTables->begin();
        return nullptr;
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
