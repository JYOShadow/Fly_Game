#include "UPDATEMAP.h"

UPDATEMAP::UPDATEMAP(size_t r, size_t c)
{
	row = r;
	col = c;

	fly_count = 0;

	b_head = new bullet_node;
	b_head->node.type = 0;
	b_head->fp = nullptr;
	b_head->next = nullptr;
	b_tail = b_head;

	f_head = new fly_node;
	f_head->node.type = 0;
	f_head->fp = nullptr;
	f_head->next = nullptr;
	f_tail = f_head;

	InitMap();
}

UPDATEMAP::~UPDATEMAP()
{
	for (size_t i = 0; i < row; i++)
	{
		delete[] map[i];
	}
	delete[]map;
	DeleteBullet();
	delete b_head;
	DeleteFly();
	delete f_head;
}

void UPDATEMAP::InitMap()
{
	map = new int* [row];
	for (size_t i = 0; i < row; i++)
	{
		map[i] = new int[col];
		for (size_t j = 0; j < col; j++)
		{
			if (i == 0 || i == row - 1) map[i][j] = 1;
			else if (j == 0 || j == row - 1) map[i][j] = 1;
			else map[i][j] = 0;
		}
	}

	fly_n player_fly = new fly_node;
	player_fly->node.x = col / 2;
	player_fly->node.y = row - 10;
	player_fly->node.type = PLAYER_FLY_TYPE;
	player_fly->node.healthy = PLAYER_FLY_HEALTHY;
	player_fly->node.bullet_type = PLAYER_FLY_BULLET;
	player_fly->node.shoot = 0;

	player_fly->next = nullptr;
	player_fly->fp = f_tail;
	f_tail->next = player_fly;
	f_tail = player_fly;

	map[player_fly->node.y][player_fly->node.x] = PLAYER_FLY_TYPE;

}

void UPDATEMAP::ShowMap()
{
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			if (map[i][j] == 0)
			{
				std::cout << "  ";
			}
			else if (map[i][j] == 1)
			{
				std::cout << "¨ˆ";
			}
			else if (map[i][j] == 2)
			{
				std::cout << "¡ï";
			}
			else if (map[i][j] == 3)
			{
				std::cout << "¡ñ";
			}
			else if (map[i][j] == 4)
			{
				std::cout << "¡ñ";
			}
			else if (map[i][j] >= 10 && map[i][j] < 20)
			{
				std::cout << "¡ó";
			}
			else if (map[i][j] >= 20 && map[i][j] <= 30)
			{
				std::cout << "£ª";
			}
		}
		std::cout << std::endl;
	}

}

void UPDATEMAP::Update()
{
	UpdateFlying();
	UpdateBullet();
	UpdateMap();
	CleanFlyNode();
	CleanBulletNode();
	//UpdateBulletNode();
}

void UPDATEMAP::UpdateFlying()
{
	int add_x = 0, add_y = 0;
	fly_n p = f_head->next;
	while (p)
	{
		add_x = 0;
		add_y = 0;
		if (p->node.type == PLAYER_FLY_TYPE)
		{
			GetMove(add_x, add_y);
		}
		else if (p->node.type == 4)
		{
			GetBossMove(add_x, add_y, p);
		}
		else
		{
			//AIMOVE
			if (AI_speed == 0)
			{
				GetAIMove(add_x, add_y);
				AI_speed = 3;
			}
			else
			{
				AI_speed--;
			}
		}
		int x = p->node.x + add_x;
		int y = p->node.y + add_y;
		int f_c = FlyCollide(x, y, p);
		FlyDecisionSettlement(f_c, x, y, p);
		if (p->node.shoot == 0)
		{
			FlyShoot(p);
		}
		else p->node.shoot--;
		p = p->next;
	}
	if (fly_count < FLY_COUNT)
	{
		ProductFly();
		fly_count++;
	}
}

void UPDATEMAP::UpdateBullet()
{
	bullet_n pr = b_head->next;
	int x = 0, y = 0;
	while (pr)
	{
		BulletMove(x, y, pr->node);
		bool b_c = BulletCollide(x, y);
		BulletDecisionSettlement(x, y, b_c, pr);
		pr = pr->next;
	}
}

