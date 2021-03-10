#include "Input.h"

//private�ϐ�
static PInput pinp = NULL;
static POINT po;

BOOL Init(PInput const p_this)
{
	pinp = p_this;
	//���͏�Ԃ̏�����(FALSE����)
	memset(pinp->m_keys, FALSE, 256 * sizeof(sKeyState));
	memset(pinp->m_mouse, FALSE, 5 * 3 * sizeof(BOOL));
	memset(pinp->m_xinput, FALSE, 34 * 3 * sizeof(BOOL));

	ZeroMemory(pinp->m_keyNewState, 256 * sizeof(short));
	ZeroMemory(pinp->m_keyOldState, 256 * sizeof(short));
	ZeroMemory(pinp->m_mouseNewState, 5 * sizeof(BOOL));
	ZeroMemory(pinp->m_mouseOldState, 5 * sizeof(BOOL));

	pinp->m_mousePosX = 0;
	pinp->m_mousePosY = 0;

	pinp->m_bConsoleInFocus = TRUE;

	return TRUE;
}



//
void UpdateInputStates(PInput const p_this)
{
	//pinp = p_this;

	//�L�[���͏��
	for (int i = 0; i < 256; i++)
	{
		pinp->m_keyNewState[i] = GetAsyncKeyState(i);

		pinp->m_keys[i].bPressed = false;
		pinp->m_keys[i].bReleased = false;

		if (pinp->m_keyNewState[i] != pinp->m_keyOldState[i])
		{
			if (pinp->m_keyNewState[i] & 0x8000)
			{
				pinp->m_keys[i].bPressed = !pinp->m_keys[i].bHeld;
				pinp->m_keys[i].bHeld = TRUE;
			}
			else
			{
				pinp->m_keys[i].bReleased = TRUE;
				pinp->m_keys[i].bHeld = FALSE;
			}
		}

		pinp->m_keyOldState[i] = pinp->m_keyNewState[i];
	}

	//�}�E�X�n���h��
	INPUT_RECORD inbuf[32];
	DWORD events = 0;

	//�n���h���C�x���g
	for (DWORD i = 0; i < events; i++)
	{
		switch (inbuf[i].EventType)
		{
		case FOCUS_EVENT:
		{
			pinp->m_bConsoleInFocus = inbuf[i].Event.FocusEvent.bSetFocus;
		}
		break;

		case MOUSE_EVENT:
		{
			switch (inbuf[i].Event.MouseEvent.dwEventFlags)
			{
			case MOUSE_MOVED:
			{
				pinp->m_mousePosX = inbuf[i].Event.MouseEvent.dwMousePosition.X;
				pinp->m_mousePosY = inbuf[i].Event.MouseEvent.dwMousePosition.Y;
			}
			break;
			case 0:
			{
				for (int m = 0; m < 5; m++)
					pinp->m_mouseNewState[m] = (inbuf[i].Event.MouseEvent.dwButtonState & (1 << m)) > 0;
			}
			break;
			default:break;

			}
		}
		break;

		default:break;

		}
	}
	//�f�X�N�g�b�v��̃}�E�X���W
	GetCursorPos(&po);

	//�}�E�X���
	for (int m = 0; m < 5; m++)
	{
		pinp->m_mouse[m].bPressed = FALSE;
		pinp->m_mouse[m].bReleased = FALSE;

		if (pinp->m_mouseNewState[m] != pinp->m_mouseOldState[m])
		{
			if (pinp->m_mouseNewState[m])
			{
				pinp->m_mouse[m].bPressed = TRUE;
				pinp->m_mouse[m].bHeld = TRUE;
			}
			else
			{
				pinp->m_mouse[m].bReleased = TRUE;
				pinp->m_mouse[m].bHeld = FALSE;
			}
		}

		pinp->m_mouseOldState[m] = pinp->m_mouseNewState[m];
	}


	return;
}
/*
*@brief		�X�N���[����̃}�E�X���WX���擾
*/
int GetScreenMouseX()
{
	return pinp->m_mousePosX;
}
/*
*@brief		�X�N���[����̃}�E�X���Wy���擾
*/
int GetScreenMouseY()
{
	return pinp->m_mousePosY;
}
/*
*@brief		�f�X�N�g�b�v��̃}�E�X���Wx���擾
*/
int GetDesktopMousePosX()
{
	return po.x;
}
/*
*@brief		�f�X�N�g�b�v��̃}�E�X���Wy���擾
*/
int GetDesktopMousePosY()
{
	return po.y;
}
/*
*@brief		�}�E�X���W���Z�b�g����
*
*@param		x,y	�C�ӂ̃}�E�X���W
*/
void SetDesktopMousePos(int x,int y)
{
	SetCursorPos(x,y);
}
BOOL PressedKey(int pk)
{
	return (pinp->m_keys[pk].bPressed ? TRUE : FALSE);
}
BOOL ReleasedKey(int pk)
{
	return (pinp->m_keys[pk].bReleased ? TRUE : FALSE);
}
BOOL HeldKey(int pk)
{
	return (pinp->m_keys[pk].bHeld ? TRUE : FALSE);
}



/**
 * @brief	�W���C�p�b�h����(XInput�Ή�)
 *
 * @param	port [����] �|�[�g�ԍ�(P*_*)
 *
 * @retval	0		����I��
 * @retval	1�ȏ�	Joystick�̓��͒l
 * @retval	-1		�G���[
 *
 * @note
 *	LR�̃X�e�B�b�N�͒��S���獶�E�Ɉړ�����ۂ̃f�b�h�]�[�����p�ӂ���Ă���
 *	�i�f�b�h�]�[���͍��E�Ɉړ������Ƃ݂Ȃ��Ȃ��G���A�̂��Ɓj
 *	 #define XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE  7849
 *	 #define XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE 8689
 */
int InputJoystickX(int id, int port)
{
	XINPUT_STATE controller_state;	// XInput�R���g���[�����

	if (XInputGetState(id, &controller_state) != ERROR_SUCCESS) {
		return -1;
	}

	switch (port) {
	case PJX_LXPOS:
		return controller_state.Gamepad.sThumbLX;
	case PJX_LYPOS:
		return controller_state.Gamepad.sThumbLY;
	case PJX_LTRG:
		return controller_state.Gamepad.bLeftTrigger;
	case PJX_RXPOS:
		return controller_state.Gamepad.sThumbRX;
	case PJX_RYPOS:
		return controller_state.Gamepad.sThumbRY;
	case PJX_RTRG:
		return controller_state.Gamepad.bRightTrigger;
	case PJX_BTNS:
		if (controller_state.dwPacketNumber) {
			return controller_state.Gamepad.wButtons;
		}
	}
	return 0;
}

void StopInput()
{
	pinp = NULL;
}

