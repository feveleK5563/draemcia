//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_BossDragon.h"

namespace  BossDragon
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		imageName = "Dragon";
		DG::Image_Create(imageName, "./data/image/Dragon.png");
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

		render2D_Priority[1] = 0.8f;
		state = State1;		//State1 = 上からドーンと落下して名前表示
							//State2 = 待機からの攻撃モーション
							//State3 = 攻撃
							//Damage = 被ダメージ
							//Death  = 死ぬ間際

		pos = { 100, -100 };
		hitBase		= { -64,  -5, 110, 50 };
		defHitBase	= { -16, -43,  70, 40 };
		draw		= { -64, -43, 128, 90 };
		angleLR = Right;

		life = 10;

		//キャラチップ読み込み
		for (int y = 0; y < 3; ++y)
		{
			for (int x = 0; x < 2; ++x)
			{
				charaChip.emplace_back(new ML::Box2D(x * 128, y * 90, 128, 90));
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
		case BChara::State1:	//上からドーン
			Move1();
			break;

		case BChara::State2:	//待機からの攻撃モーション
			Move2();
			break;

		case BChara::State3:	//攻撃
			Move3();
			break;

		case BChara::Damage:	//被ダメージ
			Damage();
			break;

		case BChara::Death:		//死の間際
			Death();
			break;

		default:
			break;
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
	//上からドーン
	void Object::Move1()
	{
		if (cntTime > 60)
		{
			cntTime = 0;
			state = State2;
		}
		else
		{
			++cntTime;
			FallAndJump(false);
			CheckFootMove();
		}
	}

	//-------------------------------------------------------------------
	//State2時の動作
	//待機からの攻撃モーション
	void Object::Move2()
	{
		++cntTime;
		if (cntTime > 90)
		{
			stateAnim = 1;
			if (cntTime > 120)
			{
				cntTime = 0;
				stateAnim = 2;
				state = State3;
			}
		}

		if (DamageEnemy())
		{
			cntTime = 0;
		}
		DamagePlayer();
	}

	//-------------------------------------------------------------------
	//State3時の動作
	//攻撃
	void Object::Move3()
	{
		++cntTime;
		++animCnt;
		if (cntTime > 180)
		{
			stateAnim = 0;
			cntTime = 0;
			animCnt = 0;
			state = State2;
		}

		if (DamageEnemy())
		{
			cntTime = 0;
		}
		DamagePlayer();
	}

	//-------------------------------------------------------------------
	//Damage時の動作
	void Object::Damage()
	{
		++cntTime;
		if (cntTime > 10)
		{
			stateAnim = 5;
			animCnt = 0;
			if (cntTime > 120)
			{
				stateAnim = 0;
				state = State2;
				cntTime = 0;
			}
		}
		else
		{
			stateAnim = 4;
			++animCnt;
		}
	}

	//-------------------------------------------------------------------
	//Death時の動作
	void Object::Death()
	{
		++cntTime;
		if (cntTime > 10)
		{
			stateAnim = 5;
			animCnt = 0;
			if (cntTime > 120)
			{
				visible -= 0.02f;
				if (visible <= 0.f)
				{
					Kill();
				}
			}
		}
		else
		{
			stateAnim = 4;
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