void UPDATEMAP::UpdateMap()
{
	fly_n f_search = f_head->next;

	//·É´¬Óë×Óµ¯Åö×²²Ù×÷
	while (f_search)
	{
		bullet_n b_search = b_head->next;
		while (b_search)
		{
			//·É´¬Óë×Óµ¯Åö×²
			if (f_search->node.x == b_search->node.x && f_search->node.y == b_search->node.y)
			{
				f_search->node.healthy -= b_search->node.damge;
				b_search->node.exist = false;
			}
			if (f_search->node.healthy <= 0)
			{
				break;
			}
			b_search = b_search->next;
		}
		if (f_search->node.healthy <= 0)
		{
			f_search->node.exist = false;
		}
		f_search = f_search->next;
	}
	//·É´¬Åö×²²Ù×÷
	f_search = f_head->next;
	while (f_search)
	{
		if (f_search->node.exist)
		{
			fly_n f_temp_search = f_search->next;
			while (f_temp_search)
			{
				if (f_temp_search->node.exist)
				{
					if (f_search->node.x == f_temp_search->node.x && f_search->node.y == f_temp_search->node.y)
					{

						if (f_search->node.type > PLAYER_FLY_TYPE && f_temp_search->node.type > PLAYER_FLY_TYPE)
						{
							//AI Óë AIÅö×²
							f_search->node.x = f_search->node.pre_x;
							f_search->node.y = f_search->node.pre_y;
							f_temp_search->node.x = f_temp_search->node.pre_x;
							f_temp_search->node.y = f_temp_search->node.pre_y;
						}
						else
						{
							//AI Óë Íæ¼ÒÅö×²
							f_search->node.healthy -= COLLISIONDAMAGE;
							f_temp_search->node.healthy -= COLLISIONDAMAGE;
							if (f_search->node.healthy <= 0 && f_temp_search->node.healthy > 0)
							{
								f_search->node.exist = false;
								break;
							}
							else if (f_search->node.healthy > 0 && f_temp_search->node.healthy <= 0)
							{
								f_temp_search->node.exist = false;
							}
							else if (f_search->node.healthy < 0 && f_temp_search->node.healthy < 0)
							{
								f_search->node.exist = false;
								f_temp_search->node.exist = false;
								break;
							}
							else
							{
								f_search->node.x = f_search->node.pre_x;
								f_search->node.y = f_search->node.pre_y;
								f_temp_search->node.x = f_temp_search->node.pre_x;
								f_temp_search->node.y = f_temp_search->node.pre_y;
							}
						}
					}
				}
				f_temp_search = f_temp_search->next;
			}
		}
		f_search = f_search->next;
	}

}

void UPDATEMAP::CleanFlyNode()
{
	fly_n p = f_head->next;
	while (p)
	{
		if (!p->node.exist)
		{
			if (p->node.type > 2)
			{
				fly_count--;
			}
			p->fp->next = p->next;
			if (f_tail == p)
			{
				f_tail = p->fp;
			}
			else
			{
				p->next->fp = p->fp;
			}
			fly_n temp = p;
			p = p->fp;
			delete temp;
			score++;
		}
		else
		{
			map[p->node.y][p->node.x] = p->node.type;
		}
		p = p->next;
	}
}

void UPDATEMAP::CleanBulletNode()
{
	bullet_n p = b_head->next;
	while (p)
	{
		if (!p->node.exist)
		{
			p->fp->next = p->next;
			if (b_tail == p)
			{
				b_tail = p->fp;
			}
			else
			{
				p->next->fp = p->fp;
			}
			bullet_n temp = p;
			p = p->fp;
			delete temp;
		}
		else
		{
			map[p->node.y][p->node.x] = p->node.type;
		}
		p = p->next;
	}
}


