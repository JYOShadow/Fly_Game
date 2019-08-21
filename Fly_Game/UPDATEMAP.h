#pragma once
#include <iostream>
#include <cstdlib>
#include <windows.h>
#include <ctime>

#define FLY_COUNT 10
#define COLLISIONDAMAGE 5
#define PLAYER_FLY f_head->next->node

#define PLAYER_FLY_TYPE 2
#define AI_FLY1_TYPE 3

#define PLAYER_FLY_HEALTHY 20
#define AI_FLY1_HEALTHY 3

#define PLAYER_FLY_BULLET 10
#define AI_FLY1_BULLET 21

#define PLAYER_FLY_SHOOT 1
#define AI_FLY1_SHOOT 9






typedef struct flying
{
	int x;
	int y;
	int pre_x;
	int pre_y;
	int type;
	int healthy;
	int bullet_type;
	int shoot;
	bool exist = true;
};

typedef struct bullet
{
	int x;
	int y;
	int type;
	int damge;
	int dir_x;//0 1 -1
	int dir_y;//0 1 -1
	bool exist = true;
};

typedef struct bullet_node
{
	bullet node;
	bullet_node* next;
	bullet_node* fp;
}*bullet_n;

typedef struct fly_node
{
	fly_node* next;
	fly_node* fp;
	flying node;

}*fly_n;

class UPDATEMAP
{
	int boss_x = 1;
	int score = 0;
	int AI_speed = 3;
	bool operation = true;
	size_t clearn_number_times = 3;
	size_t row;
	size_t col;
	size_t fly_count;
	int** map;
	bullet_n b_head = nullptr;
	bullet_n b_tail = nullptr;
	fly_n f_head = nullptr;
	fly_n f_tail = nullptr;

	void InitMap();
	void ShowMap();

	void Update();
	void UpdateFlying();
	void UpdateBullet();
	void UpdateMap();
	void CleanFlyNode();
	void CleanBulletNode();


	void CreateBulletType10(fly_n p);
	void CreateBulletType20(fly_n p);
	void AddBullet(bullet bn);
	void DeleteBullet();
	bullet_n SearchBullet(int x, int y);
	void ScatterBullet(int& x, int& y, bullet b);
	void BulletMove(int& x, int& y, bullet b);
	bool BulletCollide(int x, int y);//碰撞判定
	void BulletDecisionSettlement(int x, int y, bool b_c, bullet_n& pr);
	void SiteClearing();//清屏
	void ChangingBullet(bullet& b);

	void AddFly(flying fb);//向飞船链表添加新结点
	void DeleteFly();//删除飞船链表结点
	fly_n SearchFly(int x, int y);//在飞船链表寻找新结点
	int LenFly();//飞船链表长度
	void FlyShoot(fly_n& p);//飞船发射子弹
	void ProductFly();//随机生成小飞船
	int FlyCollide(int add_x, int add_y, fly_n p);//飞船碰撞判定
	void FlyDecisionSettlement(int decision, int add_x, int add_y, fly_n& p);//飞船碰撞判定结果执行
	void GetAIMove(int& add_x, int& add_y);//AI移动
	void GetBossMove(int& add_x, int& add_y, fly_n p);//Boss移动
	void GetMove(int& add_x, int& add_y);//玩家移动

public:
	UPDATEMAP(size_t r, size_t c);
	~UPDATEMAP();
	void Start();
};

