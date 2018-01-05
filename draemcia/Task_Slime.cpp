//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Slime.h"

namespace  Slime
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		imageName = "Slime";
		DG::Image_Create(imageName, "./data/image/Slime.png");
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

		render2D_Priority[1] = 0.5f;
		state = State1;		//State1 = 画面下から這い上がる動き
							//State2 = 陸上をのそのそ
							//State3 = 死ぬ間際

		pos = { float(rand() % (int(ge->screen2DWidth) - 32)) + 16,
				float(ge->screen2DHeight) + 16 };
		hitBase = { -15, -14, 30, 30 };
		
		//キャラチップ読み込み
		for (int y = 0; y < 4; ++y)
		{
			for (int x = 0; x < 2; ++x)
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
		case BChara::State1: //画面下から這い上がる動き
			Move1();
			break;

		case BChara::State2: //陸上をのそのそ
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
		EnemyRender();
	}

	//-------------------------------------------------------------------
	//State1時の動作
	void Object::Move1()
	{
		++animCnt;

		if (pos.y < ge->screen2DHeight - 30)
		{
			speed.y = -1.5f;
			if (pos.y < ge->screen2DHeight - 45)
			{
				pos.y = float(ge->screen2DHeight) - 51;
				state = State2;
				stateAnim = 2;
				speed.y = 0.f;

				animCnt = 0;
				moveType = rand() % 3;
			}
		}
		else
		{
			speed.y = -0.7f;
		}

		NomalMove();

		if (DamageEnemy())
			stateAnim += 4;
	}

	//-------------------------------------------------------------------
	//State2時の動作
	void Object::Move2()
	{
		switch (moveType)
		{
		case 0:	//待機
			speed.x = 0;
			++cntTime;
			if (cntTime > 60)
			{
				cntTime = 0;
				moveType = rand() % 2 + 1;
			}
			break;

		case 1:	//右に向いて移動
			angleLR = Right;
			speed.x = 1.f;
			++cntTime;
			if (cntTime > 90)
			{
				cntTime = 0;
				moveType = 0;
			}
			break;

		case 2: //左に向いて移動
			angleLR = Left;
			speed.x = -1.f;
			++cntTime;
			if (cntTime > 90)
			{
				cntTime = 0;
				moveType = 0;
			}
			break;

		default:
			return;
		}
		++animCnt;

		FallAndJump(false);
		OutCheckMove();
		CheckFootMove();

		if (DamageEnemy())
			stateAnim += 4;

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

	//-------------------------------------------------------------------
	//State3時の動作

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