void UPDATEMAP::CreateBulletType10(fly_n p)
{
	if (p->node.bullet_type == 10)
	{
		bullet new_bullet;
		new_bullet.x =p->node.x;
		new_bullet.y =p->node.y;
		new_bullet.type =p->node.bullet_type;
		new_bullet.dir_x = 0;
		new_bullet.dir_y = -1;
		new_bullet.damge = 1;
		AddBullet(new_bullet);
	}
	else if (p->node.bullet_type == 11)
	{
		for (int i = -1; i < 2; i++)
		{
			bullet new_bullet;
			new_bullet.x =p->node.x;
			new_bullet.y =p->node.y;
			new_bullet.type =p->node.bullet_type;
			new_bullet.dir_x = i;
			new_bullet.dir_y = -1;
			new_bullet.damge = 1;
			AddBullet(new_bullet);
		}
	}
	else if (p->node.bullet_type == 12)
	{
		for (int i = -2; i < 3; i++)
		{
			bullet new_bullet;
			new_bullet.x =p->node.x;
			new_bullet.y =p->node.y;
			new_bullet.type =p->node.bullet_type;

			if (i == -2 || i == 2)
			{
				new_bullet.dir_x = i;
				new_bullet.dir_y = 0;
			}
			else
			{
				new_bullet.dir_x = i;
				new_bullet.dir_y = -2;
			}

			new_bullet.damge = 1;
			AddBullet(new_bullet);
		}
	}
	else if (p->node.bullet_type == 13)
	{
		for (int i = -2; i < 4; i++)
		{
			bullet new_bullet;
			new_bullet.x =p->node.x;
			new_bullet.y =p->node.y;
			new_bullet.type =p->node.bullet_type;

			if (i == -2 || i == 2)
			{
				new_bullet.dir_x = i;
				new_bullet.dir_y = 0;
			}
			else if (i == 3)
			{
				new_bullet.dir_x = 0;
				new_bullet.dir_y = 1;
			}
			else
			{
				new_bullet.dir_x = i;
				new_bullet.dir_y = -2;
			}

			new_bullet.damge = 1;
			AddBullet(new_bullet);
		}
	}
	else if (p->node.bullet_type == 14)
	{
		for (int i = -3; i < 5; i++)
		{
			bullet new_bullet;
			new_bullet.x =p->node.x;
			new_bullet.y =p->node.y;
			new_bullet.type =p->node.bullet_type;

			if (i == -3 || i == 3)
			{
				new_bullet.dir_x = i / 3 *2;
				new_bullet.dir_y = 0;
			}
			else if (i == 0 || i == 4)
			{
				new_bullet.dir_x = 0;
				new_bullet.dir_y = (i - 2);
			}
			else if (i == 1 || i == -1)
			{
				new_bullet.dir_x = i*2;
				new_bullet.dir_y = i*2;
			}
			else if (i == 2 || i == -2)
			{
				new_bullet.dir_x = -i ;
				new_bullet.dir_y = i ;
			}
			new_bullet.damge = 1;
			AddBullet(new_bullet);
		}
	}
	else if (p->node.bullet_type == 15)
	{
		for (int i = -1; i < 2; i += 2)
		{
			bullet new_bullet;
			new_bullet.x = p->node.x + i;
			new_bullet.y = p->node.y;
			new_bullet.type = p->node.bullet_type;
			new_bullet.dir_x = 0;
			new_bullet.dir_y = -1;
			new_bullet.damge = 1;
			AddBullet(new_bullet);
		}
	}
	else if (p->node.bullet_type == 16)
	{
		for (int i = -1; i < 2; i += 1)
		{
			bullet new_bullet;
			new_bullet.x = p->node.x + i;
			new_bullet.y = p->node.y + abs(i);
			new_bullet.type = p->node.bullet_type;
			new_bullet.dir_x = 0;
			new_bullet.dir_y = -1;
			new_bullet.damge = 1;
			AddBullet(new_bullet);
		}
	}
	else if (p->node.bullet_type == 17)
	{
		for (int i = -2; i < 3; i += 1)
		{
			bullet new_bullet;
			new_bullet.x = p->node.x + i;
			new_bullet.y = p->node.y + abs(i);
			new_bullet.type = p->node.bullet_type;
			new_bullet.dir_x = 0;
			new_bullet.dir_y = -3;
			new_bullet.damge = 1;
			AddBullet(new_bullet);
		}
	}
	else if (p->node.bullet_type == 18)
	{
		for (int i = -3; i < 4; i++)
		{
			bullet new_bullet;
			if (i == 0)
			{
				new_bullet.x = p->node.x;
				new_bullet.y = p->node.y - 2;
			}
			else if (i == 1||i==-1 || i == 3||i==-3)
			{
				new_bullet.x = p->node.x + i / abs(i);
				new_bullet.y = p->node.y - abs(i);
			}
			else if (i ==2||i==-2)
			{
				new_bullet.x = p->node.x + 2 * i;
				new_bullet.y = p->node.y - abs(i);
			}
			new_bullet.type = p->node.bullet_type;
			new_bullet.dir_x = 0;
			new_bullet.dir_y = -2;
			new_bullet.damge = 1;
			AddBullet(new_bullet);
		}
	}

	

 

}

