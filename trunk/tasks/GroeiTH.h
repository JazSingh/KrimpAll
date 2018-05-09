#ifndef FIC_GROEITH_H
#define FIC_GROEITH_H

#include <isc/ItemSetCollection.h>
#include "TaskHandler.h"

class GroeiTH : public TaskHandler{
public:
    GroeiTH(Config *conf);
    virtual ~GroeiTH();

    virtual void HandleTask();
    virtual string BuildWorkingDir();

    void Compress(Config *conf, const string tag = "");
    void DoCompress(Config *conf, Database *db, ItemSetCollection *isc, const string &tag, const string &timetag, const uint32 resumeSup, const uint64 resumeCand);

protected:

};


#endif //FIC_GROEITH_H
