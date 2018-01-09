//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Dramos.h"

namespace Dramos
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		imageName = "Dramos";
		DG::Image_Create(imageName, "./data/image/Dramos.png");
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
		state = State1;		//State1 = 画面上から降下
							//State2 = ふわふわ飛ぶ
							//Death  = 死ぬ間際

		pos = { float(rand() % (int(ge->screen2DWidth) - 32)) + 16,
				-32 };
		angleLR = rand() % 2 ? Left : Right;
		hitBase = { -15, -15, 30, 30 };
		defHitBase = hitBase;

		//キャラチップ読み込み
		for (int y = 0; y < 2; ++y)
		{
			for (int x = 0; x < 2; ++x)
			{
				charaChip.emplace_back(new ML::Box2D(x * 32, y * 32, 32, 32));
			}
		}

		moveX = 0;
		moveY = 0;

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
		case BChara::State1: //画面上から降下
			Move1();
			break;

		case BChara::State2: //ふわふわ飛ぶ
			Move2();
			break;

		case BChara::Death: //死ぬ間際
			Move3();
			break;

		default:
			return;
		}

		if (ZakoDelete())
			stateAnim += 2;
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
		if (moveY > 180)
		{
			moveY = 0;
			state = State2;
			speed.y = 0;
			moveType = rand() % 3;
			if (moveType == 1)
			{
				if (rand() % 2)
				{
					angleLR = Left;
					speed.x = -1.5f;
				}
				else
				{
					angleLR = Right;
					speed.x = 1.5f;
				}
			}
		}
		else
		{
			moveY += 2;
			speed.y = float(sin(ML::ToRadian(moveY))) * 2.5f;
		}
		NomalMove();
		++animCnt;

		if (DamageEnemy())
			stateAnim += 2;
		DamagePlayer();
	}

	//-------------------------------------------------------------------
	//State2の動作
	void Object::Move2()
	{
		switch (moveType)
		{
		case 0:		//待機
			if (cntTime == 120)
			{
				if (rand() % 2)
				{
					moveType = 1;
					if (rand() % 2)
					{
						angleLR = Left;
						speed.x = -1.5f;
					}
					else
					{
						angleLR = Right;
						speed.x = 1.5f;
					}
				}
				else
				{
					moveType = 2;
				}
			}

		case 1:		//左右移動
			if (cntTime == 90)
			{
				speed.x = 0;
				moveType = 0;
			}
			break;
			
		case 2:		//垂直落下
			if (cntTime == 120)
			{
				moveType = rand() % 2;
			}
			break;

		default:
			return;
		}
		
		moveY += 3;
		if (moveType == 2)
			speed.y = float(sin(ML::ToRadian(moveY))) * 2.4f;
		else
			speed.y = float(sin(ML::ToRadian(moveY))) / 1.5f;


		if (cntTime == 120)
			cntTime = 0;
		else
			++cntTime;

		pos.y += speed.y;
		OutCheckMove();

		if (DamageEnemy())
			stateAnim += 2;
		DamagePlayer();

		++animCnt;
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