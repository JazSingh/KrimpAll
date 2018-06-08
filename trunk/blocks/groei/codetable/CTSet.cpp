//
// Created by Jaspreet Singh on 19/04/2018.
//

#include "../../../bass/db/Database.h"
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

void CTSet::PushBack(CodeTable *codeTable) {
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
    printf("\n\n");
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
    double epsilon = std::numeric_limits<double>::epsilon();
    for (auto c : *codeTables) {
        if(c->GetCurSize()            != ct->GetCurSize() || // equal L(DB, CT)?
           c->GetCurStats().encCTSize != ct->GetCurStats().encCTSize || // equal L(CT)?
           c->GetCurStats().encDbSize != ct->GetCurStats().encDbSize || // equal L(DB|CT)?
           c->GetCurNumSets()         != ct->GetCurNumSets()) { // equal number of sets?
            continue;
        }

        if(BerekenAfstandTussen(c->GetDatabase(), ct->GetDatabase(), c, ct) < epsilon) {
            return true;
        }
    }
    return false;
}

double CTSet::BerekenAfstandTussen(Database *db1, Database *db2, CodeTable *ct1, CodeTable *ct2) {

    // Stap 1 - Prepare the Codetables
    // gefopt! dat moet je zelf maar van te voren doen!
    //ct1->AddOneToEachCount();
    //ct2->AddOneToEachCount();

    // Stap 2 - DB1
    double db1ct1 = 0, db1ct2 = 0;
    ItemSet **db1rows = db1->GetRows();
    uint32 db1numRows = db1->GetNumRows();
    for(uint32 i=0; i<db1numRows; i++) {
        db1ct1 += ct1->CalcTransactionCodeLength(db1rows[i]) * db1rows[i]->GetSupport();
        db1ct2 += ct2->CalcTransactionCodeLength(db1rows[i]) * db1rows[i]->GetSupport();
    }

    // Stap 3 - DB2
    double db2ct1 = 0, db2ct2 = 0;
    ItemSet **db2rows = db2->GetRows();
    uint32 db2numRows = db2->GetNumRows();
    for(uint32 i=0; i<db2numRows; i++) {
        db2ct1 += ct1->CalcTransactionCodeLength(db2rows[i]) * db2rows[i]->GetSupport();
        db2ct2 += ct2->CalcTransactionCodeLength(db2rows[i]) * db2rows[i]->GetSupport();
    }

    // Stap 4 - Uitrekenen die boel
    double dissimilarity = max((db1ct2-db1ct1)/db1ct1, (db2ct1-db2ct2)/db2ct2);

    return dissimilarity;
}


void CTSet::Dissimilarity(Database *db) {
    printf("\n\n* Dissimilarity:\n");
    Database *db1 = db;
    Database *db2 = db;

    uint32 ii = 0;
    uint32 jj = 0;
    ctVec::iterator i;
    ctVec::iterator j;

    double sum = 0;
    uint32 cnt = 0;

    for(i = codeTables->begin(); i != codeTables->end(); ++i) {
        CodeTable *ct1 = *i;
        ii++;
        jj = ii;
        for (j = i+1; j != codeTables->end(); ++j) {
            CodeTable *ct2 = *j;
            jj++;

            double db1ct1 = 0, db1ct2 = 0;
            ItemSet **db1rows = db1->GetRows();
            uint32 db1numRows = db1->GetNumRows();
            for (uint32 k = 0; k < db1numRows; k++) {
                db1ct1 += ct1->CalcTransactionCodeLength(db1rows[k]) * db1rows[k]->GetSupport();
                db1ct2 += ct2->CalcTransactionCodeLength(db1rows[k]) * db1rows[k]->GetSupport();
            }

            double db2ct1 = 0, db2ct2 = 0;
            ItemSet **db2rows = db2->GetRows();
            uint32 db2numRows = db2->GetNumRows();
            for (uint32 l = 0; l < db2numRows; l++) {
                db2ct1 += ct1->CalcTransactionCodeLength(db2rows[l]) * db2rows[l]->GetSupport();
                db2ct2 += ct2->CalcTransactionCodeLength(db2rows[l]) * db2rows[l]->GetSupport();
            }

            double dissimilarity = max((db1ct2 - db1ct1) / db1ct1, (db2ct1 - db2ct2) / db2ct2);
            printf("** Dissimilarity(ct%u, ct%u) = %lf\n", ii, jj, dissimilarity);
            sum += dissimilarity;
            cnt++;
        }
    }

    printf("** AVG Dissimilarity = %lf\n", sum/cnt);
}

