//-------------------------------------------------------------------
//�L�����N�^�ėp�X�[�p�[�N���X
//-------------------------------------------------------------------
#include "BChara.h"
#include "Task_Field.h"
#include "MyPG.h"

//-------------------------------------------------------------------
//�����̈ړ�
void BChara::NomalMove()
{
	pos += speed;
}

//-------------------------------------------------------------------
//��ʊO���荞�݂̈ړ�
void BChara::CheckMove()
{
	auto field = ge->GetTask_One_GN<Field::Object>("�t�B�[���h", "�t�B�[���h");
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
//�����ڐG����
bool BChara::CheckFoot()
{
	//�����蔻�����ɂ��đ�����`�𐶐�
	ML::Box2D foot(	hitBase.x,
					hitBase.y + hitBase.h,
					hitBase.w,
					1);
	foot.Offset(pos);

	return false;
}