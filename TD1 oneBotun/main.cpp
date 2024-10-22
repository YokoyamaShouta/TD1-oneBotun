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
	float hitX;
	float hitY;
	int shotCoolTime;
	int canShotTime; // isCanShotをtrueにするためのtime
	int hp;
	bool isJumping;
	bool isCanShot;  // 弾が撃てるようになるフラグ
	bool isHit;
	bool isAlive;
	bool isSmash; // つぶされた時のフラグ
	int respawnTime; // リスポーンまでの時間を管理する変数
	int flameCount;
	int flame;
};

struct Box
{
	Vector2 rightLine;
	Vector2 leftLine;
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
	if (player.pos.y + player.height / 2 >= 600.0f) {
		player.pos.y = 600.0f - player.height / 2;
		player.isJumping = false; // 地面に着地したのでジャンプ状態を解除
		player.velocity = 0.0f;   // 着地時に速度をリセット
	}
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

	//プレイヤーの初期化変数
	Charactor player;
	player.pos.x = 100.0f;
	player.pos.y = 568.0f;
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
	player.respawnTime = 0;

    for (int i = 0; i < 10; i++)
    {
        playerBullet[i].pos.x = player.pos.x;
        playerBullet[i].pos.y = player.pos.y;
        playerBullet[i].height = 32.0f;
        playerBullet[i].wide = 32.0f;
        playerBullet[i].radius = 16.0f;
        playerBullet[i].speed = 10.0f;  // 弾の速度を設定
        playerBullet[i].isHit = false;
        playerBullet[i].isBullet = false;
    }

	Box playerBox;
	playerBox.rightLine.x = 0;
	playerBox.rightLine.y = 0;
	playerBox.leftLine.x = 0;
	playerBox.leftLine.y = 0;

	int playerFlameCount = 0;
	int playerFlame = 0;

	int playerGraph = Novice::LoadTexture("./Resources/Player.png");  // プレイヤー
	int playerBulletGraph = Novice::LoadTexture("./Resources/yari.png"); // プレイヤーの弾

	int playerScore = 0;

	//プレイヤーの後ろにいる王様の初期化変数
 	Charactor king; 
	king.pos.x = 30.0f;
	king.pos.y = 568.0f;
	king.radius = 32.0f;
	king.velocity = 0.0f;
	king.height = 64.0f;
	king.wide = 64.0f;
	king.speed = 3.0f;
	king.isAlive = true;
	king.isHit = false;

	int kingGraph = Novice::LoadTexture("./Resources/king.png"); // 王様

	//地面を歩く敵
	Charactor walkingEnemy[2];
	for (int i = 0; i < 2; i++)
	{
		walkingEnemy[i].pos.x = 1290.0f;
		walkingEnemy[i].pos.y = 500.0f;
		walkingEnemy[i].wide = 64.0f;
		walkingEnemy[i].height = 64.0f;
		walkingEnemy[i].radius = 32.0f;
		walkingEnemy[i].speed = 2.0f;
		walkingEnemy[i].isAlive = false;
		walkingEnemy[i].isHit = false;
		walkingEnemy[i].isSmash = false;
		walkingEnemy[i].respawnTime = 0;
		walkingEnemy[i].flameCount = 0;
		walkingEnemy[i].flame = 0;
	}

	Charactor smashWalkEnemy[2];
	for (int i = 0; i < 2; i++)
	{
		smashWalkEnemy[i].flame = 0;
		smashWalkEnemy[i].flameCount = 0;
	}

	Box walkingBox[2];
	for (int i = 0; i < 2; i++)
	{
		walkingBox[i].rightLine.x = 0;
		walkingBox[i].rightLine.y = 0;
		walkingBox[i].leftLine.x = 0;
		walkingBox[i].leftLine.y = 0;
	}	
	
	int walkingEnemyAppearanceTime = 0;

	int warkingEnemyGraph = Novice::LoadTexture("./Resources/walkingEnemy.png"); // 歩いてる敵	 
	int warkingEnemySmashGraph = Novice::LoadTexture("./Resources/walkingEnemySmash.png"); // 踏みつぶした 
	

	//空を飛ぶ敵
	Charactor flyingEnemy[2];
	for (int i = 0; i < 2; i++)
	{
		flyingEnemy[i].pos.x = 1300.0f;
		flyingEnemy[i].pos.y = 0.0f;
		flyingEnemy[i].speed = 3.0f;
		flyingEnemy[i].isAlive = false;
		flyingEnemy[i].radius = 32.0f;
		flyingEnemy[i].wide = 64.0f;
		flyingEnemy[i].height = 64.0f;
		flyingEnemy[i].isHit = false;
		flyingEnemy[i].respawnTime = 0;
		flyingEnemy[i].flameCount = 0;
		flyingEnemy[i].flame = 0;
	}

