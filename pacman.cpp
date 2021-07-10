#include <iostream>
#include <ncurses.h> // 今回の表示全般に必要
#include <stdlib.h>	 // system()に必要
#include <locale.h>	 // setlocale()による表示日本語化に必要
#include <cstring>	 // memcpy()に必要

// プログラム中の定数（変更しない値）をまとめたクラス
class Const
{
public:
	// キャラクターの移動方向や状態を表す定数
	static const int INACTIVE = 0;
	static const int ACTIVE = 1;
	static const int UP = 2;
	static const int RIGHT = 3;
	static const int DOWN = 4;
	static const int LEFT = 5;
	static const int STOP = 6;

	// data[][]の要素や色名を参照するときのあだ名
	static const int WALL = 1;
	static const int ROAD = 2;
	static const int FOOD = 3;
	static const int TRAP = 4;
	static const int RECV = 5;
	static const int DOOR = 6;
	static const int HERO = 10; // 色でのみ使用（マップでは不使用）
	static const int WANI = 11; // 色でのみ使用（マップでは不使用）
	static const int GOST = 12; // 色でのみ使用（マップでは不使用）
	static const int MESG = 20; // 色でのみ使用（マップでは不使用）

	// ゲーム中の状態
	static const int PLAYING = 1;  // プレイしている状態
	static const int CLEARED = 2;  // 現在のステージをクリアした状態
	static const int GAMEOVER = 3; // ゲームオーバー

	// getch()でのキー入力の待ち時間（単位:ms）
	// 永久ループのインターバルを兼ねている
	static const int GETCH_WAIT = 10;

	// ステージ（面）数
	static const int NUMBER_OF_STAGES = 2;

	// 用意するワニと幽霊の頭数
	static const int NUMBER_OF_WANIS = 5;
	static const int NUMBER_OF_GHOSTS = 5;
};

// このゲームの配色を設定する（クラスに属していないC言語風の関数）
void setup_my_colors(void)
{
	// 使える色名はCOLOR_{BLACK|RED|GREEN|YELLOW|BLUE|MAGENTA|CYAN|WHITE}
	init_color(COLOR_BLACK, 0, 0, 0); //RGB成分を0〜1000で指定
	init_color(COLOR_RED, 1000, 100, 100);
	init_color(COLOR_GREEN, 200, 1000, 300);
	init_color(COLOR_YELLOW, 1000, 1000, 200);
	init_color(COLOR_BLUE, 200, 300, 1000);
	init_color(COLOR_MAGENTA, 350, 150, 150);
	init_color(COLOR_CYAN, 200, 800, 1000);
	init_color(COLOR_WHITE, 1000, 1000, 1000);

	init_pair(Const::WALL, COLOR_MAGENTA, COLOR_MAGENTA);
	init_pair(Const::ROAD, COLOR_BLACK, COLOR_BLACK);
	init_pair(Const::FOOD, COLOR_YELLOW, COLOR_BLACK);
	init_pair(Const::HERO, COLOR_CYAN, COLOR_BLACK);
	init_pair(Const::WANI, COLOR_GREEN, COLOR_BLACK);
	init_pair(Const::GOST, COLOR_WHITE, COLOR_BLACK);
	init_pair(Const::MESG, COLOR_WHITE, COLOR_BLACK);
}

// マップのクラス
class Map
{
private:
	static const int MAP_SIZE_Y = 25; // マップの縦幅
	static const int MAP_SIZE_X = 20; // マップの横幅
	int data[MAP_SIZE_Y][MAP_SIZE_X]; // マップのデータをここにコピーする

