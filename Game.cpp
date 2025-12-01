#include "Game.h"
#include <time.h>
void Game::Init(HWND hWnd)
{
	srand((unsigned)time(NULL));

	RendererInit(hWnd);			//DirectXを初期化

	sound.Init();	//サウンドを初期化
	sound.Play(SOUND_LABEL_BGM000);//BGMを再生

	m_player.Init("asset/char01.png", 3, 4);//プレイヤーを初期化
	m_player.SetPos(100.0f, 0.0f, 0.0f);//位置を特定
	m_player.SetSize(100.0f, 100.0f, 0.0f);//大きさを設定
	m_player.SetAngle(0.0f);//角度を設定
	m_player.SetColor(1.0f, 1.0f, 1.0f, 1.0f);	//色を設定
	m_player.animCounter = 0;
	m_player.animForward = true;
	m_player.keypress = false;
	m_player.m_facing = front;

	m_manual.Init("asset/setumeisyo.png");
	m_manual.SetPos(0.0f, 0.0f, 0.0f);	//位置を初期化
	m_manual.SetSize(256.0f, 384.0f, 0.0f);//大きさを設定
	m_manual.SetAngle(0.0f);			   //角度を設定

	m_enemy.Init("asset/_Idle.png", 10, 1);
	m_enemy.SetPos(-100.0f, -120.0f, 0.0f);//位置を特定
	m_enemy.SetSize(150.0f, 150.0f, 0.0f);//大きさを設定
	m_enemy.SetColor(1.0f, 1.0f, 1.0f, 1.0f);	//色を設定
	m_enemy.SetAngle(0.0f);//角度を設定
	m_enemy.keypress = false;
	m_enemy.animCounter = 0;

	m_enemy_2.Init("asset/Skeleton Idle.png", 11, 1);
	m_enemy_2.SetPos(-50.0f, -160.0f, 0.0f);//位置を特定
	m_enemy_2.SetSize(80.0f, 80.0f, 0.0f);//大きさを設定
	m_enemy_2.SetColor(1.0f, 1.0f, 1.0f, 1.0f);	//色を設定
	m_enemy_2.SetAngle(0.0f);//角度を設定
	m_enemy_2.keypress = false;
	m_enemy_2.animCounter = 0;
	m_enemy_2.m_hp = 5;

	m_background.Init("asset/tetrisBackground.png");		//背景を初期化
	m_background.SetPos(0.0f, 0.0f, 0.0f);	//位置を初期化
	m_background.SetSize(640.0f, 480.0f, 0.0f);//大きさを設定
	m_background.SetAngle(0.0f);			   //角度を設定

	m_taiyou.Init("asset/taiyou.png", 4, 3);
	m_taiyou.SetPos(-220.f, 130.0f, 0.0f);
	m_taiyou.SetSize(180.0f, 200.0f, 0.0f);//大きさを設定
	m_taiyou.SetAngle(0.0f);			   //角度を設定
	m_taiyou.animCounter = 0;

	m_aiGhost.assign(kW, std::vector<int>(kH, 0));
	m_data.assign(kW, std::vector<int>(kH, 0));
	m_typeData.assign(kW, std::vector<int>(kH, -1)); // 種類は -1 で初期化
	for (int i = 0; i < NEXT_QUEUE_SIZE; i++)
	{
		for (int x = 0; x < 4; x++)
		{
			for (int y = 0; y < 4; y++)
			{
				m_nextBlocks[i][x][y].Init("asset/block.png");
				m_nextBlocks[i][x][y].SetSize(20.0f, 20.0f, 0.0f);//フィールドより小さく表示
			}
		}
	}


	InitHoldTetromino();
	InitNextQueue();

	for (int x = 0; x < STAGE_X; x++) {
		for (int y = 0; y < STAGE_Y; y++) {
			m_blocks[x][y].Init("asset/block.png");//テトリミノを初期化
			m_blocks[x][y].SetPos(BLOCK_SIZE * (x - STAGE_X / 2), -BLOCK_SIZE * (y - STAGE_Y / 2), 0.0f);//位置を設定
			m_blocks[x][y].SetSize(BLOCK_SIZE, BLOCK_SIZE, 0.0f);//大きさを設定
			//m_typeData[x][y] = -1; // 空白
		}
	}

	m_scoreboard.Init("asset/number.png", 10, 1);//得点表示を初期化
	m_scoreboard.SetPos(-170.0f, -80.0f, 0.0f);//位置を設定
	m_scoreboard.SetSize(40.0f, 40.0f, 0.0f);  //大きさを設定
	m_score = 0;//得点を初期化

	m_title.Init("asset/tetris_title.png");	//タイトル表示を初期化
	m_title.SetPos(0.0f, 0.0f, 0.0f);//位置を設定
	m_title.SetSize(640.0f, 480.0f, 0.0f);//大きさを設定
	m_scene = 0;//シーンを初期化


}

