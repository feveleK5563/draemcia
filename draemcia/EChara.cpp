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
//敵とプレイヤーとの当たり判定
// →プレイヤーがダメージを食らう
bool EChara::DamagePlayer()
{
	//状態がないときは処理しない
	if (state == Non)
		return false;

	//プレイヤーにアクセス
	auto player = ge->GetTask_One_GN<Player::Object>("プレイヤー", "NoName");
	if (player == nullptr)
		return false;

	ML::Box2D cpyBase = hitBase.OffsetCopy(pos);
	if (cpyBase.Hit(player->hitBase.OffsetCopy(player->pos)))
	{
		if (--player->life <= 0)
			player->state = State3;
		return true;
	}
	return false;
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