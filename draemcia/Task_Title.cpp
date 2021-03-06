//-------------------------------------------------------------------
//^CgζΚ
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Title.h"
#include  "Task_Game.h"

namespace  Title
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//\[XΜϊ»
	bool  Resource::Initialize()
	{
		backImage = "TitleBackImg";
		DG::Image_Create(backImage, "./data/image/TitleFusuma.png");
		logoImage = "TitleLogoImg";
		DG::Image_Create(logoImage, "./data/image/TitleLogo.png");
		return true;
	}
	//-------------------------------------------------------------------
	//\[XΜπϊ
	bool  Resource::Finalize()
	{
		DG::Image_Erase(backImage);
		DG::Image_Erase(logoImage);
		return true;
	}
	//-------------------------------------------------------------------
	//uϊ»v^XNΆ¬ΙPρΎ―s€
	bool  Object::Initialize()
	{
		//X[p[NXϊ»
		__super::Initialize(defGroupName, defName, true);
		//\[XNXΆ¬or\[X€L
		this->res = Resource::Create();

		//f[^ϊ»
		fusumaPosX[0] = -300;
		fusumaPosX[1] = 600;
		setOK = false;

		//^XNΜΆ¬

		return  true;
	}
	//-------------------------------------------------------------------
	//uIΉv^XNΑΕΙPρΎ―s€
	bool  Object::Finalize()
	{
		//f[^^XNπϊ


		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//ψ«p¬^XNΜΆ¬
			auto nextTask = Game::Object::Create(true);
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//uXVvPt[Ιs€
	void  Object::UpDate()
	{
		auto in = DI::GPad_GetState("P1");

		if (setOK)
		{
			if (in.ST.down) {
				//©gΙΑΕvΏ
				this->Kill();
			}
		}
		else
		{
			fusumaPosX[0] += 8;
			fusumaPosX[1] -= 8;

			if (fusumaPosX[0] >= 0 && fusumaPosX[1] <= 300)
			{
				fusumaPosX[0] = 0;
				fusumaPosX[1] = 300;
				setOK = true;
			}
		}

	}
	//-------------------------------------------------------------------
	//uQc`ζvPt[Ιs€
	void  Object::Render2D_AF()
	{
		for (int i = 0; i < 2; ++i)
		{
			ML::Box2D	draw(0, 0, 300, 270);
			ML::Box2D	src(300 * i, 0, 300, 270);
			draw.Offset(fusumaPosX[i], 0);
			DG::Image_Draw(res->backImage, draw, src);
		}

		if (setOK)
		{
			ML::Box2D	draw(-180, 0, 360, 80);
			ML::Box2D	src(0, 0, 360, 80);
			draw.Offset(300, 10);
			DG::Image_Draw(res->logoImage, draw, src);
		}
	}

	//
	//ΘΊΝξ{IΙΟXsvΘ\bh
	//
	//-------------------------------------------------------------------
	//^XNΆ¬ϋ
	Object::SP  Object::Create(bool  flagGameEnginePushBack_)
	{
		Object::SP  ob = Object::SP(new  Object());
		if (ob) {
			ob->me = ob;
			if (flagGameEnginePushBack_) {
				ge->PushBack(ob);//Q[GWΙo^
			}
			if (!ob->B_Initialize()) {
				ob->Kill();//CjVCYΙΈs΅½ηKill
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
	//\[XNXΜΆ¬
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