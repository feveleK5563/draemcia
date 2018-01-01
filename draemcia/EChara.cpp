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
//敵とプレイヤーとの当たり判定
// →プレイヤーがダメージを食らう
bool EChara::DamagePlayer()
{
	//状態がないときは処理しない
	if (state == Non)
		return false;

	//プレイヤーにアクセス
	auto player = ge->GetTask_One_GN<Player::Object>("プレイヤー", "NoName");
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
//モンスターの表示
void EChara::EnemyRender()
{
	if (state == Non)
		return;

	ML::Box2D src;
	ML::Color color = { 1.f, 1.f, 1.f, 1.f };

	if (state == State3)
	{
		src = *charaChip[stateAnim];
		if (animCnt > 5)
			color = { 1.f, 1.f, 0.1f, 0.1f };
	}
	else
	{
		src = *charaChip[animCnt / 8 % 2 + stateAnim];
	}

	if (angleLR == Left) //反転
	{
		src.x += src.w;
		src.w *= -1;
	}

	ML::Box2D draw = { -16, -16, 32, 32 };
	draw.Offset(pos);
	DG::Image_Draw(imageName, draw, src, color);
	RenderFrameRed(hitBase);
}

//-------------------------------------------------------------------
//自分を殺していくスタイル
void EChara::KillMeBaby()
{
	if (auto gm = ge->GetTask_One_GN<Game::Object>("本編", "統括"))
		--gm->monsterAmount;

	/*if (auto player = ge->GetTask_One_GN<Player::Object>("プレイヤー", "NoName"))
		++player->swordLength;*/

	Kill();
}