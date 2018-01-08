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

	int stateAnim;			//状態に応じたアニメーション番号
	int life;				//体力
	int moveType;			//動作の管理番号
	ML::Box2D atHitBase;	//与ダメージ用当たり判定
	ML::Box2D draw;			//表示画像のサイズ(雑魚敵は変更不要)

	EChara() : 
		imageName(""),
		stateAnim(0),
		life(1),
		moveType(0),
		atHitBase(hitBase),
		draw(-16, -16, 32, 32){}

	~EChara(){}

	virtual bool DamagePlayer();
	virtual bool DamageEnemy();
	virtual bool ZakoDelete();
	virtual void KillMeBaby();
	virtual void EnemyRender(int = 2);
};
