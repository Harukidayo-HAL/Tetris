#pragma once
#include "Renderer.h"
#include "Texture.h"//テクスチャ読み込み




enum FacingDirection
{
	front,
	back,
	left,
	right
};

enum class EnemyState
{
	Idle,
	Attack,
	Hit,
	Dead,
	Reviving
};

class Object {
private:
	// 頂点データを決める
	Vertex m_vertexList[4] =
	{

		//	x		y	 z	   r	g	 b	  a		u	 v
		{ -0.5f, 0.5f, 0.5f ,1.0f,1.0f,1.0f,1.0f,0.0f,0.0f},  // 0番目の頂点座標　{ x, y, z }
		{  0.5f, 0.5f, 0.5f ,1.0f,1.0f,1.0f,1.0f,1.0f,0.0f},  // 1番目の頂点座標
		{ -0.5f, -0.5f,0.5f ,1.0f,1.0f,1.0f,1.0f,0.0f,1.0f},  // 2番目の頂点座標
		{  0.5f, -0.5f,0.5f ,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f},  // 3番目の頂点座標　

	};


	

	//座標
	DirectX::XMFLOAT3 m_pos = { 0.0f,0.0f,0.0f };
	//大きさ
	DirectX::XMFLOAT3 m_size = { 100.0f,100.0f,0.0f };
	//角度
	float m_angle = 0.0f;
	//色
	DirectX::XMFLOAT4 m_color = { 1.0f,1.0f,1.0f,1.0f };

	// 頂点バッファ
	ID3D11Buffer* m_pVertexBuffer;
	//テクスチャ用変数
	ID3D11ShaderResourceView* m_pTextureView;

	//テクスチャが縦横に何分割されているか
	int m_splitX = 1;
	int m_splitY = 1;

	

public:
	//左上から何段目を切り抜いて表示する
	float numU = 0;
	float numV = 0;

	int animCounter = 0;//アニメーション制御
	bool animForward = true;

	//向いている向き確認
	FacingDirection m_facing;

	bool keypress = false;

	float m_hp = 0;

	EnemyState m_state = EnemyState::Idle;

	int m_maxFrame = 10;	//現在のアニメにおけるフレーム数
	int m_animSpeed = 5;	//何カウントで次のフレームに進むか
	

	HRESULT Init(const char* imgname,int sx = 1,int sy = 1);	//初期化
	void Draw();						//描画
	void Uninit();						//終了
	void SetPos(float X, float y, float z);//座標をセット
	void SetSize(float x, float y, float z);//大きさをセット
	void SetAngle(float a);					//角度をセット
	void SetColor(float r, float g, float b, float a);//色をセット			//向きを変えてアニメーション

	DirectX::XMFLOAT3 GetPos(void);		//座標をゲット
	DirectX::XMFLOAT3 GetSize(void);	//大きさをゲット
	float GetAngle(void);				//角度をゲット
	DirectX::XMFLOAT4 GetColor(void);	//色をゲット
};
