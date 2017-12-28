//-------------------------------------------------------------------
//キャラクタ汎用スーパークラス
//-------------------------------------------------------------------
#include "BChara.h"
#include "Task_Field.h"
#include "MyPG.h"

//-------------------------------------------------------------------
//当たり判定可視化用画像の読み込みと解放
//どっか一ヶ所だけで呼んでくれぃ
void BChara::FrameCreate()
{
	DG::Image_Create("FrameRed", "./data/image/HitBaseFrameRed.png");
	DG::Image_Create("FrameBlue", "./data/image/HitBaseFrameBlue.png");
}
void BChara::FrameErase()
{
	DG::Image_Erase("FrameRed");
	DG::Image_Erase("FrameBlue");
}
//-------------------------------------------------------------------
//当たり判定を可視化させる(Red(赤)とBlue(青))
//引数：可視化させたい当たり判定
void BChara::RenderFrameRed(const ML::Box2D& hB)
{
	if (!RendFrameFlag)
		return;

	ML::Box2D draw = hB.OffsetCopy(pos);
	ML::Box2D src = { 0, 0, 32, 32 };
	DG::Image_Draw("FrameRed", draw, src);

}
void BChara::RenderFrameBlue(const ML::Box2D& hB)
{
	if (!RendFrameFlag)
		return;

	ML::Box2D draw = hB.OffsetCopy(pos);
	ML::Box2D src = { 0, 0, 32, 32 };
	DG::Image_Draw("FrameBlue", draw, src);
}


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
void BChara::FallAndJump(bool jumpFlag)
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
void BChara::CheckFootMove()
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