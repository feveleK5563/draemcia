//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Dramos.h"

namespace Dramos
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		imageName = "Dramos";
		DG::Image_Create(imageName, "./data/image/Dramos.png");
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
		render2D_Priority[1] = 0.5f;
		state = State1;		//State1 = ��ʏォ��~��
							//State2 = �ӂ�ӂ���
							//Death  = ���ʊԍ�

		pos = { float(rand() % (int(ge->screen2DWidth) - 32)) + 16,
				-32 };
		angleLR = rand() % 2 ? Left : Right;
		hitBase = { -15, -15, 30, 30 };

		//�L�����`�b�v�ǂݍ���
		for (int y = 0; y < 2; ++y)
		{
			for (int x = 0; x < 2; ++x)
			{
				charaChip.emplace_back(new ML::Box2D(x * 32, y * 32, 32, 32));
			}
		}

		moveX = 0;
		moveY = 0;

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
		case BChara::State1: //��ʏォ��~��
			Move1();
			break;

		case BChara::State2: //�ӂ�ӂ���
			Move2();
			break;

		case BChara::Death: //���ʊԍ�
			Move3();
			break;

		default:
			return;
		}

		if (ZakoDelete())
			stateAnim += 2;
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		EnemyRender();
	}

	//-------------------------------------------------------------------
	//State1���̓���
	void Object::Move1()
	{
		if (moveY > 180)
		{
			moveY = 0;
			state = State2;
			speed.y = 0;
			moveType = rand() % 3;
			if (moveType == 1)
			{
				if (rand() % 2)
				{
					angleLR = Left;
					speed.x = -1.5f;
				}
				else
				{
					angleLR = Right;
					speed.x = 1.5f;
				}
			}
		}
		else
		{
			moveY += 2;
			speed.y = float(sin(ML::ToRadian(moveY))) * 2.5f;
		}
		NomalMove();
		++animCnt;

		if (DamageEnemy())
			stateAnim += 2;
		DamagePlayer();
	}

	//-------------------------------------------------------------------
	//State2�̓���
	void Object::Move2()
	{
		switch (moveType)
		{
		case 0:		//�ҋ@
			if (cntTime == 120)
			{
				if (rand() % 2)
				{
					moveType = 1;
					if (rand() % 2)
					{
						angleLR = Left;
						speed.x = -1.5f;
					}
					else
					{
						angleLR = Right;
						speed.x = 1.5f;
					}
				}
				else
				{
					moveType = 2;
				}
			}

		case 1:		//���E�ړ�
			if (cntTime == 90)
			{
				speed.x = 0;
				moveType = 0;
			}
			break;
			
		case 2:		//��������
			if (cntTime == 120)
			{
				moveType = rand() % 2;
			}
			break;

		default:
			return;
		}
		
		moveY += 3;
		if (moveType == 2)
			speed.y = float(sin(ML::ToRadian(moveY))) * 2.4f;
		else
			speed.y = float(sin(ML::ToRadian(moveY))) / 1.5f;


		if (cntTime == 120)
			cntTime = 0;
		else
			++cntTime;

		pos.y += speed.y;
		OutCheckMove();

		if (DamageEnemy())
			stateAnim += 2;
		DamagePlayer();

		++animCnt;
	}

	//-------------------------------------------------------------------
	//State3���̓���
	void Object::Move3()
	{
		if (animCnt > 10)
		{
			state = Non;
			KillMeBaby();
		}
		else
		{
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