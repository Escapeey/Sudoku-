#include <graphics.h>
#include <conio.h>
#include <stdio.h>
#include <easyx.h>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <ctime>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include<algorithm>
#pragma comment( lib, "MSIMG32.LIB")

#define MAP_COL 9
#define MAP_ROW 9
int w = 900; //界面宽
int h = 750; //界面高

static int SelNumber; //当前选择填入的数字
clock_t t1, t2;
static int sleepTime = 0, second = 0, minute = 0, restart = 0, endGame = 0, showanswer = 0;
std::string PlayerId = "Unknown"; //玩家名称
std::string PlayerScore; //玩家得分

/*===============================================================================*/
static class CheckMouse
{
public:
	static ExMessage msg;
	static COORD currentPos; //当前位置
	static bool key; //检测左键点击的锁
public:
	/// <summary>
	/// 检测鼠标状态（左键点击和位置）
	/// </summary>
	static void checkmouse()
	{
		if (peekmessage(&msg, EM_MOUSE))
		{
			switch (msg.message)
			{
			case WM_MOUSEMOVE:
				currentPos.X = msg.x;
				currentPos.Y = msg.y;
				break;
			case WM_LBUTTONUP:
				key = !key;
				break;
			default:
				break;
			}
		}
	}
};
ExMessage CheckMouse::msg;
COORD CheckMouse::currentPos = { 0,0 };
bool CheckMouse::key = 0;
/*===============================================================================*/


/*===============================================================================*/
int Data[5][5][9][9] = { 0 }; //装数独库 级别数 * 每个级别的个数 * 行数 * 列数
int MapMain[9][9] = {{6,3,5,9,4,7,2,8,1},
					{7,4,8,1,5,2,9,3,6},
					{2,1,9,8,6,3,5,4,7},
					{4,7,6,3,9,1,8,5,2},
					{8,9,3,5,2,6,7,1,4},
					{1,5,2,4,7,8,6,9,3},
					{3,6,1,7,8,5,4,2,9},
					{9,8,7,2,3,4,1,6,5},
					{5,2,4,6,1,9,3,7,0}}; //初始矩阵
std::string nowlevel = "Easy  "; //默认难度
std::string nowProblem_id = "101"; //默认关卡

//数独题库
class SudokuData 
{
public:
	std::ifstream f;
	std::string s;
public:
	/// <summary>
	/// 读入"sudoku.txt"中数独题库
	/// </summary>
	/// <param name="i"></param>
	SudokuData(int i);

	int str2int(std::string str);
};
SudokuData::SudokuData(int i)
{
	f.open("sudoku.txt");
	if (!f)
	{
		std::cout << "file error!" << std::endl;
		exit(0);
	}
	else
	{
		int i = 0; //当前读的难度序号
		int j = 0; //难度内棋盘计数器
		int m = 0; //棋盘行数计数器
		while (1)
		{
			//std::cout << i << " " << j << " " << m <<std::endl;
			getline(f, s);
			if (!f.eof())
			{
				if (s.length() == 3) //序号行
				{
					int temp = str2int(s);
					i = temp / 100;
					j = temp % 10;
					//std::cout <<temp<<std::endl;
				}
				else if (s.length() == 0) //空行
				{
					m = 0;
				}
				else //数据行 例：0 0 0 0 0 0 0 0 0 
				{
					for (int col = 0; col < 9; col++)
					{
						Data[i][j][m][col] = str2int(s.substr(col * 2, 1));
					}
					m = (m + 1) % 9;
				}
			}
			else
				break;
		}
	}
	f.close();
}
int SudokuData::str2int(std::string str)
{
	int temp;
	std::stringstream os;
	os << str;
	os >> temp;
	return temp;
}
/*===============================================================================*/


/*===============================================================================*/
/// <summary>
/// 设置当前字体
/// </summary>
/// <param name="type">字体 1=>楷体, 2=>宋体</param>
/// <param name="fontHeight">设置字体高度</param>
static void SetFont(int type, int fontHeight)
{
	setbkmode(TRANSPARENT);
	LOGFONT f;
	gettextstyle(&f);                     // 获取当前字体设置
	f.lfHeight = fontHeight;              // 设置字体高度
	switch (type)
	{
	case 1:
		wcscpy_s(f.lfFaceName, _T("楷体"));
		break;
	case 2:
		wcscpy_s(f.lfFaceName, _T("宋体"));
		break;
	}
	f.lfQuality = ANTIALIASED_QUALITY;    // 设置输出效果为抗锯齿  
	settextstyle(&f);
}
/*===============================================================================*/


/*===============================================================================*/
//绘图类
class DrawPic
{
public:
	IMAGE imgs[20];
	
public:
	/// <summary>
	/// 读入Icon文件夹中图片
	/// </summary>
	DrawPic();
	/// <summary>
	/// Easyx实现png透明贴图
	/// </summary>
	/// <param name="x">图片x坐标</param>
	/// <param name="y">图片y坐标</param>
	/// <param name="srcimg">原始png图片</param>
	void transparentimage(IMAGE* dstimg, int x, int y, IMAGE* srcimg); 
	/// <summary>
	/// 初始页面绘制图片
	/// </summary>
	void DrawingInit();
	/// <summary>
	/// 开始页面绘制图片
	/// </summary>
	void DrawingBeginS();
	/// <summary>
	/// 结束页面绘制图片
	/// </summary>
	void DrawingEnds();
};

DrawPic::DrawPic()
{
	//loadimage(&imgs[0], _T("C:\\Users\\Discodzy\\Desktop\\课程设计\\Icon\\read2.png"), 104, 144);
	wchar_t filename[100];
	for (int i = 0; i < 3; i++)
	{
		swprintf_s(filename, L"Icon//img%d.png", i);
		loadimage(&imgs[i], filename);
	}
	for (int i = 3; i < 6; i++)
	{
		swprintf_s(filename, L"Icon//img%d.png", i);
		loadimage(&imgs[i], filename, 300, 600);
	}
	loadimage(&imgs[10],_T("Icon//read.png"), 300, 420);
	loadimage(&imgs[9], _T("Icon//bk.png"), 900, 750); //输入用户名背景板
}
void DrawPic::transparentimage(IMAGE* dstimg, int x, int y, IMAGE* srcimg) //透明背景图片接口
{
	HDC dstDC = GetImageHDC(dstimg);
	HDC srcDC = GetImageHDC(srcimg);
	int w = srcimg->getwidth();
	int h = srcimg->getheight();
	BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
	AlphaBlend(dstDC, x, y, w, h, srcDC, 0, 0, w, h, bf);
}
void DrawPic::DrawingInit()
{
	transparentimage(NULL, 0, 0, &imgs[9]);//透明png
}
void DrawPic::DrawingBeginS()
{
	transparentimage(NULL, 590, 130, &imgs[0]);//透明png
	transparentimage(NULL, 570, 320, &imgs[1]);//透明png
	transparentimage(NULL, 570, 410, &imgs[2]);//透明png
	transparentimage(NULL, 10, 340, &imgs[10]);//透明png
}
void DrawPic::DrawingEnds()
{
	//transparentimage(NULL, 610, 50, &imgs[3]);//透明png
	transparentimage(NULL, 610, 100, &imgs[4]);//透明png
	//transparentimage(NULL, 610, 50, &imgs[5]);//透明png
}
/*===============================================================================*/


