//-------------------------------------------------------------------
//�v���C���i��
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Player.h"

namespace  Player
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		this->imageName = "PlayerImg";
		DG::Image_Create(this->imageName, "./data/Image/Player.png");

		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
		DG::Image_Erase(this->imageName);
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
		controllerName = "P1";
		render2D_Priority[1] = 0.5f;

		pos = { float(ge->screen2DWidth / 2),
				float(ge->screen2DHeight / 2) };
		basisSpeed = 2.f;
		swordLength = 16;
		speed.x = -basisSpeed;
		angleLR = Left;

		state = State1;	//State1�c �ʏ���
						//State2�c ���U�����
						//State3�c ���S

		forceOfJump = -10.f;

		hitBase = { -16, -16, 32, 32 };

		//�L�����`�b�v�ǂݍ���
		for (int i = 0; i < 4; ++i)
		{
			charaChip.emplace_back(new ML::Box2D(80 * (i % 2), 32 * (i / 2), 32, 32));
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
		in = DI::GPad_GetState("P1");
		
		//�{�^�����͂ɂ��ړ����x�ύX
		ChangeSpeed();

		//��ʊO����
		OutCheckMove();
		//�����ڐG����
		CheckFoot();
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		//�L�����N�^�`��
		ML::Box2D draw[3];
		ML::Box2D src[3];
		for (int i = 0; i < 3; ++i)
		{
			draw[i] = { -16, -16, 32, 32 };
			switch (state)
			{
			case State1: //�ʏ���s���
				draw[i].x += i * 32;
				src[i] = *charaChip[0];
				src[i].x += i * 32;
				break;

			case State2: //�}���������
				src[0] = *charaChip[2];
				break;

			case State3: //���S���
				src[0] = *charaChip[3];
				break;

			default:
				return;
			}
			draw[i].Offset(pos);
			if (angleLR == Left)
			{
				src[i].x += src[i].w;
				src[i].w *= -1;
				if (state == State1)
				{
					//TODO:(+32)�̂Ƃ����swordLength�ɕύX���Ă�
					switch (i)
					{
					case 1:
						draw[i].x -= 32 + 32;
						break;

					case 2:
						draw[i].x -= 32 * 3 + 32;
						break;
					}
				}
			}
			DG::Image_Draw(this->res->imageName, draw[i], src[i]);
		}
	}

	//-------------------------------------------------------------------
	//�{�^�����͂ɂ��ړ��X�s�[�h�̕ύX
	void Object::ChangeSpeed()
	{
		//�{�^�����͂ɂ���ăv���C���[�̌�����ς���
		if (in.LStick.L.down)
		{
			if (hitFoot && state == State1 &&
				speed.x == -basisSpeed)
			{
				speed.x = -basisSpeed * 2;
			}
			else
			{
				speed.x = -basisSpeed;
				angleLR = Left;
			}
		}
		if (in.LStick.R.down)
		{
			if (hitFoot && state == State1 &&
				speed.x == basisSpeed)
			{
				speed.x = basisSpeed * 2;
			}
			else
			{
				speed.x = basisSpeed;
				angleLR = Right;
			}
		}

		//�X�s�[�h���ʏ��葬���Ƃ���
		//�X�s�[�h���X�s�[�h�ő�l��10����1��������
		if (speed.x < -basisSpeed)
		{
			speed.x += basisSpeed / 10.f;
			if (speed.x > -basisSpeed)
				speed.x = -basisSpeed;
		}
		else if (speed.x > basisSpeed)
		{
			speed.x -= basisSpeed / 10.f;
			if (speed.x < basisSpeed)
				speed.x = basisSpeed;
		}

		//�󒆂ɂ���Ƃ��ɃW�����v�{�^���������Ɖ��U����ԂɑJ�ڂ���
		bool jumpButton = in.B1.down;
		if (!hitFoot && jumpButton)
		{
			state = State2;
		}
		JumpAndFall(state == State1 && jumpButton);

		if (state == State2)
		{
			fallSpeed = 12.f;

			if (hitFoot)
			{
				state = State1;
			}
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