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
//��ʊO���荞�݂�X���ړ�
void BChara::OutCheckMove()
{
	auto field = ge->GetTask_One_GN<Field::Object>("�t�B�[���h", "�t�B�[���h");
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
//�W�����v����
void BChara::JumpAndFall(bool jumpFlag)
{
	//�����^��
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
//�����ڐG���荞�݂�Y���ړ�
void BChara::CheckFoot()
{
	auto field = ge->GetTask_One_GN<Field::Object>("�t�B�[���h", "�t�B�[���h");
	if (field == nullptr)
		return;
	
	//�����蔻�����ɂ��đ�����`�𐶐�
	ML::Box2D foot(	hitBase.x,
					hitBase.y + hitBase.h,
					hitBase.w,
					1);

	float spd = speed.y + fallSpeed;
	//�n�ʂ߂荞�ݑj�~
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