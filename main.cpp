#undef UNICODE  // Unicodeではなく、マルチバイト文字を使う
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include "Game.h"



// マクロ定義
#define CLASS_NAME   "DX21Smpl"// ウインドウクラスの名前
#define WINDOW_NAME  "DirectX初期化"// ウィンドウの名前

// 関数のプロトタイプ宣言
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#include <d3d11.h>  // DirectX11を使うためのヘッダーファイル

#define SCREEN_WIDTH (640)	// ウインドウの幅
#define SCREEN_HEIGHT (480)	// ウインドウの高さ

// 関数のプロトタイプ宣言
HRESULT RendererInit(HWND hwnd);
void    RendererDraw();
void    RendererUninit();



#include <d3dcompiler.h>


//--------------------------------------------------------------------------------------
// エントリポイント＝一番最初に実行される関数
//--------------------------------------------------------------------------------------
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	// ウィンドウクラス情報をまとめる
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_CLASSDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = CLASS_NAME;
	wc.hIconSm = NULL;

	RegisterClassEx(&wc);

	// ウィンドウの情報をまとめる
	HWND hWnd;
	hWnd = CreateWindowEx(0,	// 拡張ウィンドウスタイル
		CLASS_NAME,				// ウィンドウクラスの名前
		WINDOW_NAME,			// ウィンドウの名前
		WS_OVERLAPPEDWINDOW,	// ウィンドウスタイル
		CW_USEDEFAULT,			// ウィンドウの左上Ｘ座標
		CW_USEDEFAULT,			// ウィンドウの左上Ｙ座標 
		SCREEN_WIDTH,			// ウィンドウの幅
		SCREEN_HEIGHT,			// ウィンドウの高さ
		NULL,					// 親ウィンドウのハンドル
		NULL,					// メニューハンドルまたは子ウィンドウID
		hInstance,				// インスタンスハンドル
		NULL);					// ウィンドウ作成データ

	//ウィンドウのサイズを修正
	RECT rc1, rc2;
	GetWindowRect(hWnd, &rc1);	//ウィンドウの短径領域を取得
	GetClientRect(hWnd, &rc2);	//クライアントの短径領域を取得
	int sx = SCREEN_WIDTH;
	int sy = SCREEN_HEIGHT;
	sx += ((rc1.right - rc1.left) - (rc2.right - rc2.left));
	sy += ((rc1.bottom - rc1.top) - (rc2.bottom - rc2.top));
	SetWindowPos(hWnd, NULL, 0, 0, sx, sy, (SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE));
													//ウィンドウサイズを変更

	// 指定されたウィンドウの表示状態を設定(ウィンドウを表示)
	ShowWindow(hWnd, nCmdShow);
	// ウィンドウの状態を直ちに反映(ウィンドウのクライアント領域を更新)
	UpdateWindow(hWnd);

	//ゲーム初期化
	Game game;
	game.Init(hWnd);

	// ゲームループに入る前にDirectXの初期化をする
	
	//Object player;
	


	MSG msg;

	//　FPS計測用変数
	int fpsCounter = 0;
	long long oldTick = GetTickCount64(); //前回計測時の時間
	long long nowTick = oldTick;//今回計測時の時間

	//FPS固定用変数
	LARGE_INTEGER liWork;	//workがつく変数は作業用変数
	long long frequency;	//どれくらい細かく時間をカウント
	QueryPerformanceFrequency(&liWork);
	frequency = liWork.QuadPart;
	//時間（単位：カウント）取得
	QueryPerformanceFrequency(&liWork);
	long long oldCount = liWork.QuadPart;//前回計測時の時間
	long long nowCount = oldCount;//今回計測時の時間

	// ゲームループ
	while (1)
	{
		// 新たにメッセージがあれば
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// ウィンドウプロシージャにメッセージを送る
			DispatchMessage(&msg);

			// 「WM_QUIT」メッセージを受け取ったらループを抜ける
			if (msg.message == WM_QUIT) {
				break;
			}
		}
		else
		{
			QueryPerformanceCounter(&liWork);//現在時間を取得
			nowCount = liWork.QuadPart;
			//1/60秒が経過したか？
			if (nowCount >= oldCount + frequency / 60)
			{
				//ゲーム処理実行
				game.Update();
				game.Draw();

				fpsCounter++;//ゲーム処理を実行したら+1する
				oldCount = nowCount;
			}
			

			nowTick = GetTickCount64();//現在時間を取得
			//前回計測から1000ミリ秒が経過したか？
			if (nowTick >= oldTick + 1000)
			{
				//FPS表示
				char str[32];
				wsprintfA(str, "FPS=%d", fpsCounter);
				SetWindowTextA(hWnd, str);
				//カウンターリセット
				fpsCounter = 0;
				oldTick = nowTick;
			}
		}
	}
	//ゲーム終了
	game.Uninit();

	UnregisterClass(CLASS_NAME, hInstance);

	return (int)msg.wParam;
}

