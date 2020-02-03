#include "GameScene.h"
#include "MainMenuScene.h"
#include "LevelManager.h"
#include "GamePoint.h"

#define GAME_BUTTONS_OFFSET 10.0f

CGameScene::CGameScene()
{
	m_GamePhase = GP_NONE;
}

Scene* CGameScene::createScene()
{
	return CGameScene::create();
}

bool CGameScene::init()
{
	if (!Scene::init())
		return false;

	//------Init UI Buttons---------------------------------------------------------------------
	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto home_button = Button::create("HomeNormal.png", "HomeSelected.png");
	home_button->addTouchEventListener(CC_CALLBACK_2(CGameScene::InMainMenuCallback, this));

	float x; x = origin.x + visibleSize.width - GAME_BUTTONS_OFFSET  - home_button->getContentSize().width / 2;
	float y; y = origin.y + visibleSize.height - GAME_BUTTONS_OFFSET - home_button->getContentSize().height / 2;

	home_button->setPosition(Vec2(x, y));
	addChild(home_button);

	auto restart_button = Button::create("RestartNormal.png", "RestartSelected.png");
	restart_button->addTouchEventListener(CC_CALLBACK_2(CGameScene::RestartCallback, this));

	y -= GAME_BUTTONS_OFFSET + restart_button->getContentSize().height;

	restart_button->setPosition(Vec2(x, y));
	addChild(restart_button);
	//------------------------------------------------------------------------------------------

	if (LevelManager().Levels().empty())
		return false;

	StartLevel(LevelManager().GetCurrentLevel());
	scheduleUpdate();

	return true;
}

void CGameScene::update(float dt)
{
	switch (Phase())
	{
		case GP_INPROGRESS: CheckForWin(); break;
	}
}

const Color3B chip_colors[8] = { Color3B::YELLOW, Color3B::GREEN, Color3B::BLUE, Color3B::RED, Color3B::MAGENTA, Color3B::ORANGE, Color3B(0, 255, 255), Color3B(0, 120, 10) };
const Color4F DARK_GRAY(0.4f, 0.4f, 0.4f, 1);
constexpr GLfloat LINE_WIDTH = 6;

void CGameScene::StartLevel(CGameLevel* level)
{
	CCASSERT(level, "Level == nullptr");

	//Main
	DrawSkeleton(level);
	//Sample(Fake)
	DrawSkeleton(level, 320.0f, 0.0f, 0.3f, true);

	int i;
	for (i = 0; i < level->points_amount; ++i)
	{
		auto game_point = CGamePoint::create(level->points.at(i));
		game_point->idx = i;
		addChild(game_point);

		m_GamePoints.push_back(game_point);
	}

	for (i = 0; i < level->chips_amount; ++i)
	{
		auto game_chip = CGameChip::createWithFile("circle.png");

		if (game_chip)
		{
			game_chip->addTouchEventListener(CC_CALLBACK_2(CGameScene::OnChipTouch, this));
			game_chip->setColor(chip_colors[i]);
			game_chip->ChangePoint(m_GamePoints.at(level->default_points.at(i) - 1), true);

			addChild(game_chip);

			m_GameChips.push_back(game_chip);
		}
	}

	SwitchPhase(GP_INPROGRESS);
}

void CGameScene::DrawSkeleton(CGameLevel* level, float x, float y, float scale, bool fake_chips)
{
	CCASSERT(level, "Level == nullptr");

	auto m_DrawNode = DrawNode::create();

	if (m_DrawNode)
	{
		m_DrawNode->setPosition(Vec2(x, y));
		m_DrawNode->setLineWidth(LINE_WIDTH);
		m_DrawNode->setScale(scale);

		int i;
		for (i = 0; i < level->connections_amount; ++i)
			m_DrawNode->drawSegment(level->points.at(level->ways.at(i).a - 1),
				level->points.at(level->ways.at(i).b - 1),
				4.0f,
				DARK_GRAY);

		for (i = 0; i < level->points_amount; ++i)
			m_DrawNode->drawDot(level->points.at(i), 7.0f, Color4F::GRAY);

		if (fake_chips)
			for (i = 0; i < level->chips_amount; ++i)
				m_DrawNode->drawDot(level->points.at(level->win_points.at(i) - 1), 9.0f, (Color4F)chip_colors[i]);

		addChild(m_DrawNode);
	}
}

