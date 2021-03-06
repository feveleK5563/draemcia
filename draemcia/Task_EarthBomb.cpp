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
	//\[Xฬ๚ป
	bool  Resource::Initialize()
	{
		imageName = "EarthBomb";
		DG::Image_Create(imageName, "./data/image/EarthBomb.png");
		return true;
	}
	//-------------------------------------------------------------------
	//\[Xฬ๐๚
	bool  Resource::Finalize()
	{
		DG::Image_Erase(imageName);
		return true;
	}
	//-------------------------------------------------------------------
	//u๚ปv^XNถฌษP๑พฏsค
	bool  Object::Initialize()
	{
		//X[p[NX๚ป
		__super::Initialize(defGroupName, defName, true);
		//\[XNXถฌor\[XคL
		this->res = Resource::Create();

		//f[^๚ป
		imageName = res->imageName;

		render2D_Priority[1] = 0.9f;

		state = State1;		//State1 = บ
							//State2 = เy๔

		if (auto bd = ge->GetTask_One_GN<BossDragon::Object>("{XG", "hS"))
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


		//^XNฬถฌ

		return  true;
	}
	//-------------------------------------------------------------------
	//uIนv^XNมลษP๑พฏsค
	bool  Object::Finalize()
	{
		//f[^^XN๐๚
		int size = charaChip.size();
		for (int i = 0; i < size; ++i)
			delete charaChip[i];
		charaChip.clear();
		charaChip.shrink_to_fit();

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//๘ซpฌ^XNฬถฌ
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//uXVvPt[ษsค
	void  Object::UpDate()
	{
		switch (state)
		{
		case BChara::State1:	//บ
			Move1();
			break;

		case BChara::State2:	//เy๔
			Move2();
			break;
		}
	}
	//-------------------------------------------------------------------
	//uQc`ๆvPt[ษsค
	void  Object::Render2D_AF()
	{
		EnemyRender();
	}

	//-------------------------------------------------------------------
	//State1ฬฎ์
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
	//State2ฬฎ์
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

	//
	//ศบอ๎{IษฯXsvศ\bh
	//
	//-------------------------------------------------------------------
	//^XNถฌ๛
	Object::SP  Object::Create(bool  flagGameEnginePushBack_)
	{
		Object::SP  ob = Object::SP(new  Object());
		if (ob) {
			ob->me = ob;
			if (flagGameEnginePushBack_) {
				ge->PushBack(ob);//Q[GWษo^
			}
			if (!ob->B_Initialize()) {
				ob->Kill();//CjVCYษธsตฝ็Kill
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
	//\[XNXฬถฌ
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