//--------------------------------------------------------------------------------------
//ウィンドウプロシージャ
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static bool isFullscreen = false;
	static bool isMessageBoxShowed = false;
	switch (uMsg)
	{
	case WM_DESTROY:// ウィンドウ破棄のメッセージ
		PostQuitMessage(0);// 「WM_QUIT」メッセージを送る　→　アプリ終了
		break;

	case WM_CLOSE:  // 「x」ボタンが押されたら
	{
		int res = MessageBoxA(NULL, "終了しますか？", "確認", MB_OKCANCEL);
		if (res == IDOK) {
			DestroyWindow(hWnd);  // 「WM_DESTROY」メッセージを送る
		}
	}
	break;

	case WM_KEYDOWN: //キー入力があったメッセージ
		if (LOWORD(wParam) == VK_ESCAPE)
		{ //入力されたキーがESCAPEなら
			PostMessage(hWnd, WM_CLOSE, wParam, lParam);//「WM_CLOSE」を送る
		}

		else if (LOWORD(wParam) == VK_F11)
		{
			isFullscreen = !isFullscreen;
			if (isFullscreen) {
				//疑似フルスクリーンモードに変更
				SetWindowLongPtr(hWnd, GWL_STYLE, WS_POPUP | WS_MINIMIZEBOX);//ウィンドウ枠を削除
				//ディスプレイ解像度を取得
				int screenWidth = GetSystemMetrics(SM_CXSCREEN);
				int screenHeight = GetSystemMetrics(SM_CYSCREEN);
				SetWindowPos(hWnd, HWND_TOP, 0, 0, screenWidth, screenHeight, SWP_FRAMECHANGED | SWP_SHOWWINDOW);
			}
			else {
				//通常ウィンドウに戻す
				SetWindowLongPtr(hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);//ウィンドウ枠を戻す
				SetWindowPos(hWnd, HWND_TOP, 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SWP_FRAMECHANGED | SWP_SHOWWINDOW);

			}
		}
		break;
	case WM_ACTIVATE:
		if (wParam == WA_INACTIVE) {
			//フルスクリーン表示かつメッセージボックス非表示なら
			if (isFullscreen && !isMessageBoxShowed)
			{
				//ウィンドウを最小化する（タスク切替時に背後に残る問題対策）
				ShowWindow(hWnd, SW_MINIMIZE);
			}
		}
		//標準挙動を実行
		return DefWindowProc(hWnd, uMsg, wParam, lParam);


	case WM_SIZE://ウィンドウサイズに変更があったメッセージ
		if (wParam != SIZE_MINIMIZED)
		{
			int width = LOWORD(lParam);//横幅
			int height = HIWORD(lParam);//縦幅
			ResizeWindow(width, height);
		}
		break;

	default:
		// 受け取ったメッセージに対してデフォルトの処理を実行
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
		break;
	}

	return 0;
}