	Bullet flyingBullet[2];
	for (int i = 0; i < 2; i++)
	{
		flyingBullet[i].isBullet = false;
		flyingBullet[i].pos.x = 0.0f;
		flyingBullet[i].pos.y = 0.0f;
		flyingBullet[i].height = 64.0f;
		flyingBullet[i].wide = 64.0f;
	}
	


	int flyingEnemyAppearanceTime = 0;

	int flyingEnemyGraph = Novice::LoadTexture("./Resources/flyingEnemy.png"); // 飛んでいる敵
	int flyingEggGraph = Novice::LoadTexture("./Resources/Egg.png");

	//敵が生きていないときのグラグ　
	bool allEnemyIsAlve = false;

	//画面　切り替え
	enum MAPCHANGE
	{
		TITLE,
		GAMEPLAY,
		GAMECLEAR,
		GAMEEND,
	};

	MAPCHANGE scene = TITLE;

	float backGraundPosX = 0.0f;

	int groundGraph = Novice::LoadTexture("./Resources/ground.png"); //地面
	int backGroundGraph = Novice::LoadTexture("./Resources/forestBackGround.png"); // 背景
	int gameTitleGraph = Novice::LoadTexture("./Resources/title.png"); // タイトル 
	int gameClearGraph = Novice::LoadTexture("./Resources/Crear.png"); // ゲームクリア
	int gameoverGraph = Novice::LoadTexture("./Resources/Game_Over.png"); // ゲームオーバー

	unsigned int randam = static_cast<int>(time(nullptr));
	srand(randam);
	
#pragma endregion 変数の初期化

	

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

