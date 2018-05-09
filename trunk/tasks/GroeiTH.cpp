//
// Created by Jaspreet Singh on 19/04/2018.
//

#if defined (_WINDOWS)
#include <direct.h>
	#include <windows.h>
#elif (defined (__unix__) || (defined (__APPLE__) && defined (__MACH__)))
#include <sys/stat.h>
#endif

// -- qtils
#include <RandomUtils.h>
#include <Config.h>
#include <FileUtils.h>
#include <StringUtils.h>
#include <SystemUtils.h>
#include <TimeUtils.h>
#include <logger/Log.h>

// -- bass
#include <Bass.h>
#include <db/ClassedDatabase.h>
#include <db/DbFile.h>
#include <isc/IscFile.h>
#include <isc/ItemSetCollection.h>
#include <itemstructs/CoverSet.h>

#include "../blocks/groei/GroeiAlgo.h"
#include "../blocks/krimp/codetable/CodeTable.h"

#include "../FicMain.h"

#include "GroeiTH.h"

GroeiTH::GroeiTH(Config *conf) : TaskHandler(conf) {}

GroeiTH::~GroeiTH() {
    // not my Config *conf
}

void GroeiTH::HandleTask() {
    string command = mConfig->Read<string>("command");

    if(command.compare("compress") == 0)	Compress(mConfig);
    else	THROW("Unable to handle task `" + command + "`");
}

string GroeiTH::BuildWorkingDir() {
    return mConfig->Read<string>("taskclass") + "/";
}

void GroeiTH::Compress(Config *conf, const string tag) {

}

void GroeiTH::DoCompress(Config *conf, Database *db, ItemSetCollection *isc, const string &tag, const string &timetag,
                         const uint32 resumeSup, const uint64 resumeCand) {

}

