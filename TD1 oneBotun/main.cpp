#include <Novice.h>
#define _USE_MATH_DEFINES
#include <math.h>

const char kWindowTitle[] = "防人の冒険記";

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
	float jumpPower;
	bool isJumping;
};

void Jump(Charactor& player)
{
	if (!player.isJumping) {
		player.velocity = -player.jumpPower;
		player.isJumping = true; // ジャンプ中にする
	}
}

void ApplyGravity(Charactor& player)
{
	player.velocity += player.gravity;
	player.pos.y += player.velocity;
    //(Y座標600を地面とした場合)
	if (player.pos.y >= 535.0f) {
		player.pos.y = 535.0f;
		player.isJumping = false; // 地面に着地したのでジャンプ状態を解除
		player.velocity = 0.0f;   // 着地時に速度をリセット
	}
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
	player.pos.y = 535.0f;
	player.velocity = 0.0f;
	player.gravity = 0.8f;
	player.height = 64.0f;
	player.wide = 64.0f;
	player.jumpPower = 20.0f;
	player.isJumping = false;
	player.speed = 10.0f;

	int ground = Novice::LoadTexture("./Resources/ground.png");

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
		
		if (keys[DIK_A]) {
			player.pos.x -= player.speed;
		}
		if (keys[DIK_D]) {
			player.pos.x += player.speed;
		}

		// スペースキーが押されたらジャンプ
		if (keys[DIK_SPACE] && preKeys[DIK_SPACE] == 0)
		{
			Jump(player);
		}

		// 重力の適用
		ApplyGravity(player);

		///
		/// ↑更新処理ここまで
		///
		
		/// 
		/// ↓描画処理ここから
		/// 
		
		Novice::DrawBox(int(player.pos.x), int(player.pos.y), int(player.wide), int(player.height), 0.0f, WHITE, kFillModeSolid);
		Novice::DrawSprite(0, 0, ground, 1, 1, 0.0f,WHITE);

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
