#pragma once
#include "Object.h"
#include "input.h"
#include "sound.h"
#include <array>
#include <random>
#include <algorithm>
#include "Constants.h"



class Game
{
private:
	Input input;
	Sound sound;

	Object m_player;//プレイヤーオブジェクト
	Object m_enemy;//エネミーオブジェクト
	Object m_enemy_2;//2体目のエネミー
	Object m_background; //背景オブジェクト
	Object m_taiyou;//太陽オブジェクト
	Object m_blocks[STAGE_X][STAGE_Y];//テトリミノオブジェクト
	Object m_manual;//マニュアル

	Object m_scoreboard;//スコアオブジェクト
	Object m_title;		//タイトルオブジェクト
	std::vector<std::vector<int>> m_data;
	int m_count = 0;//自動落下のアカウント
	std::vector<std::vector<int>> m_ghostData;

	static const int NEXT_QUEUE_SIZE = 5;
	int m_nextQueue[NEXT_QUEUE_SIZE];//次に出るミノの種類を格納
	Object m_nextBlocks[NEXT_QUEUE_SIZE][4][4];
	Object m_nextBackBlocks[4][4];

	// Hold関係の変数
	int m_holdType = -1;      // ホールド中のテトリミノ（-1は空）
	bool m_holdUsed = false;  // このターンでホールドを使ったかどうか
	Object m_holdBlocks[4][4]; // ホールド用ブロック描画用

	int m_currentX;		//現在落下中のテトリミノの左上位置X
	int m_currentY;		//現在落下中のテトリミノの左上位置Y
	int m_currentType;	//現在落下中のテトリミノの種類（0～6）
	int m_currentDir;	//現在落下中のテトリミノの向き（0～3）

	int m_state = 0;	//ゲームの状態（0：落下するものがない状態、1：落下中、2：揃った状態、3：ゲームオーバー、）

	int m_score = 0;//得点
	int m_scene = 0;//シーン（0：タイトル、1：ゲーム本編）
	std::vector<std::vector<int>> m_typeData;
	
	int bestAggH = 0, bestHoles = 0;
	double bestCenterDist = 0.0;
	

	int curAggH = 0, curHoles = 0;
	double curCenterDist = 0.0;



	struct Bag {
		std::mt19937 rng;
		Bag() : rng(std::random_device{}()) {}  //ランダムにさせるためのコンストラクタ
	};
	Bag m_rng;
	std::array<int, 7> m_bag;
	int m_bagPos = 7;
	int m_lastPiece = -1;

	// Gameクラスのprivate: に追加 
	//int m_aiGhost[STAGE_X][STAGE_Y + 3] = {}; // AI最適解の着地形の可視化
	std::vector<std::vector<int>> m_aiGhost; 
	int m_aiBestX = 0, m_aiBestY = 0, m_aiBestDir = 0; // 評価関数の重み（標準的なテトリスAI風） 
	struct EvalWeight 
	{
		// lines は正、他は負が定番 
		double wLines = 1.0; 
		// 消えるライン数 
		double wHeight = -0.51066; 
		// 集計高さ 
		double wHoles = -0.35663; 
		// ホール数（空の上にブロックが載ってる穴） 
		double wBumpiness = -0.184483;// 段差凹凸 
	} m_w;



	struct Color {
		float r, g, b;
	};

	Color tetrominoColors[7] = {
		{0.0f, 1.0f, 1.0f},  // I
		{0.0f, 0.0f, 1.0f},  // J
		{1.0f, 0.5f, 0.0f},  // L
		{1.0f, 1.0f, 0.0f},  // O
		{0.0f, 1.0f, 0.0f},  // S
		{0.5f, 0.0f, 0.5f},  // T
		{1.0f, 0.0f, 0.0f}   // Z
	};

