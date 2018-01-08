#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//�L�����N�^�ėp�N���X
//-------------------------------------------------------------------
#include "GameEngine_Ver3_7.h"

class BChara : public BTask
{
	//�ύX�s����������������������������������������������������
public:
	typedef shared_ptr<BChara>		SP;
	typedef weak_ptr<BChara>		WP;

	const bool RendFrameFlag;
public:
	//�ύX������������������������������������������������������
	//�L�����N�^���ʃ����o�ϐ�

	enum State	//�ėp��ԊǗ�
	{
		Non,		//�`����X�V�����Ȃ�
		State1,		//���1
		State2,		//���2
		State3,		//���3
		Damage,		//��_���[�W���
		Death,		//��
	};
	State state;

	ML::Vec2	pos;		//�L�����N�^�ʒu
	ML::Vec2	speed;		//�L�����N�^�̃X�s�[�h
	ML::Box2D	hitBase;	//�����蔻��͈�
	ML::Vec2	moveVec;	//�ړ��x�N�g��
	int			cntTime;	//�ėp�^�C�}�[

	//�����֘A
	const float	fallAccel;		//���������x
	float		forceOfJump;	//�W�����v�͂��c�ł����˂��c
	float		fallSpeed;		//�������x

	//�ڐG����
	bool	hitFoot;	//�����ڐG����

	//�A�j���[�V����
	vector<ML::Box2D*>	charaChip;	//�L�����N�^�̑f��
	int					animCnt;	//�A�j���[�V�����J�E���^

	//���E�̌���(2D�����_�Q�[����p)
	enum AngleLR{Left, Right};
	AngleLR angleLR;

	//�����o�ϐ��̏�����
	BChara():
		RendFrameFlag(false),	//�����蔻��̕\����
		state(Non),
		pos(0.f, 0.f),
		speed(0.f, 0.f),
		hitBase(0, 0, 0, 0),
		moveVec(0.f, 0.f),
		cntTime(0),
		forceOfJump(0.f),
		fallAccel(0.4f),
		fallSpeed(0.f),
		hitFoot(0),
		angleLR(Right),
		animCnt(0){}

	virtual ~BChara(){}

	//--------------------------------------------------------
	//�����蔻������v��

	//�����蔻������p�摜�ǂݍ��݂Ɖ��
	//(�ǂ����ň�񂾂��ǂݍ���ł��ꂽ�炨������������Ȃ�(����))
	virtual void FrameCreate();
	virtual void FrameErase();
	//�����蔻�����
	virtual void RenderFrameRed(const ML::Box2D&);
	virtual void RenderFrameBlue(const ML::Box2D&);

	//--------------------------------------------------------
	//�ȉ��L�����N�^���ʃ��\�b�h

	//�����̈ړ�
	virtual void NomalMove();
	//��ʊO����t����X���ړ�
	virtual void OutCheckMove();
	//�W�����v����
	virtual void FallAndJump(bool);
	//�����ڐG����t����Y���ړ�
	virtual void CheckFootMove();
};