	int stage1[MAP_SIZE_Y][MAP_SIZE_X] = {
		// ステージ1のマップのデータ
		{2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
		{2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 3, 1, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 4, 1, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 3, 1, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 2, 1, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 3, 1, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 3, 1, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	};

	int stage2[MAP_SIZE_Y][MAP_SIZE_X] = {
		// ステージ2のマップのデータ
		{2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
		{2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1},
		{1, 3, 1, 1, 3, 1, 1, 1, 3, 1, 1, 3, 1, 1, 1, 3, 1, 1, 3, 1},
		{1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1},
		{1, 1, 1, 1, 3, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 3, 1, 1, 1, 1},
		{1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1},
		{1, 3, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 3, 1},
		{1, 3, 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1, 3, 1},
		{1, 3, 3, 3, 1, 1, 3, 1, 1, 3, 3, 1, 1, 3, 1, 1, 3, 3, 3, 1},
		{1, 3, 1, 3, 1, 3, 3, 1, 3, 3, 2, 3, 1, 3, 3, 1, 3, 1, 3, 1},
		{1, 3, 1, 3, 1, 3, 1, 1, 3, 1, 1, 3, 1, 1, 3, 1, 3, 1, 3, 1},
		{1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1},
		{1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1},
		{1, 3, 1, 3, 1, 3, 1, 3, 1, 3, 3, 1, 3, 1, 3, 1, 3, 1, 3, 1},
		{1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1},
		{1, 3, 1, 3, 1, 3, 1, 3, 1, 1, 1, 1, 3, 1, 3, 1, 3, 1, 3, 1},
		{1, 3, 3, 3, 3, 3, 1, 3, 3, 3, 3, 3, 3, 1, 3, 3, 3, 3, 3, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
		{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	};

public:
	void init(int stage) // ステージ（面）ごとの初期化用関数
	{
		switch (stage)
		{
		case 1: // ステージ2のマップのデータをdata[][]にコピー
			memcpy(data, stage1, sizeof stage1);
			break;
		case 2: // ステージ2のマップのデータをdata[][]にコピー
			memcpy(data, stage2, sizeof stage2);
			break;
		}
	}

	void drawmap() // マップを描画する関数
	{
		for (int y = 0; y < MAP_SIZE_Y; y++)
		{
			for (int x = 0; x < MAP_SIZE_X; x++)
			{
				int c = data[y][x];
				switch (c)
				{
				case Const::WALL: // 壁をいわゆる全角スペースで表示
					attrset(COLOR_PAIR(Const::WALL));
					mvaddstr(y, x * 2, "　");
					break;
				case Const::ROAD: // 道をいわゆる全角スペースで表示
					attrset(COLOR_PAIR(Const::ROAD));
					mvaddstr(y, x * 2, "　");
					break;
				case Const::FOOD: // エサ「・」を表示（🍒🍓等で代替可）
					attrset(COLOR_PAIR(Const::FOOD));
					mvaddstr(y, x * 2, "・");
					break;
				}
			}
		}
	}

	// ヒーローによってエサが食べられて消える、という処理をする関数
	void eaten(int y, int x) { data[y][x] = Const::ROAD; }
	// ROADに変化させる関数 eatenと同義
	void toRoad(int y, int x) { data[y][x] = Const::ROAD; }

	// 座標(y, x)の場所がエサならtrue、それ以外ならばfalseを返す関数
	bool isFood(int y, int x) { return data[y][x] == Const::FOOD; }

	// 座標(y, x)の場所が壁ならtrue、それ以外ならばfalseを返す関数
	bool isWall(int y, int x) { return data[y][x] == Const::WALL; }
	bool isTrap(int y, int x) { return data[y][x] == Const::TRAP; }
	bool isRecv(int y, int x) { return data[y][x] == Const::RECV; }


	// マップ上に現在エサが何個残っているかを数える関数
	int countUneatenFoods()
	{
		int q = 0;
		for (int y = 0; y < MAP_SIZE_Y; y++)
		{
			for (int x = 0; x < MAP_SIZE_X; x++)
			{
				if (data[y][x] == Const::FOOD)
				{
					q++;
				}
			}
		}
		return q;
	}

	// 文字列（定数）を指定の座標に表示する関数
	void printString(int y, int x, const char msg[])
	{
		attrset(COLOR_PAIR(Const::MESG));
		mvaddstr(y, x * 2, msg);
	}

	// 整数を指定の座標に表示する関数
	void printInt(int y, int x, int n)
	{
		attrset(COLOR_PAIR(Const::MESG));
		mvprintw(y, x * 2, "%d", n);
	}
};

// ヒーローを表現するためのクラス
class Hero
{
private:
	const int wait_max = 7; // GETCH_WAITの7倍、という意味 スピードを制御する(遅くしている)
	int wait = 0;
	int dx;
	int dy;

public:
	int y, x;
	bool trapped = false; // トラップにかかったかの判定用bool
	// ヒーローを初期化する関数
	void init(int arg_y, int arg_x)
	{
		y = arg_y;
		x = arg_x;
		wait = 0;
	}

	// ヒーローのwaitを減らす関数
	void waitupdate()
	{
		if (wait > 0)
		{
			wait--;
		}
	}

	// TRAPかチェックする関数
	void checktrap(Map *m)
	{
		if ((*m).isTrap(y, x))
		{
			trapped = true;
			(*m).toRoad(y, x);
		}
	}

	// RECVかチェックする関数
	void checkRecv(Map *m){
		if ((*m).isRecv(y, x)){
			trapped = false;
			(*m).toRoad(y, x);
		}
	}

	// 餌を食べる一連の関数
	void caneat(Map *m)
	{
		if ((*m).isFood(y, x))
		{
			(*m).eaten(y, x);
		}
	}

	// 壁を判定し、なければその方向に進む関数
	void checkmove(Map *m)
	{
		if (!(*m).isWall(y + dy, x + dx))
		{
			y = y + dy;
			x = x + dx;
		}
	}

	// マップのアイテムチェック + 動作
	void behavior(Map *m)
	{
		checkmove(m); // 壁判定
		caneat(m); // 餌判定
		checktrap(m); // トラップ判定
		checkRecv(m); // RECV判定
	}

	// 次の方向を決める関数
	void setNextDirection(int dx, int dy)
	{
		dx = dx;
		dy = dy;
	}

	// ヒーローのスピードを決定する関数
	void setmovespeed(float t){
		wait = t * wait_max;
	}

	// ヒーローの座標を更新する関数
	void move(Map *m, int direction)
	{
		if (wait > 0)
		{
			wait--;
			return;
		}
		switch (direction)
		{
		case Const::UP:
			setNextDirection(dy = -1, dx = 0);
			behavior(m);
			break;

		case Const::RIGHT:
			setNextDirection(dy = 0, dx = 1);
			behavior(m);
			break;

		case Const::DOWN:
			setNextDirection(dy = 1, dx = 0);
			behavior(m);
			break;

		case Const::LEFT:
			setNextDirection(dy = 0, dx = -1);
			behavior(m);
			break;
		}

		if (trapped == true){
			setmovespeed(3.0);
		}

		else if (trapped == false){
			setmovespeed(1.0);
		}
		
	}

	void moving(Map *mp, int ch)
	{
			if (ch == KEY_UP)
			{
				move(mp, Const::UP);
			}
			else if (ch == KEY_RIGHT)
			{
				move(mp, Const::RIGHT);
			}
			else if (ch == KEY_DOWN)
			{
				move(mp, Const::DOWN);
			}
			else if (ch == KEY_LEFT)
			{
				move(mp, Const::LEFT);
			}
	}

	// ヒーロー「コ」「ロ」（😃😊も可）を指定の座標に表示する関数
	void draw()
	{
		attrset(COLOR_PAIR(Const::HERO));
		if ((y + x) % 2)
		{
			mvaddstr(y, x * 2, "コ");
		} // 😃
		else
		{
			mvaddstr(y, x * 2, "ロ");
		} // 😊
	}
};

// ワニを表現するためのクラス
class Wani
{
private:
	int y, x;
	const int wait_max_slow = 25; // GETCH_WAITの25倍、という意味
	const int wait_max_fast = 9;  // GETCH_WAITの9倍、という意味
	int wait_max = wait_max_slow;
	int wait = 0;
	int direction;
	int state;

public:
	// ワニを初期化する関数
	void init(int arg_y, int arg_x, int arg_direction, int arg_state)
	{
		y = arg_y;
		x = arg_x;
		direction = arg_direction;
		state = arg_state;
	}

	// ワニの座標を更新する関数
	void move(Map *m, Hero h)
	{
		if (state == Const::INACTIVE)
		{
			return;
		} // 不活性状態

		if (y == h.y)
		{							  // ヒーローとy座標が同じ場合
			wait_max = wait_max_fast; // スピードアップ
			if (wait > wait_max)
			{
				wait = 0;
			} // 少し反応が早くなる
		}
		else
		{
			wait_max = wait_max_slow;
		} // 遅めのウェイト

		if (wait > 0)
		{
			wait--;
			return;
		}

		// waitが0の瞬間だけ、以下の座標の更新処理をおこなう
		wait = wait_max;

		if (y == h.y)
		{				 // ヒーローとy座標が同じ場合
			if (x < h.x) // ワニがヒーローの左側にいる場合
			{
				if ((*m).isWall(y, x + 1))
				{
					direction = Const::STOP;
				}
				else
				{
					direction = Const::RIGHT;
				}
			}
			else if (h.x < x) // ワニがヒーローの右側にいる場合
			{
				if ((*m).isWall(y, x - 1))
				{
					direction = Const::STOP;
				}
				else
				{
					direction = Const::LEFT;
				}
			}
			else
			{
				direction = Const::STOP;
			} // ヒーローと衝突したら停止
		}

		else if ((*m).isWall(y, x + 1) && (*m).isWall(y, x - 1))
		{
			direction = Const::STOP; // 左右両側が壁の場合は停止
		}
		else if ((*m).isWall(y, x + 1))
		{
			direction = Const::LEFT;
		}
		else if ((*m).isWall(y, x - 1))
		{
			direction = Const::RIGHT;
		}

		switch (direction)
		{
		case Const::RIGHT:
			x++;
			break;
		case Const::LEFT:
			x--;
			break;
		}
	}

	// ワニ「辶」（🐊も可）を指定の座標に表示する関数
	void draw()
	{
		if (state == Const::INACTIVE)
		{
			return;
		}
		attrset(COLOR_PAIR(Const::WANI));
		mvaddstr(y, x * 2, "辶"); // 🐊
	}

	// ワニがヒーローに衝突したかどうかを判定をする関数
	bool killedHero(Hero h)
	{
		if (state == Const::INACTIVE)
		{
			return false;
		}
		if ((y == h.y) && (x == h.x))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};

// 幽霊を表現するためのクラス
class Ghost
{
private:
	int y, x;
	int wait_max = 140; // GETCH_WAITの140倍、という意味
	int wait = 0;
	const int vertical = 0;
	const int horizontal = 1;
	int direction = vertical; // 縦・横どちらに移動するか
	int state;				  // 不活性かどうかの状態

public:
	// 幽霊を初期化する関数
	void init(int arg_y, int arg_x, int arg_state)
	{
		y = arg_y;
		x = arg_x;
		state = arg_state;
	}

	// 幽霊の座標を更新する関数
	void move(Map *m, Hero h)
	{
		if (state == Const::INACTIVE)
		{
			return;
		} // 不活性状態
		if (wait > 0)
		{
			wait--;
			return;
		}

		// waitが0の瞬間だけ、以下の座標の更新処理をおこなう
		wait = wait_max;

		if (direction == vertical)
		{
			if (y < h.y)
			{
				y++;
			}
			else if (h.y < y)
			{
				y--;
			}
			direction = horizontal;
		}
		else
		{
			if (x < h.x)
			{
				x++;
			}
			else if (h.x < x)
			{
				x--;
			}
			direction = vertical;
		}
	}

	// 幽霊「凧」（👻も可）を指定の座標に表示する関数
	void draw()
	{
		if (state == Const::INACTIVE)
		{
			return;
		}
		attrset(COLOR_PAIR(Const::GOST));
		mvaddstr(y, x * 2, "凧"); // 👻
	}

	// 幽霊がヒーローに衝突したかどうかを判定をする関数
	bool killedHero(Hero h)
	{
		if (state == Const::INACTIVE)
		{
			return false;
		}
		if ((y == h.y) && (x == h.x))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};

int main()
{
	setlocale(LC_ALL, "");		// 非ASCII文字（日本語や絵文字）の表示に必要
	initscr();					// ncursesを使用する際、最初におこなう初期化
	start_color();				// カラー表示を開始する
	setup_my_colors();			// このゲームの配色を設定
	curs_set(0);				// カーソルを非表示にする
	noecho();					// キー入力を非表示にする
	cbreak();					// Enterキー無しでキー入力を受け取る
	keypad(stdscr, TRUE);		// カーソルキーを有効にする
								//（上・下・左・右: KEY_{UP|DOWN|LEFT|RIGHT}）
	timeout(Const::GETCH_WAIT); // getch()で読み取るキー入力の待ち時間（単位:ms）

	// キーのオートリピートを止める（ゲーム終了時に元に戻すこと）
	system("/usr/bin/xset r off");

	int game_state; // ゲームの状態
	int stage;		// ステージ（いわゆる面数）

	Map map;		// マップを作成
	Map *mp = &map; // mpはmapを指すポインタ

	Hero hero;							  // ヒーローを作成
	Wani wani[Const::NUMBER_OF_WANIS];	  // ワニを作成
	Ghost ghost[Const::NUMBER_OF_GHOSTS]; // 幽霊を作成

	for (stage = 1; stage <= Const::NUMBER_OF_STAGES; stage++)
	{
		game_state = Const::PLAYING;

		switch (stage) // ステージ（面）の初期化
		{
		case 1:
			(*mp).init(stage);
			hero.init(11, 5);
			wani[0].init(9, 7, Const::RIGHT, Const::ACTIVE);
			wani[1].init(12, 12, Const::LEFT, Const::ACTIVE);
			wani[2].init(-1, -1, Const::STOP, Const::INACTIVE); // 表示されない
			wani[3].init(-1, -1, Const::STOP, Const::INACTIVE); // 表示されない
			wani[4].init(-1, -1, Const::STOP, Const::INACTIVE); // 表示されない
			ghost[0].init(2, 1, Const::ACTIVE);
			ghost[1].init(-1, -1, Const::INACTIVE); // 表示されない
			ghost[2].init(-1, -1, Const::INACTIVE); // 表示されない
			ghost[3].init(-1, -1, Const::INACTIVE); // 表示されない
			ghost[4].init(-1, -1, Const::INACTIVE); // 表示されない
			break;
		case 2:
			(*mp).init(stage);
			hero.init(13, 10);
			wani[0].init(15, 4, Const::RIGHT, Const::ACTIVE);
			wani[1].init(15, 16, Const::RIGHT, Const::ACTIVE);
			wani[2].init(7, 10, Const::LEFT, Const::ACTIVE);
			wani[3].init(8, 11, Const::STOP, Const::ACTIVE);
			wani[4].init(8, 8, Const::STOP, Const::ACTIVE);
			ghost[0].init(2, 1, Const::ACTIVE);
			ghost[1].init(24, 18, Const::ACTIVE);
			ghost[2].init(-1, -1, Const::INACTIVE); // 表示されない
			ghost[3].init(-1, -1, Const::INACTIVE); // 表示されない
			ghost[4].init(-1, -1, Const::INACTIVE); // 表示されない
			break;
		}

		int ch; // キーボードからの入力を受け取るための変数
		while (1)
		{
			ch = getch(); // キー入力を読み取る（Enter不要）

			if ((ch == 'q') || (ch == 'Q'))
			{
				break;
			} // while(1)から抜ける

			if ((ch == 'n') || (ch == 'N'))
			{
				game_state = Const::CLEARED;
				break;
			} // while(1)から抜ける

			// ゲームがクリアされた状態ならば
			if ((game_state == Const::CLEARED) && (ch == ' '))
			{
				break;
			}

			// ゲームを遊んでいる最中ならば
			if (game_state == Const::PLAYING)
			{
				hero.waitupdate(); // ヒーローのwaitの更新

				// カーソルキーの読み取りとヒーローの座標の更新
				// KEY_{UP|RIGHT|DOWN|LEFT}はncursesが定義している定数

				hero.moving(mp, ch); // Heroの動き

				// ワニの座標の更新とヒーローとの衝突判定（衝突したらGAMEOVER）
				for (int i = 0; i < Const::NUMBER_OF_WANIS; i++)
				{
					wani[i].move(mp, hero);
					if (wani[i].killedHero(hero))
					{
						game_state = Const::GAMEOVER;
					}
				}

				// 幽霊の座標の更新とヒーローとの衝突判定（衝突したらGAMEOVER）
				for (int i = 0; i < Const::NUMBER_OF_GHOSTS; i++)
				{
					ghost[i].move(mp, hero);
					if (ghost[i].killedHero(hero))
					{
						game_state = Const::GAMEOVER;
					}
				}
			} // if (game_state == Const::PLAYING) はここまで

			// 表示のための処理開始
			erase();		 // 画面を消去する
			(*mp).drawmap(); // マップを描画する

			// ヒーロー・ワニ・幽霊を描画する
			hero.draw();
			for (int i = 0; i < Const::NUMBER_OF_WANIS; i++)
			{
				wani[i].draw();
			}
			for (int i = 0; i < Const::NUMBER_OF_GHOSTS; i++)
			{
				ghost[i].draw();
			}

			// 画面上部に文字を表示する
			switch (game_state)
			{
			case Const::PLAYING:
				(*mp).printString(0, 1, "Eat all dot:      ");
				(*mp).printInt(0, 8, (*mp).countUneatenFoods());
				break;
			case Const::CLEARED:
				(*mp).printString(1, 1, "Hit SPACE to go to the next stage.");
				break;
			case Const::GAMEOVER:
				(*mp).printString(1, 1, "Game over!! Quit \"q\"");
				break;
			}

			// 画面下部に文字を表示する（この表示、必要無ければ消してもよい）
			(*mp).printString(30, 1, "hero.y:     ");
			(*mp).printInt(30, 5, hero.y);
			(*mp).printString(31, 1, "hero.x:     ");
			(*mp).printInt(31, 5, hero.x);
			//(*mp).printInt(32, 5, hero.trapped);

			refresh(); // このrefresh()によって実際に画面が再表示される
			// 表示のための処理終わり

			// 残っているエサが0個ならゲームの状態をCLEAREDにする
			if ((*mp).countUneatenFoods() == 0)
			{
				game_state = Const::CLEARED;
			}

		} // while(1)の永久ループはここまで

		// while(1)の永久ループから抜けた後の処理
		if (game_state == Const::PLAYING)
		{
			break;
		} // q, Qを押して終了した場合
		if (game_state == Const::GAMEOVER)
		{
			break;
		} // ヒーローがやられた場合
	}

	endwin();							   // ncursesを使った時の後始末
	system("/usr/bin/xset r rate 500 33"); // キーのオートリピートを初期値に戻す
}
