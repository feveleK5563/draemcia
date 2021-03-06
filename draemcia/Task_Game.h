#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//ゲーム本編
//-------------------------------------------------------------------
#include "GameEngine_Ver3_7.h"

namespace  Game
{
	//タスクに割り当てるグループ名と固有名
	const  string  defGroupName("本編");	//グループ名
	const  string  defName(		"統括");	//タスク名
	//-------------------------------------------------------------------
	class  Resource
	{
		bool  Initialize();
		bool  Finalize();
		Resource();
	public:
		~Resource();
		typedef  shared_ptr<Resource>	SP;
		typedef  weak_ptr<Resource>		WP;
		static   WP  instance;
		static  Resource::SP  Create();
		//共有する変数はここに追加する
	};
	//-------------------------------------------------------------------
	class  Object : public  BTask
	{
	//変更不可◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆
	public:
		virtual  ~Object();
		typedef  shared_ptr<Object>		SP;
		typedef  weak_ptr<Object>		WP;
		//生成窓口 引数はtrueでタスクシステムへ自動登録
		static  Object::SP  Create(bool flagGameEnginePushBack_);
		Resource::SP	res;
	private:
		Object();
		bool  B_Initialize();
		bool  B_Finalize();
		bool  Initialize();		//「初期化」タスク生成時に１回だけ行う処理
		void  UpDate();			//「実行」１フレーム毎に行う処理
		void  Render2D_AF();	//「2D描画」１フレーム毎に行う処理
		bool  Finalize();		//「終了」タスク消滅時に１回だけ行う処理
	//変更可◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇
	public:
		//追加したい変数・メソッドはここに追加する

		const int MonsterMaxAmount = 30;	//モンスターの最大量
		int monsterAmount;					//現在のモンスターの量

		int appMonsterTime;		//敵が出る時間計測
		int score;				//トックテーン
		enum NextLevelBoader	//次レベルまでの敵撃破数
		{
			Next2 = 10,
			Next3 = Next2 + 20,
			Next4 = Next3 + 30,
			Next5 = Next4 + 50,
			Next6 = Next5 + 50,
			Next7 = Next6 + 80,
			Next8 = Next7 + 100,
			Next9 = Next8 + 300,
		};
		NextLevelBoader nextLevelBoader;

		enum LevelofInterval	//モンスターの出現間隔
		{
			Level1 = 90,
			Level2 = 60,
			Level3 = 40,
			Level4 = 30,
			Level5 = 20,
			Level6 = 10,
			Level7 = 5,
			Level8 = 1,
			Level9 = 0,
		};
		LevelofInterval level;
	};
}