/*===============================================================================*/
/// <summary>
/// 场景类型枚举 供后续切换场景使用
/// </summary>
enum E_SceneType
{
	BeginS,
	OptionS,
	GameS,
	EndS,
};
/*===============================================================================*/


/*===============================================================================*/
//场景切换类
class ISceneUpdata
{
public:
	/// <summary>
	/// 提供给派生类重写的方法 用于页面的刷新
	/// </summary>
	virtual void Update(){}
};
/*===============================================================================*/
//游戏类
class Game
{
private:
	static ISceneUpdata* nowScene; //当前场景

public:
	/// <summary>
	/// 初始换游戏场景 设置页面大小 以及 初始场景
	/// </summary>
	Game();
	/// <summary>
	/// 开始游戏 控制游戏主循环 负责游戏场景逻辑更新
	/// </summary>
	void Start();
	/// <summary>
	/// 切换场景 
	/// </summary>
	/// <param name="type">要切换到的场景</param>
	static void ChangeScene(E_SceneType type);
};
/*===============================================================================*/


/*===============================================================================*/
//按钮类
class Button
{
public:
	Button() {}
	Button(int x, int y, int w, int h, const TCHAR* text, COLORREF color, bool key);
};

Button::Button(int x, int y, int w, int h, const TCHAR* text, COLORREF color, bool key)
{
	setbkmode(TRANSPARENT);
	setfillcolor(color);
	settextcolor(BLACK);
	if (key)
		setlinecolor(BLACK); // 显示边框
	else
		setlinecolor(WHITE); // 不显示边框
	fillrectangle(x, y, x + w, y + h);

	//设置字体
	LOGFONT f;
	gettextstyle(&f);                     // 获取当前字体设置
	f.lfHeight = h;             // 设置字体高度为 h
	wcscpy_s(f.lfFaceName, _T("楷体"));    // 设置字体为“黑体”
	f.lfQuality = ANTIALIASED_QUALITY;    // 设置输出效果为抗锯齿  
	settextstyle(&f);                     // 设置字体样式
	outtextxy(x, y, text);
}
/*===============================================================================*/

/// <summary>
/// 关卡选择按钮
/// </summary>
void OptionButton(int x1, int x2, int y1, int y2, const TCHAR* text, bool showLine, bool showShadow, bool retraction)
{
	if (CheckMouse::currentPos.X > x1 && CheckMouse::currentPos.X < x2
		&& CheckMouse::currentPos.Y > y1 && CheckMouse::currentPos.Y < y2 && showShadow)
	{
		setfillcolor(RGB(158, 160, 169));
		solidrectangle(x1, y1, x2, y2);
		settextcolor(WHITE);
	}
	else
	{
		setfillcolor(WHITE);
		if(showLine)
			setlinecolor(BLACK); // 显示边框
		fillrectangle(x1, y1, x2, y2);
		settextcolor(BLACK);
	}
	if(retraction) //是否缩进10像素
		outtextxy(x1 + 10, y1 + 10, text);
	else
		outtextxy(x1, y1 + 5, text);
}

/*===============================================================================*/
//初始场景类
class BeginScene : public ISceneUpdata
{
private:
	Button title, begin;
	ExMessage msg;
	bool temp;
public:
	/// <summary>
	/// 初始化初始场景 并弹出用户名输入框
	/// </summary>
	BeginScene();
	/// <summary>
	/// 场景刷新
	/// </summary>
	void Update();
	/// <summary>
	/// 绘图对象
	/// </summary>
	DrawPic pics;
};
BeginScene::BeginScene()
{
	temp = CheckMouse::key;

	BeginBatchDraw();
	cleardevice();
	pics.DrawingInit();
	EndBatchDraw();
	Sleep(500);
	TCHAR s[100];
	InputBox(s, 10, L"请输入用户名", L"用户名不得超过八位");
	std::wstring wStr = s;
	PlayerId = std::string(wStr.begin(), wStr.end());
	if (PlayerId == "")
	{
		PlayerId = "Unknown";
	}
}
void BeginScene::Update()
{
	BeginBatchDraw();
	cleardevice();
	//CheckMouse::checkmouse();
	//设置字体
	SetFont(1, 50);
	//绘制选项
	setbkcolor(WHITE);
	//选项栏
	OptionButton(w / 2 - 110, w / 2 + 110, h / 2 - 50, h / 2 + 20, _T("开始游戏"), 1, 1, 1);
	OptionButton(w / 2 - 110, w / 2 + 110, h / 2 + 40, h / 2 + 110, _T("选择关卡"), 1, 1, 1);
	title = Button(w / 2 - 150, h / 7 + 10, 200, 100, L"数  独", WHITE, 0);
	
	//选项操作
	if (CheckMouse::key != temp)
	{
		if (CheckMouse::currentPos.X > w / 2 - 110 && CheckMouse::currentPos.X < w / 2 + 110
			&& CheckMouse::currentPos.Y > h / 2 - 50 && CheckMouse::currentPos.Y < h / 2 + 20)
		{
			temp = CheckMouse::key;
			Game::ChangeScene(GameS);
			return;
		}

		if (CheckMouse::currentPos.X > w / 2 - 110 && CheckMouse::currentPos.X < w / 2 + 110
			&& CheckMouse::currentPos.Y > h / 2 + 40 && CheckMouse::currentPos.Y < h / 2 + 110)
		{
			temp = CheckMouse::key;
			Game::ChangeScene(OptionS);
			return;
		}

		temp = CheckMouse::key;
	}

	//绘图
	pics.DrawingBeginS();
	EndBatchDraw();
}
/*===============================================================================*/
//关卡选择场景类
class OptionScene : public ISceneUpdata
{
private:
	bool temp;
	Button title, options[5];
public:
	OptionScene();
	void Update();
	std::string float2str(float f);
};
OptionScene::OptionScene()
{
	temp = CheckMouse::key;
}

