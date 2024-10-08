#include <Novice.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>
#include <stdlib.h>

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
	int shotCoolTime;
	int hp;
	bool isJumping;
	bool isHit;
	bool isAlive;
};

struct Bullet
{
    Vector2 pos;
    float speed;        // 弾の速度を保持
    float wide;
    float height;
    float radius;
    bool isHit;
    bool isShot;
    float shotCoolTime;
};

void Jump(Charactor& player)
{
    if (!player.isJumping) {
        player.velocity = -player.jumpPower;
        player.isJumping = true; // ジャンプ中にする
    }
}


Bullet playerBullet[10];

void BulletShot(Charactor& player) // 弾の描画するための関数
{
	if (player.shotCoolTime <= 0)
	{
		for (int i = 0; i < 10; i++)
		{
			if (!playerBullet[i].isShot) //描画する
			{
				playerBullet[i].isShot = true;
				 playerBullet[i].pos.x = player.pos.x;
				playerBullet[i].pos.y = player.pos.y;
				player.shotCoolTime = 10;
				break;
			}
		}
	}
}

void BulletMove() //弾が移動する関数
{
	for (int i = 0; i < 10; i++)
	{
		if (playerBullet[i].isShot)
		{
			playerBullet[i].pos.x += playerBullet[i].speed; //右方向に飛んでいく
		}
	}
}

void ApplyGravity(Charactor& player)
{
    player.velocity += player.gravity;
    player.pos.y += player.velocity;
    //(Y座標600を地面とした場合)
	if (player.pos.y + player.height >= 600.0f) {
		player.pos.y = 600.0f - player.height;
		player.isJumping = false; // 地面に着地したのでジャンプ状態を解除
		player.velocity = 0.0f;   // 着地時に速度をリセット
	}
}

float HitJudge(Vector2 a, Vector2 b) //body同士の当たり判定の関数
{
	return sqrtf((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

float HitJudgeBullet(Vector2 a) //弾との当たり判定
{
	for (int i = 0; i < 10; i++)
	{
		return sqrtf((playerBullet[i].pos.x - a.x) * (playerBullet[i].pos.x - a.x) + (playerBullet[i].pos.y - a.y) * (playerBullet[i].pos.y - a.y));
	}
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    // ライブラリの初期化
    Novice::Initialize(kWindowTitle, 1280, 720);

    // キー入力結果を受け取る箱
    char keys[256] = { 0 };
    char preKeys[256] = { 0 };

#pragma region 
	//int ground = Novice::LoadTexture("./Resources/ground.png");
#pragma endregion 画像の導入

#pragma region 

	//プレイヤーの初期化変数
	Charactor player;
	player.pos.x = 100.0f;
	player.pos.y = 600.0f;
	player.velocity = 0.0f;
	player.gravity = 0.8f;
	player.height = 64.0f;
	player.wide = 64.0f;
	player.jumpPower = 20.0f;
	player.isJumping = false;
	player.speed = 3.0f;
	player.shotCoolTime = 10;
	player.isAlive = true;
	player.isHit = false;
	player.hp = 3;

    for (int i = 0; i < 10; i++)
    {
        playerBullet[i].pos.x = player.pos.x;
        playerBullet[i].pos.y = player.pos.y;
        playerBullet[i].height = 32.0f;
        playerBullet[i].wide = 32.0f;
        playerBullet[i].radius = 32.0f;
        playerBullet[i].speed = 10.0f;  // 弾の速度を設定
        playerBullet[i].isHit = false;
        playerBullet[i].isShot = false;
        playerBullet[i].shotCoolTime = 20;
    }

	//プレイヤーの後ろにいる王様の初期化変数
 	Charactor king; 
	king.pos.x = 10.0f;
	king.pos.y = 536.0f;
	king.radius = 32.0f;
	king.velocity = 0.0f;
	king.height = 64.0f;
	king.wide = 64.0f;
	king.speed = 3.0f;
	king.hp = 3;
	king.isAlive = true;
	king.isHit = false;

	//float scrollX = 0.0f;
	//float scrollMax = 3840.0f;

	Charactor moveEnemy; //地面を歩く敵
	moveEnemy.pos.x = 0.0f;
	moveEnemy.pos.y = 0.0f;
	moveEnemy.radius = 32.0f;
	moveEnemy.speed = 5.0f;
	moveEnemy.hp = 1;
	moveEnemy.height = 16.0f;
	moveEnemy.wide = 16.0f;
	moveEnemy.isAlive = false;
	moveEnemy.isHit = false;

	Charactor flyingEnemy; //空飛んでる敵
	flyingEnemy.pos.x = 0.0f;
	flyingEnemy.pos.y = 0.0f;
	flyingEnemy.radius = 32.0f;
	flyingEnemy.speed = 5.0f;
	flyingEnemy.height = 16.0f;
	flyingEnemy.wide = 16.0f;

#pragma endregion 変数の初期化

	enum MAPCHANGE
	{
		TITLE,
		STAGE1,
		STAGE2,
		STAGE3,
	};


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
		
		//次の弾が発射されるまでのクールタイム
		if (player.shotCoolTime >= 0)
		{
			player.shotCoolTime--;
		}
		
		// スペースキーが押されたらジャンプ
		if (keys[DIK_SPACE] && !preKeys[DIK_SPACE])
		{
			Jump(player);		

			if (player.shotCoolTime <= 0)
			{
				if (keys[DIK_SPACE] && !preKeys[DIK_SPACE])
				{
					//弾の描画
					BulletShot(player);
				}
			}
		}



		//弾の描画後の移動
		BulletMove();
			
		// 重力の適用
		ApplyGravity(player);

		///
		/// ↑更新処理ここまで
		///
		
		/// 
		/// ↓描画処理ここから
		/// 
		
		if (player.isAlive)
		{
			Novice::DrawBox(int(player.pos.x - player.wide / 2), int(player.pos.y - player.height / 2), int(player.wide), int(player.height), 0.0f, WHITE, kFillModeSolid);
		}

		if (king.isAlive)
		{
			Novice::DrawBox(int(king.pos.x - king.wide / 2), int(king.pos.y - king.height / 2), int(king.wide), int(king.height), 0.0f, RED, kFillModeSolid);
		}

        for (int i = 0; i < 10; i++)
        {
            if (playerBullet[i].isShot)
            {
                Novice::DrawEllipse(int(playerBullet[i].pos.x), int(playerBullet[i].pos.y), 10, 10, 0.0f, BLUE, kFillModeSolid);
            }
        }

		Novice::DrawEllipse(static_cast<int>(king.pos.x), static_cast<int>(king.pos.y), 1, 1, 0.0f, BLUE, kFillModeSolid);

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
