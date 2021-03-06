//-------------------------------------------------------------------
//Q[{า
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Title.h"
#include  "Task_Game.h"
#include  "Task_GameBG.h"
#include  "Task_Field.h"
#include  "Task_Player.h"
#include  "Task_UI.h"

#include  "Task_Slime.h"
#include  "Task_Dramos.h"
#include  "Task_Snake.h"
#include  "Task_BossDragon.h"

namespace  Game
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//\[Xฬ๚ป
	bool  Resource::Initialize()
	{
		return true;
	}
	//-------------------------------------------------------------------
	//\[Xฬ๐๚
	bool  Resource::Finalize()
	{
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
		srand((unsigned)time(NULL));
		monsterAmount = 0;
		appMonsterTime = 0;
		score = 0;
		nextLevelBoader = Next2;
		level = Level1;
		
		//^XNฬถฌ
		//wi^XN
		auto  bg = GameBG::Object::Create(true);
		//tB[h^XN
		auto  fd = Field::Object::Create(true);
		//vC^XN
		auto  pl = Player::Object::Create(true);
		//UI^XN
		auto  ui = UI::Object::Create(true);

		return  true;
	}
	//-------------------------------------------------------------------
	//uIนv^XNมลษP๑พฏsค
	bool  Object::Finalize()
	{
		//f[^^XN๐๚
		ge->KillAll_G("{า");
		ge->KillAll_G("tB[h");
		ge->KillAll_G("vC[");
		ge->KillAll_G("G");
		ge->KillAll_G("{XG");

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//๘ซpฌ^XNฬถฌ
			auto nextTask = Title::Object::Create(true);
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//uXVvPt[ษsค
	void  Object::UpDate()
	{
		auto in = DI::GPad_GetState("P1");

		//xฬใธ
		if (score >= nextLevelBoader)
		{
			switch (level)
			{
			case Level1: level = Level2; nextLevelBoader = Next3; break;
			case Level2: level = Level3; nextLevelBoader = Next4; break;
			case Level3: level = Level4; nextLevelBoader = Next5; break;
			case Level4: level = Level5; nextLevelBoader = Next6; break;
			case Level5: level = Level6; nextLevelBoader = Next7; break;
			case Level6: level = Level7; nextLevelBoader = Next8; break;
			case Level7: level = Level8; nextLevelBoader = Next9; break;

			case Level8:
			{
				level = Level9;
				auto  bd = BossDragon::Object::Create(true);
			}
			break;

			}
		}

		//X^[ฬoป
		if (int(level) > 0 && !(appMonsterTime++ % int(level)))
		{
			if (monsterAmount < MonsterMaxAmount)
			{
				switch (rand() % 3)
				{
				case 0:
				{
					auto sm = Slime::Object::Create(true);
				}
					break;

				case 1:
				{
					auto dm = Dramos::Object::Create(true);
				}
					break;

				case 2:
				{
					auto sk = Snake::Object::Create(true);
				}
					break;
				}
				++monsterAmount;
			}
		}

		if (in.ST.down) {
			//ฉgษมลvฟ
			Kill();
		}
	}
	//-------------------------------------------------------------------
	//uQc`ๆvPt[ษsค
	void  Object::Render2D_AF()
	{
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