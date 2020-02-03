#include "GameScene.h"
#include "GamePoint.h"

CGameChip::CGameChip() :
_hitted(false),
_touchListener(nullptr),
_touchBeganPosition(Vec2::ZERO),
_touchMovePosition(Vec2::ZERO),
_touchEndPosition(Vec2::ZERO)
{
	m_current_point = nullptr;
	m_target_point_idx = -1;
}

CGameChip::~CGameChip() 
{
	//clean up _touchListener
	_eventDispatcher->removeEventListener(_touchListener);
	CC_SAFE_RELEASE_NULL(_touchListener);
}

CGameChip* CGameChip::createWithFile(const string& filename)
{
	CGameChip* game_chip = new (nothrow) CGameChip();

	if (game_chip && game_chip->initWithFile(filename))
	{
		game_chip->autorelease();
		game_chip->initTouchable();
		return game_chip;
	}

	CC_SAFE_DELETE(game_chip);
	return nullptr;
}

void CGameChip::initTouchable()
{
	_touchListener = EventListenerTouchOneByOne::create();
	CC_SAFE_RETAIN(_touchListener);
	_touchListener->setSwallowTouches(true);
	_touchListener->onTouchBegan = CC_CALLBACK_2(CGameChip::onTouchBegan, this);
	_touchListener->onTouchMoved = CC_CALLBACK_2(CGameChip::onTouchMoved, this);
	_touchListener->onTouchEnded = CC_CALLBACK_2(CGameChip::onTouchEnded, this);
	_touchListener->onTouchCancelled = CC_CALLBACK_2(CGameChip::onTouchCancelled, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(_touchListener, this);
}

bool CGameChip::hitTest(const Vec2& pt)
{
	Vec2 nsp = convertToNodeSpace(pt);
	Rect bb;
	bb.size = _contentSize;
	if (bb.containsPoint(nsp))
	{
		return true;
	}
	return false;
}

bool CGameChip::onTouchBegan(Touch* touch, Event* unusedEvent)
{
	_hitted = false;
	_touchBeganPosition = touch->getLocation();
	if (hitTest(_touchBeganPosition))
	{
		_hitted = true;

		runAction(ScaleTo::create(0.2f, 1.2f));
	}

	if (!_hitted)
	{
		return false;
	}

	pushDownEvent();
	return true;

}

void CGameChip::onTouchMoved(Touch* touch, Event* unusedEvent)
{
	_touchMovePosition = touch->getLocation();

	auto points = GameScene().Points();
	CCASSERT(!points.empty(), "points is empty");

	for (auto &I : points)
		I->Select(_touchMovePosition.distance(I->getPosition()) <= 8.0f);

	moveEvent();
}

void CGameChip::onTouchEnded(Touch* touch, Event* unusedEvent)
{
	_touchEndPosition = touch->getLocation();

	auto points = GameScene().Points();
	CCASSERT(!points.empty(), "points is empty");

	for (auto &I : points)
	{
		if (I->IsSelected())
		{
			I->Select(false);
			m_target_point_idx = I->idx;
			cancelUpEvent();
			break;
		}
	}

	runAction(ScaleTo::create(0.2f, 1.0f));

	if (hitTest(_touchEndPosition))
	{
		releaseUpEvent();
	}
	else
		GameScene().HighlightPoints(false);
}

void CGameChip::onTouchCancelled(Touch* touch, Event* unusedEvent)
{
	cancelUpEvent();
}

void CGameChip::addTouchEventListener(const ccTouchCallback& callback)
{
	this->_touchEventCallback = callback;
}

void CGameChip::pushDownEvent()
{
	this->retain();
	if (_touchEventCallback)
	{
		_touchEventCallback(this, TouchEventType::BEGAN);
	}
	this->release();
}

void CGameChip::moveEvent()
{
	this->retain();
	if (_touchEventCallback)
	{
		_touchEventCallback(this, TouchEventType::MOVED);
	}
	this->release();
}

void CGameChip::releaseUpEvent()
{
	this->retain();
	if (_touchEventCallback)
	{
		_touchEventCallback(this, TouchEventType::ENDED);
	}	
	this->release();
}

void CGameChip::cancelUpEvent()
{
	this->retain();
	if (_touchEventCallback)
	{
		_touchEventCallback(this, TouchEventType::CANCELED);
	}
	this->release();
}

int CGameChip::GetPointIdx() const
{
	return m_current_point->idx; 
}

int	CGameChip::GetTargetIdx() const
{
	return m_target_point_idx;
}

void CGameChip::ChangePoint(CGamePoint* point, bool apply_pos)
{
	m_current_point = point;

	if (apply_pos)
		setPosition(m_current_point->getPosition());
}