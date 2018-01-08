//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Player.h"
#include  "Task_Snake.h"

namespace  Snake
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		imageName = "Snake";
		DG::Image_Create(imageName, "./data/image/Snake.png");
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
		state = State1;		//State1 = �n�ʃ{�R�{�R
							//State2 = �{�̂��Ԃ�[���ăV�����[
							//Death  = ���ʊԍ�

		pos = { float(rand() % (int(ge->screen2DWidth) - 32)) + 16,
				float(ge->screen2DHeight) - 51 };
		angleLR = rand() % 2 ? Left : Right;
		hitBase = { -15, -15, 30, 30 };
		
		//�L�����`�b�v�ǂݍ���
		for (int y = 0; y < 3; ++y)
		{
			for (int x = 0; x < 3; ++x)
			{
				charaChip.emplace_back(new ML::Box2D(x * 32, y * 32, 32, 32));
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
		case BChara::State1: //�n�ʃ{�R�{�R
			Move1();
			break;

		case BChara::State2: //�{�̂��o�Ă�
			Move2();
			break;

		case BChara::Death: //���ʊԍ�
			Move3();
			break;

		default:
			return;
		}

		if (ZakoDelete())
		{
			if (0 <= stateAnim && stateAnim <= 2)
				KillMeBaby();
			else
				stateAnim += 3;
		}
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		if (state == State1)
			EnemyRender(3);
		else
			EnemyRender();
	}

	//-------------------------------------------------------------------
	//State1���̓���
	void Object::Move1()
	{
		if (animCnt > 144)
		{
			animCnt = 0;
			state = State2;
			stateAnim = 3;
			moveType = 0;
			pos.y -= 10;
			render2D_Priority[1] = 0.7f;
		}
		else
		{
			++animCnt;
		}
	}

	//-------------------------------------------------------------------
	//State2���̓���
	void Object::Move2()
	{
		++cntTime;
		switch (moveType)
		{
		case 0:		//�o������쓮
			if (cntTime > 60)
			{
				auto player = ge->GetTask_One_GN<Player::Object>("�v���C���[", "NoName");
				if (player == nullptr)
					return;

				if (pos.x < player->pos.x)
				{
					angleLR = Right;
					speed.x = 4;
				}
				else
				{
					angleLR = Left;
					speed.x = -3;
				}
				cntTime = 0;
				moveType = 1;
			}
			break;

		case 1:		//�߂����ᑖ��
			if (cntTime > 90)
			{
				moveType = 2;
				cntTime = 0;
				speed.x = 0;
			}
			break;

		case 2:		//��������̑ҋ@
			if (cntTime > 60)
			{
				moveType = 0;
				cntTime = 0;
			}
			break;

		default:
			break;
		}

		FallAndJump(false);
		OutCheckMove();
		CheckFootMove();

		++animCnt;

		if (DamageEnemy())
			stateAnim += 3;

		DamagePlayer();
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