void OptionScene::Update()
{
	BeginBatchDraw();
	cleardevice();
	//CheckMouse::checkmouse();
	//设置字体
	SetFont(1, 50);
	//绘制选项
	setbkcolor(WHITE);
	//选项栏 关卡
	for (int i = 1; i <= 5; i++)
	{
		for (int j = 1; j <= 5; j++)
		{
			TCHAR strNum[7];
			_itow_s(i * 100 + j, strNum, 10); //int 转TCHAR
			OptionButton((j + 2) * 100, (j + 2) * 100 + 100, (i + 1) * 100, (i + 1) * 100 + 75, strNum, 1, 1, 1);
		}
	}
	//设置难度名称	
	options[0] = Button(50, 200, 230, 75, _T("简单"), WHITE, 0);
	options[1] = Button(50, 300, 230, 75, _T("中等"), WHITE, 0);
	options[2] = Button(50, 400, 230, 75, _T("困难"), WHITE, 0);
	options[3] = Button(50, 500, 230, 75, _T("专家"), WHITE, 0);
	options[4] = Button(50, 600, 230, 75, _T("噩梦"), WHITE, 0);

	title = Button(w / 2 - 150, h / 10, 200, 100, L"关  卡", WHITE, 0);

	//选项操作
	if (CheckMouse::key != temp)
	{
		for (int i = 1; i <= 5; i++)
		{
			for (int j = 1; j <= 5; j++)
			{
				if (CheckMouse::currentPos.X > (j + 2) * 100 && CheckMouse::currentPos.X < (j + 2) * 100 + 100
					&& CheckMouse::currentPos.Y >(i + 1) * 100 && CheckMouse::currentPos.Y < (i + 1) * 100 + 75)
				{
					for (int n = 0; n < 9; n++)
						for (int m = 0; m < 9; m++) //设置初始矩阵
							MapMain[n][m] = Data[i - 1][j - 1][n][m];
					//存储当前关卡信息
					nowProblem_id = float2str(i * 100 + j);
					switch (i)
					{
					case 1:
						nowlevel = "Easy  ";
						break;
					case 2:
						nowlevel = "Medium";
						break;
					case 3:
						nowlevel = "Hard  ";
						break;
					case 4:
						nowlevel = "Expert";
						break;
					case 5:
						nowlevel = "Evil  ";
						break;
					}

					temp = CheckMouse::key;
					Game::ChangeScene(GameS);
					return;
				}
			}
		}
		temp = CheckMouse::key;
	}

	EndBatchDraw();
}
std::string OptionScene::float2str(float f)
{
	std::string temp;
	std::stringstream os;
	os << f;
	os >> temp;
	return temp;
}

/*===============================================================================*/
/// <summary>
/// 当前游戏记录
/// </summary>
typedef struct
{
	std::string id;
	std::string time;
	std::string level;
	std::string problem_id;
	std::string score;
}ScoreRecords;

//结束场景类
class EndScene : public ISceneUpdata
{
private:
	bool temp;
	std::ifstream ifs;
	std::ofstream ofs;
	std::string s;
	std::string::size_type pos, n, n1, n2, n3, n4;
	std::vector<ScoreRecords> InfoVec; //装文件中已经记录的信息
	ScoreRecords nowInfo; //当前局记录信息

	Button title, Info[25];
	std::string str_level_List[6] = {"Level", "Easy  ", "Medium", "Hard  ", "Expert", "Evil  "};
	//TCHAR* level_List[6]; //难度列表
	//TCHAR* problemId_List[5][5]; //关卡列表

	int levelIndex = 0; 
	int idIndex = 0;
	int playerIndex = 0;

	DrawPic pics;
public:
	static std::map<std::string, int> levelMap; //例： "Easy"：1
	static int sortType; //排序方式
public:
	EndScene();
	static bool cmp(ScoreRecords s1, ScoreRecords s2);
	void SetInfo();
	std::string GetNowTime();
	/// <summary>
	/// 从ScoreTable.txt中读入历史信息
	/// </summary>
	void ReadScore();
	/// <summary>
	/// 向ScoreTable.txt添加当前局信息
	/// </summary>
	void AddScore();
	/// <summary>
	/// 绘制单条信息
	/// </summary>
	/// <param name="i">在所有数据中是第几条数据</param>
	/// <param name="cnt">当前页面中显示的序号</param>
	void DrawInfo(int i, int cnt);
	/// <summary>
	/// 绘制结束界面所有信息 切换排序的按钮 排序的方法 高亮当前局信息等
	/// </summary>
	void ShowScore();
	int str2int(std::string str);
	void Update();
}; 
std::map<std::string, int> EndScene::levelMap = { {"Easy  ",1}, {"Medium",2}, {"Hard  ",3}, {"Expert",4}, {"Evil  ",5} };
int EndScene::sortType = 1;