void CTSet::DissimilarityFile(Database *db, FILE *file) {
    printf("\n\n* Dissimilarity:\n");
    Database *db1 = db;
    Database *db2 = db;

    uint32 ii = 0;
    uint32 jj = 0;
    ctVec::iterator i;
    ctVec::iterator j;

    double sum = 0;
    uint32 cnt = 0;

    for(i = codeTables->begin(); i != codeTables->end(); ++i) {
        CodeTable *ct1 = *i;
        ii++;
        jj = ii;
        for (j = i+1; j != codeTables->end(); ++j) {
            CodeTable *ct2 = *j;
            jj++;

            double db1ct1 = 0, db1ct2 = 0;
            ItemSet **db1rows = db1->GetRows();
            uint32 db1numRows = db1->GetNumRows();
            for (uint32 k = 0; k < db1numRows; k++) {
                db1ct1 += ct1->CalcTransactionCodeLength(db1rows[k]) * db1rows[k]->GetSupport();
                db1ct2 += ct2->CalcTransactionCodeLength(db1rows[k]) * db1rows[k]->GetSupport();
            }

            double db2ct1 = 0, db2ct2 = 0;
            ItemSet **db2rows = db2->GetRows();
            uint32 db2numRows = db2->GetNumRows();
            for (uint32 l = 0; l < db2numRows; l++) {
                db2ct1 += ct1->CalcTransactionCodeLength(db2rows[l]) * db2rows[l]->GetSupport();
                db2ct2 += ct2->CalcTransactionCodeLength(db2rows[l]) * db2rows[l]->GetSupport();
            }

            double dissimilarity = max((db1ct2 - db1ct1) / db1ct1, (db2ct1 - db2ct2) / db2ct2);
            fprintf(file,"** Dissimilarity(ct%u, ct%u) = %lf\n", ii, jj, dissimilarity);
            fflush(file);
            sum += dissimilarity;
            cnt++;
        }
    }

    fprintf(file,"** AVG Dissimilarity = %lf\n", sum/cnt);
    fflush(file);
}

void CTSet::CalcProbs(Database *db) {
    if(encLengths == nullptr) {
        CalcEncLengths(db);
    }
    uint32 numRows = db->GetNumRows();
    uint64 numTables = codeTables->size();
    probs = new double*[numRows];
    for(uint32 i = 0; i < numRows; i++) {
        probs[i] = new double[numTables];
        double sum = 0;
        for(uint64 j = 0; j < numTables; j++) {
            sum += pow(2, -encLengths[i][j]);
        }
        for(uint64 j = 0; j < numTables; j++) {
            probs[i][j] = pow(2, -encLengths[i][j])/sum;
        }
    }

}

void CTSet::CalcEncLengths(Database *db) {
    uint32 numRows = db->GetNumRows();
    uint64 numTables = codeTables->size();
    encLengths = new double*[numRows];
    for(uint32 i = 0; i < numRows; i++) {
        encLengths[i] = new double[numTables];
        for(uint64 j = 0; j < numTables; j++) {
            encLengths[i][j] = (*(codeTables->begin() + j))->CalcTransactionCodeLength(db->GetRow(i));
        }
    }
}

void CTSet::CalcEntropy(Database *db) {
    if(probs == nullptr) {
        CalcProbs(db);
    }
    uint64 numTables = codeTables->size();
    uint32 numRows = db->GetNumRows();
    entropies = new double[numTables];
    for(uint64 i = 0; i < numTables; i++) {
        double entropy = 0;

        for(uint32 j = 0; j < numRows; j++) {
            entropy += probs[j][i]*log2(probs[j][i]);
        }
        entropies[i] = -entropy/((double) numRows);
    }

    double sum = 0;
    for(uint64 i = 0; i < numTables; i++) {
        sum += entropies[i];
    }
    totalEntropy = sum/((double) GetNumTables());
}



