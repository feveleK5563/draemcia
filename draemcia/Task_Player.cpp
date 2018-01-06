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
		render2D_Priority[1] = 0.7f;

		pos = { float(ge->screen2DWidth) + 80,
				float(ge->screen2DHeight) - 60};
		basisSpeed = 2.f;
		life = LifeMax;
		swordLength = 16;
		speed.x = -basisSpeed;
		angleLR = Left;

		state = State1;	//State1�c �ʏ���
						//State2�c ���U�����
						//State3�c ���S

		hitDamage = false;
		start = false;

		forceOfJump = -9.5f;

		hitBase = { -12, -13, 24, 29 };
		swordHitBase = { 16, -16, (int)swordLength + 9, 20 };

		srcNum = 0;
		//�L�����`�b�v�ǂݍ���
		for (int y = 0; y < 4; ++y)
		{
			for (int x = 0; x < 6; ++x)
			{
				charaChip.emplace_back(new ML::Box2D(x * 32, y * 32, 32, 32));
			}
		}

		FrameCreate();

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

		FrameErase();

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

		if (start)
		{
			if (state != Non && state != State3)
			{
				//�{�^�����͂ɂ��ړ����x�ύX
				ChangeSpeed();

				//��ʊO�����X���ړ�
				OutCheckMove();
			}
			else if (state == State3)
			{
				FallAndJump(false);
			}
		}
		else
		{
			if (pos.x < 600 - 16)
			{
				start = true;
			}
			else
			{
				speed.x = -basisSpeed;
				FallAndJump(false);

				NomalMove();
			}
		}

		//�����ڐG�����Y���ړ�
		CheckFootMove();

		//�A�j���[�V����
		if (hitDamage)
			DamageAnim();

		++animCnt;
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		//�L�����N�^�`�� (�u�v���C���[�v�u���g�v�u����v�̎O��)
		ML::Box2D draw;
		ML::Color color = { 1.f, 1.f, 1.f, 1.f };
		int *changeDXY = &draw.x,
			*changeDWH = &draw.w;

		switch (state)
		{
		case State1: //���n��
			srcNum = (animCnt / 8 % 2) * 3;
			break;

		case State2: //�W�����v��
			srcNum = 6;
			changeDXY = &draw.y;
			changeDWH = &draw.h;
			break;

		case State3: //��
			srcNum = 9;
			break;

		default:
			//���W�������������ւԂ���΂�
			pos = { -1000, -1000 };
			return;
		}

		if (hitDamage) //��_����
		{
			srcNum += 12;
			if (cntTime > 5)
			{
				color = { 1.f, 1.f, 0.1f, 0.1f };
			}
		}

		//�v���C���[�̕\��
		{
			draw = { -16, -16, 32, 32 };
			TurnaroundDraw(draw, *charaChip[srcNum], 0, color);
		}

		//���Ԃ͈ȉ��̏����s�v
		if (state == State3)
			return;

		//���g�̕\��
		int plusSL = swordLength;
		*changeDXY += 32;
		ML::Box2D src;
		int* changeSWH;
		if (srcNum == 6)
			changeSWH = &src.h;
		else
			changeSWH = &src.w;
		while (plusSL != 0)
		{
			src = *charaChip[srcNum + 1];
			if (plusSL >= 32)
			{
				*changeDWH = 32;
				*changeSWH = 32;
				plusSL -= 32;
			}
			else
			{
				*changeDWH = plusSL;
				*changeSWH = plusSL;
				plusSL = 0;
			}
			TurnaroundDraw(draw, src, 1, color);
			*changeDXY += *changeDWH;
		}

		//����̕\��
		{
			*changeDWH = 32;
			TurnaroundDraw(draw, *charaChip[srcNum + 2], 2, color);
		}

		//�����蔻������v��
		RenderFrameBlue(hitBase);
		RenderFrameRed(swordHitBase);
	}

	//-------------------------------------------------------------------
	//�����]���`��
	void Object::TurnaroundDraw(const ML::Box2D& draw, const ML::Box2D& src, int i, const ML::Color& color)
	{
		ML::Box2D cpydr = draw;
		ML::Box2D cpysr = src;
		if (angleLR == Left)
		{
			cpysr.x += src.w;
			cpysr.w *= -1;
			if (state == State1 || state == State3)
			{
				switch (i)
				{
				case 1:
					cpydr.x -= 32 + swordLength;
					break;

				case 2:
					cpydr.x -= (32 + swordLength) * 2;
					break;
				}
			}
		}

		cpydr.Offset(pos);
		DG::Image_Draw(res->imageName, cpydr, cpysr, color);
	}

	//-------------------------------------------------------------------
	//�{�^�����͂ɂ��ړ��X�s�[�h�̕ύX
	void Object::ChangeSpeed()
	{
		//�v���C���[�������Ă�������ɐi�܂���
		bool Ldown = in.LStick.L.down,
			 Rdown = in.LStick.R.down;
		if (angleLR == Left)
		{
			swordHitBase = { -((int)swordLength + 8) - 16, -5, (int)swordLength + 8, 21 };
			if (Ldown && speed.x == -basisSpeed && hitFoot) //�X�s�[�h�㏸
			{
				speed.x = -basisSpeed * 2.2f;
			}
			if (Rdown) //�����]��
			{
				angleLR = Right;
				speed.x = basisSpeed;
			}
			if (!speed.x) //������������̕��A
			{
				speed.x = -basisSpeed;
			}
		}
		else
		{
			swordHitBase = { 16, -5, (int)swordLength + 8, 21 };
			if (Rdown && speed.x == basisSpeed && hitFoot)
			{
				speed.x = basisSpeed * 2.2f;
			}
			if (Ldown) 
			{
				angleLR = Left;
				speed.x = -basisSpeed;
			}
			if (!speed.x)
			{
				speed.x = basisSpeed;
			}
		}

		//�X�s�[�h�̐�Βl���ő�l��荂���Ƃ���
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
		else 
		{
			FallAndJump(jumpButton);
		}

		if (state == State2)
		{
			swordHitBase = { -16, 16, 32, (int)swordLength + 7 };
			fallSpeed += 1.5f;
			speed.x = 0.f;

			if (hitFoot)
			{
				state = State1;
			}
		}
	}

	//-------------------------------------------------------------------
	//��_���[�W���̃A�j���[�V����
	void Object::DamageAnim()
	{
		if (cntTime > 10)
		{
			hitDamage = false;
			cntTime = 0;
		}
		else
		{
			++cntTime;
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