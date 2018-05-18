//
// Created by Jaspreet Singh on 19/04/2018.
//

#include "Groei.h"
#include <db/Database.h>
#include "../slim/codetable/CCPUCodeTable.h"
#include <StringUtils.h>
#include <boost/tokenizer.hpp>
#include <omp.h>

using namespace boost;

/**
 * Constructors
 */
Groei::Groei(CodeTable *ct, HashPolicyType hashPolicy, Config *config)
        : GroeiAlgo(ct, hashPolicy, config) {
    mWriteLogFile = true;
}

CodeTable *Groei::DoeJeDing(const uint64 candidateOffset, const uint32 startSup) {
    // Read properties from config
    uint32 beamWidth = 1; //mConfig->Read<uint32>("beamWidth");
    uint32 numComplexities = 1; //mConfig->Read<uint32>("numComplexities");
    string sComplexities = "46"; //mConfig->Read<string>("complexities", "");
    uint32 *complexities = StringUtils::TokenizeUint32(sComplexities, numComplexities);
    uint32 *maxComplexity = std::max_element(complexities, complexities + numComplexities);

    { // Extract minsup parameter from tag
        string dbName;
        string settings;
        string type;
        IscOrderType order;
        ItemSetCollection::ParseTag(mTag, dbName, type, settings, order);
        float minsupfloat;
        if(settings.find('.') != string::npos && (minsupfloat = (float)atof(settings.c_str())) <= 1.0f)
            mMinSup = (uint32) floor(minsupfloat * mDB->GetNumTransactions());
        else
            mMinSup = (uint32) atoi(settings.c_str());
    }

    mCompressionStartTime = omp_get_wtime();
    mNumCandidates = 0;
    mProgress = -1;
    uint32 candidateCount = 0;//MG

    if(mWriteReportFile == true)
        OpenReportFile(true);
    if(mWriteCTLogFile == true) {
        OpenCTLogFile();
        mCT->SetCTLogFile(mCTLogFile);
    }
    if (mWriteLogFile == true)
        OpenLogFile();

    mStartTime = time(NULL);
    mScreenReportTime = time(NULL);
    mScreenReportCandidateIdx = 0;
    mScreenReportCandPerSecond = 0;
    mScreenReportCandidateDelta = 5000;

    // Initialize
    uint32 complexityLvl = 0;
    uint32 iteration = 0;
   // mCT->CoverDB(mCT->GetCurStats());
    auto *candidates = new CTSet();
    auto ctAlpha = mCT->Clone();
    candidates->Add(ctAlpha);

    uint64 numIsc = mISC->GetNumItemSets();
    mISC->LoadItemSets(numIsc);
    ItemSet **ctlist = mISC->GetLoadedItemSets();

    //TODO
    CoverStats stats = mCT->GetCurStats();
    stats.numCandidates = mNumCandidates;
    printf(" * Start:\t\t(stdTable, %da,%du,%" I64d ",%.0lf,%.0lf,%.0lf)\n",stats.alphItemsUsed, stats.numSetsUsed, stats.usgCountSum,stats.encDbSize,stats.encCTSize,stats.encSize);

    if(mWriteProgressToDisk == true) {
        ProgressToDisk(mCT, 0, 0, numIsc, true, true);
    }

    // Start: for all complexity levels from [0..max]:
    while (iteration <= *maxComplexity) {
        CTSet *prevBest = candidates;
        CoverStats prevBestStats = prevBest->GetBest();
        CoverStats prevWorstStats;
        if(candidates->GetNumTables() == 1) {
            prevWorstStats = prevBestStats;
        } else  {
            prevWorstStats = prevBest->GetWorst();
        }
        candidates = new CTSet();

        // For all item sets in the item set collection:
        for (uint64 i = 0; i < numIsc; i++) {
            ItemSet *itemSet = ctlist[i]->Clone();
            if(mNeedsOccs) {
                mDB->DetermineOccurrences(itemSet);
            }
            itemSet->SetID(i);
            uint32 sup = itemSet->GetSupport();

            // ignore singletons and item sets under minimum support
            if(itemSet->GetLength() <= 1 || sup < mMinSup) {
                delete itemSet;
                continue;
            }

            // Add to all clones of all code tables
            prevBest->ResetIterator();
            while (!prevBest->IsIteratorEnd()) {
                CodeTable *curTable = prevBest->NextCodeTable();
                candidates->Add(curTable->Clone());
                if (!candidates->ContainsItemSet(curTable, itemSet)) {
                    curTable = curTable->Clone();
                    curTable->Add(itemSet, itemSet->GetID());
                    itemSet->SetUsageCount(0);
                    curTable->CoverDB(curTable->GetCurStats());
                    curTable->CommitAdd(mWriteCTLogFile);
                    if (curTable->GetCurStats().encSize < prevWorstStats.encSize) {
                        candidates->Add(curTable); // Only add those that improve compression.
                    }
                }
            }
        }
        candidates->SortAndPrune(beamWidth);

        if(candidates->AvgCompression() < 0) {
            THROW("L(D|M) < 0. That's not good.");
        }

        if (iteration == *(complexities + complexityLvl)) {
            //TODO something interesting
            complexityLvl++;
        }

        mCT = candidates->GetBestTable();
        if(mWriteProgressToDisk == true) {
            ProgressToDisk(mCT, 0, 0, numIsc, true, true);
        }
        iteration++;
    }

    mCT->SetCodeTableSet(candidates);

    double timeCompression = omp_get_wtime() - mCompressionStartTime;
    printf(" * Time:    \t\tCompressing the database took %f seconds.\t\t\n", timeCompression);

    candidates->PrintStats();

    CloseCTLogFile();
    CloseReportFile();
    CloseLogFile();

    mCT->EndOfKrimp();


    return mCT;
}
