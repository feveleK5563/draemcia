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
	if (player == nullptr || player->state == Death)
		return false;

	ML::Box2D cpyBase = defHitBase.OffsetCopy(pos);
	if (cpyBase.Hit(player->swordHitBase.OffsetCopy(player->pos)))
	{
		if (--life <= 0)
		{
			state = Death;
			animCnt = 0;
			++player->swordLength;
			if (auto game = ge->GetTask_One_GN<Game::Object>("�{��", "����"))
				++game->score;
			return true;
		}
		else
		{
			state = Damage;
			animCnt = 0;
			return true;
		}
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
	if (player == nullptr || player->state == Death || player->hitDamage)
		return false;

	ML::Box2D cpyBase = hitBase.OffsetCopy(pos);
	if (cpyBase.Hit(player->hitBase.OffsetCopy(player->pos)))
	{
		player->swordLength -= 10;
		if (player->swordLength < 0)
		{
			player->swordLength = 0;
			player->state = Death;
			player->pos.y -= 10;
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
//�{�X�����X�^�[���o��������G���S���S
bool EChara::ZakoDelete()
{
	if (state == Death)
		return false;

	if (auto game = ge->GetTask_One_GN<Game::Object>("�{��", "����"))
	{
		if (game->level == game->Level9)
		{
			state = Death;
			animCnt = 0;
			return true;
		}
	}
	return false;
}

//-------------------------------------------------------------------
//�������E���Ă����X�^�C��
void EChara::KillMeBaby()
{
	if (auto gm = ge->GetTask_One_GN<Game::Object>("�{��", "����"))
		--gm->monsterAmount;

	Kill();
}

//-------------------------------------------------------------------
//�����X�^�[�̕\��
void EChara::EnemyRender(int width)
{
	if (state == Non)
		return;

	ML::Box2D src;
	ML::Color color = { visible, 1.f, 1.f, 1.f };

	if (state == Death || state == Damage)
	{
		src = *charaChip[stateAnim];
		if (animCnt > 5)
			color = { visible, 1.f, 0.1f, 0.1f };
	}
	else
	{
		src = *charaChip[animCnt / 8 % width + stateAnim];
	}

	if (angleLR == Left) //���]
	{
		src.x += src.w;
		src.w *= -1;
	}

	ML::Box2D cpydraw = draw.OffsetCopy(pos);
	DG::Image_Draw(imageName, cpydraw, src, color);
	RenderFrameRed(hitBase);
	RenderFrameBlue(defHitBase);
}
