//
// Created by Jaspreet Singh on 19/04/2018.
//

#include "Groei.h"
#include <db/Database.h>
#include "../slim/codetable/CCPUCodeTable.h"
#include "../slim/SlimAlgo.h"
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
    uint32 beamWidth = 5; //mConfig->Read<uint32>("beamWidth");
    uint32 numComplexities = 1; //mConfig->Read<uint32>("numComplexities");
    string sComplexities = "20"; //mConfig->Read<string>("complexities", "");
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
    vector<bool> tabooList(numIsc);

    //TODO
    CoverStats stats = mCT->GetCurStats();
    stats.numCandidates = mNumCandidates;
    printf(" * Start:\t\t(%da,%du,%" I64d ",%.0lf,%.0lf,%.0lf)\n",stats.alphItemsUsed, stats.numSetsUsed, stats.usgCountSum,stats.encDbSize,stats.encCTSize,stats.encSize);

    CoverStats &curStats = mCT->GetCurStats();

    // Start: for all complexity levels from [0..max]:
    while (iteration <= *maxComplexity) {
        CTSet *prevBest = candidates;
        //delete candidates;
        candidates = new CTSet();

        // For all item sets in the item set collection:
        for (uint64 i = 0; i < numIsc; i++) {
            if(tabooList[i]) {
                continue;
            }
            ItemSet *itemSet = ctlist[i]->Clone();
            if(mNeedsOccs) {
                mDB->DetermineOccurrences(itemSet);
            }
            itemSet->SetID(i);
            uint32 sup = itemSet->GetSupport();

            if(itemSet->GetLength() <= 1 || sup < mMinSup) {
                tabooList[i] = true;
                continue;
            }

            // Add to all clones of all code tables
            prevBest->ResetIterator();
            while (!prevBest->IsIteratorEnd()) {
                CodeTable *curTable = prevBest->NextCodeTable()->Clone();

                curTable->Add(itemSet, itemSet->GetID());
                itemSet->SetUsageCount(0);
                curTable->CoverDB(curTable->GetCurStats());
                candidates->Add(curTable);
            }
            tabooList[i] = true;
        }
        candidates->SortAndPrune(beamWidth);
        if(candidates->AvgCompression() < 0) {
            THROW("L(D|M) < 0. That's not good.");
        }
        if (iteration == *(complexities + complexityLvl)) {
            //TODO something interesting
            complexityLvl++;
        }
        iteration++;
    }
    ctAlpha->SetCodeTableSet(candidates);

    double timeCompression = omp_get_wtime() - mCompressionStartTime;
    printf(" * Time:    \t\tCompressing the database took %f seconds.\t\t\n", timeCompression);
    //printf(" * Cache Hits:\t\t%lu (%.2f%%)  \n", mNumCacheHits, ((100.0 * (double)mNumCacheHits)/((double)mNumCacheHits + (double)mNumGainCalculations)));

    stats = mCT->GetCurStats();
    printf(" * Result:\t\t(%da,%du,%" I64d ",%.0lf,%.0lf,%.0lf)\n",stats.alphItemsUsed, stats.numSetsUsed, stats.usgCountSum,candidates->AvgCompression(),stats.encCTSize,stats.encSize);

    //printf(" * Amount of Fun:\t%d / %d OMGLOL\t%d / %d ROFLCOPTER\n", mLOLCounter, mOMGCounter, mCopterCounter, mROFLCounter);

    if(mWriteProgressToDisk == true) {
        ProgressToDisk(mCT, 0, 0, numIsc, true, true);
    }
    CloseCTLogFile();
    CloseReportFile();
    CloseLogFile();

    mCT->EndOfKrimp();


    return ctAlpha;
}