void Game::Update(void)
{
	input.Update();

	switch (m_scene)
	{
	case 0: //タイトル画面
		//キー入力で本編へ
		if (input.GetKeyTrigger(VK_RETURN))
		{
			m_scene = 1;

			//data配列初期化
			for (int x = 0; x < kW; x++)
			{
				for (int y = 0; y < kH; y++)
				{
					m_data[x][y] = 0;
				}
			}
			m_state = 0; //ゲームの状態を初期化
			m_score = 0; //得点を初期化
		}
		break;
	case 1://ゲーム本編

		//ゲームオーバーならゲームの進行を止める
		if (m_state == 3)
		{
			//キー入力でタイトル画面へ戻る
			if (input.GetKeyTrigger(VK_RETURN))
			{
				m_scene = 0;
			}
			return;
		}

		//新たにテトリミノを出現させる
		if (m_state == 0)
		{
			//ゲームの状態を「1：落下中」に変更
			m_state = 1;

			m_currentType = m_nextQueue[0];
			m_currentDir = 0;//向きをリセット

			//キューを1つ詰める
			for (int i = 0; i < NEXT_QUEUE_SIZE - 1; i++)
			{
				m_nextQueue[i] = m_nextQueue[i + 1];
			}

			//次のテトリミノ補充
			PushOneFromSevenBag();

			// 出したミノをm_lastPieceに格納
			OnSpawnedPiece(m_currentType);

			for (int x = 0; x < 4; x++)
			{
				for (int y = 0; y < 4; y++)
				{
					if (m_tetrominoData[m_currentType][m_currentDir][y][x] == 2) {
						m_data[x + 4][y] = 2; // 落下中の状態
						m_typeData[x + 4][y] = m_currentType; // テトリミノの種類
					}
				}
			}

			m_count = 0;

			//落下中のテトリミノの位置をリセット
			m_currentX = 4;
			m_currentY = 0;
		}


		//テトリミノを落とす処理
		m_count++;
		if (m_count > 30)
		{
			m_count = 0;

			//揃っていれば
			if (m_state == 2)
			{
				//テトリミノ消去
				DeleteTetromino();

				//ゲームの状態を「0：落下するものがない状態」に変更
				m_state = 0;
			}

			//テトリミノ落下
			DropTetromino();
		}


		//左移動
		if (input.GetKeyTrigger(VK_LEFT))
		{
			MoveTetromino(-1);
		}

		//右移動
		if (input.GetKeyTrigger(VK_RIGHT))
		{
			MoveTetromino(1);
		}

		//回転
		if (input.GetKeyTrigger(VK_UP))
		{
			RotateTetromino();
		}

		// 下キーを押した瞬間にハードドロップ
		if (input.GetKeyPress(VK_DOWN))
		{
			m_count = 30;
		}

		//Cキーでホールド
		if (input.GetKeyTrigger(VK_SHIFT) && m_state == 1 && !m_holdUsed)
		{
			HoldTetromino();
		}

		if (input.GetKeyTrigger(VK_SPACE))
		{
			while (!IsInvalidMove(0, 1))
			{
				ShiftTetrimino(0, 1); // 1マスずつ落とす
			}
			sound.Play(SOUND_LABEL_SE_DROP);
			// 最後に着地処理を呼ぶ
			DropTetromino();

		}

		// 色を付ける
		for (int x = 0; x < STAGE_X; x++)
		{
			for (int y = 0; y < STAGE_Y; y++)
			{
				int data = m_data[x][y + 3];
				int type = m_typeData[x][y + 3];

				// まずAI最適解ゴーストを最優先で表示（やや黄緑・半透明など）
				if (m_ghostData[x][y + 3] == 1)
				{
					Color c = tetrominoColors[m_currentType];
					m_blocks[x][y].SetColor(c.r, c.g, c.b, 0.35f); // プレイヤーのゴースト
					
				}
				else if (m_aiGhost[x][y + 3] == 1)
				{
					m_blocks[x][y].SetColor(0.6f, 1.0f, 0.6f, 0.5f); // AI提案（薄い緑）
				}
				else if (data == 0)
				{
					m_blocks[x][y].SetColor(1.0f, 1.0f, 1.0f, 0.2f); // 空白
				}
				else if (type >= 0 && type < 7)
				{
					Color c = tetrominoColors[type];
					float alpha = 1.0f;
					if (data == 2) alpha = 1.0f;
					else if (data == 3) c = { 0.4f, 0.4f, 1.0f };
					else if (data == 4) c = { 1.0f, 0.2f, 0.2f };
					m_blocks[x][y].SetColor(c.r, c.g, c.b, alpha);
				}
			}
		}
		BuildAIGhostForCurrent();
		break;
	}


}

