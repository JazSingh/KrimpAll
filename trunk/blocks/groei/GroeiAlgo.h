//
// Created by Jaspreet Singh on 19/04/2018.
//

#ifndef FIC_GROEIALGO_H
#define FIC_GROEIALGO_H


#include "../krimp/KrimpAlgo.h"
#include "../slim/SlimAlgo.h"
#include "../krimp/KrimpAlgo.h"

#include <Config.h>

class GroeiAlgo: public KrimpAlgo {
public:
    GroeiAlgo(CodeTable* ct, HashPolicyType hashPolicy, Config* config);

    virtual string	GetShortName();

    virtual void	SetReportIter(const bool b) { mReportIteration = b; }
    virtual void	SetReportIterStyle(ReportType repIter) { mReportIterType = repIter; }

    void			SetResumeCodeTable(const string& ctFile) { mCTFile = ctFile; }
    void			LoadCodeTable(const string& ctFile);

    void			OpenLogFile();
    void			CloseLogFile();

    /* Static factory method */
    static KrimpAlgo*	CreateAlgo(const string &algoname, ItemSetType type, Config* config);

    static string	HashPolicyTypeToString(HashPolicyType type);
    static string	StringToHashPolicyType(string algoname, HashPolicyType& hashPolicy);

    virtual void	ProgressToScreen(const uint64 curCandidate, CodeTable *ct);

protected:
    //static uint32		CountUsageMatches(const uint32* usage_x, const uint32 usage_cnt_x, const uint32* usage_y, const uint32 usage_cnt_y, const uint32 minUsage);

    bool			mWriteLogFile;

    FILE			*mLogFile;
    HashPolicyType 	mHashPolicy;

    bool			mReportIteration;		// report codetable each iteration
    ReportType		mReportIterType;

    string			mCTFile;

    // reportToScreen
    double mScreenReportBits;
};


#endif //FIC_GROEIALGO_H
