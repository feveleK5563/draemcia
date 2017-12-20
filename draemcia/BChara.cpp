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
//画面外判定込みの移動
void BChara::CheckMove()
{
	auto field = ge->GetTask_One_GN<Field::Object>("フィールド", "フィールド");
	if (field == nullptr)
		return;

	float spd = speed.x;
	while (spd != 0)
	{
		float preX = pos.x;
		if (spd >= 1.f) { pos.x += 1.f;	spd -= 1.f; }
		else if (spd <= -1.f) { pos.x -= 1.f;	spd += 1.f; }
		else { pos.x += spd;	spd = 0.f; }

		ML::Box2D hit = hitBase.OffsetCopy(pos);
		if (field->OutScreen(hit))
		{
			pos.x = preX;
			break;
		}
	}
}

//-------------------------------------------------------------------
//足元接触判定
bool BChara::CheckFoot()
{
	//当たり判定を基にして足元矩形を生成
	ML::Box2D foot(	hitBase.x,
					hitBase.y + hitBase.h,
					hitBase.w,
					1);
	foot.Offset(pos);

	return false;
}