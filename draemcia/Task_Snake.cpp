//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Player.h"
#include  "Task_Snake.h"

namespace  Snake
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		imageName = "Snake";
		DG::Image_Create(imageName, "./data/image/Snake.png");
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		DG::Image_Erase(imageName);
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
		imageName = res->imageName;

		render2D_Priority[1] = 0.6f;
		state = State1;		//State1 = 地面ボコボコ
							//State2 = 本体がぶわーしてシュワー
							//State3 = 死ぬ間際

		pos = { float(rand() % (int(ge->screen2DWidth) - 32)) + 16,
				float(ge->screen2DHeight) - 51 };
		angleLR = rand() % 2 ? Left : Right;
		hitBase = { -15, -15, 30, 30 };
		
		//キャラチップ読み込み
		for (int y = 0; y < 3; ++y)
		{
			for (int x = 0; x < 3; ++x)
			{
				charaChip.emplace_back(new ML::Box2D(x * 32, y * 32, 32, 32));
			}
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
		switch (state)
		{
		case BChara::State1: //地面ボコボコ
			Move1();
			break;

		case BChara::State2: //本体がぶわーしてシュワー
			Move2();
			break;

		case BChara::State3: //死ぬ間際
			Move3();
			break;

		default:
			return;
		}
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		if (state == State1)
			EnemyRender(3);
		else
			EnemyRender();
	}

	//-------------------------------------------------------------------
	//State1時の動作
	void Object::Move1()
	{
		if (animCnt > 144)
		{
			animCnt = 0;
			state = State2;
			stateAnim = 3;
			moveType = 0;
			pos.y -= 10;
		}
		else
		{
			++animCnt;
		}
	}

	//-------------------------------------------------------------------
	//State2時の動作
	void Object::Move2()
	{
		++cntTime;
		switch (moveType)
		{
		case 0:		//出現から作動
			if (cntTime > 60)
			{
				auto player = ge->GetTask_One_GN<Player::Object>("プレイヤー", "NoName");
				if (player == nullptr)
					return;

				if (pos.x < player->pos.x)
				{
					angleLR = Right;
					speed.x = 4;
				}
				else
				{
					angleLR = Left;
					speed.x = -3;
				}
				cntTime = 0;
				moveType = 1;
			}
			break;

		case 1:		//めっちゃ走る
			if (cntTime > 90)
			{
				moveType = 2;
				cntTime = 0;
				speed.x = 0;
			}
			break;

		case 2:		//走った後の待機
			if (cntTime > 60)
			{
				moveType = 0;
				cntTime = 0;
			}
			break;

		default:
			break;
		}

		FallAndJump(false);
		OutCheckMove();
		CheckFootMove();

		++animCnt;

		if (DamageEnemy())
			stateAnim += 3;

		DamagePlayer();
	}

	//-------------------------------------------------------------------
	//State3時の動作
	void Object::Move3()
	{
		if (animCnt > 10)
		{
			state = Non;
			KillMeBaby();
		}
		else
		{
			++animCnt;
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