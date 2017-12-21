#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//キャラクタ汎用クラス
//-------------------------------------------------------------------
#include "GameEngine_Ver3_7.h"

class BChara : public BTask
{
	//変更不可◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆
public:
	typedef shared_ptr<BChara>		SP;
	typedef weak_ptr<BChara>		WP;
public:
	//変更可◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇
	//キャラクタ共通メンバ変数

	enum State	//汎用状態管理
	{
		Non,		//描画も更新もしない
		State1,		//状態1
		State2,		//状態2
		State3,		//状態3
	};
	State state;

	ML::Vec2	pos;		//キャラクタ位置
	ML::Vec2	speed;		//キャラクタのスピード
	ML::Box2D	hitBase;	//当たり判定範囲
	ML::Vec2	moveVec;	//移動ベクトル
	int			cntTime;	//汎用タイマー

	//落下関連
	const float	fallAccel;	//落下加速度
	float	forceOfJump;			//ジャンプ力ぅ…ですかねぇ…
	float	fallSpeed;				//落下速度

	//接触判定
	bool	hitFoot; //足元接触判定

	//アニメーション
	vector<ML::Box2D*>	charaChip;	//キャラクタの素材
	float				animCnt;	//アニメーションカウンタ

	//左右の向き(2D横視点ゲーム専用)
	enum AngleLR{Left, Right};
	AngleLR angleLR;

	//メンバ変数の初期化
	BChara():
		state(Non),
		pos(0.f, 0.f),
		speed(0.f, 0.f),
		hitBase(0, 0, 0, 0),
		moveVec(0.f, 0.f),
		cntTime(0),
		forceOfJump(0.f),
		fallAccel(0.4f),
		fallSpeed(0.f),
		hitFoot(0),
		angleLR(Right),
		animCnt(0.f){}

	virtual ~BChara(){}

	//--------------------------------------------------------
	//以下キャラクタ共通メソッド

	//ただの移動
	virtual void NomalMove();
	//画面外判定付きのX軸移動
	virtual void OutCheckMove();
	//ジャンプ処理
	virtual void JumpAndFall(bool = false);
	//足元接触判定付きのY軸移動
	virtual void CheckFoot();
};
