#include <Novice.h>
#define _USE_MATH_DEFINES
#include <math.h>

const char kWindowTitle[] = "GC1B_14_ヨコヤマ_ショウタ_タイトル";

struct Vector2
{
	float x;
	float y;
};

struct Charactor
{
	Vector2 pos;
	float speed;
	float radius;
	float wide;
	float height;
	float velocity;
	float gravity;
	float jump;
};

float length = 0.0f;

void PlayerMove(float y, float speed)
{
	y -= speed;
}

void Jump(float y, float jump)
{
	y -= jump;
}

void Grabity(float velocity, float gravity, float posy)
{
	velocity += gravity;
	posy += velocity;
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	//プレイヤーの初期化変数
	Charactor player;
	player.pos.x = 64.0f;
	player.pos.y = 600.0f;
	player.velocity = 0.0f;
	player.gravity = 0.8f;
	player.height = 64.0f;
	player.wide = 64.0f;
	player.jump = 20.0f;

	//int isSpacePushCount = 0;

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///
		if (keys[DIK_SPACE] && preKeys[DIK_SPACE] ==0)
		{
			Jump(player.pos.y, player.jump);
		}
		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		
		Novice::DrawBox(static_cast<int>(player.pos.x), int(player.pos.y), static_cast<int>(player.wide), static_cast<int>(player.height), 0.0f, WHITE, kFillModeSolid);

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
