
//Camera.cpp

#include	"Camera.h"
#include	"keyboard.h"

Camera		g_Camera;	//�J�����Ǘ��\����


void	InitCamera()
{
	g_Camera.Position = XMFLOAT3(0.0f, 0.9f, -2.6f);
	g_Camera.UpVector = XMFLOAT3(0.0f, 1.0f, 0.0f);
	g_Camera.AtPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Camera.Fov = 45.0f;
	g_Camera.Nearclip = 0.1f;
	g_Camera.Farclip = 1000.0f;

	g_Camera.Yaw = 0.0f;
	g_Camera.Pitch = 0.0f;

	// --- ���x�Ƒ��x�̏����l������������ ---
	g_Camera.MoveSpeed = 0.03f;          // �����ړ����x��x�� (��: 0.1f)
	g_Camera.MouseSensitivity = 0.0008f; // �}�E�X���x��Ⴍ (��: 0.002f)
}

void	FinalizeCamera()
{

}

void	UpdateCamera()
{

	//g_Camera.Rotation = 0;

	//if (Keyboard_IsKeyDown(KK_A))
	//{
	//	g_Camera.Rotation = 0.3f;
	//}
	//else if (Keyboard_IsKeyDown(KK_D))
	//{
	//	g_Camera.Rotation = -0.3f;
	//}

	//float co = cosf(XMConvertToRadians(g_Camera.Rotation));
	//float si = sinf(XMConvertToRadians(g_Camera.Rotation));
	//float posx = (g_Camera.Position.x * co) - (g_Camera.Position.z * si);
	//float posz = (g_Camera.Position.x * si) + (g_Camera.Position.z * co);

	//g_Camera.Position.x = posx;
	//g_Camera.Position.z = posz;

	static bool isTracking = false;
	static POINT savedMousePos = { 0, 0 };
	// --- �ǉ��F�E�N���b�N�J�n�̍ŏ��̃t���[�����𔻒肷��t���O ---
	static bool isFirstFrame = false;

	ImGuiIO& io = ImGui::GetIO();

	// ==============================
	// 0. �E�N���b�N�̓��͔��� (Windows API)
	// ==============================
	if (io.WantCaptureKeyboard) return;

	bool isRightMouseDown = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;

	if (isRightMouseDown && !isTracking)
	{
		if (!io.WantCaptureMouse)
		{
			isTracking = true;
			isFirstFrame = true; // --- �ŏ��̃t���[���ł��邱�Ƃ��L�^ ---
			GetCursorPos(&savedMousePos);
			ShowCursor(FALSE);
		}
	}
	else if (!isRightMouseDown && isTracking)
	{
		isTracking = false;
		SetCursorPos(savedMousePos.x, savedMousePos.y);
		ShowCursor(TRUE);
	}

	// ==============================
	// 1. �}�E�X�X�N���[���ɂ�鑬�x�ύX�i�����j
	// ==============================
	int wheelDelta = GetMouseWheelDelta();
	if (wheelDelta != 0)
	{
		if (!io.WantCaptureMouse)
		{
			float speedChange = (float)(wheelDelta / 120) * 0.005f;
			g_Camera.MoveSpeed += speedChange;
			if (g_Camera.MoveSpeed < 0.005f) g_Camera.MoveSpeed = 0.005f;
			if (g_Camera.MoveSpeed > 0.5f)   g_Camera.MoveSpeed = 0.5f;
		}
		ClearMouseWheelDelta();
	}

	// ==============================
	// 2. �}�E�X�ɂ�鎋�_����i�E�N���b�N���̂ݎ��s�j
	// ==============================
	float mouseMoveX = 0.0f;
	float mouseMoveY = 0.0f;

	if (isTracking)
	{
		int centerX = GetSystemMetrics(SM_CXSCREEN) / 2;
		int centerY = GetSystemMetrics(SM_CYSCREEN) / 2;

		POINT currentPos;
		GetCursorPos(&currentPos);

		// �ŏ��̃t���[�������́A��ʒ����ւ̋������[�v���s��������
		// �ړ��ʂ̌v�Z�i���_�̉�]�j���X�L�b�v����
		if (isFirstFrame)
		{
			mouseMoveX = 0.0f;
			mouseMoveY = 0.0f;
			isFirstFrame = false; // 2�t���[���ڈȍ~�̂��߂ɉ���
		}
		else
		{
			// 2�t���[���ڈȍ~�́A��ɒ�������̈ړ��ʂ𐳂����v�Z
			mouseMoveX = (float)(currentPos.x - centerX);
			mouseMoveY = (float)(currentPos.y - centerY);
		}

		// ���t���[�����S�ɌŒ�
		SetCursorPos(centerX, centerY);

		// �p�x�̍X�V
		g_Camera.Yaw += mouseMoveX * g_Camera.MouseSensitivity;
		g_Camera.Pitch -= mouseMoveY * g_Camera.MouseSensitivity;

		if (g_Camera.Pitch > XM_PIDIV2 - 0.01f) g_Camera.Pitch = XM_PIDIV2 - 0.01f;
		if (g_Camera.Pitch < -XM_PIDIV2 + 0.01f) g_Camera.Pitch = -XM_PIDIV2 + 0.01f;
	}

	// �����_�v�Z
	float cosPitch = cosf(g_Camera.Pitch);
	XMFLOAT3 lookDir = XMFLOAT3(
		cosPitch * sinf(g_Camera.Yaw),
		sinf(g_Camera.Pitch),
		cosPitch * cosf(g_Camera.Yaw)
	);
	g_Camera.AtPosition = XMFLOAT3(
		g_Camera.Position.x + lookDir.x,
		g_Camera.Position.y + lookDir.y,
		g_Camera.Position.z + lookDir.z
	);

	// ==============================
	// 3. �L�[�{�[�h�ɂ��ړ������i�����j
	// ==============================
	if (isTracking)
	{
		XMFLOAT3 forwardDir = lookDir;
		XMFLOAT3 rightDir = XMFLOAT3(
			cosf(g_Camera.Yaw),
			0.0f,
			-sinf(g_Camera.Yaw)
		);

		if (Keyboard_IsKeyDown(KK_W))
		{
			g_Camera.Position.x += forwardDir.x * g_Camera.MoveSpeed;
			g_Camera.Position.y += forwardDir.y * g_Camera.MoveSpeed;
			g_Camera.Position.z += forwardDir.z * g_Camera.MoveSpeed;
		}
		if (Keyboard_IsKeyDown(KK_S))
		{
			g_Camera.Position.x -= forwardDir.x * g_Camera.MoveSpeed;
			g_Camera.Position.y -= forwardDir.y * g_Camera.MoveSpeed;
			g_Camera.Position.z -= forwardDir.z * g_Camera.MoveSpeed;
		}
		if (Keyboard_IsKeyDown(KK_D))
		{
			g_Camera.Position.x += rightDir.x * g_Camera.MoveSpeed;
			g_Camera.Position.z += rightDir.z * g_Camera.MoveSpeed;
		}
		if (Keyboard_IsKeyDown(KK_A))
		{
			g_Camera.Position.x -= rightDir.x * g_Camera.MoveSpeed;
			g_Camera.Position.z -= rightDir.z * g_Camera.MoveSpeed;
		}

		if (Keyboard_IsKeyDown(KK_E))
		{
			g_Camera.Position.y += g_Camera.MoveSpeed;
		}
		if (Keyboard_IsKeyDown(KK_Q))
		{
			g_Camera.Position.y -= g_Camera.MoveSpeed;
		}
	}
}

