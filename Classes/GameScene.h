#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "ui/UIButton.h"
#include "GameChip.h"

using namespace cocos2d;
using namespace ui;
using namespace std;

class CGameLevel;
class CGameChip;
class CGamePoint;

enum GamePhases
{
	GP_NONE,
	GP_INPROGRESS,
	GP_END_ROUND
};

class CGameScene : public Scene
{
public:
				 CGameScene();
	static Scene* createScene();
			
	virtual bool init();
			void SwitchPhase(GamePhases new_phase) { m_GamePhase = new_phase; }
	  GamePhases Phase() { return m_GamePhase; };
			void update(float dt);

			void CheckForWin();
			void HighlightPoints(bool value);

			//game callbacks
			void InMainMenuCallback(Ref* pSender, Widget::TouchEventType type);
			void RestartCallback(Ref* pSender, Widget::TouchEventType type);
			void OnChipTouch(Ref* pSender, CGameChip::TouchEventType type);

			vector<int> BuildWay(int from, int to);

			vector<CGamePoint*>& Points() { return m_GamePoints; }

	CREATE_FUNC(CGameScene);

private:
	GamePhases m_GamePhase;

	vector<CGamePoint*> m_GamePoints;
	vector<CGameChip*> m_GameChips;

	void StartLevel(CGameLevel* level);
	void DrawSkeleton(CGameLevel* level, float x = 0.0f, float y = 0.0f, float scale = 1.0f, bool fake_chips = false);
};

inline CGameScene& GameScene() { return *dynamic_cast<CGameScene*>(Director::getInstance()->getRunningScene()); }

#endif // __GAME_SCENE_H__