double *CTSet::CalcProbs(double *encLengths) {
    uint64 numTables = codeTables->size();
    auto* probas = new double[numTables];
    double sum = 0;
    for(uint64 i = 0; i < numTables; i++) {
        sum += pow(2, -encLengths[i]);
    }
    for(uint64 j = 0; j < numTables; j++) {
        probas[j] = pow(2, -encLengths[j])/sum;
    }
    return probas;
}

double *CTSet::CalcEncLengths(ItemSet *is) {
    uint64 numTables = codeTables->size();
    auto *encoLengths = new double[numTables];
    for(uint32 i = 0; i < numTables; i++) {
        encoLengths[i] = (*(codeTables->begin() + i))->CalcTransactionCodeLength(is);
    }
    return encoLengths;
}

double *CTSet::SummarizeProbs(Database *db) {
    uint32 numRows = db->GetNumRows();
    uint64 numTables = codeTables->size();
    auto *summary = new double[numTables];

    double allSum = 0;
    for(uint32 i = 0; i < numTables; i++) {
        double sum = 0;
        for(uint64 j = 0; j < numRows; j++) {
            sum += probs[j][i];
            allSum += probs[j][i];
        }
        summary[i] = sum;
    }
    for(uint32 i = 0; i < numTables; i++) {
        summary[i] = summary[i]/allSum;
    }

    return summary;
}

double CTSet::GetAvgEncLength(ItemSet *is) {
    double *encLengths = CalcEncLengths(is);
    double *probs = CalcProbs(encLengths);
    double wAvg = 0;
    uint64 numTables = codeTables->size();
    for(uint64 i = 0; i < numTables; i++) {
        wAvg += (probs[i] * encLengths[i]);
    }
    return wAvg;
}

void CTSet::ReadFromDisk(const string &folder, vector<string> *tables, const bool needfreqs) {
    uint64 i = 0;
    for (auto c : *codeTables) {
        string path = folder;
        string file  = *(tables->begin()+i);
        string all = path + file;
        c->ReadFromDisk(all , needfreqs);
        i++;
    }
}

CTSet *CTSet::CreateCTForClassification(const string &name, ItemSetType dataType, uint64 numTables) {
    CTSet *set = new CTSet(numTables);
    for(uint64 i = 0; i < numTables; i++) {
        set->PushBack(CodeTable::CreateCTForClassification("coverfull", dataType));
    }
    return set;
}

void CTSet::UseThisStuff(Database *db, ItemSetType type, uint32 maxCTElemLength, uint32 toMinSup) {
    for (auto c : *codeTables) {
        c->UseThisStuff(db, type, static_cast<CTInitType>(0), maxCTElemLength, toMinSup);
    }
}

void CTSet::AddOneToEachUsageCount() {
    for (auto c : *codeTables) {
        c->AddOneToEachUsageCount();
    }
}

void CTSet::SetAlphabetCount(uint32 item, uint32 count) {
    for (auto c : *codeTables) {
        c->SetAlphabetCount(item, count);
    }
}

void CTSet::UpdateUsageCountSums(uint32 delta) {
    for (auto c : *codeTables) {
        c->GetCurStats().usgCountSum += delta;
    }
}

void CTSet::PrintStatsFile(FILE *pFILE) {
    ctVec::iterator i;
    int count = 0;
    for (i = codeTables->begin(); i != codeTables->end(); ++i) {
        count++;
        CodeTable *ct = *i;
        CoverStats stats = ct->GetCurStats();
        fprintf(pFILE, " * Result:\t\t(ct%d, %da,%du,%" I64d ",%.0lf,%.0lf,%.0lf)\n", count, stats.alphItemsUsed,
               stats.numSetsUsed, stats.usgCountSum, stats.encDbSize, stats.encCTSize, stats.encSize);
        fflush(pFILE);
    }
}
