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
	int canShotTime; // isCanShotをtrueにするためのtime
	int hp;
	bool isJumping;
	bool isCanShot;  // 弾が撃てるようになるフラグ
	bool isHit;
	bool isAlive;
	int respawnTime; // リスポーンまでの時間を管理する変数
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

void BulletMove() //弾が移動する関数
{
	for (int i = 0; i < 10; i++)
	{
		if (playerBullet[i].isBullet)
		{
			playerBullet[i].pos.x += playerBullet[i].speed; //右方向に飛んでいく

			// 弾が画面外に出たら消す（画面サイズに合わせて条件を設定）
			if (playerBullet[i].pos.x > 1280) {
				playerBullet[i].isBullet = false; // 弾を無効化
			}
		}
	}
}

void BulletShot(Charactor& player) // 弾の描画するための関数
{
	if (player.shotCoolTime <= 0)
	{
		for (int i = 0; i < 10; i++)
		{
			if (!playerBullet[i].isBullet) // 発射可能な弾がある場合
			{
				playerBullet[i].isBullet = true;
				playerBullet[i].pos.x = player.pos.x;
				playerBullet[i].pos.y = player.pos.y;
				player.shotCoolTime = 10; // クールタイムを設定
				break;
			}
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

float HitJudgeBullet(Charactor &enemy, Bullet &bullet) //弾との当たり判定
{
	return sqrtf((bullet.pos.x - enemy.pos.x) * (bullet.pos.x - enemy.pos.x) + (bullet.pos.x - enemy.pos.y) * (bullet.pos.x - enemy.pos.y));
}

// 弾と敵の当たり判定
bool BulletHitEnemy(Bullet& bullet, Charactor& enemy)
{
	if (bullet.isBullet && enemy.isAlive) {
		// 距離計算（簡易な矩形当たり判定）
		if (bullet.pos.x > enemy.pos.x - enemy.wide / 2 &&
			bullet.pos.x < enemy.pos.x + enemy.wide / 2 &&
			bullet.pos.y > enemy.pos.y - enemy.height / 2 &&
			bullet.pos.y < enemy.pos.y + enemy.height / 2) {
			return true;
		}
	}
	return false;
}

// 敵のリスポーン処理
void RespawnEnemy(Charactor& enemy)
{
	if (!enemy.isAlive) {
		enemy.respawnTime--;
		if (enemy.respawnTime <= 0) {
			enemy.isAlive = true;
			enemy.pos.x = 1280.0f; // 再登場する位置
			enemy.pos.y = 600.0f - enemy.height; // 地面に配置
		}
	}
}

void MoveEnemy(Charactor& enemy)
{
	if (enemy.isAlive) {
		if (!enemy.isJumping) {
			Jump(enemy); // 敵がジャンプを開始
		}
		enemy.pos.x -= enemy.speed; // 左に移動
		ApplyGravity(enemy);

		// 画面外に出たらリセット
		if (enemy.pos.x + enemy.wide < 0) {
			enemy.pos.x = 1280.0f; // 画面右側に戻す
			enemy.pos.y = 600.0f - enemy.height;
		}
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
	int ground = Novice::LoadTexture("./Resources/ground.png");
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
        playerBullet[i].isBullet = false;
    }

	// 敵の初期化
	Charactor enemy;
	enemy.pos.x = 1280.0f; // 画面外から登場
	enemy.pos.y = 600.0f - 64.0f; // 地面に配置
	enemy.velocity = 0.0f;
	enemy.gravity = 0.8f;
	enemy.height = 64.0f;
	enemy.wide = 64.0f;
	enemy.jumpPower = 20.0f;
	enemy.isJumping = false;
	enemy.isAlive = true;
	enemy.speed = 3.0f;

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

		if (keys[DIK_D]) {
			player.pos.x += player.speed;
		}
		if (keys[DIK_A]) {
			player.pos.x -= player.speed;
		}
		
		// スペースキーが押されたらジャンプ（1回目のスペース）
		if (keys[DIK_SPACE] && !preKeys[DIK_SPACE])
		{
			if (!player.isJumping) {
				Jump(player);  // 最初のジャンプ
			}
			else if (player.isCanShot) { // ジャンプ中にもう一度スペースキーが押された場合
				BulletShot(player);  // 弾の発射
			}
		}

		if (player.isJumping) //ジャンプ中の処理
		{
			player.canShotTime++;

			// 空中で弾を撃てるようになるまでの時間
			if (player.canShotTime >= 10)
			{
				player.isCanShot = true;
			}
		}
		else
		{
			player.canShotTime = 0;
			player.isCanShot = false;
		}

		// 敵の動き
		MoveEnemy(enemy);

		// 敵のリスポーン処理
		RespawnEnemy(enemy);

		// 弾と敵の当たり判定処理
		for (int i = 0; i < 10; i++)
		{
			if (BulletHitEnemy(playerBullet[i], enemy)) {
				playerBullet[i].isBullet = false; // 弾を消す
				enemy.isAlive = false;  // 敵を倒す
				enemy.respawnTime = 120; // 120フレーム後にリスポーン
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
            if (playerBullet[i].isBullet)
            {
                Novice::DrawEllipse(int(playerBullet[i].pos.x), int(playerBullet[i].pos.y), 10, 10, 0.0f, BLUE, kFillModeSolid);
            }
        }
		if (enemy.isAlive) {
			Novice::DrawBox(int(enemy.pos.x - enemy.wide / 2), int(enemy.pos.y - enemy.height / 2), int(enemy.wide), int(enemy.height), 0.0f, RED, kFillModeSolid);
		}
		Novice::DrawEllipse(static_cast<int>(king.pos.x), static_cast<int>(king.pos.y), 1, 1, 0.0f, BLUE, kFillModeSolid);
		Novice::DrawSprite(0, 600, ground, 1, 1, 0.0f, WHITE);

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