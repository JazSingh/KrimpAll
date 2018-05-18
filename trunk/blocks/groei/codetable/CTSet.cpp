//
// Created by Jaspreet Singh on 19/04/2018.
//

#include "CTSet.h"
#include "../../krimp/codetable/CodeTable.h"

using namespace std;

bool SortingMethod(CodeTable *ct1, CodeTable *ct2) {
    return ct1->GetCurStats().encSize < ct2->GetCurStats().encSize;
};

bool SortingMethodReverse(CodeTable *ct1, CodeTable *ct2) {
    return ct1->GetCurStats().encSize > ct2->GetCurStats().encSize;
};

CTSet::CTSet() {
    codeTables = new ctVec();
}

uint64 CTSet::GetNumTables() {
    return codeTables->size();
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
    Sort();
    if(codeTables->size() <= numTablesRemain) {
        return;
    } // Is this even possible?
    codeTables->resize(numTablesRemain);
    codeTables->shrink_to_fit();
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

bool CTSet::ContainsItemSet(CodeTable *ct, ItemSet *is) {
    islist* iss = ct->GetItemSetList();
    islist::iterator i;
    for(i = iss->begin(); i != iss->end(); ++i) {
        ItemSet* itemSet = *i;
        if(itemSet->Equals(is)) {
            return true;
        }
    }
    return false;
}

void CTSet::PrintStats() {
    ctVec::iterator i;
    int count = 0;
    for(i = codeTables->begin(); i != codeTables->end(); ++i) {
        count++;
        CodeTable* ct = *i;
        CoverStats stats = ct->GetCurStats();
        printf(" * Result:\t\t(ct%d, %da,%du,%" I64d ",%.0lf,%.0lf,%.0lf)\n", count, stats.alphItemsUsed, stats.numSetsUsed, stats.usgCountSum, stats.encDbSize, stats.encCTSize, stats.encSize);
    }
}

void CTSet::Merge(CTSet* cts) {
    ctVec* tables = cts->GetCodeTables();
    ctVec* merged;
    merged->reserve(codeTables->size() + tables->size());
    merged->insert(merged->end(), codeTables->begin(), codeTables->end());
    merged->insert(merged->end(), tables->begin(),     tables->end());
    codeTables = merged;
}

ctVec *CTSet::GetCodeTables() {
    return codeTables;
}

CoverStats& CTSet::GetBest() {
    Sort();
    return (*codeTables->begin())->GetCurStats();
}

CoverStats& CTSet::GetWorst() {
    Sort();
    return (*codeTables->end())->GetCurStats();
}

CodeTable *CTSet::GetBestTable() {
    Sort();
    return *codeTables->begin();
}

CodeTable *CTSet::GetWorstTable() {
    Sort();
    return *codeTables->end();
}
