#include "cocos2d.h"

using namespace cocos2d;
using namespace std;

class CGamePoint;

class CGameChip : public Sprite
{
public:
	enum class TouchEventType
	{
		BEGAN,
		MOVED,
		ENDED,
		CANCELED
	};

	typedef function<void(Ref*, CGameChip::TouchEventType)> ccTouchCallback;

	static CGameChip* createWithFile(const string& filename);

    CGameChip();
    virtual ~CGameChip();

	void initTouchable();

	bool hitTest(const Vec2& pt);

	bool onTouchBegan(Touch* touch, Event* unusedEvent);
	void onTouchMoved(Touch* touch, Event* unusedEvent);
	void onTouchEnded(Touch* touch, Event* unusedEvent);
	void onTouchCancelled(Touch* touch, Event* unusedEvent);

	void addTouchEventListener(const ccTouchCallback& callback);

	void pushDownEvent();
	void moveEvent();

	virtual void releaseUpEvent();
	virtual void cancelUpEvent();

	Vec2 GetTouchMovePosition() { return _touchMovePosition; }
	int  GetPointIdx() const;
	int	 GetTargetIdx() const;
	void ChangePoint(CGamePoint* point, bool apply_pos = false);

private:
	bool _hitted;
	EventListenerTouchOneByOne* _touchListener;
	Vec2 _touchBeganPosition;
	Vec2 _touchMovePosition;
	Vec2 _touchEndPosition;
	
	ccTouchCallback _touchEventCallback;

	CGamePoint* m_current_point;
	int m_target_point_idx;
};