//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Field.h"

namespace  Field
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//\[XΜϊ»
	bool  Resource::Initialize()
	{
		return true;
	}
	//-------------------------------------------------------------------
	//\[XΜπϊ
	bool  Resource::Finalize()
	{
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
		render2D_Priority[1] = 0.9f;
		
		imageName = "Field";
		DG::Image_Create(imageName, "./data/image/Field1.png");
		
		//^XNΜΆ¬

		return  true;
	}
	//-------------------------------------------------------------------
	//uIΉv^XNΑΕΙPρΎ―s€
	bool  Object::Finalize()
	{
		//f[^^XNπϊ
		DG::Image_Erase(imageName);

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//ψ«p¬^XNΜΆ¬
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//uXVvPt[Ιs€
	void  Object::UpDate()
	{
	}
	//-------------------------------------------------------------------
	//uQc`ζvPt[Ιs€
	void  Object::Render2D_AF()
	{
		ML::Box2D src(0, 0, 480, 72);
		for (int i = 0; i < 2; ++i)
		{
			ML::Box2D draw(480 * i, ge->screen2DHeight - groundBorder - 36, 480, groundBorder + 36);
			DG::Image_Draw(imageName, draw, src);
		}
	}

	//-------------------------------------------------------------------
	//ζΚO»θ
	bool Object::OutScreen(const ML::Box2D& hitBase)
	{
		//»θpι`πέθ
		RECT hb = {
			hitBase.x,
			hitBase.y,
			hitBase.x + hitBase.w,
			hitBase.y + hitBase.h
		};

		if (hb.left < 0 || hb.right >  int(ge->screen2DWidth))
			return true;

		return false;
	}

	//-------------------------------------------------------------------
	//«³ΪG»θ
	bool Object::HitFoot(const ML::Box2D& hitBase)
	{
		//»θpι`πέθ
		RECT hb = {
			hitBase.x,
			hitBase.y,
			hitBase.x + hitBase.w,
			hitBase.y + hitBase.h
		};

		if (hb.bottom > int(ge->screen2DHeight) - groundBorder)
			return true;

		return false;
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