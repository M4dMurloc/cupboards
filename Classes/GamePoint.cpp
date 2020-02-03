#include "GamePoint.h"

const Color3B LIGHT_GRAY(200, 200, 200);

CGamePoint* CGamePoint::create(Vec2& pos)
{
	CGamePoint* point = new (std::nothrow) CGamePoint();
	if (point && point->initWithFile("small_circle.png"))
	{
		point->autorelease();
		point->setColor(LIGHT_GRAY);
		point->setPosition(pos);
		point->setVisible(false);
		return point;
	}
	CC_SAFE_DELETE(point);
	return nullptr;
}

void CGamePoint::Highlight(bool value)
{
	setVisible(value);
}

void CGamePoint::Select(bool value)
{
	m_bSelected = value;

	setColor(value ? Color3B::WHITE : LIGHT_GRAY);
}