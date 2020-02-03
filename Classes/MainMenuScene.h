#ifndef __MEINMENU_SCENE_H__
#define __MEINMENU_SCENE_H__

#include "cocos2d.h"
#include "ui/UIScrollView.h"

using namespace cocos2d;
using namespace ui;
using namespace std;

class CMainMenuScene : public Scene
{
public:
    static Scene* createScene();

    virtual bool init();
    
    //menu callbacks
	void menuPlayCallback(Ref* pSender);
    void menuCloseCallback(Ref* pSender);

	void levelChangeCallback(Ref* pSender, Widget::TouchEventType type);
    
    // implement the "static create()" method manually
    CREATE_FUNC(CMainMenuScene);

private:

	ScrollView* m_LevelChangerWnd;

	void InitMenuButton(MenuItemImage* item, LPCSTR text, float x = 0.0f, float y = 0.0f);
	void InitLevelChanger(float x = 0.0f, float y = 0.0f);
};

#endif // __MEINMENU_SCENE_H__
