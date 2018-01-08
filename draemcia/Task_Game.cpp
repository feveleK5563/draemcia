//-------------------------------------------------------------------
//�Q�[���{��
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Title.h"
#include  "Task_Game.h"
#include  "Task_GameBG.h"
#include  "Task_Field.h"
#include  "Task_Player.h"
#include  "Task_UI.h"

#include  "Task_Slime.h"
#include  "Task_Dramos.h"
#include  "Task_Snake.h"
#include  "Task_BossDragon.h"

namespace  Game
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
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
		srand((unsigned)time(NULL));
		monsterAmount = 0;
		appMonsterTime = 0;
		score = 0;
		nextLevelBoader = Next2;
		level = Level1;
		
		//���^�X�N�̐���
		//�w�i�^�X�N
		auto  bg = GameBG::Object::Create(true);
		//�t�B�[���h�^�X�N
		auto  fd = Field::Object::Create(true);
		//�v���C���^�X�N
		auto  pl = Player::Object::Create(true);
		//UI�^�X�N
		auto  ui = UI::Object::Create(true);

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�I���v�^�X�N���Ŏ��ɂP�񂾂��s������
	bool  Object::Finalize()
	{
		//���f�[�^���^�X�N���
		ge->KillAll_G("�{��");
		ge->KillAll_G("�t�B�[���h");
		ge->KillAll_G("�v���C���[");
		ge->KillAll_G("�G");
		ge->KillAll_G("�{�X�G");

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//�������p���^�X�N�̐���
			auto nextTask = Title::Object::Create(true);
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�X�V�v�P�t���[�����ɍs������
	void  Object::UpDate()
	{
		auto in = DI::GPad_GetState("P1");

		//���x���̏㏸
		if (score >= nextLevelBoader)
		{
			switch (level)
			{
			case Level1: level = Level2; nextLevelBoader = Next3; break;
			case Level2: level = Level3; nextLevelBoader = Next4; break;
			case Level3: level = Level4; nextLevelBoader = Next5; break;
			case Level4: level = Level5; nextLevelBoader = Next6; break;
			case Level5: level = Level6; nextLevelBoader = Next7; break;
			case Level6: level = Level7; nextLevelBoader = Next8; break;
			case Level7: level = Level8; nextLevelBoader = Next9; break;

			case Level8:
			{
				level = Level9;
				auto  bd = BossDragon::Object::Create(true);
			}
			break;

			}
		}

		//�����X�^�[�̏o��
		if (int(level) > 0 && !(appMonsterTime++ % int(level)))
		{
			if (monsterAmount < MonsterMaxAmount)
			{
				switch (rand() % 3)
				{
				case 0:
				{
					auto sm = Slime::Object::Create(true);
				}
					break;

				case 1:
				{
					auto dm = Dramos::Object::Create(true);
				}
					break;

				case 2:
				{
					auto sk = Snake::Object::Create(true);
				}
					break;
				}
				++monsterAmount;
			}
		}

		if (in.ST.down) {
			//���g�ɏ��ŗv��
			Kill();
		}
	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
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