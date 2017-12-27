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
		render2D_Priority[1] = 0.5f;

		pos = { float(ge->screen2DWidth / 2),
				float(ge->screen2DHeight / 2) };
		basisSpeed = 2.f;
		swordLength = 16;
		speed.x = -basisSpeed;
		angleLR = Left;

		state = State1;	//State1… 通常状態
						//State2… 下攻撃状態
						//State3… 死亡

		forceOfJump = -9.5f;

		hitBase = { -16, -16, 32, 32 };
		swordHitBase = { 16, -16, (int)swordLength + 7, 32 };

		//キャラチップ読み込み
		for (int y = 0; y < 2; ++y)
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

		//ボタン入力による移動速度変更
		ChangeSpeed();

		//画面外判定
		OutCheckMove();
		//足元接触判定
		CheckFoot();

		//アニメーション
		++animCnt;
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		//キャラクタ描画 (「プレイヤー」「剣身」「剣先」の三回)
		ML::Box2D draw;
		int srcNum;
		int *changeDXY = &draw.x,
			*changeDWH = &draw.w;

		switch (state)
		{
		case BChara::State1:
			srcNum = (animCnt / 10 % 2) * 3;
			break;

		case BChara::State2:
			srcNum = 6;
			changeDXY = &draw.y;
			changeDWH = &draw.h;
			break;

		case BChara::State3:
			srcNum = 9;
			break;

		default:
			return;
		}

		//プレイヤー
		{
			draw = { -16, -16, 32, 32 };
			TurnaroundDraw(draw, *charaChip[srcNum], 0);
		}

		//墓状態は以下の処理不要
		if (state == State3)
			return;

		//剣身
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
			TurnaroundDraw(draw, src, 1);
			*changeDXY += *changeDWH;
		}

		//剣先
		{
			*changeDWH = 32;
			TurnaroundDraw(draw, *charaChip[srcNum + 2], 2);
		}

		//当たり判定可視化計画
		RenderFrameBlue(hitBase);
		RenderFrameRed(swordHitBase);
	}

	//-------------------------------------------------------------------
	//方向転換描画
	void Object::TurnaroundDraw(const ML::Box2D& draw, const ML::Box2D& src, int i)
	{
		ML::Box2D cpydr = draw;
		ML::Box2D cpysr = src;
		if (angleLR == Left)
		{
			cpysr.x += src.w;
			cpysr.w *= -1;
			if (state == State1)
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
		DG::Image_Draw(this->res->imageName, cpydr, cpysr);
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
			swordHitBase = { -((int)swordLength + 7) - 16, -16, (int)swordLength + 7, 32 };
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
			swordHitBase = { 16, -16, (int)swordLength + 7, 32 };
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
			JumpAndFall(jumpButton);
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