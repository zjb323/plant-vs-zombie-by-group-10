#define _CRT_SECURE_NO_WARNINGS 1
#include<stdio.h>
#include<graphics.h>
#include"tools.h"

#define WIN_WIDTH  900
#define WIN_HEIGHT 600//宏定义便于接下来维护代码

//enum { WAN_DOU, XISNG_RI_KUI, ZHI_WU_COUNT };
enum{ XISNG_RI_KUI, WAN_DOU,ZHI_WU_COUNT};

IMAGE imgBg;//表示背景图片
IMAGE imgBar;//表示植物卡片栏
IMAGE imgCards[ZHI_WU_COUNT];
IMAGE* imgZhiWu[ZHI_WU_COUNT][20];

int curX, curY;//当前选中的植物在移动过程中的位置
int curZhiWu;//0表示没有选中，1表示选中了第一种

struct zhiwu
{
	int type;// 0表示没有植物
	int frameIndex;//序列帧的序号
};

struct zhiwu map[3][9];
struct sunshineBall
{
	int x, y;
	int frameIndex;//之后为了防止重名可能要进行更改
};

bool fileExist(const char* name)
{
	FILE* fp = fopen(name, "r");
	if (fp == NULL)
	{
		return false;
	}
	else
	{
		fclose(fp);
		return true;
	}
}
void gameInit()//游戏初始化，创建游戏窗口
{
	curZhiWu = 0;
	loadimage(&imgBg, "res/bg.jpg");
	initgraph(WIN_WIDTH, WIN_HEIGHT, 1);//这个1貌似用处不大？
	loadimage(&imgBar, "res/bar5.png");
	memset(imgZhiWu, 0, sizeof(imgZhiWu));
	memset(map, 0, sizeof(map));

	char name[64];
	for (int i = 0;i < ZHI_WU_COUNT;i++)
	{
		sprintf_s(name, sizeof(name), "res/Cards/card_%d.png", i + 1);
		loadimage(&imgCards[i], name);
		for (int j = 0;j < 20;j++)
		{
			sprintf_s(name, sizeof(name), "res/zhiwu/%d/%d.png", i, j + 1);
			if (fileExist(name))//检查地址“name”所指向的图片是否存在
			{
				imgZhiWu[i][j] = new IMAGE;//?
				loadimage(imgZhiWu[i][j], name);

			}
			else break;
		}
	}

}


void updateWindow()
{
	BeginBatchDraw();//将绘图操作结果暂时保存到内存中
	putimage(0, 0, &imgBg);
	putimagePNG(250, 0, &imgBar);
	for (int j = 0;j < ZHI_WU_COUNT;j++)
	{
		int x = 338 + j * 65;
		int y = 6;
		putimage(x, y, &imgCards[j]);
	}//将植物卡牌放进植物栏里


	

	for (int i = 0;i < 3;i++)
	{
		for (int j = 0;j < 9;j++)
		{
			if (map[i][j].type > 0)
			{
				int x = 256 + j * 81;
				int y = 179 + i * 102;
				int zhiWuType = map[i][j].type-1;
				int index = map[i][j].frameIndex;
				putimagePNG(x, y, imgZhiWu[zhiWuType][index]);
			}
		}
	}
	if (curZhiWu)//不为零
	{
		IMAGE* img = imgZhiWu[curZhiWu - 1][0];
		putimagePNG(curX - img->getwidth() / 2, curY - img->getheight() / 2, img);
	}//让选中的植物跟随鼠标移动（鼠标指向植物中心）
	EndBatchDraw();//将绘图结果输出
}


void userClick()
{
	ExMessage msg;
	static int status = 0;
	if (peekmessage(&msg))
	{
		if (msg.message == WM_LBUTTONDOWN)
		{
			if (msg.x > 338 && msg.x < 338 + 65 * ZHI_WU_COUNT && msg.y < 96)
			{
				int index = (msg.x - 338) / 65;
				status = 1;
				curZhiWu = index + 1;
				curX = -100;//!!
				curY = -100;//!!
			}
		}
		else if (msg.message == WM_MOUSEMOVE && status == 1)
		{
			curX = msg.x;
			curY = msg.y;
		}
		else if (msg.message == WM_LBUTTONUP)
		{
			if (msg.x > 256 && msg.y < 489)
			{
				int row = (msg.y - 179) / 102;
				int col = (msg.x - 256) / 81;
				if (map[row][col].type == 0)
				{
					map[row][col].type = curZhiWu;
					map[row][col].frameIndex = 0;
				}
			}
			    curZhiWu = 0;
				status = 0;
		}
	}
}

void updateGame()
{
	for (int i = 0;i < 3;i++)
	{
		for (int j = 0;j < 9;j++)
		{
			if (map[i][j].type > 0)
			{
				map[i][j].frameIndex++;
				int zhiWuType = map[i][j].type - 1;
				int index = map[i][j].frameIndex;
				if (imgZhiWu[zhiWuType][index] == NULL)
				{
					map[i][j].frameIndex = 0;
				}
			}
		}
	}
}

void startUI()
{
	IMAGE imgBg, imgMenu1, imgMenu2;//尽管之前定义了一个同名的全局变量，
	                                //但在该函数中是该变量起作用
	loadimage(&imgBg, "res/menu.png");
	loadimage(&imgMenu1, "res/menu1.png");
	loadimage(&imgMenu2, "res/menu2.png");
	int flag = 0;

	while (1)
	{
		
		BeginBatchDraw();
		putimage(0, 0, &imgBg);
		putimagePNG(475, 75, flag ? &imgMenu2 : &imgMenu1);
		ExMessage msg;
		if (peekmessage(&msg))
		{
			if (msg.message == WM_LBUTTONDOWN &&
				msg.x > 474 && msg.x < 474 + 300 &&
				msg.y>75 && msg.y < 75 + 140)
			{
				flag = 1;         
				//EndBatchDraw();
			}
			else if (msg.message == WM_LBUTTONUP &&flag==1)
			{
				return;
			}
		}

		EndBatchDraw();
		
	}
	
}

int main(void)
{
	int timer = 0;
	bool flag = true;
	gameInit();
	startUI();
	while (1)//死循环
	{   
		userClick();//??
		timer += getDelay();
		if (timer > 70)
		{
			flag = true;
			timer = 0;
		}
		if (flag)
		{
			flag = false;
			updateWindow();
			updateGame();
		}
	
	}
	system("pause");

}