void Game::Draw(void)
{
	RendererDrawStart();	//描画開始

	switch (m_scene)
	{
	case 0: //タイトル画面
		m_title.Draw();		//タイトル描画
		if (input.GetKeyPress(VK_S))
		{
			m_manual.Draw();
		}
		break;

	case 1://ゲーム本編
		m_background.Draw();	//背景を描画

		ShowGhostPiece();		//テトリミノのゴーストを表示
		HoldTetrominoDraw();	//ホールドテトリミノ表示
		//テトリミノを表示
		for (int x = 0; x < STAGE_X; x++)
		{
			for (int y = 0; y < STAGE_Y; y++)
			{
				m_blocks[x][y].Draw();
			}
		}

		NEXT_tetrominoDraw();//次のテトリミノ表示

		//スコアを表示
		DirectX::XMFLOAT3 pos = m_scoreboard.GetPos();
		DirectX::XMFLOAT3 size = m_scoreboard.GetSize();
		int keta = 0;
		do
		{
			m_scoreboard.numU = m_score % (int)pow(10, keta + 1) / (int)pow(10, keta);//一桁を切り出す
			m_scoreboard.SetPos(pos.x - 20 * keta, pos.y, pos.z);//位置を設定
			m_scoreboard.Draw();//描画
			keta++;
		} while (m_score >= (int)pow(10, keta));
		m_scoreboard.SetPos(pos.x, pos.y, pos.z);//位置を元に戻す
		break;
	}



	RendererDrawEnd();		//描画終了
}

void Game::Uninit(void)
{
	m_player.Uninit();		//プレイヤーを終了
	m_enemy.Uninit();		//エネミーを終了
	m_background.Uninit();	//背景を終了
	m_taiyou.Uninit();		//太陽を終了
	m_enemy_2.Uninit();		//エネミー２を終了

	//テトリミノを終了
	for (int x = 0; x < STAGE_X; x++)
	{
		for (int y = 0; y < STAGE_Y; y++)
		{
			m_blocks[x][y].Uninit();

		}
	}
	m_scoreboard.Uninit();//スコアを終了
	m_title.Uninit();	//タイトル終了
	sound.Uninit();		//サウンドを終了
	RendererUninit();		//DirectXを終了
}

