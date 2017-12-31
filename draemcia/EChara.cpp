#include "MyPG.h"
#include "EChara.h"
#include "Task_Game.h"
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
	if (player == nullptr || player->state == State3)
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
	if (player == nullptr || player->state == State3 || player->hitDamage)
		return false;

	ML::Box2D cpyBase = hitBase.OffsetCopy(pos);
	if (cpyBase.Hit(player->hitBase.OffsetCopy(player->pos)))
	{
		if (--player->life <= 0)
		{
			player->state = State3;
			player->render2D_Priority[1] = 0.4f;
		}
		else
		{
			player->hitDamage = true;
			player->cntTime = 0;
		}

		return true;
	}
	return false;
}

//-------------------------------------------------------------------
//�������E���Ă����X�^�C��
void EChara::KillMeBaby()
{
	if (auto gm = ge->GetTask_One_GN<Game::Object>("�{��", "����"))
		--gm->monsterAmount;

	/*if (auto player = ge->GetTask_One_GN<Player::Object>("�v���C���[", "NoName"))
		++player->swordLength;*/

	Kill();
}