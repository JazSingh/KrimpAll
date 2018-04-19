//
// Created by Jaspreet Singh on 19/04/2018.
//

#include "CTSet.h"

CTSet::CTSet() {
    nTables = 10;
}

uint32 CTSet::GetNumTables() {
    return nTables;
}

void CTSet::Add(CodeTable *codeTable) {

}

void CTSet::Sort() {

}

void CTSet::Prune(uint32 numTablesRemain) {

}

void CTSet::SortAndPrune(uint32 numTablesRemain) {

}

CodeTable *CTSet::GetCodeTable(uint32 index) {
    return nullptr;
}

void CTSet::SortAndPrune() {

}

uint64 CTSet::GetEncodedSize() {
    return 0;
}