EndScene::EndScene()
{
	temp = CheckMouse::key;
	/*for (int i = 1; i <= 5; i++)
	{
		for (int j = 1; j <= 5; j++)
		{
			TCHAR strNum[7];
			_itow_s(i * 100 + j, strNum, 10); //int 转TCHAR
			problemId_List[i-1][j-1] = strNum;
		}
	}

	for (int i = 0; i < 6; i++)
	{
		std::wstring wstr(str_level_List[i].begin(), str_level_List[i].end());
		TCHAR * temp = (_TCHAR*)(&wstr[0]);
		level_List[i] = temp;
	}*/
	SetInfo();
	AddScore();
	ReadScore();
	std::sort(InfoVec.begin(), InfoVec.end(), cmp);
}
bool EndScene::cmp(ScoreRecords s1, ScoreRecords s2)
{
	switch (sortType)
	{
	case 1: //先按level排序，level相同时按关卡号排序
		if (levelMap[s1.level] < levelMap[s2.level])
			return 1;
		else if (levelMap[s1.level] == levelMap[s2.level])
		{
			if (s1.problem_id < s2.problem_id)
				return 1;
			else if (s1.problem_id == s2.problem_id)
				return s1.score < s2.score;
		}
		return 0;
	case 2: //按分数(通关用时)排序
		return s1.score < s2.score;
	case 3: //按id排序
		return s1.id < s2.id;
	case 4: //按通过时当地时间排序
		return s1.time > s2.time;
	default:
		return 1;
	}
}
void EndScene::SetInfo()
{
	nowInfo.level = nowlevel;

	nowInfo.problem_id = nowProblem_id;

	nowInfo.id = PlayerId;

	nowInfo.time = GetNowTime();

	nowInfo.score = PlayerScore;
}
std::string EndScene::GetNowTime()
{
	char str[80];
	time_t nowtime;
	struct tm p;
	time(&nowtime);
	localtime_s(&p, &nowtime);
	sprintf_s(str, "%02d-%02d-%02d %02d:%02d", p.tm_year + 1900, p.tm_mon + 1, p.tm_mday, p.tm_hour, p.tm_min);
	return str;
}
void EndScene::ReadScore() //读入分数
{

	ifs.open("ScoreTable.txt");
	if (!ifs)
	{
		std::cout << "file error!" << std::endl;
		exit(0);
	}
	else
	{
		while (1)
		{
			ScoreRecords tempInfo; //文件储存的每一行数据
			pos = 0;
			getline(ifs, s);
			if (!ifs.eof())
			{
				n1 = s.find('#', pos);
				tempInfo.level = s.substr(pos, n1 - pos);
				n2 = s.find('#', n1 + 1);
				tempInfo.problem_id = s.substr(n1 + 1, n2 - n1 - 1);
				n3 = s.find('#', n2 + 1);
				tempInfo.id = s.substr(n2 + 1, n3 - n2 - 1);
				n4 = s.find('#', n3 + 1);
				tempInfo.time = s.substr(n3 + 1, n4 - n3 - 1);
				tempInfo.score = s.substr(n4 + 1);

				InfoVec.push_back(tempInfo);
			}
			else
				break;
		}
	}
	ifs.close();
}
void EndScene::AddScore() //添加得分记录
{
	ofs.open("ScoreTable.txt", std::ios::app);
	if (!ofs)
	{
		std::cout << "file error!" << std::endl;
		exit(0);
	}
	else //格式 2022-12-01 13:05
	{
		std::string temp = nowInfo.level + '#' + nowInfo.problem_id + '#' + nowInfo.id + '#' +
			nowInfo.time + '#' + nowInfo.score + "\n";
		ofs << temp;
	}
	ofs.close();
}
//绘制单条信息
void EndScene::DrawInfo(int i, int cnt)
{
	if (cnt <= 9)
	{
		std::string temp;
		std::string space = "";
		if (InfoVec[i].id.length() < 8)
		{
			int j = 8 - InfoVec[i].id.length();
			while (j)
			{
				space += " ";
				j--;
			}
			temp = InfoVec[i].level + " " + InfoVec[i].problem_id + "  " +
				InfoVec[i].id + "  " + space + InfoVec[i].time + "  >>>  " + InfoVec[i].score;
		}
		else
			temp = InfoVec[i].level + " " + InfoVec[i].problem_id + "  " +
			InfoVec[i].id + "  " + InfoVec[i].time + "  >>>  " + InfoVec[i].score;

		//string转TCHAR
		std::wstring wstr(temp.begin(), temp.end());
		_TCHAR* Tstr = (_TCHAR*)(&wstr[0]);

		if (InfoVec[i].time == nowInfo.time) //高亮当前分数信息
		{
			setfillcolor(RGB(226, 235, 243));
			setfillstyle(BS_SOLID); //填充
			fillrectangle(65, 200 + cnt * 50, 595, 220 + cnt * 50);

			//设置字体
			LOGFONT f;
			gettextstyle(&f);                     // 获取当前字体设置
			f.lfHeight = 20;             // 设置字体高度为 h
			wcscpy_s(f.lfFaceName, _T("楷体"));
			f.lfQuality = ANTIALIASED_QUALITY;    // 设置输出效果为抗锯齿  
			settextstyle(&f);
			settextcolor(RGB(52, 72, 97));
			outtextxy(70, 200 + cnt * 50, Tstr);
		}
		else
			Info[i] = Button(70, 200 + cnt * 50, 230, 20, Tstr, WHITE, 0);
	}
}
void EndScene::ShowScore()
{
	SetFont(2, 40);

	//按键显示
	setlinecolor(WHITE);
	switch (levelIndex)
	{
	case 0:
		OptionButton(50, 270, 120, 170, _T("  Level "), 0, 0, 0);
		break;
	case 1:
		OptionButton(50, 270, 120, 170, _T("  Easy    1"), 0, 0, 0);
		break;
	case 2:
		OptionButton(50, 270, 120, 170, _T("  Medium  2"), 0, 0, 0);
		break;
	case 3:
		OptionButton(50, 270, 120, 170, _T("  Hard    3"), 0, 0, 0);
		break;
	case 4:
		OptionButton(50, 270, 120, 170, _T("  Expert  4"), 0, 0, 0);
		break;
	case 5:
		OptionButton(50, 270, 120, 170, _T("  Evil    5"), 0, 0, 0);
		break;
	}
	switch (idIndex)
	{
	case 0:
		OptionButton(270, 325, 120, 170, _T("0X"), 0, 0, 0);
		break;
	case 1:
		OptionButton(270, 325, 120, 170, _T("01"), 0, 0, 0);
		break;
	case 2:
		OptionButton(270, 325, 120, 170, _T("02"), 0, 0, 0);
		break;
	case 3:
		OptionButton(270, 325, 120, 170, _T("03"), 0, 0, 0);
		break;
	case 4:
		OptionButton(270, 325, 120, 170, _T("04"), 0, 0, 0);
		break;
	case 5:
		OptionButton(270, 325, 120, 170, _T("05"), 0, 0, 0);
		break;
	}
	switch (playerIndex)
	{
	case 0:
		OptionButton(380, 550, 120, 170, _T("Player"), 0, 0, 0);
		break;
	case 1:
		std::wstring wstr(PlayerId.begin(), PlayerId.end());
		_TCHAR* temp = (_TCHAR*)(&wstr[0]);
		OptionButton(380, 550, 120, 170, temp, 0, 0, 0);
		break;
	}

	//绘制表格线
	outtextxy(215, 67, _T("玩家得分表"));
	setfillstyle(BS_NULL); //不填充
	setlinecolor(BLACK); // 显示边框
	setlinestyle(PS_SOLID, 3); //实线
	fillrectangle(50, 50, 600, 700);

	setlinestyle(PS_DASH, 1); //虚线
	line(50, 120, 600, 120);
	line(50, 170, 600, 170);
	line(325, 120, 325, 170);

	//选项操作
	if (CheckMouse::key != temp)
	{
		//50, 270, 120, 170     270, 325, 120, 170     380, 550, 120, 170
		if (CheckMouse::currentPos.X > 50 && CheckMouse::currentPos.X < 270
			&& CheckMouse::currentPos.Y >120 && CheckMouse::currentPos.Y < 170)
		{
			levelIndex = (levelIndex + 1) % 6;
		}
		if (CheckMouse::currentPos.X > 270 && CheckMouse::currentPos.X < 325
			&& CheckMouse::currentPos.Y >120 && CheckMouse::currentPos.Y < 170)
		{
			idIndex = (idIndex + 1) % 6;
		}
		if (CheckMouse::currentPos.X > 380 && CheckMouse::currentPos.X < 550
			&& CheckMouse::currentPos.Y >120 && CheckMouse::currentPos.Y < 170)
		{
			playerIndex = !playerIndex;
		}
		temp = CheckMouse::key;
	}


	//按要求显示游戏记录
	int cnt = 0;
	for (int i = 0; i < InfoVec.size(); i++)
	{
		//按玩家id显示 0表示显示所有玩家 1表示显示当前玩家
		if (playerIndex == 0)
		{
			//显示所有游戏记录
			if (levelIndex == 0 &&
				idIndex == 0)
			{
				DrawInfo(i, cnt);
				cnt++;
			}
			//显示相同序号的游戏记录
			if (levelIndex == 0 &&
				InfoVec[i].problem_id.substr(1) == '0' + std::to_string(idIndex))
			{
				DrawInfo(i, cnt);
				cnt++;
			}
			//显示相同难度的游戏记录
			if (InfoVec[i].level == str_level_List[levelIndex] &&
				idIndex == 0)
			{
				DrawInfo(i, cnt);
				cnt++;
			}
			//显示难度和序号都相同的游戏记录
			if (InfoVec[i].level == str_level_List[levelIndex] &&
				InfoVec[i].problem_id == std::to_string(levelIndex * 100 + idIndex))
			{
				DrawInfo(i, cnt);
				cnt++;
			}
		}
		else if (playerIndex == 1)
		{
			if (InfoVec[i].id == PlayerId)
			{
				//显示所有游戏记录
				if (levelIndex == 0 &&
					idIndex == 0)
				{
					DrawInfo(i, cnt);
					cnt++;
				}
				//显示相同序号的游戏记录
				if (levelIndex == 0 &&
					InfoVec[i].problem_id.substr(1) == '0' + std::to_string(idIndex))
				{
					DrawInfo(i, cnt);
					cnt++;
				}
				//显示相同难度的游戏记录
				if (InfoVec[i].level == str_level_List[levelIndex] &&
					idIndex == 0)
				{
					DrawInfo(i, cnt);
					cnt++;
				}
				//显示难度和序号都相同的游戏记录
				if (InfoVec[i].level == str_level_List[levelIndex] &&
					InfoVec[i].problem_id == std::to_string(levelIndex * 100 + idIndex))
				{
					DrawInfo(i, cnt);
					cnt++;
				}
			}
		}

	}
}
int EndScene::str2int(std::string str)
{
	int temp;
	std::stringstream os;
	os << str;
	os >> temp;
	return temp;
}
void EndScene::Update()
{
	BeginBatchDraw();
	cleardevice();
	//CheckMouse::checkmouse();
	//设置字体
	SetFont(1, 50);
	//绘制选项
	setbkcolor(WHITE);
	ShowScore();
	pics.DrawingEnds();
	EndBatchDraw();
}
/*===============================================================================*/


