//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_EarthBomb.h"
#include  "Task_BossDragon.h"

namespace  EarthBomb
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		imageName = "EarthBomb";
		DG::Image_Create(imageName, "./data/image/EarthBomb.png");
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

		render2D_Priority[1] = 0.9f;

		state = State1;		//State1 = ����
							//State2 = �y��

		if (auto bd = ge->GetTask_One_GN<BossDragon::Object>("�{�X�G", "�h���S��"))
		{
			pos = bd->pos;
			pos.x += 50;
			pos.y -= 10;
		}
		hitBase = { -32, -16, 64, 32 };
		draw    = { -32, -16, 64, 32 };

		speed.x = (float(rand() % 70) / 10.f) + 1.f;
		speed.y = -10.f;
		angleLR = Right;
		
		dokaan = true;

		charaChip.emplace_back(new ML::Box2D(0, 0, 64, 32));
		for (int i = 0; i < 2; ++i)
			charaChip.emplace_back(new ML::Box2D(64 * i, 32, 64, 96));


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
		case BChara::State1:	//����
			Move1();
			break;

		case BChara::State2:	//�y��
			Move2();
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
	void Object::Move1()
	{
		if (hitFoot)
		{
			state = State2;
			pos.y += 16;
			hitBase = { -20, -96, 40, 96 };
			draw = { -32, -96, 64, 96 };
			stateAnim = 1;
			return;
		}

		FallAndJump(false);
		OutCheckMove();
		CheckFootMove();
	}

	//-------------------------------------------------------------------
	//State2���̓���
	void Object::Move2()
	{
		++animCnt;
		if (animCnt > 60)
		{
			visible -= 0.05f;
			if (visible < 0.f)
			{
				Kill();
			}
			return;
		}

		if (dokaan && pos.y > 0 && animCnt % 2)
		{
			dokaan = false;

			auto eb = EarthBomb::Object::Create(true);
			eb->state = State2;
			eb->pos.x = pos.x;
			eb->pos.y = pos.y - 96;
			eb->hitBase = { -20, -96, 40, 96 };
			eb->draw = { -32, -96, 64, 96 };
			eb->stateAnim = 1;
			eb->animCnt = animCnt;
		}

		DamagePlayer();
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