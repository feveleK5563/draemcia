#include "MyPG.h"
#include "EChara.h"
#include "Task_Player.h"

//-------------------------------------------------------------------
//�G�ƃv���C���[�̌��Ƃ̓����蔻��
// ���G���_���[�W��H�炤(�G���͑���)
bool EChara::DamageEnemy()
{
	//��Ԃ��Ȃ��Ƃ��͏������Ȃ�
	if (state == Non)
		return false;

	//�v���C���[�ɃA�N�Z�X
	auto player = ge->GetTask_One_GN<Player::Object>("�v���C���[", "NoName");
	if (player == nullptr)
		return false;

	ML::Box2D cpyBase = hitBase.OffsetCopy(pos);
	if (cpyBase.Hit(player->swordHitBase.OffsetCopy(player->pos)))
	{
		state = State3;
		animCnt = 0;
		return true;
	}
	return false;
}

//-------------------------------------------------------------------
//�G�ƃv���C���[�Ƃ̓����蔻��
// ���v���C���[���_���[�W��H�炤
bool EChara::DamagePlayer()
{
	//��Ԃ��Ȃ��Ƃ��͏������Ȃ�
	if (state == Non)
		return false;

	//�v���C���[�ɃA�N�Z�X
	auto player = ge->GetTask_One_GN<Player::Object>("�v���C���[", "NoName");
	if (player == nullptr)
		return false;

	ML::Box2D cpyBase = hitBase.OffsetCopy(pos);
	if (cpyBase.Hit(player->hitBase.OffsetCopy(player->pos)))
	{
		player->state = Non;
		return true;
	}
	return false;
}