//テトリミノ落下
void Game::DropTetromino(void)
{
	//落とすことが可能か調べる
	bool ngFg = IsInvalidMove(0, 1);

	//落とすことが不可能なら
	if (ngFg == true)
	{
		//着地
		for (int x = 0; x < STAGE_X; x++)
		{
			for (int y = 0; y < kH; y++)
			{
				if (m_data[x][y] == 2)
				{
					m_data[x][y] = 1;
				}
			}
		}
		m_holdUsed = false;
		//ゲームの状態を「0：落下するものがない状態」に変更
		m_state = 0;

		//1列揃った行が無いかチェックする
		for (int y = 0; y < kH; y++)
		{
			bool completeFg = true;
			for (int x = 0; x < STAGE_X; x++)
			{
				if (m_data[x][y] == 0)
				{
					completeFg = false;
					break;
				}
			}
			//揃っていれば色をつける
			if (completeFg == true)
			{
				for (int x = 0; x < STAGE_X; x++)
				{
					m_data[x][y] = 3;//消去待ち
				}

				//ゲームの状態を「2：揃った状態」に変更
				m_state = 2;
			}
		}
		//ゲームオーバーになっていないかチェックする
		CheckGameOver();
	}


	//落とすことが可能なら
	else
	{
		ShiftTetrimino(0, 1);//移動させる
	}
}

//テトリミノ左右移動
void Game::MoveTetromino(int dirX)
{
	//落下中でなければ無視
	if (m_state != 1)return;

	//左右移動が可能か調べる
	bool ngFg = IsInvalidMove(dirX, 0);

	//左右移動が可能なら
	if (ngFg == false)
	{
		sound.Play(SOUND_LABEL_SE_SHIFT);
		ShiftTetrimino(dirX, 0);//移動させる
	}
}


//テトリミノ回転
void Game::RotateTetromino(void)
{
	//落下中でなければ無視
	if (m_state != 1)return;

	//向きを変更する
	int newDir = m_currentDir + 1;
	if (newDir > 3) { newDir = 0; }

	//回転が可能か調べる
	bool ngFg = false;
	for (int x = 0; x < 4; x++)
	{
		for (int y = 0; y < 4; y++)
		{
			if (m_tetrominoData[m_currentType][newDir][y][x] != 0)
			{
				int px = m_currentX + x;
				int py = m_currentY + y;

				//範囲外はNG
				if (px < 0 || px >= STAGE_X || py < 0 || py >= STAGE_Y + 3)
				{
					ngFg = true;
					break;
				}

				//自分自身（2）以外のブロックと重なっている場合はNG
				if (m_data[px][py] != 0 && m_data[px][py] != 2)
				{
					ngFg = true;
					break;
				}
			}
		}
		if (ngFg) break;
	}

	//回転が可能なら
	if (ngFg == false)
	{
		sound.Play(SOUND_LABEL_SE_KAITEN);
		//消す
		for (int x = 0; x < STAGE_X; x++)
		{
			for (int y = 0; y < kH; y++)
			{
				if (m_data[x][y] == 2)
				{
					m_data[x][y] = 0;
					m_typeData[x][y] = -1;
				}
			}
		}
		//回転後の配置
		for (int x = 0; x < 4; x++)
		{
			for (int y = 0; y < 4; y++)
			{
				if (m_tetrominoData[m_currentType][newDir][y][x] != 0)
				{
					int px = m_currentX + x;
					int py = m_currentY + y;

					m_data[px][py] = 2;
					m_typeData[px][py] = m_currentType;//←これが大事
				}
			}
		}
		m_currentDir = newDir;//方向を更新



	}
}

//テトリミノ消去
void Game::DeleteTetromino(void)
{
	sound.Play(SOUND_LABEL_SE_1);
	//1行揃った行が無いかチェックする
	int cnt = 0;	//消した列数をカウントする変数
	for (int cy = STAGE_Y + 2; cy >= 0;)
	{
		if (m_data[0][cy] == 3)
		{//消去待ちなら
			for (int y = cy - 1; y >= 0; y--)
			{
				for (int x = 0; x < STAGE_X; x++)
				{
					m_data[x][y + 1] = m_data[x][y];
					m_typeData[x][y + 1] = m_typeData[x][y]; // ←追加！
				}
			}
			cnt++;
		}
		else
		{
			cy--;
		}
	}
	//得点を加算する
	switch (cnt)
	{
	case 1: m_score += 100; break; //シングル（1列消し）
	case 2: m_score += 300; break; //ダブル（2列消し）
	case 3: m_score += 500; break; //トリプル（3列消し）
	case 4: m_score += 800; break; //テトリス（4列消し）
	}
}

