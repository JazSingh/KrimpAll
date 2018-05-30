//
// Created by Jaspreet Singh on 19/04/2018.
//

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
    if(!codeTables->empty() && ContainsCodeTable(codeTable)) {
        delete codeTable;
        return;
    }
    codeTables->push_back(codeTable);
    ResetIterator();
}

void CTSet::PopBack() {
    CodeTable *del = codeTables->back();
    delete del;
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
    if (codeTables->size() <= numTablesRemain) {
        return;
    }
    auto i = codeTables->begin();
    for(uint64 j = numTablesRemain; j < codeTables->size(); j++) {
        CodeTable *toDel = *(i + j);
        delete toDel;
    }
    codeTables->resize(numTablesRemain);
    codeTables->shrink_to_fit();
    ResetIterator();
}

CodeTable *CTSet::NextCodeTable() {
    CodeTable *ct = *curTable;
    curTable++;
    return ct;
}

double CTSet::AvgCompression() {
    uint64 numTab = GetNumTables();
    double sumCompression = 0;
    ctVec::iterator iter;
    uint64 it = 0;
    for (iter = codeTables->begin(); iter != codeTables->end(); ++iter) {
        CodeTable *curTab = *iter;
        sumCompression += curTab->GetCurStats().encSize;
        it++;
    }
    if (isnan(sumCompression)) {
        return -1; // WILL THROW ERROR
    }
    double avgCompression = sumCompression / numTab;
    return avgCompression;
}

bool CTSet::IsIteratorEnd() {
    return curTable == codeTables->end();
}

void CTSet::ResetIterator() {
    curTable = codeTables->begin();
}

bool CTSet::ContainsItemSet(CodeTable *ct, ItemSet *is) {
    islist *iss = ct->GetItemSetList();
    islist::iterator i;
    for (i = iss->begin(); i != iss->end(); ++i) {
        ItemSet *itemSet = *i;
        if (itemSet->Equals(is)) {
            return true;
        }
    }
    return false;
}

void CTSet::PrintStats() {
    ctVec::iterator i;
    int count = 0;
    for (i = codeTables->begin(); i != codeTables->end(); ++i) {
        count++;
        CodeTable *ct = *i;
        CoverStats stats = ct->GetCurStats();
        printf(" * Result:\t\t(ct%d, %da,%du,%" I64d ",%.0lf,%.0lf,%.0lf)\n", count, stats.alphItemsUsed,
               stats.numSetsUsed, stats.usgCountSum, stats.encDbSize, stats.encCTSize, stats.encSize);
    }
}

ctVec *CTSet::GetCodeTables() {
    return codeTables;
}

CoverStats &CTSet::GetBestStats() {
    Sort();
    return (*codeTables->begin())->GetCurStats();
}

CoverStats &CTSet::GetWorstStats() {
    if (codeTables->size() == 1) {
        return (*codeTables->begin())->GetCurStats();
    }
    Sort();
    return (*codeTables->begin())->GetCurStats();
}

CodeTable *CTSet::GetBestTable() {
    Sort();
    return *codeTables->begin();
}

CodeTable *CTSet::GetWorstTable() {
    if (codeTables->size() == 1) {
        return *codeTables->begin();
    }
    Sort();
    return *codeTables->end();
}

void CTSet::AddLim(CodeTable *codeTable, CoverStats &prevBestStats) {
    double ctEncSize = codeTable->GetCurStats().encSize;
    if(ctEncSize >= prevBestStats.encSize) {
        delete codeTable;
        return;
    }

    auto encSizeThreshold = DOUBLE_MAX_VALUE;
    if (!codeTables->empty()) {
        auto wStats = GetWorstStats();
        encSizeThreshold = wStats.encSize;
    }

    if (codeTables->size() < maxTables) {
        Add(codeTable);
    } else if (ctEncSize < encSizeThreshold) {
        Sort();
        PopBack();
        Add(codeTable);
    } else {
        delete codeTable;
    }
}

CTSet::CTSet(uint64 maxTables) : CTSet() {
    this->maxTables = maxTables;
}

CTSet::~CTSet() {
    for (auto &codeTable : *codeTables) {
        delete codeTable;
    }
    codeTables->clear();
    delete codeTables;
}

bool CTSet::ContainsCodeTable(CodeTable *ct) {
    islist *ctList = ct->GetItemSetList();
    for (auto c : *codeTables) {
        islist *cList = c->GetItemSetList();

        if(cList->size() != ctList->size()) {
            continue;
        }

        auto iC = cList->begin();
        bool allSame = true;
        for(auto a: *ctList) {
            ItemSet *b = *(iC);
            iC++;
            if(!a->Equals(b)) {
                allSame = false;
                break;
            }
        }

        if(allSame) {
            return true;
        }

    }
    return false;
}