	int m_tetrominoData[7][4][4][4] =
	{
		{//直線（Iミノ）
			{//0
				{0,0,0,0},
				{2,2,2,2},
				{0,0,0,0},
				{0,0,0,0},
			},
			{//1
				{0,0,2,0},
				{0,0,2,0},
				{0,0,2,0},
				{0,0,2,0},
			},
			{//2
				{0,0,0,0},
				{0,0,0,0},
				{2,2,2,2},
				{0,0,0,0},
			},
			{//3
				{0,2,0,0},
				{0,2,0,0},
				{0,2,0,0},
				{0,2,0,0},
			},

		},

		{//四角（0ミノ）
			{//0
				{0,0,0,0},
				{0,2,2,0},
				{0,2,2,0},
				{0,0,0,0},
			},
			{//1
				{0,0,0,0},
				{0,2,2,0},
				{0,2,2,0},
				{0,0,0,0},
			},
			{//2
				{0,0,0,0},
				{0,2,2,0},
				{0,2,2,0},
				{0,0,0,0},
			},
			{//3
				{0,0,0,0},
				{0,2,2,0},
				{0,2,2,0},
				{0,0,0,0},
			},

		},

		{//S字（Sミノ）
			{//0
				{0,2,2,0},
				{2,2,0,0},
				{0,0,0,0},
				{0,0,0,0},
			},
			{//1
				{0,2,0,0},
				{0,2,2,0},
				{0,0,2,0},
				{0,0,0,0},
			},
			{//2
				{0,0,0,0},
				{0,2,2,0},
				{2,2,0,0},
				{0,0,0,0},
			},
			{//3
				{2,0,0,0},
				{2,2,0,0},
				{0,2,0,0},
				{0,0,0,0},
			},


		},
		{//逆S字（Zミノ）
			{//0
				{2,2,0,0},
				{0,2,2,0},
				{0,0,0,0},
				{0,0,0,0},
			},
			{//1
				{0,0,2,0},
				{0,2,2,0},
				{0,2,0,0},
				{0,0,0,0},
			},
			{//2
				{0,0,0,0},
				{2,2,0,0},
				{0,2,2,0},
				{0,0,0,0},
			},
			{//3
				{0,2,0,0},
				{2,2,0,0},
				{2,0,0,0},
				{0,0,0,0},
			},

		},
		{//逆L字（Jミノ）
			{//0
				{2,0,0,0},
				{2,2,2,0},
				{0,0,0,0},
				{0,0,0,0},
			},
			{//1
				{0,2,2,0},
				{0,2,0,0},
				{0,2,0,0},
				{0,0,0,0},
			},
			{//2
				{0,0,0,0},
				{2,2,2,0},
				{0,0,2,0},
				{0,0,0,0},
			},
			{//3
				{0,2,0,0},
				{0,2,0,0},
				{2,2,0,0},
				{0,0,0,0},
			},

		},
		{//L字（Lミノ）
			{//0
				{0,0,2,0},
				{2,2,2,0},
				{0,0,0,0},
				{0,0,0,0},
			},
			{//1
				{0,2,0,0},
				{0,2,0,0},
				{0,2,2,0},
				{0,0,0,0},
			},
			{//2
				{0,0,0,0},
				{2,2,2,0},
				{2,0,0,0},
				{0,0,0,0},
			},
			{//3
				{2,2,0,0},
				{0,2,0,0},
				{0,2,0,0},
				{0,0,0,0},
			},

		},
		{//T字（Tミノ）
			{//0
				{0,2,0,0},
				{2,2,2,0},
				{0,0,0,0},
				{0,0,0,0},
			},
			{//1
				{0,2,0,0},
				{0,2,2,0},
				{0,2,0,0},
				{0,0,0,0},
			},
			{//2
				{0,0,0,0},
				{2,2,2,0},
				{0,2,0,0},
				{0,0,0,0},
			},
			{//3
				{0,2,0,0},
				{2,2,0,0},
				{0,2,0,0},
				{0,0,0,0},
			},

		},
	};

	void HoldTetrominoDraw();
	void InitHoldTetromino();
	void HoldTetromino();
	void InitNextQueue();
	void PushOneFromSevenBag();
	void OnSpawnedPiece(int pieceId);
	void RefillWithSevenBag(void);	//2個連続で同じ形にしない。ランダム
	bool IsInvalidMoveAt(int type, int dir, int posX, int posY);
	void ShowGhostPiece(void);		//テトリミノのゴーストを表示
	void DropTetromino(void);		//テトリミノ落下
	void NEXT_tetrominoDraw(void);	//次のテトリミノ表示
	void BuildAIGhostForCurrent();	//AI最適解ゴースト
	void MoveTetromino(int dirX);	//テトリミノ左右移動
	void RotateTetromino(void);		//テトリミノ回転
	void DeleteTetromino(void);		//テトリミノ消去
	void CheckGameOver(void);		//ゲームオーバーか確認
	bool IsInvalidMove(int dirX, int dirY);
	//テトリミノを動かすことができるか
	void ShiftTetrimino(int dirX, int DirY);
	//m_dataを更新してテトリミノを移動させる

public:
	void Init(HWND hWnd);	//初期化
	void Update(void);		//更新
	void Draw();			//描画
	void Uninit();			//終了
};