//ゲームオーバーになっていないかチェック
void Game::CheckGameOver(void)
{
	//ゲームオーバーになっていないかチェックする
	for (int tx = 0; tx < STAGE_X; tx++)
	{//一番上の列をチェック
		if (m_data[tx][3] == 1)
		{
			//テトリミノの色を変える
			for (int x = 0; x < STAGE_X; x++)
			{
				for (int y = 0; y < kH; y++)
				{
					if (m_data[x][y] == 1)
					{
						m_data[x][y] = 4;
					}
				}
			}

			//ゲームの状態を「3：ゲームオーバー」に変更
			m_state = 3;
			break;
		}

	}
}

//テトリミノを移動が無効かどうか確認
bool Game::IsInvalidMove(int dirX, int dirY)
{
	//落とすことが可能か調べる
	bool ngFg = false;
	for (int x = 0; x < STAGE_X; x++)
	{
		for (int y = 0; y < STAGE_Y + 3; y++)
		{
			if (m_data[x][y] == 2 && (x + dirX < 0 || x + dirX >= STAGE_X || y + dirY >= STAGE_Y + 3 || m_data[x + dirX][y + dirY] == 1))
			{
				ngFg = true;
				break;
			}
		}
		if (ngFg == true) { break; }
	}
	return ngFg;
}


//m_dataを更新してテトリミノを移動させる
void Game::ShiftTetrimino(int dirX, int dirY)
{

	int tmp[STAGE_X][STAGE_Y + 3] = {};
	int tmpType[STAGE_X][STAGE_Y + 3];
	for (int x = 0; x < STAGE_X; x++)
		for (int y = 0; y < STAGE_Y + 3; y++)
			tmpType[x][y] = -1;


	for (int x = 0; x < STAGE_X; x++)
	{
		for (int y = 0; y < STAGE_Y + 3; y++)
		{
			if (m_data[x][y] == 2)
			{
			 tmp[x + dirX][y + dirY] = 2;
			 tmpType[x + dirX][y + dirY] = m_typeData[x][y];
			}
			else if (m_data[x][y] == 1)
			{
				tmp[x][y] = 1;
				tmpType[x][y] = m_typeData[x][y];
			}
		}
	}

	for (int x = 0; x < STAGE_X; x++)
	{
		for (int y = 0; y < STAGE_Y + 3; y++)
		{
			m_data[x][y] = tmp[x][y];
			m_typeData[x][y] = tmpType[x][y];
		}
	}

	m_currentX += dirX;
	m_currentY += dirY;
}

//ゴーストピース描画
void Game::ShowGhostPiece()
{
	/*//配列を初期化
	for (int x = 0; x < STAGE_X; x++)
	{
		for (int y = 0; y < STAGE_Y + 3; y++)
		{
			m_ghostData[x][y] = 0;
		}
	}*/

	m_ghostData.assign(kW, std::vector<int>(kH, 0));

	//現在の位置を基準にしてゴーストを落とす
	int ghostX = m_currentX;
	int ghostY = m_currentY;
	int dir = m_currentDir;
	int type = m_currentType;


	while (!IsInvalidMoveAt(type, dir, ghostX, ghostY + 1))
	{
		ghostY++;
	}

	//ゴーストを配置
	for (int x = 0; x < 4; x++)
	{
		for (int y = 0; y < 4; y++)
		{
			if (m_tetrominoData[type][dir][y][x] != 0)
			{
				int px = ghostX + x;
				int py = ghostY + y;


				//ゴースト用の番号を入れる
				if (m_data[px][py] == 0)
				{
					m_ghostData[px][py] = 1;//ゴーストあり
				}
			}
		}
	}
}

