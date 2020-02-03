#include "LevelManager.h"
#include "GamePoint.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string> 
#include <stack>

Vec2 GetVecFromStr(LPCSTR str);
way GetWayFromStr(LPCSTR str);

void CGameLevel::FillGraph()
{
	int i;
	for (i = 0; i < points_amount; ++i)
	{
		vector<bool> vec;
		for (int j = 0; j < points_amount; ++j)
			vec.push_back(false);

		graph.push_back(vec);
	}
	for (i = 0; i < connections_amount; ++i)
	{
		const way cur_way = ways.at(i);
		graph.at(cur_way.a - 1).at(cur_way.b - 1) = true;
		graph.at(cur_way.b - 1).at(cur_way.a - 1) = true;
	}
}

void CGameLevel::FillDefaultPoints(LPCSTR str) { FillPointsVector(default_points, str); }
void CGameLevel::FillWinPoints(LPCSTR str) { FillPointsVector(win_points, str); }

void CGameLevel::FillPointsVector(vector<int>& vec, LPCSTR str)
{
	istringstream ss(str);

	for (int i = 0; i < chips_amount; ++i)
	{
		string s;
		if (!getline(ss, s, ',')) break;

		vec.push_back(atoi(s.c_str()));
	}
}

//-----------------------------------------------------------------------------------------

CLevelManager* g_level_manager = nullptr;

CLevelManager* CLevelManager::Instance()
{
	if (!g_level_manager)
		g_level_manager = new CLevelManager();

	return g_level_manager;
}

CLevelManager::~CLevelManager()
{
	g_level_manager->m_levels.clear();
	g_level_manager = nullptr;
}

void CLevelManager::LoadGameLevels()
{
	ifstream stream;
	int it = 1;

	while (true)
	{
		string path = "Resources/levels/level_";
		path += to_string(it) + ".txt";

		stream.open(path);
		if (stream.is_open())
		{
			m_levels.push_back(new (nothrow) CGameLevel);
			CGameLevel* level = m_levels.back();

			stream >> level->chips_amount;
			stream >> level->points_amount;

			char buf[64];
			int i;

			for (i = 0; i < level->points_amount; ++i)
			{
				stream >> buf;
				level->points.push_back(GetVecFromStr(buf));
			}

			stream >> buf;
			level->FillDefaultPoints(buf);

			stream >> buf;
			level->FillWinPoints(buf);

			stream >> level->connections_amount;

			for (i = 0; i < level->connections_amount; ++i)
			{
				stream >> buf;
				level->ways.push_back(GetWayFromStr(buf));
			}

			level->FillGraph();

			it++;
		}
		else
			break;

		stream.close();
	}
}

//----stuff--------------------------------------------------------------------------------

Vec2 GetVecFromStr(LPCSTR str)
{
	Vec2 vec;

	int cnt = sscanf(str, "%f,%f", &vec.x, &vec.y);
	if (cnt != 2)
		MessageBox("Invalid data format.", "Warning!");

	return vec;
}

way GetWayFromStr(LPCSTR str)
{
	way _way;

	int cnt = sscanf(str, "%d,%d", &_way.a, &_way.b);
	if (cnt != 2)
		MessageBox("Invalid data format.", "Warning!");

	return _way;
}