/*#pragma once
#include "Object.h"
#include "input.h"
#include "sound.h"
#include <array>
#include <random>
#include <algorithm>

#define STAGE_X		(10)//横のマス数
#define STAGE_Y		(20)//縦のマス数
#define BLOCK_SIZE	(22)//1マスのサイズ


class Game
{
private:
	Input input;
	Sound sound;

	Object m_player;//プレイヤーオブジェクト
	Object m_enemy;//エネミーオブジェクト
	Object m_enemy_2;//2体目のエネミー
	Object m_background; //背景オブジェクト
	Object m_taiyou;//太陽オブジェクト
	Object m_blocks[STAGE_X][STAGE_Y];//テトリミノオブジェクト
	Object m_manual;//マニュアル

	Object m_scoreboard;//スコアオブジェクト
	Object m_title;		//タイトルオブジェクト
	int m_data[STAGE_X][STAGE_Y + 3] = {};//データ配列
	int m_count = 0;//自動落下のアカウント
	int m_ghostData[STAGE_X][STAGE_Y + 3];

	static const int NEXT_QUEUE_SIZE = 5;
	int m_nextQueue[NEXT_QUEUE_SIZE];//次に出るミノの種類を格納
	Object m_nextBlocks[NEXT_QUEUE_SIZE][4][4];
	Object m_nextBackBlocks[4][4];

	// Hold関係の変数
	int m_holdType = -1;      // ホールド中のテトリミノ（-1は空）
	bool m_holdUsed = false;  // このターンでホールドを使ったかどうか
	Object m_holdBlocks[4][4]; // ホールド用ブロック描画用

	int m_currentX;		//現在落下中のテトリミノの左上位置X
	int m_currentY;		//現在落下中のテトリミノの左上位置Y
	int m_currentType;	//現在落下中のテトリミノの種類（0～6）
	int m_currentDir;	//現在落下中のテトリミノの向き（0～3）

	int m_state = 0;	//ゲームの状態（0：落下するものがない状態、1：落下中、2：揃った状態、3：ゲームオーバー、）

	int m_score = 0;//得点
	int m_scene = 0;//シーン（0：タイトル、1：ゲーム本編）

	int m_typeData[STAGE_X][STAGE_Y + 3];//テトリミノの種類（0～6）を保持。空白は-1



	struct Bag {
		std::mt19937 rng;
		Bag() : rng(std::random_device{}()) {}  //ランダムにさせるためのコンストラクタ
	};
	Bag m_rng;
	std::array<int, 7> m_bag;
	int m_bagPos = 7;          
	int m_lastPiece = -1;     
	
	// Gameクラスのprivate: に追加
	int m_aiGhost[STAGE_X][STAGE_Y + 3] = {}; // AI最適解の着地形の可視化
	int m_aiBestX = 0, m_aiBestY = 0, m_aiBestDir = 0;

	// 評価関数の重み（標準的なテトリスAI風）
	struct EvalWeight {
		// lines は正、他は負が定番
		double wLines = 1.0;      // 消えるライン数
		double wHeight = -0.51066; // 集計高さ
		double wHoles = -0.35663; // ホール数（空の上にブロックが載ってる穴）
		double wBumpiness = -0.184483;// 段差凹凸
	} m_w;



	struct Color {
		float r, g, b;
	};

	Color tetrominoColors[7] = {
		{0.0f, 1.0f, 1.0f},  // I
		{0.0f, 0.0f, 1.0f},  // J
		{1.0f, 0.5f, 0.0f},  // L
		{1.0f, 1.0f, 0.0f},  // O
		{0.0f, 1.0f, 0.0f},  // S
		{0.5f, 0.0f, 0.5f},  // T
		{1.0f, 0.0f, 0.0f}   // Z
	};

	int m_tetrominoData[7][4][4][4] = 
	{
		{//直線（Iミノ）
			{//0
				{0,0,0,0},
				{2,2,2,2},
				{0,0,0,0},
				{0,0,0,0},
			},
			{//1
				{0,0,2,0},
				{0,0,2,0},
				{0,0,2,0},
				{0,0,2,0},
			},
			{//2
				{0,0,0,0},
				{0,0,0,0},
				{2,2,2,2},
				{0,0,0,0},
			},
			{//3
				{0,2,0,0},
				{0,2,0,0},
				{0,2,0,0},
				{0,2,0,0},
			},
		
		},

		{//四角（0ミノ）
			{//0
				{0,0,0,0},
				{0,2,2,0},
				{0,2,2,0},
				{0,0,0,0},
			},
			{//1
				{0,0,0,0},
				{0,2,2,0},
				{0,2,2,0},
				{0,0,0,0},
			},
			{//2
				{0,0,0,0},
				{0,2,2,0},
				{0,2,2,0},
				{0,0,0,0},
			},
			{//3
				{0,0,0,0},
				{0,2,2,0},
				{0,2,2,0},
				{0,0,0,0},
			},

		},

		{//S字（Sミノ）
			{//0
				{0,2,2,0},
				{2,2,0,0},
				{0,0,0,0},
				{0,0,0,0},
			},
			{//1
				{0,2,0,0},
				{0,2,2,0},
				{0,0,2,0},
				{0,0,0,0},
			},
			{//2
				{0,0,0,0},
				{0,2,2,0},
				{2,2,0,0},
				{0,0,0,0},
			},
			{//3
				{2,0,0,0},
				{2,2,0,0},
				{0,2,0,0},
				{0,0,0,0},
			},

			
		},
		{//逆S字（Zミノ）
			{//0
				{2,2,0,0},
				{0,2,2,0},
				{0,0,0,0},
				{0,0,0,0},
			},
			{//1
				{0,0,2,0},
				{0,2,2,0},
				{0,2,0,0},
				{0,0,0,0},
			},
			{//2
				{0,0,0,0},
				{2,2,0,0},
				{0,2,2,0},
				{0,0,0,0},
			},
			{//3
				{0,2,0,0},
				{2,2,0,0},
				{2,0,0,0},
				{0,0,0,0},
			},

		},
		{//逆L字（Jミノ）
			{//0
				{2,0,0,0},
				{2,2,2,0},
				{0,0,0,0},
				{0,0,0,0},
			},
			{//1
				{0,2,2,0},
				{0,2,0,0},
				{0,2,0,0},
				{0,0,0,0},
			},
			{//2
				{0,0,0,0},
				{2,2,2,0},
				{0,0,2,0},
				{0,0,0,0},
			},
			{//3
				{0,2,0,0},
				{0,2,0,0},
				{2,2,0,0},
				{0,0,0,0},
			},

		},
		{//L字（Lミノ）
			{//0
				{0,0,2,0},
				{2,2,2,0},
				{0,0,0,0},
				{0,0,0,0},
			},
			{//1
				{0,2,0,0},
				{0,2,0,0},
				{0,2,2,0},
				{0,0,0,0},
			},
			{//2
				{0,0,0,0},
				{2,2,2,0},
				{2,0,0,0},
				{0,0,0,0},
			},
			{//3
				{2,2,0,0},
				{0,2,0,0},
				{0,2,0,0},
				{0,0,0,0},
			},

		},
		{//T字（Tミノ）
			{//0
				{0,2,0,0},
				{2,2,2,0},
				{0,0,0,0},
				{0,0,0,0},
			},
			{//1
				{0,2,0,0},
				{0,2,2,0},
				{0,2,0,0},
				{0,0,0,0},
			},
			{//2
				{0,0,0,0},
				{2,2,2,0},
				{0,2,0,0},
				{0,0,0,0},
			},
			{//3
				{0,2,0,0},
				{2,2,0,0},
				{0,2,0,0},
				{0,0,0,0},
			},

		},
    };

	void HoldTetrominoDraw();
	void InitHoldTetromino();
	void HoldTetromino();
	void InitNextQueue();
	void PushOneFromSevenBag();
	void OnSpawnedPiece(int pieceId);
	void RefillWithSevenBag(void);	//2個連続で同じ形にしない。ランダム
	bool IsInvalidMoveAt(int type, int dir, int posX, int posY);
	void ShowGhostPiece(void);		//テトリミノのゴーストを表示
	void DropTetromino(void);		//テトリミノ落下
	void NEXT_tetrominoDraw(void);	//次のテトリミノ表示
	//void BuildAIGhostForCurrent();	//AIの最適解ゴースト
	void MoveTetromino(int dirX);	//テトリミノ左右移動
	void RotateTetromino(void);		//テトリミノ回転
	void DeleteTetromino(void);		//テトリミノ消去
	void CheckGameOver(void);		//ゲームオーバーか確認
	bool IsInvalidMove(int dirX, int dirY);
									//テトリミノを動かすことができるか
	void ShiftTetrimino(int dirX, int DirY);
									//m_dataを更新してテトリミノを移動させる

public:
	void Init(HWND hWnd);	//初期化
	void Update(void);		//更新
	void Draw();			//描画
	void Uninit();			//終了
};*/
