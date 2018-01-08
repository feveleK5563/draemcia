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
	//リソースの初期化
	bool  Resource::Initialize()
	{
		imageName = "Length";
		DG::Image_Create(imageName, "./data/image/Length.png");
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
		render2D_Priority[1] = 0.2f;

		pos = { 10, 7 };
		if (auto player = ge->GetTask_One_GN<Player::Object>("プレイヤー", "NoName"))
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

		//小0~9,大0~9
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

		//★タスクの生成

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放
		int size = numberChip.size();
		for (int i = 0; i < size; ++i)
			delete numberChip[i];
		numberChip.clear();
		numberChip.shrink_to_fit();

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//★引き継ぎタスクの生成
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
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
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		int sideLength = 0;
		for (int i = 0; i < 2; ++i)
		{	//整数
			ML::Box2D draw = *numberChip[numberBig];
			draw.x = 0; draw.y = 0;
			draw.Offset(pos.x - (numberSizePlus * 4) + sideLength, pos.y - numberSizePlus);
			draw.w += numberSizePlus * 2; draw.h += numberSizePlus * 2;
			DG::Image_Draw(res->imageName, draw, *numberChip[numberBig + integer[i]], numberColor);
			sideLength += draw.w;
		}
		{	//ドット
			ML::Box2D draw = *numberChip[dot];
			draw.x = 0; draw.y = 0;
			draw.Offset(pos.x - numberSizePlus + sideLength, pos.y - numberSizePlus);
			draw.w += numberSizePlus * 2; draw.h += numberSizePlus * 2;
			DG::Image_Draw(res->imageName, draw, *numberChip[dot], numberColor);
			sideLength += draw.w;
		}
		for (int i = 0; i < 2; ++i)
		{	//小数
			ML::Box2D draw = *numberChip[numberSmall];
			draw.x = 0; draw.y = 0;
			draw.Offset(pos.x - numberSizePlus + sideLength, pos.y + 10 - numberSizePlus);
			draw.w += numberSizePlus * 2; draw.h += numberSizePlus * 2;
			DG::Image_Draw(res->imageName, draw, *numberChip[numberSmall + decimal[i]], numberColor);
			sideLength += draw.w;
		}
		{	//メートル
			ML::Box2D draw = *numberChip[meter];
			draw.x = 0; draw.y = 0;
			draw.Offset(pos.x + 100, pos.y);
			DG::Image_Draw(res->imageName, draw, *numberChip[meter], ML::Color(1.f, 0.f, 0.f, 0.f));
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