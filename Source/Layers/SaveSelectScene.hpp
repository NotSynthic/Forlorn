#ifndef __SAVESELECTSCENE_H__
#define __SAVESELECTSCENE_H__

#include <axmol.h>

class SaveSelectScene : public ax::Scene
{   
    public:
        static SaveSelectScene* create();
        bool init() override;
        void onBack(Ref* sender);
};

#endif