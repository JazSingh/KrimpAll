//
// Created by Jaspreet Singh on 19/04/2018.
//

#include <omp.h>
#include <db/Database.h>
#include "../slim/codetable/CCPUCodeTable.h"
#include <StringUtils.h>
#include <boost/tokenizer.hpp>
#include <isc/ItemSetCollection.h>
#include "GroeiSlim.h"

GroeiSlim::GroeiSlim(CodeTable *ct, HashPolicyType hashPolicy, Config *config) : GroeiAlgo(ct, hashPolicy, config) {
    mWriteLogFile = true;
}

CodeTable *GroeiSlim::DoeJeDing(const uint64 candidateOffset, const uint32 startSup) {
    // Read properties from config
    uint32 beamWidth = mConfig->Read<uint32>("beamWidth");
    uint32 numComplexities = mConfig->Read<uint32>("numComplexities");
    string sComplexities = mConfig->Read<string>("complexities", "");
    uint32 *complexities = StringUtils::TokenizeUint32(sComplexities, numComplexities);
    uint32 *maxComplexity = std::max_element(complexities, complexities + numComplexities);

    std::cout << " ** Parameters:\t [beam width: " << beamWidth << "], [number of complexities: " << numComplexities << "], [complexities: " << sComplexities << "]\n" ;

    { // Extract minsup parameter from tag
        string dbName;
        string settings;
        string type;
        IscOrderType order;
        ItemSetCollection::ParseTag(mTag, dbName, type, settings, order);
        float minsupfloat;
        if (settings.find('.') != string::npos && (minsupfloat = (float) atof(settings.c_str())) <= 1.0f)
            mMinSup = (uint32) floor(minsupfloat * mDB->GetNumTransactions());
        else
            mMinSup = (uint32) atoi(settings.c_str());
    }

    mCompressionStartTime = omp_get_wtime();
    mNumCandidates = 0;
    mProgress = -1;

    if (mWriteReportFile == true)
        OpenReportFile(true);
    if (mWriteCTLogFile == true) {
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
    uint32 complexityLvl = 1;
    uint32 iteration = 1;

    auto *candidates = new CTSet();
    auto ctAlpha = mCT->Clone();
    candidates->Add(ctAlpha);

    // Init stats
    CoverStats stats = mCT->GetCurStats();
    stats.numCandidates = mNumCandidates;
    printf(" * Start:\t\t(stdTable, %da,%du,%" I64d ",%.0lf,%.0lf,%.0lf)\n", stats.alphItemsUsed, stats.numSetsUsed,
           stats.usgCountSum, stats.encDbSize, stats.encCTSize, stats.encSize);

    uint64 numCandidates;
    numCandidates = 0;

    ItemSet *m;

    if (mWriteProgressToDisk == true) {
        ProgressToDisk(mCT, 0, 0, 0, false, true);
    }

    while (iteration <= *maxComplexity) {
        CTSet *best_prev = candidates;
        CoverStats &prevBestStats = best_prev->GetBestStats();
        candidates = new CTSet(beamWidth);
        best_prev->ResetIterator();

        while (!best_prev->IsIteratorEnd()) {
            CodeTable *prev = best_prev->NextCodeTable();
            CoverStats &prevStats = prev->GetCurStats();

            islist *ctlist = prev->GetItemSetList();
            ctlist->splice(ctlist->end(), *prev->GetSingletonList());

            islist::iterator cend_i = ctlist->end();
            cend_i--;
            islist::iterator cend_j = ctlist->end();

            for (islist::iterator cit = ctlist->begin(); cit != cend_i; ++cit) {
                for (islist::iterator cjt = cit; cjt != cend_j; ++cjt) {
                    if (cit != cjt) {

                        m = (*cit)->Union(*cjt);

                        if (m->GetLength() > mDB->GetMaxSetLength()) {
                            delete m;
                            continue;
                        }

                        ++numCandidates;

                        if (mNeedsOccs)
                            mDB->DetermineOccurrences(m);
                        mDB->CalcSupport(m);
                        mDB->CalcStdLength(m); // BUGFIX: itemset has no valid standard encoded size

                        if (m->GetSupport() < mMinSup) {
                            delete m;
                            continue;
                        }

                        if (mWriteLogFile) fprintf(mLogFile, "Candidate: %s\n", m->ToCodeTableString().c_str());

                        CodeTable *candidate = prev->Clone();
                        CoverStats &curStats = candidate->GetCurStats();

                        ItemSet *accepted = m->Clone();
                        candidate->Add(accepted);
                        accepted->SetUsageCount(0);
                        candidate->CoverDB(curStats);
                        candidate->CommitAdd(false);

                        if (curStats.encDbSize < 0) {
                            THROW("L(D|M) < 0. That's not good.");
                        }

                        if (mPruneStrategy == PostAcceptPruneStrategy) { // Post-decide On-the-fly pruning
                            PrunePostAccept(candidate);
                        }

                        if (candidate->GetCurStats().encSize < prevBestStats.encSize) {
                            if (candidates->GetNumTables() < beamWidth) {
                                candidates->Add(candidate);
                            } else if (candidate->GetCurStats().encSize <
                                       candidates->GetWorstStats().encSize) {
                                candidates->Add(candidate);
                                candidates->SortAndPrune(beamWidth);
                            } else {
                                delete candidate;
                            }
                        } else {
                            delete candidate;
                        }

                        delete m;
                    }
                }
            }
            delete ctlist;
        }

        if (candidates->GetNumTables() == 0) {
            candidates = best_prev;
            printf("No improvement compared to last iteration. Quitting...\n");
            break;
        }

        candidates->SortAndPrune(beamWidth);

        if (candidates->AvgCompression() < 0) {
            THROW("L(D|M) < 0. That's not good.\n");
        }

        if(candidates->GetBestTable()->GetCurSize() >= mCT->GetCurSize()) {
            candidates = best_prev;
            printf("No improvement compared to last iteration. Quitting...\n");
            break;
        }

        if (iteration == *(complexities + complexityLvl)) {
            //TODO something interesting
            complexityLvl++;
        }

        mCT = candidates->GetBestTable();
        if (mWriteProgressToDisk == true) {
            ProgressToDisk(mCT, 0, 0, 0, false, true);
        }
        stats = mCT->GetCurStats();
        stats.numCandidates = mNumCandidates;
        printf(" * Busy:\t\t(%ui, %da,%du,%" I64d ",%.0lf,%.0lf,%.0lf)\n", iteration, stats.alphItemsUsed,
               stats.numSetsUsed,
               stats.usgCountSum, stats.encDbSize, stats.encCTSize, stats.encSize);

        iteration++;
        delete best_prev;
    }

    mCT->SetCodeTableSet(candidates);

    double timeCompression = omp_get_wtime() - mCompressionStartTime;
    printf(" * Time:    \t\tCompressing the database took %f seconds.\t\t\n", timeCompression);

    candidates->PrintStats();
    candidates->CalcEncLengths(mCT->GetDatabase());
    candidates->CalcProbs(mCT->GetDatabase());
    candidates->CalcEntropy(mCT->GetDatabase());

    uint64 numCt = candidates->GetNumTables();

    double *entropies = candidates->GetEntropies();
    printf(" * AVG Entropy: %lf\n", candidates->GetTotalEntropy());
    for(uint64 i = 0; i < numCt; i++) {
        printf(" * \tEntropy CT%llu: %lf\n", i+1, entropies[i]);
    }

    double **probs = candidates->GetProbs();
    Database *db = mCT->GetDatabase();
    uint64 numRows = db->GetNumRows();
//    printf("\n\n * Probabilities:\n");
//    for(uint64 i = 0; i < numRows; i++) {
//        printf(" * itemset %llu: ", i);
//        for(uint64 j = 0; j < numCt; j ++) {
//            printf(" CT%llu: %lf,", j+1, probs[i][j]);
//        }
//        printf("\n");
//    }

    double *summaryProbs = candidates->SummarizeProbs(db);
    printf("\n\n * SUMMARY probs (CDF):");
    for(uint64 j = 0; j < numCt; j ++) {
        printf(" CT%llu: %lf,", j+1, summaryProbs[j]);
    }
    printf("\n");

//    double **encLengths = candidates->GetEncLengths();
//    printf("\n\n * Encoded Lengths:\n");
//    for(uint64 i = 0; i < numRows; i++) {
//        printf(" * itemset %llu: ", i+1);
//        for(uint64 j = 0; j < numCt; j ++) {
//            printf(" CT%llu: %lf,", j+1, encLengths[i][j]);
//        }
//        printf("\n");
//    }

    candidates->Dissimilarity(db);

    CloseCTLogFile();
    CloseReportFile();
    CloseLogFile();

    mCT->EndOfKrimp();

    if (mWriteProgressToDisk == true) {
        ProgressToDisk(mCT, 0, 0, 0, true, true);
    }

    return mCT;
}
