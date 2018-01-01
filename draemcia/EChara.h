#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//�G�ėp�N���X
//-------------------------------------------------------------------
#include "BChara.h"

class EChara : public BChara
{
	//�ύX�s����������������������������������������������������
private:
	typedef shared_ptr<EChara>		SP;
	typedef weak_ptr<EChara>		WP;
public:
	//�ύX������������������������������������������������������
	string imageName;

	int stateAnim;		//��Ԃɉ������A�j���[�V�����ԍ�
	int moveType;		//����̊Ǘ��ԍ�

	EChara() : 
		imageName(""),
		stateAnim(0),
		moveType(0){}

	~EChara(){}

	virtual bool DamagePlayer();
	virtual bool DamageEnemy();
	virtual void EnemyRender();
	virtual void KillMeBaby();
};
