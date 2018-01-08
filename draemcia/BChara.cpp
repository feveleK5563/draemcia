//-------------------------------------------------------------------
//�L�����N�^�ėp�X�[�p�[�N���X
//-------------------------------------------------------------------
#include "BChara.h"
#include "Task_Field.h"
#include "MyPG.h"

//-------------------------------------------------------------------
//�����蔻������p�摜�̓ǂݍ��݂Ɖ��
//�ǂ����ꃖ�������ŌĂ�ł��ꂡ
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
//�����蔻�������������(Red(��)��Blue(��))
//�����F�����������������蔻��
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
//�����F�W�����v���邩�ۂ�(bool true�ŃW�����v����)
void BChara::FallAndJump(bool jumpFlag)
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
void BChara::CheckFootMove()
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