/*===============================================================================*/
//九宫格
int MapFilling[9][9]; //0为不可填的位置 1为填过 2为可以填但未填的位置
class Chessboard
{
private:
	int Map_HL[9][9]; //高亮矩阵
	int Map_Mistake[9][9]; //出错矩阵
	bool showanswer;
	bool temp;
public:
	Chessboard();
	void Print(int x, int y);
	void InitMap();
	void DrawGrid();
	void UpdateMainMap();
	void UpdateHLMap();
	bool UpdateMisMap();
	void Highlight();
	void DrawNumber(bool isShow, int curBoard[9][9], int curMapFill[9][9]); //当前所画表
	bool CheckFull();
};

Chessboard::Chessboard()
{
	temp = CheckMouse::key;
	showanswer = 0;
	setbkcolor(WHITE); //设置背景颜色
	settextcolor(BLACK);
	setbkmode(1);  //设置字体背景为透明
	cleardevice();
	SetFont(1, 40); //设置字体
}
void Chessboard::Print(int x, int y)
{
	char a = (char)CheckMouse::currentPos.Y;
	TCHAR c = a;
	outtextxy(x, y, c);
}
void Chessboard::InitMap()
{
	for (int i = 0; i < MAP_ROW; i++)
		for (int j = 0; j < MAP_COL; j++)
		{
			if (MapFilling[i][j] == 1 || MapFilling[i][j] == 2) //重新开始时的初始化
				MapMain[i][j] = 0;

			MapFilling[i][j] = Map_HL[i][j] = Map_Mistake[i][j] = 0;
			if (MapMain[i][j] == 0)
				MapFilling[i][j] = 2;
		}
}
void Chessboard::DrawGrid()
{
	//画九宫格的框线
	setlinecolor(BLACK);
	for (int row = 1; row <= MAP_ROW + 1; row++) //横线
	{
		if ((row - 1) % 3 == 0)
			setlinestyle(PS_SOLID, 3); //实线
		else
			setlinestyle(PS_DASH, 1); //虚线
		line(60, row * 60, 600, row * 60);
	}
	for (int col = 1; col <= MAP_COL + 1; col++) //竖线
	{
		if ((col - 1) % 3 == 0)
			setlinestyle(PS_SOLID, 3);
		else
			setlinestyle(PS_DASH, 1);
		line(col * 60, 60, col * 60, 600);
	}
}
void Chessboard::UpdateMainMap()
{
	if (temp != CheckMouse::key)
	{
		int curI = CheckMouse::currentPos.Y / 60 - 1;
		int curJ = CheckMouse::currentPos.X / 60 - 1;
		if (curI >= 0 && curI <= 8 && curJ >= 0 && curJ <= 8)
		{
			if (SelNumber != 10 && MapFilling[curI][curJ] == 2) //防止修改原始题目数字
			{
				MapMain[curI][curJ] = SelNumber;
				MapFilling[curI][curJ] = 1; //更新两个矩阵
			}
			else if (SelNumber == 10 && MapFilling[curI][curJ] == 1)//防止删除原始题目数字
			{
				MapMain[curI][curJ] = 0; //X删除填入的数字
				MapFilling[curI][curJ] = 2;
			}

		}
	}
	temp = CheckMouse::key;
}
void Chessboard::UpdateHLMap()
{
	int curI = CheckMouse::currentPos.Y / 60 - 1;
	int curJ = CheckMouse::currentPos.X / 60 - 1;
	int i, j;
	//Map_HL设置
	//清除上次的高亮数据
	for (i = 0; i < 9; i++)
		for (j = 0; j < 9; j++)
			Map_HL[i][j] = 0;
	if (curI >= 0 && curI <= 8 && curJ >= 0 && curJ <= 8)
	{
		if (MapMain[curI][curJ] != 0) // 高亮相同数字 置为1
		{
			int target = MapMain[curI][curJ];
			for (i = 0; i < MAP_ROW; i++)
				for (j = 0; j < MAP_COL; j++)
				{
					if (MapMain[i][j] == target && i != curI && j != curJ)
						Map_HL[i][j] = 1;
					else if (i == curI && j == curJ)
						Map_HL[i][j] = 3;
				}
		}
		else // 高亮同行列宫 置为2
		{
			for (i = 0; i < MAP_ROW; i++)
				for (j = 0; j < MAP_COL; j++)
				{
					if (i == curI || j == curJ)
						Map_HL[i][j] = 2;
					if (i / 3 == curI / 3 && j / 3 == curJ / 3)
						Map_HL[i][j] = 2;
					if (i == curI && j == curJ)
						Map_HL[i][j] = 3;
				}
		}
	}
}
bool Chessboard::UpdateMisMap()
{
	//Map_Mistake设置
	//清除上次的错误数据
	int i, j, r, c;
	bool flag = 0;
	for (i = 0; i < 9; i++)
		for (j = 0; j < 9; j++)
			Map_Mistake[i][j] = 0;
	//对每列检查错误
	for (i = 0; i < 9; i++)
	{
		int arr[10] = { 0 };
		for (j = 0; j < 9; j++)
			arr[MapMain[i][j]]++;
		for (j = 0; j < 9; j++)
			if (arr[MapMain[i][j]] >= 2 && MapMain[i][j] != 0)
			{
				Map_Mistake[i][j] = 1;
				flag = 1;
			}
	}
	//对每行检查错误
	for (i = 0; i < 9; i++)
	{
		int arr[10] = { 0 };
		for (j = 0; j < 9; j++)
			arr[MapMain[j][i]]++;
		for (j = 0; j < 9; j++)
			if (arr[MapMain[j][i]] >= 2 && MapMain[j][i] != 0)
			{
				flag = 1;
				Map_Mistake[j][i] = 1;
			}
	}
	//对每宫检查错误
	for (i = 0; i <= 6; i += 3)
	{
		for (j = 0; j <= 6; j += 3)
		{
			int arr[10] = { 0 };
			for (r = i; r <= i + 2; r++)
				for (c = j; c <= j + 2; c++)
					arr[(MapMain[r][c])]++;
			for (r = i; r <= i + 2; r++)
				for (c = j; c <= j + 2; c++)
					if (arr[MapMain[r][c]] >= 2 && MapMain[r][c] != 0)
					{
						Map_Mistake[r][c] = 1;
						flag = 1;
					}
		}
	}
	return flag;
}
void Chessboard::Highlight()
{
	//先高亮后outtextxy数字，否则数字会被覆盖
	//1.高亮提示数字
	UpdateHLMap();
	for (int i = 0; i < 9; i++)
		for (int j = 0; j < 9; j++)
			if (Map_HL[i][j])
			{
				if (Map_HL[i][j] == 1)
					setfillcolor(RGB(195, 214, 234));   //相同数字用绿色显示
				else if (Map_HL[i][j] == 2)
					setfillcolor(RGB(226, 235, 243)); //同行列宫用浅灰色显示
				else
					setfillcolor(RGB(187, 222, 251)); //当前位置
				solidrectangle((j + 1) * 60 + 2, (i + 1) * 60 + 2, (j + 2) * 60 - 2, (i + 2) * 60 - 2);
			}

	//2.高亮错误数字所在的框格
	bool flag = UpdateMisMap();
	if (flag)
	{
		setfillcolor(RGB(247, 207, 214));
		for (int i = 0; i < 9; i++)
			for (int j = 0; j < 9; j++)
				if (Map_Mistake[i][j])
					solidrectangle((j + 1) * 60 + 1, (i + 1) * 60 + 1, (j + 2) * 60 - 1, (i + 2) * 60 - 1);
	}

}
void Chessboard::DrawNumber(bool isShow, int curBoard[9][9], int curMapFill[9][9]) //当前所画表
{
	if (isShow) //显示答案状态
	{
		SetFont(1, 40);
		for (int row = 0; row < MAP_ROW; row++)
			for (int col = 0; col < MAP_COL; col++)
				curMapFill[row][col] = 0;
	}
	//画宫格数字
	TCHAR str[10];
	for (int row = 0; row < MAP_ROW; row++)
	{
		for (int col = 0; col < MAP_COL; col++)
		{
			//数字0就不需要显示了
			if (curBoard[row][col])
			{
				if (curMapFill[row][col])
					settextcolor(BLUE);
				//else if (Map_HL[row][col] || Map_Mistake[row][col])
					//settextcolor(WHITE);
				else
					settextcolor(RGB(52, 72, 97));
				_stprintf_s(str, _T("%d"), curBoard[row][col]);
				outtextxy((col + 1) * 60 + 20, (row + 1) * 60 + 10, str);
			}
		}
	}
}
bool Chessboard::CheckFull()
{
	for (int i = 0; i < 9; i++)
		for (int j = 0; j < 9; j++)
			if (MapMain[i][j] == 0 && MapFilling[i][j] != 1)
				return 0;
	return 1;
}
/*===============================================================================*/


