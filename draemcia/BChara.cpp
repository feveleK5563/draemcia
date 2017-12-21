//-------------------------------------------------------------------
//キャラクタ汎用スーパークラス
//-------------------------------------------------------------------
#include "BChara.h"
#include "Task_Field.h"
#include "MyPG.h"

//-------------------------------------------------------------------
//ただの移動
void BChara::NomalMove()
{
	pos += speed;
}

//-------------------------------------------------------------------
//画面外判定込みのX軸移動
void BChara::OutCheckMove()
{
	auto field = ge->GetTask_One_GN<Field::Object>("フィールド", "フィールド");
	if (field == nullptr)
		return;

	float spd = speed.x;
	while (spd != 0)
	{
		float preX = pos.x;
		if		(spd >=  1.f) { pos.x += 1.f;	spd -= 1.f; }
		else if (spd <= -1.f) { pos.x -= 1.f;	spd += 1.f; }
		else				  { pos.x += spd;	spd  = 0.f; }

		ML::Box2D hit = hitBase.OffsetCopy(pos);
		if (field->OutScreen(hit))
		{
			pos.x = preX;
			break;
		}
	}
}

//-------------------------------------------------------------------
//ジャンプ処理
void BChara::JumpAndFall(bool jumpFlag)
{
	//落下運動
	if (hitFoot)
	{
		fallSpeed = 0.f;

		if (jumpFlag)
		{
			fallSpeed += forceOfJump;
		}
	}
	fallSpeed += fallAccel;
}

//-------------------------------------------------------------------
//足元接触判定込みのY軸移動
void BChara::CheckFoot()
{
	auto field = ge->GetTask_One_GN<Field::Object>("フィールド", "フィールド");
	if (field == nullptr)
		return;
	
	//当たり判定を基にして足元矩形を生成
	ML::Box2D foot(	hitBase.x,
					hitBase.y + hitBase.h,
					hitBase.w,
					1);

	float spd = speed.y + fallSpeed;
	//地面めり込み阻止
	hitFoot = false;
	while (spd != 0)
	{
		float preY = pos.y;
		if		(spd >=  1.f) { pos.y += 1.f;	spd -= 1.f; }
		else if (spd <= -1.f) { pos.y -= 1.f;	spd += 1.f; }
		else				  { pos.y += spd;	spd  = 0.f; }

		ML::Box2D foot = hitBase.OffsetCopy(pos);
		if (field->HitFoot(foot))
		{
			pos.y = preY;
			hitFoot = true;
			break;
		}
	}
}