//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Slime.h"

namespace  Slime
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		imageName = "Slime";
		DG::Image_Create(imageName, "./data/image/Slime.png");
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
		render2D_Priority[1] = 0.8f;
		state = State1;		//State1 = ��ʉ����甇���オ�铮��
							//State2 = ������̂��̂�
							//State3 = ���ʊԍ�

		pos = { float(rand() % (int(ge->screen2DWidth) - 32)) + 32,
				float(ge->screen2DHeight) + 16 };
		hitBase = { -16, -16, 32, 32 };
		
		//�L�����`�b�v�ǂݍ���
		for (int y = 0; y < 4; ++y)
		{
			for (int x = 0; x < 2; ++x)
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
		case BChara::State1:
			Move1();
			break;

		case BChara::State2:
			Move2();
			break;

		case BChara::State3:
			Move3();
			break;

		default:
			return;
		}
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		if (state == Non)
			return;

		ML::Box2D src;
		ML::Color color = { 1.f, 1.f, 1.f, 1.f };

		if (state == State3)
		{
			src = *charaChip[stateAnim];
			if (animCnt > 3)
				color = { 1.f, 1.f, 0.1f, 0.1f };
		}
		else
		{
			src = *charaChip[animCnt / 10 % 2 + stateAnim];
		}

		if (angleLR == Left) //���]
		{
			src.x += src.w;
			src.w *= -1;
		}

		ML::Box2D draw = { -16, -16, 32, 32 };
		draw.Offset(pos);
		DG::Image_Draw(res->imageName, draw, src, color);
		RenderFrameRed(hitBase);
	}

	//-------------------------------------------------------------------
	//State1���̓���
	void Object::Move1()
	{
		++animCnt;

		if (pos.y < ge->screen2DHeight - 30)
		{
			speed.y = -1.5f;
			if (pos.y < ge->screen2DHeight - 45)
			{
				pos.y = float(ge->screen2DHeight) - 51;
				state = State2;
				stateAnim = 2;
				speed.y = 0.f;

				animCnt = 0;
				moveType = rand() % 3;
			}
		}
		else
		{
			speed.y = -0.7f;
		}

		NomalMove();

		if (DamageEnemy())
			stateAnim += 4;
	}

	//-------------------------------------------------------------------
	//State2���̓���
	void Object::Move2()
	{
		switch (moveType)
		{
		case 0:	//�ҋ@
			speed.x = 0;
			++cntTime;
			if (cntTime > 60)
			{
				cntTime = 0;
				moveType = rand() % 2 + 1;
			}
			break;

		case 1:	//�E�Ɍ����Ĉړ�
			angleLR = Right;
			speed.x = 1.f;
			++cntTime;
			if (cntTime > 90)
			{
				cntTime = 0;
				moveType = 0;
			}
			break;

		case 2: //���Ɍ����Ĉړ�
			angleLR = Left;
			speed.x = -1.f;
			++cntTime;
			if (cntTime > 90)
			{
				cntTime = 0;
				moveType = 0;
			}
			break;
		}
		++animCnt;

		FallAndJump(false);
		OutCheckMove();
		CheckFootMove();

		if (DamageEnemy())
			stateAnim += 4;
		else
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

	//-------------------------------------------------------------------
	//State3���̓���

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