/*===============================================================================*/
class ChoiceList
{
private:
	int index;
	int list_HL[10];
	int list_Sel[10];
	bool temp;

public:
	ChoiceList();
	void SelNum(); //选中要填的数字 用绿色标注
	void UpdateList(); //更新悬浮高亮数组
	void DrawList(); //绘制数字列表
};

ChoiceList::ChoiceList()
{
	SelNumber = 1;
	index = 0;
	for (int i = 0; i < 10; i++)
		list_HL[i] = list_Sel[i] = 0;

	LOGFONT f;
	gettextstyle(&f);                     // 获取当前字体设置
	f.lfHeight = 40;                      // 设置字体高度为 40
	wcscpy_s(f.lfFaceName, _T("楷体"));    // 设置字体为“楷体”(高版本 VC 推荐使用 _tcscpy_s 函数)
	f.lfQuality = ANTIALIASED_QUALITY;    // 设置输出效果为抗锯齿  
	settextstyle(&f);
}
void ChoiceList::SelNum() //选中要填的数字 用绿色标注
{
	if (temp != CheckMouse::key)
	{
		int curI = (CheckMouse::currentPos.X + 30) / 60 - 1;
		if (CheckMouse::currentPos.Y <= 690 && CheckMouse::currentPos.Y >= 630 && curI >= 0 && curI <= 9)
		{
			for (int i = 0; i < 10; i++)
				list_Sel[i] = 0;
			list_Sel[curI] = 1;
			SelNumber = curI + 1; //更新当前欲填入数字
		}
	}
	temp = CheckMouse::key;
}
void ChoiceList::UpdateList() //更新悬浮高亮数组
{
	//先重置list
	for (int i = 0; i < 10; i++)
		list_HL[i] = 0;

	int curI = (CheckMouse::currentPos.X + 30) / 60 - 1;
	if (CheckMouse::currentPos.Y <= 690 && CheckMouse::currentPos.Y >= 630 && curI >= 0 && curI <= 9)
		list_HL[curI] = 1;
}
void ChoiceList::DrawList() //绘制数字列表
{
	TCHAR str[10];
	for (int i = 0; i < 10; i++)
	{
		if (list_HL[i] == 0)
			setfillcolor(RGB(174, 174, 174));  //未选中数字用浅灰色显示
		else if (list_HL[i] == 1)
			setfillcolor(RGB(226, 235, 243)); //当前位置
		if (list_Sel[i] == 1)
			setfillcolor(RGB(187, 222, 251));

		fillcircle((i + 1) * 60, 11 * 60, 30); //(i + 2) * 60 - 2, 12 * 60
		settextcolor(WHITE);
		if (i != 9)
			_stprintf_s(str, _T("%d"), i + 1);
		else
			_stprintf_s(str, _T("%c"), 88);
		outtextxy((i + 1) * 60 - 9, 11 * 60 - 19, str);
	}
}
/*===============================================================================*/


