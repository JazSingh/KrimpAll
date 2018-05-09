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
    if(algoname.compare(0, 5, "groei-") == 0) {
        string strippedAlgoname = algoname.substr(5);
        HashPolicyType hashPolicy;
        strippedAlgoname = StringToHashPolicyType(strippedAlgoname, hashPolicy);
        return new Groei(CodeTable::Create(strippedAlgoname, type), hashPolicy, config);
    }
    return KrimpAlgo::CreateAlgo(algoname, type);
}

string GroeiAlgo::HashPolicyTypeToString(HashPolicyType type) {
    return std::__cxx11::string();
}

string GroeiAlgo::StringToHashPolicyType(string algoname, HashPolicyType &hashPolicy) {
    return std::__cxx11::string();
}

void GroeiAlgo::ProgressToScreen(const uint64 curCandidate, CodeTable *ct) {

}

void GroeiAlgo::LoadCodeTable(const string &ctFile) {

}

void GroeiAlgo::OpenLogFile() {

}

void GroeiAlgo::CloseLogFile() {

}

