//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Player.h"
#include  "Task_UI.h"

namespace  UI
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//\[Xฬ๚ป
	bool  Resource::Initialize()
	{
		imageName = "Length";
		DG::Image_Create(imageName, "./data/image/Length.png");
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
		render2D_Priority[1] = 0.2f;

		pos = { 10, 7 };
		if (auto player = ge->GetTask_One_GN<Player::Object>("vC[", "NoName"))
			swordLength = &(player->swordLength);

		meterRevision = 0;
		for (int i = 0; i < 2; ++i)
		{
			integer[i] = 0;
			decimal[i] = 0;
		}

		beforeSL = *swordLength;
		numberColor = { 1.f, 0.f, 0.f, 0.f };
		numberSizePlus = 0;

		//ฌ0~9,ๅ0~9
		int height = 0;
		for (int y = 0; y < 4; ++y)
		{
			for (int x = 0; x < 5; ++x)
			{
				if (y < 2)
					numberChip.emplace_back(new ML::Box2D(18 * x, height, 18, 22));
				else
					numberChip.emplace_back(new ML::Box2D(24 * x, height, 24, 32));
			}
			if (y < 2)
				height += 22;
			else
				height += 32;
		}
		numberChip.emplace_back(new ML::Box2D(120, 44,  10, 32)); //.
		numberChip.emplace_back(new ML::Box2D(120, 76, 40, 32));  //m

		//^XNฬถฌ

		return  true;
	}
	//-------------------------------------------------------------------
	//uIนv^XNมลษP๑พฏsค
	bool  Object::Finalize()
	{
		//f[^^XN๐๚
		int size = numberChip.size();
		for (int i = 0; i < size; ++i)
			delete numberChip[i];
		numberChip.clear();
		numberChip.shrink_to_fit();

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//๘ซpฌ^XNฬถฌ
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//uXVvPt[ษsค
	void  Object::UpDate()
	{
		meterRevision = float(*swordLength) / 32.f;

		int in = (int)meterRevision;
		int de = int(meterRevision * 100.f) % 100;

		if (beforeSL < *swordLength)
		{
			numberSizePlus = 5;
			numberColor = { 1.f, 0.f, 0.f, 1.f };
		}
		else if (beforeSL > *swordLength)
		{
			numberSizePlus = 5;
			numberColor = { 1.f, 1.f, 0.f, 0.f };
		}
		else
		{
			if (numberSizePlus <= 0)
			{
				numberSizePlus = 0;
				if (!(*swordLength))
					numberColor = { 1.f, 1.f, 0.f, 0.f };
				else
					numberColor = { 1.f, 0.f, 0.f, 0.f };
			}
			else
			{
				--numberSizePlus;
			}
		}
		beforeSL = *swordLength;

		integer[0] = in / 10;
		integer[1] = in % 10;
		decimal[0] = de / 10;
		decimal[1] = de % 10;
		for (int i = 0; i < 2; ++i)
		{
			if (integer[i] > 9)
				integer[i] = 9;
			if (decimal[i] > 9)
				decimal[i] = 9;
		}
	}
	//-------------------------------------------------------------------
	//uQc`ๆvPt[ษsค
	void  Object::Render2D_AF()
	{
		int sideLength = 0;
		for (int i = 0; i < 2; ++i)
		{	//ฎ
			ML::Box2D draw = *numberChip[numberBig];
			draw.x = 0; draw.y = 0;
			draw.Offset(pos.x - (numberSizePlus * 4) + sideLength, pos.y - numberSizePlus);
			draw.w += numberSizePlus * 2; draw.h += numberSizePlus * 2;
			DG::Image_Draw(res->imageName, draw, *numberChip[numberBig + integer[i]], numberColor);
			sideLength += draw.w;
		}
		{	//hbg
			ML::Box2D draw = *numberChip[dot];
			draw.x = 0; draw.y = 0;
			draw.Offset(pos.x - numberSizePlus + sideLength, pos.y - numberSizePlus);
			draw.w += numberSizePlus * 2; draw.h += numberSizePlus * 2;
			DG::Image_Draw(res->imageName, draw, *numberChip[dot], numberColor);
			sideLength += draw.w;
		}
		for (int i = 0; i < 2; ++i)
		{	//ฌ
			ML::Box2D draw = *numberChip[numberSmall];
			draw.x = 0; draw.y = 0;
			draw.Offset(pos.x - numberSizePlus + sideLength, pos.y + 10 - numberSizePlus);
			draw.w += numberSizePlus * 2; draw.h += numberSizePlus * 2;
			DG::Image_Draw(res->imageName, draw, *numberChip[numberSmall + decimal[i]], numberColor);
			sideLength += draw.w;
		}
		{	//[g
			ML::Box2D draw = *numberChip[meter];
			draw.x = 0; draw.y = 0;
			draw.Offset(pos.x + 100, pos.y);
			DG::Image_Draw(res->imageName, draw, *numberChip[meter], ML::Color(1.f, 0.f, 0.f, 0.f));
		}
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