#include "cocos2d.h"

using namespace cocos2d;

class CGamePoint : public Sprite
{
	//typedef Vec2 inherited;
public:
	CGamePoint() : m_bSelected(false), idx(-1) {};

	int idx;

	static CGamePoint* create(Vec2& pos);

	void Highlight(bool value);
	void Select(bool value);
	bool IsSelected() const { return m_bSelected; }
	
private:
	bool m_bSelected;
	/*Sprite* m_sprite;*/
};