void	DrawCamera()//3D�g�p��
{
	////�v���W�F�N�V�����s����쐬
	//XMMATRIX	ProjectionMatrix =
	//	XMMatrixPerspectiveFovLH(
	//		XMConvertToRadians(g_Camera.Fov),
	//		(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
	//		g_Camera.Nearclip,
	//		g_Camera.Farclip
	//	);
	////�v���W�F�N�V�����s����Z�b�g
	//SetProjectionMatrix(ProjectionMatrix);

	////�J�����s����쐬
	//XMVECTOR	eyev = XMLoadFloat3(&g_Camera.AtPosition);
	//XMVECTOR	pos = XMLoadFloat3(&g_Camera.Position);
	//XMVECTOR	up = XMLoadFloat3(&g_Camera.UpVector);
	//XMMATRIX	ViewMatrix =
	//	XMMatrixLookAtLH(pos, eyev, up);

	////�J�����s����Z�b�g
	//SetViewMatrix(ViewMatrix);
	////�J�������W�Z�b�g
	//SetCameraPosition(g_Camera.Position);

	// �v���W�F�N�V�����s����쐬
	XMMATRIX ProjectionMatrix =
		XMMatrixPerspectiveFovLH(
			XMConvertToRadians(g_Camera.Fov),
			(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
			g_Camera.Nearclip,
			g_Camera.Farclip
		);
	// �v���W�F�N�V�����s����Z�b�g
	SetProjectionMatrix(ProjectionMatrix);

	// �J�����s����쐬
	XMVECTOR eyev = XMLoadFloat3(&g_Camera.Position);
	XMVECTOR pos = XMLoadFloat3(&g_Camera.AtPosition);
	XMVECTOR up = XMLoadFloat3(&g_Camera.UpVector);
	XMMATRIX ViewMatrix =
		XMMatrixLookAtLH(eyev, pos, up); // �������Ƒ��������t�]�����Ă��܂�

	// �J�����s����Z�b�g
	SetViewMatrix(ViewMatrix);
	// �J�������W�Z�b�g
	SetCameraPosition(g_Camera.Position);
}