		switch (scene)
		{
		case TITLE:

			

			if (keys[DIK_SPACE] && !preKeys[DIK_SPACE])
			{
				scene = GAMEPLAY;
			}
			break;

		case GAMEPLAY:

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
					
			//弾が発射できるようになった時
			if (player.isCanShot) 
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

			//場外に出たら弾が撃てるようになる
			for (int i = 0; i < 10; i++)
			{
				if (playerBullet[i].pos.x >= 1280)
				{
					playerBullet[i].isBullet = false;
				}
			}

			//敵の出現

			for (int i = 0; i < 2; i++) // 歩く敵
			{
				if (!walkingEnemy[i].isAlive)
				{
					walkingEnemyAppearanceTime++;
				}

				if (walkingEnemyAppearanceTime >= 200)
				{
					walkingEnemy[i].isAlive = true;
					walkingEnemy[i].pos.x = 1300.0f;
					walkingEnemy[i].pos.y = 568.0f;
					walkingEnemyAppearanceTime = 0;
					break;
				}
			}


			for (int i = 0; i < 2; i++) // 空を飛ぶ敵
			{
				if (!flyingEnemy[i].isAlive)
				{
					flyingEnemyAppearanceTime++;
				}

				if (flyingEnemyAppearanceTime >= 150)
				{
					flyingEnemy[i].isAlive = true;
					flyingEnemy[i].pos.x = 1300.0f;
					flyingEnemy[i].pos.y = static_cast<float>(rand() % 110 + 348);
					flyingEnemyAppearanceTime = 0;
					break;
				}
			}

			// 敵の移動処理

			// 歩いてる敵		
			for (int i = 0; i < 2; i++)
			{
				if (walkingEnemy[i].isAlive)
				{
					walkingEnemy[i].pos.x -= walkingEnemy[i].speed;
				}
			}
			
			// 飛んでる敵
			for (int i = 0; i < 2; i++)
			{
				if (flyingEnemy[i].isAlive)
				{
					flyingEnemy[i].pos.x -= flyingEnemy[i].speed;

					if (flyingEnemy[i].pos.x <= player.pos.x && flyingEnemy[i].pos.x >= player.pos.x - 20 && !flyingBullet[i].isBullet)
					{
						flyingBullet[i].isBullet = true;
						flyingBullet[i].pos.x = flyingEnemy[i].pos.x;
						flyingBullet[i].pos.y = flyingEnemy[i].pos.y;
						break;
					}
				}
			}
			
			for (int i = 0; i < 2; i++)
			{
				if (flyingBullet[i].isBullet)
				{
					flyingBullet[i].pos.y += 5.0f;
				}
			}
			

			//弾と敵の当たり判定
			for (int i = 0; i < 10; i++) 
			{
				for (int j = 0; j < 2; j++)
				{
					if (walkingEnemy[j].isAlive && playerBullet[i].isBullet)
					{
						if (playerBullet[i].pos.x - 32.0f <= walkingEnemy[j].pos.x + 32.0f &&
							walkingEnemy[j].pos.x - 32.0f <= playerBullet[i].pos.x + 32.0f &&
							playerBullet[i].pos.y - 2.0f <= walkingEnemy[j].pos.y + 32.0f &&
							walkingEnemy[j].pos.y - 32.0f <= playerBullet[i].pos.y + 5.0f)
						{
							playerBullet[i].isBullet = false;
							walkingEnemy[j].isAlive = false;
							playerScore += 1;
						}
					}
				}
				
				for (int j = 0; j < 2; j++)
				{
					if (flyingEnemy[i].isAlive && playerBullet[i].isBullet)
					{
						if (playerBullet[i].pos.x - 32.0f <= flyingEnemy[j].pos.x + 32.0f &&
							flyingEnemy[j].pos.x - 32.0f <= playerBullet[i].pos.x + 32.0f &&
							playerBullet[i].pos.y - 2.0f <= flyingEnemy[j].pos.y + 32.0f &&
							flyingEnemy[j].pos.y - 32.0f <= playerBullet[i].pos.y + 5.0f)
						{
							playerBullet[i].isBullet = false;
							flyingEnemy[j].isAlive = false;
							playerScore += 1;
						}
					}				
				}				
			}

			// 踏みつけた時の処理

			playerBox.rightLine.x = player.pos.x + player.wide / 2;
			playerBox.rightLine.y = player.pos.y - player.height / 2;
			playerBox.leftLine.x = player.pos.x - player.wide / 2;
			playerBox.leftLine.y = player.pos.y + player.wide / 2;

			for (int i = 0; i < 2; i++)
			{
				walkingBox[i].rightLine.x = walkingEnemy[i].pos.x + walkingEnemy[i].wide / 2;
				walkingBox[i].rightLine.y = walkingEnemy[i].pos.y - walkingEnemy[i].height / 2;
				walkingBox[i].leftLine.x = walkingEnemy[i].pos.x - walkingEnemy[i].wide / 2;
				walkingBox[i].leftLine.y = walkingEnemy[i].pos.y + walkingEnemy[i].height / 2;
				
				if (player.isJumping && walkingEnemy[i].isAlive && walkingBox[i].leftLine.x < playerBox.rightLine.x && walkingBox[i].rightLine.x > playerBox.leftLine.x && walkingBox[i].rightLine.y < playerBox.leftLine.y && walkingBox[i].leftLine.y > playerBox.rightLine.y)
				{
					playerScore += 1;
					walkingEnemy[i].isSmash = true;
					walkingEnemy[i].isAlive = false;
				}			
				
				if (walkingEnemy[i].isAlive && walkingBox[i].leftLine.x < playerBox.rightLine.x && walkingBox[i].rightLine.x > playerBox.leftLine.x && player.isAlive && !player.isJumping)
				{
					player.hp -= 1;
					player.isAlive = false;
				}
			}

			//tamgaonoatari
			for (int i = 0; i < 2; i++)
			{
				if (flyingBullet[i].pos.y >= 568 && player.isAlive && flyingBullet[i].isBullet && flyingEnemy[i].isAlive )
				{
					player.isAlive = false;
					player.hp -= 1;
					flyingBullet[i].isBullet = false;
				}
			}

			if (!player.isAlive)
			{
				player.respawnTime++;
			}

			if (player.respawnTime >= 30)
			{
				player.isAlive = true;
			}
	
			// 重力の適用
			ApplyGravity(player);

			//アニメーション置き場
			MoveAnimation(playerFlameCount, playerFlame, 6);

			for (int i = 0; i < 2; i++)
			{
				if (flyingEnemy[i].isAlive)
				{
					MoveAnimation(flyingEnemy[i].flameCount, flyingEnemy[i].flame, 4);
				}

				if (flyingEnemy[i].pos.x <= 0)
				{
					flyingEnemy[i].isAlive = false;
				}
			}
			
			for (int i = 0; i < 2; i++)
			{
				if (walkingEnemy[i].isAlive)
				{
					MoveAnimation(walkingEnemy[i].flameCount, walkingEnemy[i].flame, 4);
				}

				if (walkingEnemy[i].pos.x <= 0)
				{
					walkingEnemy[i].isAlive = false;
				}
			}
		
			for (int i = 0; i < 2; i++)
			{
				if (walkingEnemy[i].isSmash)
				{
					MoveAnimation(smashWalkEnemy[i].flameCount, smashWalkEnemy[i].flame, 4);

					Novice::ScreenPrintf(10, 20 * i, "%d", smashWalkEnemy[i].flameCount);
					Novice::ScreenPrintf(40, 20 * i, "%d", smashWalkEnemy[i].flame);
				}

				if (smashWalkEnemy[i].flameCount >= 39)
				{
					walkingEnemy[i].isSmash = false;
				}
			}

			
			backGraundPosX --;
			

			if (backGraundPosX <= -1280)
			{
				backGraundPosX = 0;
			}

			if (playerScore >= 40)
			{
				playerScore = 0;
				player.hp = 5;
				scene = GAMECLEAR;
			}

 			if (player.hp <= 0)
			{
				playerScore = 0;
				player.hp = 5;
				scene = GAMEEND;
			}

			Novice::ScreenPrintf(10, 10, "%d", player.hp);

			break;
		case GAMECLEAR:
			
			if (keys[DIK_SPACE] && !preKeys[DIK_SPACE])
			{
				scene = TITLE;
			}


			break;
		case GAMEEND:
		
			if (keys[DIK_SPACE] && !preKeys[DIK_SPACE])
			{
				scene = TITLE;
			}


			break;
		default:
			break;
		}

