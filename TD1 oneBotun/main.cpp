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
	int shotCoolTime; //弾が発射されるクールタイム
	int canShotTime; //　isCanShotをtrueにするためのtime
	int hp;
	int flameCount; // アニメーション
	int flame; // アニメーション
	int revivalTime; // isHItがfalseの時にtrueにするタイマー
	bool isJumping;
	bool isCanShot; // 弾が撃てるようになるフラグ
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
    bool isBullet;
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
			if (!playerBullet[i].isBullet) //描画する
			{
				playerBullet[i].isBullet = true;
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
		if (playerBullet[i].isBullet)
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

float HitJudge(Vector2 &a, Vector2 &b) //body同士の当たり判定の関数
{
	return sqrtf((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

float HitJudgeBullet(Charactor &enemy, Bullet &bullet) //弾との当たり判定
{
	return sqrtf((bullet.pos.x - enemy.pos.x) * (bullet.pos.x - enemy.pos.x) + (bullet.pos.x - enemy.pos.y) * (bullet.pos.x - enemy.pos.y));
}

void MoveAnimation(int &animetionFlameCount, int &flameNumber, int flameSheets) //画像に切り替わりの変数
{
	animetionFlameCount++;
	flameNumber = (animetionFlameCount / 10) % flameSheets;

	if (animetionFlameCount >= flameSheets * 10)
	{
		animetionFlameCount = 0;
	}
}


int zikkenAnimationFlameCount = 0;
int zikkenFlameNumber = 0;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    // ライブラリの初期化
    Novice::Initialize(kWindowTitle, 1280, 720);

    // キー入力結果を受け取る箱
    char keys[256] = { 0 };
    char preKeys[256] = { 0 };

#pragma region 
	int groundGraph = Novice::LoadTexture("./Resources/ground.png"); //地面
	int backGroundGraph = Novice::LoadTexture("./Resources/forestBackGround.png"); // 背景
	int kingGraph = Novice::LoadTexture("./Resources/king.png"); // 王様
	int flyingEnemyGraph = Novice::LoadTexture("./Resources/flyingEnemy.png"); // 飛んでいる敵
	int warkingEnemyGraph = Novice::LoadTexture("./Resources/workjngEnemy.png"); // 歩いてる敵	 
	int playerGraph = Novice::LoadTexture("./Resources/Player.png");  // プレイヤー
	int playerBulletGraph = Novice::LoadTexture("./Resources/yari.png"); // プレイヤーの弾
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
	player.isCanShot = false;
	player.speed = 3.0f;
	player.shotCoolTime = 10;
	player.canShotTime = 0;
	player.isAlive = true;
	player.isHit = false;
	player.hp = 5;

    for (int i = 0; i < 10; i++)
    {
        playerBullet[i].pos.x = player.pos.x;
        playerBullet[i].pos.y = player.pos.y;
        playerBullet[i].height = 32.0f;
        playerBullet[i].wide = 32.0f;
        playerBullet[i].radius = 32.0f;
        playerBullet[i].speed = 10.0f;  // 弾の速度を設定
        playerBullet[i].isHit = false;
        playerBullet[i].isBullet = false;
    }

	int playerFlameCount = 0;
	int playerFlame = 0;

	//プレイヤーの後ろにいる王様の初期化変数
 	Charactor king; 
	king.pos.x = 30.0f;
	king.pos.y = 536.0f;
	king.radius = 32.0f;
	king.velocity = 0.0f;
	king.height = 64.0f;
	king.wide = 64.0f;
	king.speed = 3.0f;
	king.hp = 3;
	king.isAlive = true;
	king.isHit = false;

	Charactor walkingEnemy[5];
	for (int i = 0; i < 5; i++)//地面を歩く敵
	{
		walkingEnemy[i].pos.x = 300.0f;
		walkingEnemy[i].pos.y = 536.0f;
		walkingEnemy[i].radius = 32.0f;
		walkingEnemy[i].speed = 5.0f;
		walkingEnemy[i].hp = 1;
		walkingEnemy[i].isAlive = true;
		walkingEnemy[i].isHit = false;
		walkingEnemy[i].revivalTime = 0;
		walkingEnemy[i].flameCount = 0;
		walkingEnemy[i].flame = 0;
	}

	Charactor flyingEnemy[5];
	for (int i = 0; i < 5; i++)
	{
		flyingEnemy[i].pos.x = 400.0f;
		flyingEnemy[i].pos.y = 400.0f;
		flyingEnemy[i].isAlive = true;
		flyingEnemy[i].radius = 32.0f;
		flyingEnemy[i].hp = 1;
		flyingEnemy[i].isHit = false;
		flyingEnemy[i].revivalTime = 0;
		flyingEnemy[i].flameCount = 0;
		flyingEnemy[i].flame = 0;
	}

	int backGraundPosX = 0;

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
		
		// スペースキーが押されたらジャンプ
		if (keys[DIK_SPACE] && !preKeys[DIK_SPACE])
		{
			Jump(player);
		}		

		//次の弾が発射されるまでのクールタイム
		if (player.shotCoolTime >= 0)
		{
			player.shotCoolTime--;
		}
		
		if (player.isJumping) //jumpしているときの処理
		{
			player.canShotTime++;

			if (player.canShotTime >= 10)
			{
				player.isCanShot = true;
			}
			else
			{
				player.isCanShot = false;
			}
		}
		else
		{
			player.canShotTime = 0;
		}

		

		if (player.isCanShot) //弾が発射できるようになった時
		{
			if (player.shotCoolTime <= 0)
			{
				if (keys[DIK_SPACE])
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

		MoveAnimation(playerFlameCount, playerFlame, 6);

		
		for (int i = 0; i < 1; i++)
		{
			if (flyingEnemy[i].isAlive)
			{
				MoveAnimation(flyingEnemy[i].flameCount, flyingEnemy[i].flame, 4);
			}

			if (walkingEnemy[i].isAlive)
			{
				MoveAnimation(walkingEnemy[i].flameCount, walkingEnemy[i].flame, 4);
			}
		}

		///
		/// ↑更新処理ここまで
		///
		
		/// 
		/// ↓描画処理ここから
		/// 
		Novice::DrawSprite(backGraundPosX, 0, backGroundGraph, 1, 1, 0.0f, WHITE);

		Novice::DrawSprite(600, 600, groundGraph, 1, 1, 0.0f, WHITE);

		for (int i = 0; i < 5; i++)
		{
			if (player.isAlive && !flyingEnemy[i].isAlive && !walkingEnemy[i].isAlive)
			{
				Novice::DrawSpriteRect(int(player.pos.x - player.wide / 2), int(player.pos.y - player.height / 2), playerFlame * 64, 0, 64, 64, playerGraph, 1.0f / 6.0f, 1.0f, 0.0f, WHITE);
			}
			else
			{
				Novice::DrawSpriteRect(int(player.pos.x - player.wide / 2), int(player.pos.y - player.height / 2), 0, 0, 64, 64, playerGraph, 1.0f / 6.0f, 1.0f, 0.0f, WHITE);
			}
		}
		

		

		if (king.isAlive)
		{
			Novice::DrawSprite(int(king.pos.x - king.wide / 2), int(king.pos.y - king.height / 2), kingGraph, 1, 1, 0.0f, WHITE);
		}

        for (int i = 0; i < 10; i++)
        {
            if (playerBullet[i].isBullet)
            {
				Novice::DrawSprite(int(playerBullet[i].pos.x), int(playerBullet[i].pos.y), playerBulletGraph, 1, 1, 0.0f, WHITE);
            }
        }

		for (int i = 0; i < 5; i++)
		{
			if (flyingEnemy[i].isAlive)
			{
				Novice::DrawSpriteRect(int(flyingEnemy[i].pos.x - flyingEnemy[i].radius), int(flyingEnemy[i].pos.y - flyingEnemy[i].radius), flyingEnemy[i].flame * 64, 0, 64, 64, flyingEnemyGraph, 1.0f / 4.0f, 1.0f, 0.0f, WHITE);
			}
		}
		
		for (int i = 0; i < 5; i++)
		{
			if (walkingEnemy[i].isAlive)
			{
				Novice::DrawSpriteRect(int(walkingEnemy[i].pos.x - walkingEnemy[i].radius), int(walkingEnemy[i].pos.y - walkingEnemy[i].radius), walkingEnemy[i].flame * 64, 0, 64, 64, warkingEnemyGraph, 1.0f / 4.0f, 1.0f, 0.0f, WHITE);
			}

			Novice::DrawEllipse(int(walkingEnemy[i].pos.x), int(walkingEnemy[i].pos.y), 32, 32, 0.0f, RED, kFillModeSolid);
			Novice::DrawEllipse(int(flyingEnemy[i].pos.x), int(flyingEnemy[i].pos.y), 32, 32, 0.0f, RED, kFillModeSolid);
		}

		
		
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
