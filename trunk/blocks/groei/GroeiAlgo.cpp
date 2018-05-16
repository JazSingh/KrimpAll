//
// Created by Jaspreet Singh on 19/04/2018.
//

#include <time.h>
#include <omp.h>

#include "GroeiAlgo.h"
#include "Groei.h"

GroeiAlgo::GroeiAlgo(CodeTable *ct, HashPolicyType hashPolicy, Config *config) {
    mConfig = config;
    mHashPolicy = hashPolicy;
    mCT = ct;

    mReportIteration = false;
    mReportIterType = ReportAll;

    mLogFile = NULL;

    mWriteCTLogFile = config->Read<bool>("writeCTLogFile", true);
    mWriteReportFile = config->Read<bool>("writeReportFile", true);
    mWriteProgressToDisk = config->Read<bool>("writeProgressToDisk", true);
    mWriteLogFile = config->Read<bool>("writeLogFile", true);
}

string GroeiAlgo::GetShortName() {
    return HashPolicyTypeToString(mHashPolicy) + "-" + KrimpAlgo::GetShortName();
}

KrimpAlgo *GroeiAlgo::CreateAlgo(const string &algoname, ItemSetType type, Config *config) {
    if(algoname.compare(0, 5, "groei") == 0) {
        string strippedAlgoname = algoname.substr(6);
        HashPolicyType hashPolicy;
        strippedAlgoname = StringToHashPolicyType(strippedAlgoname, hashPolicy);
        return new Groei(CodeTable::Create(strippedAlgoname, type), hashPolicy, config);
    }
    return KrimpAlgo::CreateAlgo(algoname, type);
}

string GroeiAlgo::HashPolicyTypeToString(HashPolicyType type) {
    switch (type) {
        case hashNoCandidates:
            return "n";
        case hashAllCandidates:
            return "a";
        case hashCtCandidates:
            return "c";
    }
    THROW("Say whut?");}

string GroeiAlgo::StringToHashPolicyType(string algoname, HashPolicyType &hashPolicy) {
    hashPolicy = hashNoCandidates;
    if (algoname.compare(0, 2, "n-") == 0) {
        hashPolicy = hashNoCandidates;
        algoname = algoname.substr(2);
    } else if (algoname.compare(0, 2, "a-") == 0) {
        hashPolicy = hashAllCandidates;
        algoname = algoname.substr(2);
    } else if (algoname.compare(0, 2, "c-") == 0) {
        hashPolicy = hashCtCandidates;
        algoname = algoname.substr(2);
    }
    return algoname;}

void GroeiAlgo::ProgressToScreen(const uint64 curCandidate, CodeTable *ct) {
    if(Bass::GetOutputLevel() > 0) {
        uint64 canDif = curCandidate - mScreenReportCandidateIdx;

        if(true) {
            // Calculate Candidates per Second, update per 30s
            time_t curTime = time(NULL);
            if(canDif >= mScreenReportCandidateDelta) {
                uint32 timeDif = (uint32) (curTime - mScreenReportTime);
                mScreenReportCandPerSecond = (uint32) (curCandidate - mScreenReportCandidateIdx) / (timeDif > 0 ? timeDif : 1);
                mScreenReportCandidateDelta = (mScreenReportCandPerSecond + 1) * 30;
                mScreenReportTime = curTime;
                mScreenReportCandidateIdx = curCandidate;
            }


            double numBits = ct->GetCurStats().encSize;
            double difBits = mScreenReportBits - numBits;
            mScreenReportBits = numBits;

            // Calculate Percentage
            int d = curCandidate % 4;
            char c = '*';
            switch(d) {
                case 0 : c = '-'; break;
                case 1 : c = '\\'; break;
                case 2 : c = '|'; break;
                default : c = '/';
            }
            uint32 numprinted = 0;
            numprinted = printf_s(" %c Progress:\t\t%" I64d " (%d/s), %.00fb (-%.01fb) (%ds)    ", c, curCandidate, mScreenReportCandPerSecond, numBits, difBits, curTime - mStartTime);
            if(Bass::GetOutputLevel() == 3)
                printf("\n");
            else {
                if(numprinted < 45)
                    for(uint32 i=0; i< (45-numprinted); i++)
                        printf(" ");
                printf("\r");
            }
            fflush(stdout);
        }
    }
}

void GroeiAlgo::LoadCodeTable(const string &ctFile) { //TODO
    mCT->ReadFromDisk(ctFile, false);
    mCT->CoverDB(mCT->GetCurStats());
    mCT->CommitAdd(); // mAdded = NULL; prevStats = curStats;
}

void GroeiAlgo::OpenLogFile() {
    string logFilename = mOutDir;
    logFilename.append(mTag);
    logFilename.append(".log");
//	printf("open LogFile: %s\n", logFilename.c_str());
    mLogFile = fopen(logFilename.c_str(), "w");
}

void GroeiAlgo::CloseLogFile() {
    if(mLogFile) {
//		printf("close LogFile\n");
        fclose(mLogFile);
        mLogFile = NULL;
    }
}