		///
		/// ↑更新処理ここまで
		///
		
		/// 
		/// ↓描画処理ここから
		/// 
		
		switch (scene)
		{
		case TITLE:

			Novice::DrawSprite(0, 0, gameTitleGraph, 1, 1, 0.0f, WHITE);

			break;
		case GAMEPLAY:

			Novice::DrawSprite((int)backGraundPosX, 0, backGroundGraph, 1, 1, 0.0f, WHITE);
			Novice::DrawSprite((int)backGraundPosX + 1280, 0, backGroundGraph, 1, 1, 0.0f, WHITE);

			Novice::DrawSprite(0, 0, groundGraph, 1, 1, 0.0f, WHITE);			

			if (!allEnemyIsAlve && player.isAlive)
			{
				Novice::DrawSpriteRect(int(player.pos.x - player.wide / 2), int(player.pos.y - player.height / 2), playerFlame * 64, 0, 64, 64, playerGraph, 1.0f / 6.0f, 1.0f, 0.0f, WHITE);
			}
			else if (player.isAlive )
			{
				Novice::DrawSpriteRect(int(player.pos.x - player.wide / 2), int(player.pos.y - player.height / 2), 0, 0, 64, 64, playerGraph, 1.0f / 6.0f, 1.0f, 0.0f, WHITE);
			}
				
			if (king.isAlive)
			{
				Novice::DrawSprite(int(king.pos.x - king.wide / 2), int(king.pos.y - king.height / 2), kingGraph, 1, 1, 0.0f, WHITE);
			}

			for (int i = 0; i < 10; i++)
			{
				if (playerBullet[i].isBullet)
				{
					Novice::DrawSprite(int(playerBullet[i].pos.x - 32.0f), int(playerBullet[i].pos.y - 32.0f), playerBulletGraph, 1, 1, 0.0f, WHITE);
				}
			}

			for (int i = 0; i < 2; i++)
			{
				if (flyingEnemy[i].isAlive)
				{
					Novice::DrawSpriteRect(int(flyingEnemy[i].pos.x - flyingEnemy[i].radius), int(flyingEnemy[i].pos.y - flyingEnemy[i].radius), flyingEnemy[i].flame * 64, 0, 64, 64, flyingEnemyGraph, 1.0f / 4.0f, 1.0f, 0.0f, WHITE);
				}

			}

			for (int i = 0; i < 2; i++)
			{
				if (walkingEnemy[i].isAlive)
				{
					Novice::DrawSpriteRect(int(walkingEnemy[i].pos.x - walkingEnemy[i].wide /2), int(walkingEnemy[i].pos.y - walkingEnemy[i].height /  2), walkingEnemy[i].flame * 64, 0, 64, 64, warkingEnemyGraph, 1.0f / 4.0f, 1.0f, 0.0f, WHITE);
				}
				else if (walkingEnemy[i].isSmash && !walkingEnemy[i].isAlive)
				{
					Novice::DrawSpriteRect(int(walkingEnemy[i].pos.x - walkingEnemy[i].radius), int(walkingEnemy[i].pos.y - 64.0f / 2), smashWalkEnemy[i].flame * 64, 0, 64, 64, warkingEnemySmashGraph, 1.0f / 4.0f, 1.0f, 0.0f, WHITE);
				}			
			}

			for (int i = 0; i < 2; i++)
			{
				if (flyingBullet[i].isBullet)
				{
					Novice::DrawSprite(int(flyingBullet[i].pos.x - flyingBullet[i].wide / 2), int(flyingBullet[i].pos.y - flyingBullet[i].height / 2), flyingEggGraph, 1, 1, 0.0f, WHITE);
				}
			}
			

			break;
		case GAMECLEAR:

			Novice::DrawSprite(0, 0, gameClearGraph, 1, 1, 0.0f, WHITE);

			break;
		case GAMEEND:

			Novice::DrawSprite(0, 0, gameoverGraph, 1, 1, 0.0f, WHITE);

			break;
		default:
			break;
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
