
//Camera.cpp

#include	"Camera.h"
#include	"keyboard.h"

Camera		g_Camera;	//カメラ管理構造体


void	InitCamera()
{
	g_Camera.Position = XMFLOAT3(0.0f, 1.0f, -5.0f);
	g_Camera.UpVector = XMFLOAT3(0.0f, 1.0f, 0.0f);
	g_Camera.AtPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Camera.Fov = 45.0f;
	g_Camera.Nearclip = 0.1f;
	g_Camera.Farclip = 1000.0f;

	g_Camera.Yaw = 0.0f;
	g_Camera.Pitch = 0.0f;

	// --- 感度と速度の初期値を小さく調整 ---
	g_Camera.MoveSpeed = 0.03f;          // 初期移動速度を遅く (旧: 0.1f)
	g_Camera.MouseSensitivity = 0.0008f; // マウス感度を低く (旧: 0.002f)
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
	// --- 追加：右クリック開始の最初のフレームかを判定するフラグ ---
	static bool isFirstFrame = false;

	ImGuiIO& io = ImGui::GetIO();

	// ==============================
	// 0. 右クリックの入力判定 (Windows API)
	// ==============================
	if (io.WantCaptureKeyboard) return;

	bool isRightMouseDown = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;

	if (isRightMouseDown && !isTracking)
	{
		if (!io.WantCaptureMouse)
		{
			isTracking = true;
			isFirstFrame = true; // --- 最初のフレームであることを記録 ---
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
	// 1. マウススクロールによる速度変更（中略）
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
	// 2. マウスによる視点操作（右クリック中のみ実行）
	// ==============================
	float mouseMoveX = 0.0f;
	float mouseMoveY = 0.0f;

	if (isTracking)
	{
		int centerX = GetSystemMetrics(SM_CXSCREEN) / 2;
		int centerY = GetSystemMetrics(SM_CYSCREEN) / 2;

		POINT currentPos;
		GetCursorPos(&currentPos);

		// 最初のフレームだけは、画面中央への強制ワープを行うだけで
		// 移動量の計算（視点の回転）をスキップする
		if (isFirstFrame)
		{
			mouseMoveX = 0.0f;
			mouseMoveY = 0.0f;
			isFirstFrame = false; // 2フレーム目以降のために解除
		}
		else
		{
			// 2フレーム目以降は、常に中央からの移動量を正しく計算
			mouseMoveX = (float)(currentPos.x - centerX);
			mouseMoveY = (float)(currentPos.y - centerY);
		}

		// 毎フレーム中心に固定
		SetCursorPos(centerX, centerY);

		// 角度の更新
		g_Camera.Yaw += mouseMoveX * g_Camera.MouseSensitivity;
		g_Camera.Pitch -= mouseMoveY * g_Camera.MouseSensitivity;

		if (g_Camera.Pitch > XM_PIDIV2 - 0.01f) g_Camera.Pitch = XM_PIDIV2 - 0.01f;
		if (g_Camera.Pitch < -XM_PIDIV2 + 0.01f) g_Camera.Pitch = -XM_PIDIV2 + 0.01f;
	}

	// 注視点計算
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
	// 3. キーボードによる移動処理（中略）
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

void	DrawCamera()//3D使用時
{
	////プロジェクション行列を作成
	//XMMATRIX	ProjectionMatrix =
	//	XMMatrixPerspectiveFovLH(
	//		XMConvertToRadians(g_Camera.Fov),
	//		(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
	//		g_Camera.Nearclip,
	//		g_Camera.Farclip
	//	);
	////プロジェクション行列をセット
	//SetProjectionMatrix(ProjectionMatrix);

	////カメラ行列を作成
	//XMVECTOR	eyev = XMLoadFloat3(&g_Camera.AtPosition);
	//XMVECTOR	pos = XMLoadFloat3(&g_Camera.Position);
	//XMVECTOR	up = XMLoadFloat3(&g_Camera.UpVector);
	//XMMATRIX	ViewMatrix =
	//	XMMatrixLookAtLH(pos, eyev, up);

	////カメラ行列をセット
	//SetViewMatrix(ViewMatrix);
	////カメラ座標セット
	//SetCameraPosition(g_Camera.Position);

	// プロジェクション行列を作成
	XMMATRIX ProjectionMatrix =
		XMMatrixPerspectiveFovLH(
			XMConvertToRadians(g_Camera.Fov),
			(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
			g_Camera.Nearclip,
			g_Camera.Farclip
		);
	// プロジェクション行列をセット
	SetProjectionMatrix(ProjectionMatrix);

	// カメラ行列を作成
	XMVECTOR eyev = XMLoadFloat3(&g_Camera.Position);
	XMVECTOR pos = XMLoadFloat3(&g_Camera.AtPosition);
	XMVECTOR up = XMLoadFloat3(&g_Camera.UpVector);
	XMMATRIX ViewMatrix =
		XMMatrixLookAtLH(eyev, pos, up); // 第一引数と第二引数を逆転させています

	// カメラ行列をセット
	SetViewMatrix(ViewMatrix);
	// カメラ座標セット
	SetCameraPosition(g_Camera.Position);
}




