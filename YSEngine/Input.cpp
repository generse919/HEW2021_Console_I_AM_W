#include "Input.h"

//private変数
static PInput pinp = NULL;
static POINT po;

BOOL Init(PInput const p_this)
{
	pinp = p_this;
	//入力状態の初期化(FALSE埋め)
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

	//キー入力状態
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

	//マウスハンドラ
	INPUT_RECORD inbuf[32];
	DWORD events = 0;

	//ハンドルイベント
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
	//デスクトップ上のマウス座標
	GetCursorPos(&po);

	//マウス状態
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
*@brief		スクリーン上のマウス座標Xを取得
*/
int GetScreenMouseX()
{
	return pinp->m_mousePosX;
}
/*
*@brief		スクリーン上のマウス座標yを取得
*/
int GetScreenMouseY()
{
	return pinp->m_mousePosY;
}
/*
*@brief		デスクトップ上のマウス座標xを取得
*/
int GetDesktopMousePosX()
{
	return po.x;
}
/*
*@brief		デスクトップ上のマウス座標yを取得
*/
int GetDesktopMousePosY()
{
	return po.y;
}
/*
*@brief		マウス座標をセットする
*
*@param		x,y	任意のマウス座標
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
 * @brief	ジョイパッド入力(XInput対応)
 *
 * @param	port [入力] ポート番号(P*_*)
 *
 * @retval	0		正常終了
 * @retval	1以上	Joystickの入力値
 * @retval	-1		エラー
 *
 * @note
 *	LRのスティックは中心から左右に移動する際のデッドゾーンが用意されている
 *	（デッドゾーンは左右に移動したとみなさないエリアのこと）
 *	 #define XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE  7849
 *	 #define XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE 8689
 */
int InputJoystickX(int id, int port)
{
	XINPUT_STATE controller_state;	// XInputコントローラ情報

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

