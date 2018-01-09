//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_BossDragon.h"

namespace  BossDragon
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		imageName = "Dragon";
		DG::Image_Create(imageName, "./data/image/Dragon.png");
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
		DG::Image_Erase(imageName);
		return true;
	}
	//-------------------------------------------------------------------
	//�u�������v�^�X�N�������ɂP�񂾂��s������
	bool  Object::Initialize()
	{
		//�X�[�p�[�N���X������
		__super::Initialize(defGroupName, defName, true);
		//���\�[�X�N���X����or���\�[�X���L
		this->res = Resource::Create();

		//���f�[�^������
		imageName = res->imageName;

		render2D_Priority[1] = 0.8f;
		state = State1;		//State1 = �ォ��h�[���Ɨ������Ė��O�\��
							//State2 = �ҋ@����̍U�����[�V����
							//State3 = �U��
							//Damage = ��_���[�W
							//Death  = ���ʊԍ�

		pos = { 100, -100 };
		hitBase		= { -64,  -5, 110, 50 };
		defHitBase	= { -16, -43,  70, 40 };
		draw		= { -64, -43, 128, 90 };
		angleLR = Right;

		life = 10;

		//�L�����`�b�v�ǂݍ���
		for (int y = 0; y < 3; ++y)
		{
			for (int x = 0; x < 2; ++x)
			{
				charaChip.emplace_back(new ML::Box2D(x * 128, y * 90, 128, 90));
			}
		}
		
		//���^�X�N�̐���

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�I���v�^�X�N���Ŏ��ɂP�񂾂��s������
	bool  Object::Finalize()
	{
		//���f�[�^���^�X�N���
		int size = charaChip.size();
		for (int i = 0; i < size; ++i)
			delete charaChip[i];
		charaChip.clear();
		charaChip.shrink_to_fit();

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//�������p���^�X�N�̐���
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�X�V�v�P�t���[�����ɍs������
	void  Object::UpDate()
	{
		switch (state)
		{
		case BChara::State1:	//�ォ��h�[��
			Move1();
			break;

		case BChara::State2:	//�ҋ@����̍U�����[�V����
			Move2();
			break;

		case BChara::State3:	//�U��
			Move3();
			break;

		case BChara::Damage:	//��_���[�W
			Damage();
			break;

		case BChara::Death:		//���̊ԍ�
			Death();
			break;

		default:
			break;
		}
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		EnemyRender();
	}

	//-------------------------------------------------------------------
	//State1���̓���
	//�ォ��h�[��
	void Object::Move1()
	{
		if (cntTime > 60)
		{
			cntTime = 0;
			state = State2;
		}
		else
		{
			++cntTime;
			FallAndJump(false);
			CheckFootMove();
		}
	}

	//-------------------------------------------------------------------
	//State2���̓���
	//�ҋ@����̍U�����[�V����
	void Object::Move2()
	{
		++cntTime;
		if (cntTime > 90)
		{
			stateAnim = 1;
			if (cntTime > 120)
			{
				cntTime = 0;
				stateAnim = 2;
				state = State3;
			}
		}

		if (DamageEnemy())
		{
			cntTime = 0;
		}
		DamagePlayer();
	}

	//-------------------------------------------------------------------
	//State3���̓���
	//�U��
	void Object::Move3()
	{
		++cntTime;
		++animCnt;
		if (cntTime > 180)
		{
			stateAnim = 0;
			cntTime = 0;
			animCnt = 0;
			state = State2;
		}

		if (DamageEnemy())
		{
			cntTime = 0;
		}
		DamagePlayer();
	}

	//-------------------------------------------------------------------
	//Damage���̓���
	void Object::Damage()
	{
		++cntTime;
		if (cntTime > 10)
		{
			stateAnim = 5;
			animCnt = 0;
			if (cntTime > 120)
			{
				stateAnim = 0;
				state = State2;
				cntTime = 0;
			}
		}
		else
		{
			stateAnim = 4;
			++animCnt;
		}
	}

	//-------------------------------------------------------------------
	//Death���̓���
	void Object::Death()
	{
		++cntTime;
		if (cntTime > 10)
		{
			stateAnim = 5;
			animCnt = 0;
			if (cntTime > 120)
			{
				visible -= 0.02f;
				if (visible <= 0.f)
				{
					Kill();
				}
			}
		}
		else
		{
			stateAnim = 4;
			++animCnt;
		}
	}

	//������������������������������������������������������������������������������������
	//�ȉ��͊�{�I�ɕύX�s�v�ȃ��\�b�h
	//������������������������������������������������������������������������������������
	//-------------------------------------------------------------------
	//�^�X�N��������
	Object::SP  Object::Create(bool  flagGameEnginePushBack_)
	{
		Object::SP  ob = Object::SP(new  Object());
		if (ob) {
			ob->me = ob;
			if (flagGameEnginePushBack_) {
				ge->PushBack(ob);//�Q�[���G���W���ɓo�^
			}
			if (!ob->B_Initialize()) {
				ob->Kill();//�C�j�V�����C�Y�Ɏ��s������Kill
			}
			return  ob;
		}
		return nullptr;
	}
	//-------------------------------------------------------------------
	bool  Object::B_Initialize()
	{
		return  this->Initialize();
	}
	//-------------------------------------------------------------------
	Object::~Object() { this->B_Finalize(); }
	bool  Object::B_Finalize()
	{
		auto  rtv = this->Finalize();
		return  rtv;
	}
	//-------------------------------------------------------------------
	Object::Object() {	}
	//-------------------------------------------------------------------
	//���\�[�X�N���X�̐���
	Resource::SP  Resource::Create()
	{
		if (auto sp = instance.lock()) {
			return sp;
		}
		else {
			sp = Resource::SP(new  Resource());
			if (sp) {
				sp->Initialize();
				instance = sp;
			}
			return sp;
		}
	}
	//-------------------------------------------------------------------
	Resource::Resource() {}
	//-------------------------------------------------------------------
	Resource::~Resource() { this->Finalize(); }
}