void Game::HoldTetrominoDraw()
{
	if (m_scene == 1 && m_holdType != -1)
	{
		for (int x = 0; x < 4; x++)
		{
			for (int y = 0; y < 4; y++)
			{
				if (m_tetrominoData)
				{


					if (m_tetrominoData[m_holdType][0][y][x] != 0)
					{
						float drawX = -240.0f + x * 20.0f; // 画面左側に配置
						float drawY = 150.0f - y * 20.0f;

						m_holdBlocks[x][y].SetPos(drawX, drawY, 0.0f);
						Color c = tetrominoColors[m_holdType];
						m_holdBlocks[x][y].SetColor(c.r, c.g, c.b, 1.0f);
						m_holdBlocks[x][y].Draw();
					}
				}

			}
		}
	}
}

void Game::InitHoldTetromino()
{
		m_holdType = -1;
		m_holdUsed = false;
		m_currentType = -1;
		//Init()内に追加
		for (int x = 0; x < 4; x++)
		{
			for (int y = 0; y < 4; y++)
			{
				m_holdBlocks[x][y].Init("asset/block.png");
				m_holdBlocks[x][y].SetSize(20.0f, 20.0f, 0.0f);
			}
		}
}

void Game::HoldTetromino()
{
	sound.Play(SOUND_LABEL_SE_SHIFT);//任意の効果音

	if (m_holdType == -1)//初ホールド
	{
		m_holdType = m_currentType;

		//次のミノを出す
		m_currentType = m_nextQueue[0];
		m_currentDir = 0;
		for (int i = 0; i < NEXT_QUEUE_SIZE - 1; i++)
		{
			m_nextQueue[i] = m_nextQueue[i + 1];
		}
		PushOneFromSevenBag();
	}
	else //ミノ交換
	{
		std::swap(m_holdType, m_currentType);
		m_currentDir = 0;
	}

	//現在のミノを初期位置に再配置
	for (int x = 0; x < STAGE_X; x++)
	{
		for (int y = 0; y < STAGE_Y + 3; y++)
		{
			if (m_data[x][y] == 2)
			{
				m_data[x][y] = 0;
				m_typeData[x][y] = -1;
			}
		}
	}

	m_currentX = 4;
	m_currentY = 0;
	for (int x = 0; x < 4; x++)
	{
		for (int y = 0; y < 4; y++)
		{
			if (m_tetrominoData[m_currentType][0][y][x] != 0)
			{
				m_data[x + m_currentX][y + m_currentY] = 2;
				m_typeData[x + m_currentX][y + m_currentY] = m_currentType;
			}
		}
	}

	m_holdUsed = true;
}

void Game::InitNextQueue()
{
	RefillWithSevenBag();
	for (int i = 0; i < NEXT_QUEUE_SIZE; i++)
	{
		m_nextQueue[i] = m_bag[m_bagPos++];
	}
}

void Game::PushOneFromSevenBag()
{
	if (m_bagPos >= 7)
	{
		RefillWithSevenBag();
	}
	m_nextQueue[NEXT_QUEUE_SIZE - 1] = m_bag[m_bagPos++];
}

void Game::OnSpawnedPiece(int pieceId)
{
	m_lastPiece = pieceId;
}

void Game::RefillWithSevenBag(void)
{
	m_bag = { 0,1,2,3,4,5,6 };
	std::shuffle(m_bag.begin(), m_bag.end(), m_rng.rng);

	int boundary = (NEXT_QUEUE_SIZE >= 2) ? m_nextQueue[NEXT_QUEUE_SIZE - 2] : m_lastPiece;

	if (boundary != -1 && m_bag[0] == boundary)
	{
		for (int i = 1; i < 7; ++i)
		{
			if (m_bag[i] != boundary && m_bag[0] == boundary)
			{
				for (int i = 1; i < 7; i++)
				{
					if (m_bag[i] != boundary)
					{
						std::swap(m_bag[0], m_bag[i]); break;
					}
				}

			}
		}
	}
	m_bagPos = 0;

}

