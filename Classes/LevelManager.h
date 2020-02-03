#include "cocos2d.h"

using namespace cocos2d;
using namespace std;

struct way;
class CGameLevel;
class CLevelManager;

struct way
{
	int a;
	int b;
};

class CGameLevel
{
public:
	int chips_amount; //���������� �����
	int points_amount; //���������� �����, ���� ����� ��������� �����
	vector<Vec2> points; //������� �����, ���� ����� ��������� �����
	vector<int>  default_points; //��������� ������������ ����� �� ������ 
	vector<int>  win_points; //���������� ��������� ����� �� ������

	int connections_amount; //���������� ����������

	vector<way> ways; //������ ���������� ����� ������ �����

	vector<vector<bool>> graph;

	void FillGraph();
	void FillDefaultPoints(LPCSTR str);
	void FillWinPoints(LPCSTR str);

private:
	void FillPointsVector(vector<int>& vec, LPCSTR str);
};

class CLevelManager
{
public:
	static CLevelManager* Instance();

	CLevelManager() : current_lvl_id(0) {}
	~CLevelManager();

	void LoadGameLevels();
	void SetCurrentLevel(int id) { current_lvl_id = id; }

	CGameLevel* GetCurrentLevel() { return m_levels.at(current_lvl_id); }
	vector<CGameLevel*>& Levels() { return m_levels; }

private:
	vector<CGameLevel*> m_levels;
	int current_lvl_id;

	CLevelManager(const CLevelManager&);
	CLevelManager& operator=(CLevelManager&);
};

inline CLevelManager& LevelManager() { return *CLevelManager::Instance(); }