/*===============================================================================*/
class RecordBoard
{
private:
	COORD currentPos;
	bool temp; //控制点击

public:
	RecordBoard();
	void DrawOptions();
	void Options();
	int faults = 0;
	bool End();
	std::string float2str(float f);
};
RecordBoard::RecordBoard()
{
	currentPos.X = CheckMouse::currentPos.X;
	currentPos.Y = CheckMouse::currentPos.Y;
	temp = CheckMouse::key;
}
void RecordBoard::DrawOptions()
{
	//settextstyle(30, 0, _T("宋体"));
	setfillcolor(RGB(158, 160, 169));

	//更新currentPos
	currentPos.X = CheckMouse::currentPos.X;
	currentPos.Y = CheckMouse::currentPos.Y;

	//绘制积分板
	//框线
	setlinestyle(PS_SOLID, 1);
	rectangle(660, 60, 830, 160);
	rectangle(650, 50, 840, 170);
	//名称
	settextcolor(BLACK);
	outtextxy(670, 70, _T("  计时器 "));
	//计时器
	TCHAR str[10];
	settextcolor(RGB(9, 74, 247));
	t2 = clock();
	second = (int)(double)(t2 - t1) / CLOCKS_PER_SEC;

	second -= sleepTime; //暂停时抵消的时长
	minute = second / 60;
	second %= 60;
	_stprintf_s(str, _T("%02d : %02d"), minute, second);
	outtextxy(695, 120, str);

	setlinestyle(PS_SOLID, 1);
	rectangle(660, 200, 830, 290);
	rectangle(650, 190, 840, 300);
	settextcolor(BLUE);
	std::wstring wstr(PlayerId.begin(), PlayerId.end());
	TCHAR* temp = (_TCHAR*)(&wstr[0]);
	outtextxy(690, 230, temp);

	//绘制选项
	setfillcolor(RGB(158, 160, 169));
	if (currentPos.X > 660 && currentPos.X < 840 && currentPos.Y > 350 && currentPos.Y < 400)
	{
		solidrectangle(660, 350, 840, 400);
		settextcolor(WHITE);
	}
	else
		settextcolor(BLACK);
	outtextxy(670, 360, _T(" 暂    停"));

	if (currentPos.X > 660 && currentPos.X < 840 && currentPos.Y > 420 && currentPos.Y < 470)
	{
		solidrectangle(660, 420, 840, 470);
		settextcolor(WHITE);
	}
	else
		settextcolor(BLACK);
	outtextxy(670, 430, _T(" 重新开始"));
	if (currentPos.X > 660 && currentPos.X < 840 && currentPos.Y > 490 && currentPos.Y < 540)
	{
		solidrectangle(660, 490, 840, 540);
		settextcolor(WHITE);
	}
	else
		settextcolor(BLACK);
	outtextxy(670, 500, _T(" 退    出"));
	if (currentPos.X > 660 && currentPos.X < 840 && currentPos.Y > 560 && currentPos.Y < 610)
	{
		solidrectangle(660, 560, 840, 610);
		settextcolor(WHITE);
	}
	else
		settextcolor(BLACK);
	outtextxy(670, 570, _T(" 显示答案"));

	//绘制黑点
	setfillcolor(BLACK);
	solidcircle(650, 380, 5);
	solidcircle(650, 450, 5);
	solidcircle(650, 520, 5);
	solidcircle(650, 590, 5);
}
void RecordBoard::Options()
{
	//更新currentPos
	currentPos.X = CheckMouse::currentPos.X;
	currentPos.Y = CheckMouse::currentPos.Y;

	if (temp != CheckMouse::key)
	{
		if (currentPos.X > 660 && currentPos.X < 840 && currentPos.Y > 350 && currentPos.Y < 400)
		{
			clock_t t3, t4;
			t3 = clock();
			MessageBox(GetHWnd(), L"暂停中...", L"提示", 0);
			t4 = clock();
			sleepTime += (int)(double)(t4 - t3) / CLOCKS_PER_SEC;
		}
		if (currentPos.X > 660 && currentPos.X < 840 && currentPos.Y > 420 && currentPos.Y < 470)
		{
			clock_t t3, t4;
			t3 = clock();
			int result = MessageBox(GetHWnd(), _T("重新开始?"), _T("提示"), MB_YESNO);
			switch (result)
			{
			case IDNO:
				break;
			case IDYES:
				restart = 1;
				break;
			}
			t4 = clock();
			sleepTime += (int)(double)(t4 - t3) / CLOCKS_PER_SEC;
		}

		if (currentPos.X > 660 && currentPos.X < 840 && currentPos.Y > 490 && currentPos.Y < 540)
		{
			int result = MessageBox(GetHWnd(), L"确认退出？", L"提示", MB_YESNO);
			switch (result)
			{
			case IDNO:
				break;
			case IDYES:
				endGame = 1;
				break;
			}
		}

		else if (currentPos.X > 660 && currentPos.X < 840 && currentPos.Y > 560 && currentPos.Y < 610)
		{
			clock_t t3, t4;
			t3 = clock();

			int result = MessageBox(GetHWnd(), L"显示答案", L"提示", MB_YESNO);
			switch (result)
			{
			case IDNO:
				break;
			case IDYES:
			{
				showanswer = 1;
				break;
			}
			}
			t4 = clock();
			sleepTime += (int)(double)(t4 - t3) / CLOCKS_PER_SEC;
		}
	}
	temp = CheckMouse::key;
}
bool RecordBoard::End()
{
	setbkcolor(WHITE);
	settextcolor(BLACK);
	setbkmode(1);
	setfillcolor(RGB(158, 160, 169));
	cleardevice();
	SetFont(1, 200);

	outtextxy(30, 50, _T("Well"));
	outtextxy(30, 350, _T("Done!"));

	settextstyle(40, 0, _T("宋体"));

	outtextxy(550, 60, _T("完成时间："));
	solidrectangle(550, 110, 850, 160);

	outtextxy(550, 190, _T("总得分  ："));
	solidrectangle(550, 240, 850, 290);


	TCHAR str[20];
	settextcolor(WHITE);
	_stprintf_s(str, _T("%02d分%02d秒"), minute, second);
	outtextxy(620, 110, str);

	//打分
	float LevelTime = 600; //10分钟归一化
	float scores = (LevelTime - (minute * 60 + second - sleepTime)) / LevelTime * 100;

	PlayerScore = float2str(scores); //记录当前得分

	_stprintf_s(str, _T("%.1f"), scores);
	outtextxy(650, 240, str);
	EndBatchDraw();
	Sleep(1000);
	/*
	int result = MessageBox(GetHWnd(), _T("再来一局?"), _T("hello"), MB_YESNO);
	switch (result)
	{
	case IDNO:
		//整个程序的结束
		return 1;
	case IDYES:
		endGame = 0;
		restart = 1;
		break;
	}
	return 0;*/
	return 1;
}
std::string RecordBoard::float2str(float f)
{
	std::string temp;
	std::stringstream os;
	os << f;
	os >> temp;
	return temp;
}
/*===============================================================================*/


