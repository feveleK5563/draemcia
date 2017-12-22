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

		forceOfJump = -10.f;

		hitBase = { -16, -16, 32, 32 };

		//キャラチップ読み込み
		for (int i = 0; i < 4; ++i)
		{
			charaChip.emplace_back(new ML::Box2D(80 * (i % 2), 32 * (i / 2), 32, 32));
		}

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
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		//キャラクタ描画
		ML::Box2D draw[3];
		ML::Box2D src[3];
		for (int i = 0; i < 3; ++i)
		{
			draw[i] = { -16, -16, 32, 32 };
			switch (state)
			{
			case State1: //通常歩行状態
				draw[i].x += i * 32;
				src[i] = *charaChip[0];
				src[i].x += i * 32;
				break;

			case State2: //急速落下状態
				src[0] = *charaChip[2];
				break;

			case State3: //死亡状態
				src[0] = *charaChip[3];
				break;

			default:
				return;
			}
			draw[i].Offset(pos);
			if (angleLR == Left)
			{
				src[i].x += src[i].w;
				src[i].w *= -1;
				if (state == State1)
				{
					//TODO:(+32)のところはswordLengthに変更してね
					switch (i)
					{
					case 1:
						draw[i].x -= 32 + 32;
						break;

					case 2:
						draw[i].x -= 32 * 3 + 32;
						break;
					}
				}
			}
			DG::Image_Draw(this->res->imageName, draw[i], src[i]);
		}
	}

	//-------------------------------------------------------------------
	//ボタン入力による移動スピードの変更
	void Object::ChangeSpeed()
	{
		//ボタン入力によってプレイヤーの向きを変える
		if (in.LStick.L.down)
		{
			if (hitFoot && state == State1 &&
				speed.x == -basisSpeed)
			{
				speed.x = -basisSpeed * 2;
			}
			else
			{
				speed.x = -basisSpeed;
				angleLR = Left;
			}
		}
		if (in.LStick.R.down)
		{
			if (hitFoot && state == State1 &&
				speed.x == basisSpeed)
			{
				speed.x = basisSpeed * 2;
			}
			else
			{
				speed.x = basisSpeed;
				angleLR = Right;
			}
		}

		//スピードが通常より速いときは
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
		JumpAndFall(state == State1 && jumpButton);

		if (state == State2)
		{
			fallSpeed = 12.f;

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