void UPDATEMAP::CreateBulletType20(fly_n p)
{
	if (p->node.bullet_type == 20)
	{
		bullet new_bullet;
		new_bullet.x = p->node.x;
		new_bullet.y = p->node.y;
		new_bullet.type = p->node.bullet_type;
		new_bullet.dir_x = 0;
		new_bullet.dir_y = 1;
		new_bullet.damge = 1;
		AddBullet(new_bullet);
	}
	else if ((p->node.bullet_type == 21))
	{
		for (int i = 0; i < 3; i++)
		{
			bullet new_bullet;
			if (p->node.x < col / 2)
			{
				new_bullet.x = p->node.x + i;
				if (p->node.y + i < row - 1)
				{
					new_bullet.y = p->node.y + i;
				}
				else
				{
					new_bullet.y = p->node.y;
				}
			}
			else 
			{
				new_bullet.x = p->node.x - i;
				if (p->node.y + i < row - 1)
				{
					new_bullet.y = p->node.y + i;
				}
				else
				{
					new_bullet.y = p->node.y;
				}
			}
			new_bullet.type = p->node.bullet_type;
			if (p->node.x < col / 2)
			{
				new_bullet.dir_x = 1;
				new_bullet.dir_y = 1;
			}
			else 
			{
				new_bullet.dir_x = -1;
				new_bullet.dir_y = 1;
			}
			
			new_bullet.damge = 1;
			AddBullet(new_bullet);
		}
	}

}

void UPDATEMAP::AddBullet(bullet bn)
{
	bullet_n n = new bullet_node;
	n->node = bn;
	b_tail->next = n;
	n->fp = b_tail;
	n->fp = b_tail;
	n->next = nullptr;
	b_tail = n;
}

void UPDATEMAP::DeleteBullet()
{
	bullet_n temp = nullptr;
	while (b_tail == b_head)
	{
		temp = b_tail->fp;
		delete b_tail;
		b_tail = temp;
	}
}

bullet_n UPDATEMAP::SearchBullet(int x, int y)
{
	bullet_n p = b_head->next;
	while (p)
	{
		if (p->node.x == x && p->node.y == y)
		{
			break;
		}
		p = p->next;
	}
	return p;
}

void UPDATEMAP::ScatterBullet(int& x, int& y, bullet b)
{
	x = b.x + b.dir_x;
	y = b.y + b.dir_y;
}

void UPDATEMAP::BulletMove(int& x, int& y, bullet b)
{

	if (b.type >= 10 && b.type < 30)
	{
		ScatterBullet(x, y, b);
	}
}

bool UPDATEMAP::BulletCollide(int x, int y)//Åö×²ÅÐ¶¨
{
	if (x<=0||x>=col-1||y<=0||y>=row-1)
	{
		return 1;
	}
	else if (map[y][x] == 0||(map[y][x]>=10 && map[y][x] <= 30))
	{
		return 0;
	}
}

void UPDATEMAP::BulletDecisionSettlement(int x, int y, bool b_c, bullet_n& pr)//pr->node.
{
	if (b_c == 0)
	{
		if (map[pr->node.y][pr->node.x] >= 10)
		{
			map[pr->node.y][pr->node.x] = 0;
		}
		pr->node.x = x;
		pr->node.y = y;
	}
	else if (b_c == 1)
	{
		if (map[pr->node.y][pr->node.x] >= 10)
		{
			map[pr->node.y][pr->node.x] = 0;
		}
		pr->node.exist = false;
	}

}

void UPDATEMAP::SiteClearing()
{
	bullet_n pr = nullptr;
	while (b_tail == b_head)
	{
		pr = b_tail;
		map[pr->node.y][pr->node.x] = 0;
		b_tail = pr->fp;
		b_tail->next = nullptr;
		delete pr;
	}
	clearn_number_times--;
}

void UPDATEMAP::ChangingBullet(bullet& b)
{
	if (b.type >= 10 && b.type < 20)
	{
		b.type += 1;
	}
	else if (b.type >= 20 && b.type < 30)
	{
		b.type += 1;
	}

}



void UPDATEMAP::AddFly(flying fb)
{
	fly_n new_fly = new fly_node;
	new_fly->node = fb;
	new_fly->next = nullptr;
	new_fly->fp = f_tail;
	f_tail->next = new_fly;
	f_tail = new_fly;
}

void UPDATEMAP::DeleteFly()
{
	fly_n temp = nullptr;
	while (f_tail == f_head)
	{
		temp = f_tail->fp;
		delete f_tail;
		f_tail = temp;
	}
}

