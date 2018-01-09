//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_EarthBomb.h"
#include  "Task_BossDragon.h"

namespace  EarthBomb
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		imageName = "EarthBomb";
		DG::Image_Create(imageName, "./data/image/EarthBomb.png");
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

		render2D_Priority[1] = 0.9f;

		state = State1;		//State1 = 投下
							//State2 = 炸裂

		if (auto bd = ge->GetTask_One_GN<BossDragon::Object>("ボス敵", "ドラゴン"))
		{
			pos = bd->pos;
			pos.x += 50;
			pos.y -= 10;
		}
		hitBase = { -32, -16, 64, 32 };
		draw    = { -32, -16, 64, 32 };

		speed.x = (float(rand() % 70) / 10.f) + 1.f;
		speed.y = -10.f;
		angleLR = Right;
		
		dokaan = true;

		charaChip.emplace_back(new ML::Box2D(0, 0, 64, 32));
		for (int i = 0; i < 2; ++i)
			charaChip.emplace_back(new ML::Box2D(64 * i, 32, 64, 96));


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
		case BChara::State1:	//投下
			Move1();
			break;

		case BChara::State2:	//炸裂
			Move2();
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
	void Object::Move1()
	{
		if (hitFoot)
		{
			state = State2;
			pos.y += 16;
			hitBase = { -20, -96, 40, 96 };
			draw = { -32, -96, 64, 96 };
			stateAnim = 1;
			return;
		}

		FallAndJump(false);
		OutCheckMove();
		CheckFootMove();
	}

	//-------------------------------------------------------------------
	//State2時の動作
	void Object::Move2()
	{
		++animCnt;
		if (animCnt > 60)
		{
			visible -= 0.05f;
			if (visible < 0.f)
			{
				Kill();
			}
			return;
		}

		if (dokaan && pos.y > 0 && animCnt % 2)
		{
			dokaan = false;

			auto eb = EarthBomb::Object::Create(true);
			eb->state = State2;
			eb->pos.x = pos.x;
			eb->pos.y = pos.y - 96;
			eb->hitBase = { -20, -96, 40, 96 };
			eb->draw = { -32, -96, 64, 96 };
			eb->stateAnim = 1;
			eb->animCnt = animCnt;
		}

		DamagePlayer();
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