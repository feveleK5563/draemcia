#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//�Q�[���{��
//-------------------------------------------------------------------
#include "GameEngine_Ver3_7.h"

namespace  Game
{
	//�^�X�N�Ɋ��蓖�Ă�O���[�v���ƌŗL��
	const  string  defGroupName("�{��");	//�O���[�v��
	const  string  defName(		"����");	//�^�X�N��
	//-------------------------------------------------------------------
	class  Resource
	{
		bool  Initialize();
		bool  Finalize();
		Resource();
	public:
		~Resource();
		typedef  shared_ptr<Resource>	SP;
		typedef  weak_ptr<Resource>		WP;
		static   WP  instance;
		static  Resource::SP  Create();
		//���L����ϐ��͂����ɒǉ�����
	};
	//-------------------------------------------------------------------
	class  Object : public  BTask
	{
	//�ύX�s����������������������������������������������������
	public:
		virtual  ~Object();
		typedef  shared_ptr<Object>		SP;
		typedef  weak_ptr<Object>		WP;
		//�������� ������true�Ń^�X�N�V�X�e���֎����o�^
		static  Object::SP  Create(bool flagGameEnginePushBack_);
		Resource::SP	res;
	private:
		Object();
		bool  B_Initialize();
		bool  B_Finalize();
		bool  Initialize();		//�u�������v�^�X�N�������ɂP�񂾂��s������
		void  UpDate();			//�u���s�v�P�t���[�����ɍs������
		void  Render2D_AF();	//�u2D�`��v�P�t���[�����ɍs������
		bool  Finalize();		//�u�I���v�^�X�N���Ŏ��ɂP�񂾂��s������
	//�ύX������������������������������������������������������
	public:
		//�ǉ��������ϐ��E���\�b�h�͂����ɒǉ�����

		const int MonsterMaxAmount = 30;	//�����X�^�[�̍ő��
		int monsterAmount;					//���݂̃����X�^�[�̗�

		int appMonsterTime;		//�G���o�鎞�Ԍv��
		int score;				//�g�b�N�e�[��
		enum NextLevelBoader	//�����x���܂ł̓G���j��
		{
			Next2 = 20,
			Next3 = Next2 + 30,
			Next4 = Next3 + 40,
			Next5 = Next4 + 50,
			Next6 = Next5 + 70,
			Next7 = Next6 + 100,
			Next8 = Next7 + 300,
		};
		NextLevelBoader nextLevelBoader;

		enum LevelofInterval	//�����X�^�[�̏o���Ԋu
		{
			Level1 = 90,
			Level2 = 60,
			Level3 = 40,
			Level4 = 30,
			Level5 = 20,
			Level6 = 10,
			Level7 = 5,
			Level8 = 1,
		};
		LevelofInterval level;
	};
}