vector<int> CGameScene::BuildWay(int from, int to)
{
	//string text = "From " + to_string(from) + " To " + to_string(to);
	//MessageBox(text.c_str(), "RESULT");

	auto level = LevelManager().GetCurrentLevel();
	CCASSERT(level, "Level == nullptr");

	const int points_count = level->points_amount;

	vector<int> route;

	stack<int> Stack;
	stack<way> Ways;
	way w;

	vector<int> nodes;				// вершины графа
	for (int i = 0; i < points_count; ++i)
		nodes.push_back(0);			// исходно все вершины равны 0

	for (auto I : m_GameChips)
	{
		if (!I) continue;

		if (I->GetPointIdx() != from)
			nodes.at(I->GetPointIdx()) = 2;
	}

	Stack.push(from);				// помещаем в очередь первую вершину
	while (!Stack.empty())			// пока стек не пуст
	{
		int node = Stack.top();		// извлекаем вершину
		Stack.pop();
		if (nodes[node] == 2) continue;
		nodes[node] = 2;			// отмечаем ее как посещенную

		for (int j = points_count - 1; j >= 0; --j)
		{							// проверяем для нее все смежные вершины
			if (level->graph[node][j] == true && nodes[j] != 2)
			{						 // если вершина смежная и не обнаружена
				Stack.push(j);		 // добавляем ее в cтек
				nodes[j] = 1;		 // отмечаем вершину как обнаруженную
				w.a = node; w.b = j;
				Ways.push(w);
				if (node == to) break;
			}
		}
		//if (node == to) break;
	}

	if (!Ways.empty())
	{
		route.push_back(to);

		while (!Ways.empty())
		{
			w = Ways.top();
			Ways.pop();

			if (w.b == to)
			{
				to = w.a;
				route.push_back(to);
			}
		}

		route.pop_back();

		reverse(route.begin(), route.end());
	}

	//string txt;
	//for (int i = 0; i < route.size(); ++i)
	//{
	//	if (i > 0) txt += " -> ";
	//	txt += to_string(route.at(i));
	//}

	//MessageBox(txt.c_str(), "RESULT");

	return route;
}


void CGameScene::CheckForWin()
{
	auto level = LevelManager().GetCurrentLevel();
	CCASSERT(level, "Level == nullptr");

	for (int i = 0; i < level->chips_amount; ++i)
		if (level->points.at(level->win_points.at(i) - 1) != m_GameChips.at(i)->getPosition())
			return;

	MessageBox("YOU WIN!", "GAME");

	SwitchPhase(GP_END_ROUND);
}

void CGameScene::HighlightPoints(bool value)
{
	auto level = LevelManager().GetCurrentLevel();
	CCASSERT(level, "Level == nullptr");
	
	for (auto& I : m_GamePoints)
		I->Highlight(value);
}

void CGameScene::InMainMenuCallback(Ref* pSender, Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::ENDED)
	{
		auto scene = CMainMenuScene::createScene();
		Director::getInstance()->replaceScene(TransitionFade::create(0.5f, scene));
	}
}

void CGameScene::RestartCallback(Ref* pSender, Widget::TouchEventType type)
{
	if (type == Widget::TouchEventType::ENDED)
	{
		auto scene = CGameScene::createScene();
		Director::getInstance()->replaceScene(TransitionFade::create(0.5f, scene));
	}
}

void CGameScene::OnChipTouch(Ref* pSender, CGameChip::TouchEventType type)
{
	switch (type)
	{
		case CGameChip::TouchEventType::BEGAN: HighlightPoints(true); break;
		case CGameChip::TouchEventType::MOVED: break;
		case CGameChip::TouchEventType::ENDED: HighlightPoints(false); break;
		case CGameChip::TouchEventType::CANCELED:
		{
			HighlightPoints(false);

			auto points = LevelManager().GetCurrentLevel()->points;
			CCASSERT(points.size(), "points.size == 0");

			auto chip = dynamic_cast<CGameChip*>(pSender);

			if (chip)
			{
				vector<int> route = BuildWay(chip->GetPointIdx(), chip->GetTargetIdx());

				if (route.size())
				{
					Vector<FiniteTimeAction*> actions;

					chip->ChangePoint(m_GamePoints.at(route.back()));

					for (auto I : route)
						actions.pushBack(MoveTo::create(0.5f, points.at(I)));

					auto seq = Sequence::create(actions);
					chip->runAction(seq);
				}
			}
		}break;
	}
}