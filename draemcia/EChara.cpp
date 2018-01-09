#include "MyPG.h"
#include "EChara.h"
#include "Task_Game.h"
#include "Task_Player.h"

//-------------------------------------------------------------------
//敵とプレイヤーの剣との当たり判定
// →敵がダメージを食らう(雑魚は即死)
bool EChara::DamageEnemy()
{
	//状態がないときは処理しない
	if (state == Non)
		return false;

	//プレイヤーにアクセス
	auto player = ge->GetTask_One_GN<Player::Object>("プレイヤー", "NoName");
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
			if (auto game = ge->GetTask_One_GN<Game::Object>("本編", "統括"))
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
//敵とプレイヤーとの当たり判定
// →プレイヤーがダメージを食らう
bool EChara::DamagePlayer()
{
	//状態がないときは処理しない
	if (state == Non)
		return false;

	//プレイヤーにアクセス
	auto player = ge->GetTask_One_GN<Player::Object>("プレイヤー", "NoName");
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
//ボスモンスターが出現したら雑魚全死亡
bool EChara::ZakoDelete()
{
	if (state == Death)
		return false;

	if (auto game = ge->GetTask_One_GN<Game::Object>("本編", "統括"))
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
//自分を殺していくスタイル
void EChara::KillMeBaby()
{
	if (auto gm = ge->GetTask_One_GN<Game::Object>("本編", "統括"))
		--gm->monsterAmount;

	Kill();
}

//-------------------------------------------------------------------
//モンスターの表示
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

	if (angleLR == Left) //反転
	{
		src.x += src.w;
		src.w *= -1;
	}

	ML::Box2D cpydraw = draw.OffsetCopy(pos);
	DG::Image_Draw(imageName, cpydraw, src, color);
	RenderFrameRed(hitBase);
	RenderFrameBlue(defHitBase);
}