/*===============================================================================*/
//算法逻辑
class Algorithm
{
private:
	int rowArr[9][9];
	int colArr[9][9];
	int areaArr[9][9];
	bool finish;
public:
	int tempMainMap[9][9];
	int tempMapFill[9][9];
public:
	Algorithm();
	//得到当前坐标对应的宫序号
	int getAreaNumber(int row, int col);
	//解数独
	void solveSudoku();
	//深度优先遍历
	void dfs(int curBoard[9][9], int index);
};

Algorithm::Algorithm()
{
	finish = 0;
	for (int i = 0; i < 9; i++)
		for (int j = 0; j < 9; j++)
		{
			rowArr[i][j] = colArr[i][j] = areaArr[i][j] = 0;
			tempMainMap[i][j] = MapMain[i][j];
		}

}
int Algorithm::getAreaNumber(int row, int col)
{
	return (row / 3) * 3 + (col / 3);
}
void Algorithm::solveSudoku()
{
	for (int i = 0; i < 9; i++)
		for (int j = 0; j < 9; j++)
		{
			int val = tempMainMap[i][j] - 1;
			if (val != -1)
			{
				rowArr[i][val] = 1;
				colArr[j][val] = 1;
				areaArr[getAreaNumber(i, j)][val] = 1;
			}
		}
	dfs(tempMainMap, 0);
}
void Algorithm::dfs(int curBoard[9][9], int index)
{
	//递归出口 检查所有格子 共81个 全部检查结束后方可退出
	if (index == 81) {
		finish = 1;
		return;
	}
	int row = index / 9;
	int col = index % 9;
	int val = curBoard[row][col] - 1;
	if (val != -1) //表明当前格已有数字
	{
		//从下一格递归
		dfs(curBoard, index + 1);
	}
	else //若无数字
	{
		for (int i = 0; i < 9; i++) //遍历0-9所有数字
		{
			//先记录当前格填入i后是否发生冲突
			int tempRow = rowArr[row][i];
			int tempCol = colArr[col][i];
			int tempArea = areaArr[getAreaNumber(row, col)][i];
			if (!tempRow && !tempCol && !tempArea) //填入i不发生冲突
			{
				rowArr[row][i] = colArr[col][i] = areaArr[getAreaNumber(row, col)][i] = 1; //更新九宫格记录
				curBoard[row][col] = i + 1; //将数字i+1填入当前格
				dfs(curBoard, index + 1); //继续从下一格递归
				if (finish) //若全部结束 则直接返回上一层 最终退出递归
					return;
				curBoard[row][col] = 0;  //若从下一层出来 说明下一层遇到了死胡同
				rowArr[row][i] = tempRow; //重置三个二维数组 并等待检验i+1
				colArr[col][i] = tempCol;
				areaArr[getAreaNumber(row, col)][i] = tempArea;
			}
		}
	}
}
/*===============================================================================*/


/*===============================================================================*/
class GameScene : public ISceneUpdata
{
private:
	Chessboard chess;
	ChoiceList list;
	RecordBoard board;
	Algorithm algorithm;
public:
	GameScene();
	void Update();
};

GameScene::GameScene()
{
	chess.DrawGrid();
	chess.InitMap();
	t1 = clock(); //计时器
}
void GameScene::Update()
{
	BeginBatchDraw();
	cleardevice();
	//棋盘
	SetFont(1, 40); //设置字体
	chess.DrawGrid();
	//CheckMouse::checkmouse();
	chess.UpdateMainMap();
	chess.Highlight();
	chess.DrawNumber(0, MapMain, MapFilling);
	//输入表
	list.UpdateList();
	list.SelNum();
	list.DrawList();
	//选项
	SetFont(2, 30); //设置字体
	board.DrawOptions();
	board.Options();

	if (restart == 1) //重新开始
	{
		chess.InitMap();
		//Game::ChangeScene(OptionS);
		t1 = clock(); //计时器
		sleepTime = restart = 0;
	}

	if (endGame == 1)
	{
		if (board.End()) //游戏完全结束
			Game::ChangeScene(EndS);
	}

	if (showanswer == 1)
	{
		algorithm.solveSudoku();
		chess.DrawNumber(1, algorithm.tempMainMap, algorithm.tempMapFill);
		EndBatchDraw();

		Sleep(2000);
		int result = MessageBox(GetHWnd(), _T("再来一局?"), _T("提示"), MB_YESNO);
		switch (result)
		{
		case IDNO:
			//整个程序的结束
			exit(0);
		case IDYES:
			endGame = 0;
			restart = 1;
			showanswer = 0;
			break;
		}
		
	}
	EndBatchDraw();
	if (chess.CheckFull())
	{ // 数独完成
		Sleep(1000);
		endGame = 1;
	}
}
/*===============================================================================*/

/*===============================================================================*/
ISceneUpdata* Game::nowScene;

Game::Game()
{
	SudokuData data(1);
	initgraph(w, h);
	ChangeScene(BeginS);
}
void Game::Start()
{
	//游戏主循环 负责游戏场景逻辑更新
	ISceneUpdata* temp;
	while (true)
	{
		if (nowScene != NULL)
		{
			CheckMouse::checkmouse();
			nowScene->Update();
		}
		//break;
	}
}
void Game::ChangeScene(E_SceneType type)
{
	cleardevice();
	switch (type)
	{ // 子类转父类 父类只能调用子类的虚方法
	case BeginS:
		nowScene = new BeginScene();
		break;
	case GameS:
		nowScene = new GameScene();
		break;
	case EndS:
		nowScene = new EndScene();
		break;
	case OptionS:
		nowScene = new OptionScene();
	}
}
/*===============================================================================*/

int main()
{
	Game game;
	game.Start();
	system("pause");
	return 0;
}