fly_n UPDATEMAP::SearchFly(int x, int y)
{
	fly_n p = f_head->next;
	while (p)
	{
		if (p->node.x == x && p->node.y == y)
			break;
		p = p->next;
	}
	return p;
}

int UPDATEMAP::LenFly()
{
	int len = 0;
	fly_n fn = f_head->next;
	while (fn)
	{
		len++;
		fn = fn->next;
	}
	return len;
}

void UPDATEMAP::FlyShoot(fly_n& p)
{
	if (p->node.bullet_type >= 10 && p->node.bullet_type < 20)
	{
		CreateBulletType10(p);
	}
	else if (p->node.bullet_type >= 20 && p->node.bullet_type < 30)
	{
		CreateBulletType20(p);
	}
	switch (p->node.type)
	{
	case 2:
		p->node.shoot = PLAYER_FLY_SHOOT;
		break;
	case 3:
		p->node.shoot = AI_FLY1_SHOOT;
		break;
	default:
		break;
	}
}

void UPDATEMAP::ProductFly()
{
	int y = 1;
	int x = rand() % col - 1;
	while (true)
	{
		if (x == 0)
		{
			x = rand() % col;
		}
		else if (map[y][x] == 2 || map[y][x] == 3)
		{
			x = rand() % col;
		}
		else break;
	}
	flying new_fly;
	new_fly.x = x;
	new_fly.y = y;
	new_fly.pre_x = x;
	new_fly.pre_y = y;
	new_fly.type = AI_FLY1_TYPE;
	new_fly.shoot = AI_FLY1_SHOOT;
	new_fly.healthy = AI_FLY1_HEALTHY;
	new_fly.bullet_type = rand() % 2 + 20;
	AddFly(new_fly);
}

int  UPDATEMAP::FlyCollide(int x, int y, fly_n p)
{
	if (y >= row - 1 || y <= 0 || x <= 0 || x >= col - 1)
	{
		if (p->node.type != 2)
		{
			if (y == row - 1)
			{
				return 1;//AI ·É³öµ×Ïß
			}
		}
		return 0;//²»½øÐÐÒÆ¶¯
	}
	else return 2;

}

void UPDATEMAP::FlyDecisionSettlement(int decision, int x, int y, fly_n& p)
{
	switch (decision)
	{
	case 0://×²Ç½
		break;
	case 1:
	{
		//AI ·É³öµ×Ïß
		map[p->node.y][p->node.x] = 0;
		p->node.exist = false;
		break;
	}
	case 2:
		//Õý³£ÒÆ¶¯
		map[p->node.y][p->node.x] = 0;
		p->node.pre_x = p->node.x;
		p->node.pre_y = p->node.y;
		p->node.x = x;
		p->node.y = y;
		break;
	default:
		break;
	}
}

void UPDATEMAP::GetAIMove(int& add_x, int& add_y)
{
	add_x = rand() % 3 - 1;
	add_y = rand() % 2;
}

void UPDATEMAP::GetBossMove(int& add_x, int& add_y, fly_n p)
{
	add_y = 0;
	if (p->node.x == 1)
		boss_x = 1;
	else if (p->node.x == 28) boss_x = -1;
	add_x = boss_x;
}

void  UPDATEMAP::GetMove(int& add_x, int& add_y)
{
	int x = 0, y = 0;
	if (GetAsyncKeyState(VK_LEFT) || GetAsyncKeyState('A'))
	{
		x = -1;
		y = 0;
	}
	else if (GetAsyncKeyState(VK_RIGHT) || GetAsyncKeyState('D'))
	{
		x = 1;
		y = 0;
	}
	else if (GetAsyncKeyState(VK_UP) || GetAsyncKeyState('W'))
	{
		x = 0;
		y = -1;
	}
	else if (GetAsyncKeyState(VK_DOWN) || GetAsyncKeyState('S'))
	{
		x = 0;
		y = 1;
	}
	add_x = x;
	add_y = y;
}



void UPDATEMAP::Start()
{
	srand(time(0));
	while (1)
	{
		ShowMap();
		std::cout << "healthy : " << PLAYER_FLY.healthy << std::endl;
		std::cout << "score : " << score << std::endl;
		if (PLAYER_FLY.type != 2)
		{
			std::cout << "you dead." << std::endl;
			break;
		}
		Update();
		Sleep(300);
		system("cls");

	}
}