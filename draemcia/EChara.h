#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//敵汎用クラス
//-------------------------------------------------------------------
#include "BChara.h"

class EChara : public BChara
{
	//変更不可◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆
private:
	typedef shared_ptr<EChara>		SP;
	typedef weak_ptr<EChara>		WP;
public:
	//変更可◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇
	string imageName;

	int stateAnim;		//状態に応じたアニメーション番号
	int moveType;		//動作の管理番号

	EChara() : 
		imageName(""),
		stateAnim(0),
		moveType(0){}

	~EChara(){}

	virtual bool DamagePlayer();
	virtual bool DamageEnemy();
	virtual void EnemyRender(int = 2);
	virtual void KillMeBaby();
};
