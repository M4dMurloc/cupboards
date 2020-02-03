#include "MainMenuScene.h"
#include "SimpleAudioEngine.h"
#include "LevelManager.h"
#include "GameScene.h"
#include "ui/UIButton.h"
#include <string> 

#define PLAY_BUTTON_Y_OFFSET 120.0f
#define EXIT_BUTTON_Y_OFFSET 170.0f

Scene* CMainMenuScene::createScene()
{
    return CMainMenuScene::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

bool CMainMenuScene::init()
{
    if (!Scene::init())
        return false;

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	//------Init Header---------------------------------------------------------------------
	auto label = Label::createWithTTF("Cupboards", "fonts/Marker Felt.ttf", 24);

	if (label == nullptr)
		problemLoading("'fonts/Marker Felt.ttf'");
	else
	{
		label->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - label->getContentSize().height));
		addChild(label, 1);
	}

    //------Init Main Menu-----------------------------------------------------------
	auto playItem = MenuItemImage::create("ButtonNormal.png", "ButtonSelected.png", CC_CALLBACK_1(CMainMenuScene::menuPlayCallback, this));

	float x; x = origin.x + visibleSize.width / 2;
	float y; y = origin.y + visibleSize.height - playItem->getContentSize().height / 2 - PLAY_BUTTON_Y_OFFSET;

	InitMenuButton(playItem, "Play", x, y);

    auto closeItem = MenuItemImage::create("ButtonNormal.png", "ButtonSelected.png", CC_CALLBACK_1(CMainMenuScene::menuCloseCallback, this));

	y = origin.y + visibleSize.height - closeItem->getContentSize().height / 2 - EXIT_BUTTON_Y_OFFSET;

	InitMenuButton(closeItem, "Exit", x, y);

    // create menu, it's an autorelease object
    auto menu = Menu::create(playItem, closeItem, NULL);
	if (menu)
	{
		menu->setPosition(Vec2::ZERO);
		addChild(menu, 1);
	}

	//------Init Level Changer-----------------------------------------------------------
	if (LevelManager().Levels().empty())
		LevelManager().LoadGameLevels();

	InitLevelChanger(x, y);

    return true;
}

void CMainMenuScene::InitMenuButton(MenuItemImage* item, LPCSTR text, float x, float y)
{
	if (item == nullptr ||
		item->getContentSize().width <= 0 ||
		item->getContentSize().height <= 0)
	{
		problemLoading("'Normal' and 'Selected' image");
	}
	else
	{
		item->setPosition(Vec2(x, y));

		auto label = Label::createWithTTF(text, "fonts/Arcade.ttf", 16);
		if (label)
		{
			label->setTextColor(Color4B::BLACK);
			label->setPosition(Vec2(item->getContentSize().width / 2, item->getContentSize().height / 2));

			item->addChild(label, 1);
		}
	}
}

void CMainMenuScene::InitLevelChanger(float x, float y)
{	
	//Есть баг с InnerContainer из-за перевернутой системы координат(с якорями не разобрался)
	m_LevelChangerWnd = ScrollView::create();
	if (m_LevelChangerWnd != nullptr)
	{
		m_LevelChangerWnd->setDirection(ScrollView::Direction::VERTICAL);
		m_LevelChangerWnd->setBackGroundImage("lvl_changer_panel.png");
		m_LevelChangerWnd->setContentSize(m_LevelChangerWnd->getBackGroundImageTextureSize());
		m_LevelChangerWnd->setScrollBarEnabled(false);
		m_LevelChangerWnd->setAnchorPoint(Vec2(0.5f, 0.5f));
		m_LevelChangerWnd->setPosition(Vec2(x, y));
		m_LevelChangerWnd->setVisible(false);
		addChild(m_LevelChangerWnd, 1);

		float constexpr step = 10.0f;
		float x_pos = 0.0f, y_pos = 0.0f;

		size_t count = LevelManager().Levels().size();
		for (size_t idx = 0; idx < count; ++idx)
		{
			auto button = Button::create("LevelNormal.png", "LevelSelected.png");
			if (button)
			{
				button->addTouchEventListener(CC_CALLBACK_2(CMainMenuScene::levelChangeCallback, this));

				if (m_LevelChangerWnd->getChildren().size())
				{
					x_pos = m_LevelChangerWnd->getChildren().back()->getPositionX();
					y_pos = m_LevelChangerWnd->getChildren().back()->getPositionY();

					if (x_pos + button->getContentSize().width + step < m_LevelChangerWnd->getInnerContainerSize().width)
						x_pos += button->getContentSize().width + step;
					else
					{
						x_pos = button->getContentSize().width;
						y_pos -= button->getContentSize().height + step;
					}
				}
				else
				{
					x_pos = button->getContentSize().width;
					y_pos = m_LevelChangerWnd->getContentSize().height - button->getContentSize().height;
				}

				button->setPosition(Vec2(x_pos, y_pos));
				button->setTitleColor(Color3B::BLACK);
				button->setTitleText(to_string(idx + 1));

				m_LevelChangerWnd->addChild(button);
			}
		}
	}
}

void CMainMenuScene::menuPlayCallback(Ref* pSender)
{
	m_LevelChangerWnd->setVisible(true);
}

void CMainMenuScene::menuCloseCallback(Ref* pSender) { Director::getInstance()->end(); }

void CMainMenuScene::levelChangeCallback(Ref* pSender, Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::ENDED)
	{
		const Button* btn = dynamic_cast<Button*>(pSender);

		if (btn)
		{
			LevelManager().SetCurrentLevel(atoi(btn->getTitleText().c_str()) - 1);

			auto scene = CGameScene::createScene();
			Director::getInstance()->replaceScene(TransitionFade::create(0.5f, scene));
		}
	}
}