bool Game::IsInvalidMoveAt(int type, int dir, int posX, int posY)
{
	for (int x = 0; x < 4; x++)
	{
		for (int y = 0; y < 4; y++)
		{
			if (m_tetrominoData[type][dir][y][x] != 0)
			{
				int px = posX + x;
				int py = posY + y;

				if (px < 0 || px >= STAGE_X || py < 0 || py >= STAGE_Y + 3)
					return true;

				if (m_data[px][py] != 0 && m_data[px][py] != 2)//自分自身はOK
					return true;

			}
		}
	}
	return false;
}

void Game::NEXT_tetrominoDraw(void)
{
	for (int i = 0; i < NEXT_QUEUE_SIZE; i++)
	{
		int type = m_nextQueue[i];

		//テトリミノの形状データをもとに描画
		for (int x = 0; x < 4; x++)
		{
			for (int y = 0; y < 4; y++)
			{
				if (m_tetrominoData[type][0][y][x] != 0)//0は空白
				{
					//表示位置（右側に縦に並べる）
					float drawX = 200.0f + x * 20.0f;
					float drawY = 125.0f - i * 100.0f - y * 20.0f; //縦に並べる

					m_nextBlocks[i][x][y].SetPos(drawX, drawY, 0.0f);

					Color c = tetrominoColors[type];
					m_nextBlocks[i][x][y].SetColor(c.r, c.g, c.b, 1.0f);
					m_nextBlocks[i][x][y].Draw(); // 上から塗り直す

				}
			}
		}

	}

}
// -----------------------------
// AI最適解のゴースト（改良版：行消去優先）
// -----------------------------
void Game::BuildAIGhostForCurrent()
{
	// 1. クリア
	for (int x = 0; x < STAGE_X; ++x)
	{
		for (int y = 0; y < STAGE_Y + 3; ++y)
		{
			m_aiGhost[x][y] = 0;
		}
	}

	// 2. 状態チェック
	if (m_state != 1) return;
	if (m_currentType < 0) return;

	// 3. base を作る（固定済みブロックのみ）
	int base[STAGE_X][STAGE_Y + 3] = {};
	for (int x = 0; x < STAGE_X; ++x)
	{
		for (int y = 0; y < STAGE_Y + 3; ++y)
		{
			base[x][y] = (m_data[x][y] == 1) ? 1 : 0;
		}
	}

	// canPlace
	auto canPlace = [&](int type, int dir, int px, int py) -> bool
	{
		for (int x = 0; x < 4; ++x)
		{
			for (int y = 0; y < 4; ++y)
			{
				if (m_tetrominoData[type][dir][y][x] == 0) continue;
				int gx = px + x;
				int gy = py + y;
				if (gx < 0 || gx >= STAGE_X || gy < 0 || gy >= STAGE_Y + 3) return false;
				if (base[gx][gy] == 1) return false;
			}
		}
		return true;
	};

	auto hardDropY = [&](int type, int dir, int px) -> int
	{
		int py = 0;
		while (canPlace(type, dir, px, py + 1)) ++py;
		if (!canPlace(type, dir, px, py)) return -1;
		return py;
	};

	auto simulatePlaceAndClear = [&](int type, int dir, int px, int py, int outGrid[STAGE_X][STAGE_Y + 3]) -> int
	{
		for (int x = 0; x < STAGE_X; ++x)
			for (int y = 0; y < STAGE_Y + 3; ++y)
				outGrid[x][y] = base[x][y];

		for (int x = 0; x < 4; ++x)
		{
			for (int y = 0; y < 4; ++y)
			{
				if (m_tetrominoData[type][dir][y][x] != 0)
				{
					int gx = px + x, gy = py + y;
					if (gx >= 0 && gx < STAGE_X && gy >= 0 && gy < STAGE_Y + 3)
						outGrid[gx][gy] = 1;
				}
			}
		}

		int lines = 0;
		for (int cy = STAGE_Y + 2; cy >= 0;)
		{
			bool full = true;
			for (int x = 0; x < STAGE_X; ++x)
			{
				if (outGrid[x][cy] == 0) { full = false; break; }
			}
			if (full)
			{
				for (int y = cy - 1; y >= 0; --y)
				{
					for (int x = 0; x < STAGE_X; ++x)
						outGrid[x][y + 1] = outGrid[x][y];
				}
				for (int x = 0; x < STAGE_X; ++x) outGrid[x][0] = 0;
				++lines;
			}
			else
			{
				--cy;
			}
		}
		return lines;
	};

	auto evaluateGrid = [&](int grid[STAGE_X][STAGE_Y + 3], int linesCleared) -> double
	{
		int heights[STAGE_X] = {};
		for (int x = 0; x < STAGE_X; ++x)
		{
			int h = 0;
			for (int y = STAGE_Y + 2; y >= 0; --y)
			{
				if (grid[x][y]) { h = y + 1; break; }
			}
			heights[x] = h;
		}

		int aggregateHeight = 0;
		int maxHeight = 0;
		for (int x = 0; x < STAGE_X; ++x)
		{
			aggregateHeight += heights[x];
			if (heights[x] > maxHeight) maxHeight = heights[x];
		}

		int holes = 0;
		for (int x = 0; x < STAGE_X; ++x)
		{
			bool seen = false;
			for (int y = 0; y < STAGE_Y + 3; ++y)
			{
				if (grid[x][y]) seen = true;
				else if (seen) ++holes;
			}
		}

		int bumpiness = 0;
		for (int x = 0; x < STAGE_X - 1; ++x)
			bumpiness += std::abs(heights[x] - heights[x + 1]);

		const double wLines = 1000.0;
		const double wHeight = -4.5;
		const double wHoles = -7.0;
		const double wBumpiness = -3.5;
		const double wMaxHeight = -1.0;

		double score = 0.0;
		score += wLines * linesCleared;
		score += wHeight * aggregateHeight;
		score += wHoles * holes;
		score += wBumpiness * bumpiness;
		score += wMaxHeight * maxHeight;
		return score;
	};

	double bestScore = -1e300;
	int bestX = m_currentX, bestY = m_currentY, bestDir = m_currentDir;
	int bestLines = -1;
	const int type = m_currentType;

	const double centerX = (STAGE_X - 1) / 2.0;
	const double EPS = 1e-6;

	for (int dir = 0; dir < 4; ++dir)
	{
		for (int px = -4; px <= STAGE_X + 1; ++px)
		{
			// 上段で置けるか確認
			if (!canPlace(type, dir, px, 0)) continue;
			int py = hardDropY(type, dir, px);
			if (py < 0) continue;

			int sim[STAGE_X][STAGE_Y + 3];
			int lines = simulatePlaceAndClear(type, dir, px, py, sim);
			double sc = evaluateGrid(sim, lines);

			bool shouldUpdate = false;
			if (lines > bestLines)
			{
				shouldUpdate = true; // 行数が多いものを最優先
			}
			else if (lines == bestLines)
			{
				if (sc > bestScore + EPS)
				{
					shouldUpdate = true; // 同じ行数ならスコアで比較
				}
				else if (std::abs(sc - bestScore) <= EPS)
				{
					// スコアも同じなら中央に近い方を優先
					double curCenterDist = std::abs(px - centerX);
					double bestCenterDist = std::abs(bestX - centerX);
					if (curCenterDist < bestCenterDist)
					{
						shouldUpdate = true;
					}
				}
			}

			if (shouldUpdate)
			{
				bestLines = lines;
				bestScore = sc;
				bestX = px; bestY = py; bestDir = dir;
			}
		}
	}

	// 9. 結果を書き込む
	m_aiBestX = bestX; m_aiBestY = bestY; m_aiBestDir = bestDir;

	// 盤面に AI ゴーストを描く（空きマスにだけ）
	for (int x = 0; x < 4; ++x)
	{
		for (int y = 0; y < 4; ++y)
		{
			if (m_tetrominoData[type][bestDir][y][x] != 0)
			{
				int gx = bestX + x;
				int gy = bestY + y;
				if (gx >= 0 && gx < STAGE_X && gy >= 0 && gy < STAGE_Y + 3)
				{
					if (m_data[gx][gy] == 0) m_aiGhost[gx][gy] = 1;
				}
			}
		}
	}
}

