//-------------------------------------------------------------------
//プレイヤ（仮
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Player.h"

namespace  Player
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->imageName = "PlayerImg";
		DG::Image_Create(this->imageName, "./data/Image/Player.png");

		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		DG::Image_Erase(this->imageName);
		return true;
	}
	//-------------------------------------------------------------------
	//「初期化」タスク生成時に１回だけ行う処理
	bool  Object::Initialize()
	{
		//スーパークラス初期化
		__super::Initialize(defGroupName, defName, true);
		//リソースクラス生成orリソース共有
		this->res = Resource::Create();

		//★データ初期化
		controllerName = "P1";
		render2D_Priority[1] = 0.7f;

		pos = { float(ge->screen2DWidth) + 80,
				float(ge->screen2DHeight) - 60};
		basisSpeed = 2.f;
		life = LifeMax;
		swordLength = 16;
		speed.x = -basisSpeed;
		angleLR = Left;

		state = State1;	//State1 = 通常状態
						//State2 = 下攻撃状態
						//Death  = 死亡

		hitDamage = false;
		start = false;

		forceOfJump = -9.5f;

		hitBase = { -12, -13, 24, 29 };
		swordHitBase = { 16, -16, (int)swordLength + 9, 20 };

		srcNum = 0;
		//キャラチップ読み込み
		for (int y = 0; y < 4; ++y)
		{
			for (int x = 0; x < 6; ++x)
			{
				charaChip.emplace_back(new ML::Box2D(x * 32, y * 32, 32, 32));
			}
		}

		FrameCreate();

		//★タスクの生成

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放
		int size = charaChip.size();
		for (int i = 0; i < size; ++i)
			delete charaChip[i];
		charaChip.clear();
		charaChip.shrink_to_fit();

		FrameErase();

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//★引き継ぎタスクの生成
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
		in = DI::GPad_GetState("P1");

		if (start)
		{
			if (state != Non && state != Death)
			{
				//ボタン入力による移動速度変更
				ChangeSpeed();

				//画面外判定とX軸移動
				OutCheckMove();
			}
			else if (state == Death)
			{
				FallAndJump(false);
			}
		}
		else
		{
			if (pos.x < 600 - 16)
			{
				start = true;
			}
			else
			{
				speed.x = -basisSpeed;
				FallAndJump(false);

				NomalMove();
			}
		}

		//足元接触判定とY軸移動
		CheckFootMove();

		//アニメーション
		if (hitDamage)
			DamageAnim();

		++animCnt;
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		//キャラクタ描画 (「プレイヤー」「剣身」「剣先」の三回)
		ML::Box2D draw;
		ML::Color color = { 1.f, 1.f, 1.f, 1.f };
		int *changeDXY = &draw.x,
			*changeDWH = &draw.w;

		switch (state)
		{
		case State1: //着地面
			srcNum = (animCnt / 8 % 2) * 3;
			break;

		case State2: //ジャンプ中
			srcNum = 6;
			changeDXY = &draw.y;
			changeDWH = &draw.h;
			break;

		case Death: //墓
			srcNum = 9;
			break;

		default:
			//座標を何処か遠くへぶっ飛ばす
			pos = { -1000, -1000 };
			return;
		}

		if (hitDamage) //被ダメ中
		{
			srcNum += 12;
			if (cntTime > 5)
			{
				color = { 1.f, 1.f, 0.1f, 0.1f };
			}
		}

		//プレイヤーの表示
		{
			draw = { -16, -16, 32, 32 };
			TurnaroundDraw(draw, *charaChip[srcNum], 0, color);
		}

		//墓状態は以下の処理不要
		if (state == Death)
			return;

		//剣身の表示
		int plusSL = swordLength;
		*changeDXY += 32;
		ML::Box2D src;
		int* changeSWH;
		if (srcNum == 6)
			changeSWH = &src.h;
		else
			changeSWH = &src.w;
		while (plusSL != 0)
		{
			src = *charaChip[srcNum + 1];
			if (plusSL >= 32)
			{
				*changeDWH = 32;
				*changeSWH = 32;
				plusSL -= 32;
			}
			else
			{
				*changeDWH = plusSL;
				*changeSWH = plusSL;
				plusSL = 0;
			}
			TurnaroundDraw(draw, src, 1, color);
			*changeDXY += *changeDWH;
		}

		//剣先の表示
		{
			*changeDWH = 32;
			TurnaroundDraw(draw, *charaChip[srcNum + 2], 2, color);
		}

		//当たり判定可視化計画
		RenderFrameBlue(hitBase);
		RenderFrameRed(swordHitBase);
	}

	//-------------------------------------------------------------------
	//方向転換描画
	void Object::TurnaroundDraw(const ML::Box2D& draw, const ML::Box2D& src, int i, const ML::Color& color)
	{
		ML::Box2D cpydr = draw;
		ML::Box2D cpysr = src;
		if (angleLR == Left)
		{
			cpysr.x += src.w;
			cpysr.w *= -1;
			if (state == State1 || state == Death)
			{
				switch (i)
				{
				case 1:
					cpydr.x -= 32 + swordLength;
					break;

				case 2:
					cpydr.x -= (32 + swordLength) * 2;
					break;
				}
			}
		}

		cpydr.Offset(pos);
		DG::Image_Draw(res->imageName, cpydr, cpysr, color);
	}

	//-------------------------------------------------------------------
	//ボタン入力による移動スピードの変更
	void Object::ChangeSpeed()
	{
		//プレイヤーを向いている方向に進ませる
		bool Ldown = in.LStick.L.down,
			 Rdown = in.LStick.R.down;
		if (angleLR == Left)
		{
			swordHitBase = { -((int)swordLength + 8) - 16, -5, (int)swordLength + 8, 21 };
			if (Ldown && speed.x == -basisSpeed && hitFoot) //スピード上昇
			{
				speed.x = -basisSpeed * 2.2f;
			}
			if (Rdown) //方向転換
			{
				angleLR = Right;
				speed.x = basisSpeed;
			}
			if (!speed.x) //強制落下からの復帰
			{
				speed.x = -basisSpeed;
			}
		}
		else
		{
			swordHitBase = { 16, -5, (int)swordLength + 8, 21 };
			if (Rdown && speed.x == basisSpeed && hitFoot)
			{
				speed.x = basisSpeed * 2.2f;
			}
			if (Ldown) 
			{
				angleLR = Left;
				speed.x = -basisSpeed;
			}
			if (!speed.x)
			{
				speed.x = basisSpeed;
			}
		}

		//スピードの絶対値が最大値より高いときは
		//スピードをスピード最大値の10分の1ずつ下げる
		if (speed.x < -basisSpeed)
		{
			speed.x += basisSpeed / 10.f;
			if (speed.x > -basisSpeed)
				speed.x = -basisSpeed;
		}
		else if (speed.x > basisSpeed)
		{
			speed.x -= basisSpeed / 10.f;
			if (speed.x < basisSpeed)
				speed.x = basisSpeed;
		}

		//空中にいるときにジャンプボタンを押すと下攻撃状態に遷移する
		bool jumpButton = in.B1.down;
		if (!hitFoot && jumpButton)
		{
			state = State2;
		}
		else 
		{
			FallAndJump(jumpButton);
		}

		if (state == State2)
		{
			swordHitBase = { -16, 16, 32, (int)swordLength + 7 };
			fallSpeed += 1.5f;
			speed.x = 0.f;

			if (hitFoot)
			{
				state = State1;
			}
		}
	}

	//-------------------------------------------------------------------
	//被ダメージ中のアニメーション
	void Object::DamageAnim()
	{
		if (cntTime > 10)
		{
			hitDamage = false;
			cntTime = 0;
		}
		else
		{
			++cntTime;
		}
	}

	//★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//以下は基本的に変更不要なメソッド
	//★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//-------------------------------------------------------------------
	//タスク生成窓口
	Object::SP  Object::Create(bool  flagGameEnginePushBack_)
	{
		Object::SP  ob = Object::SP(new  Object());
		if (ob) {
			ob->me = ob;
			if (flagGameEnginePushBack_) {
				ge->PushBack(ob);//ゲームエンジンに登録
			}
			if (!ob->B_Initialize()) {
				ob->Kill();//イニシャライズに失敗したらKill
			}
			return  ob;
		}
		return nullptr;
	}
	//-------------------------------------------------------------------
	bool  Object::B_Initialize()
	{
		return  this->Initialize();
	}
	//-------------------------------------------------------------------
	Object::~Object() { this->B_Finalize(); }
	bool  Object::B_Finalize()
	{
		auto  rtv = this->Finalize();
		return  rtv;
	}
	//-------------------------------------------------------------------
	Object::Object() {	}
	//-------------------------------------------------------------------
	//リソースクラスの生成
	Resource::SP  Resource::Create()
	{
		if (auto sp = instance.lock()) {
			return sp;
		}
		else {
			sp = Resource::SP(new  Resource());
			if (sp) {
				sp->Initialize();
				instance = sp;
			}
			return sp;
		}
	}
	//-------------------------------------------------------------------
	Resource::Resource() {}
	//-------------------------------------------------------------------
	Resource::~Resource() { this->Finalize(); }
}