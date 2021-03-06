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

	int stateAnim;			//��Ԃɉ������A�j���[�V�����ԍ�
	int life;				//�̗�
	int moveType;			//����̊Ǘ��ԍ�
	ML::Box2D defHitBase;	//��_���[�W�p�����蔻��
	ML::Box2D draw;			//�\���摜�̃T�C�Y(�G���G�͕ύX�s�v)

	float visible;			//���ߓx

	EChara() : 
		imageName(""),
		stateAnim(0),
		life(1),
		moveType(0),
		defHitBase(0, 0, 0, 0),
		draw(-16, -16, 32, 32),
		visible(1.f){}

	~EChara(){}

	virtual bool DamagePlayer();
	virtual bool DamageEnemy();
	virtual bool ZakoDelete();
	virtual void KillMeBaby();
	virtual void EnemyRender(int = 2);
};
