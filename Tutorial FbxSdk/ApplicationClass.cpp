#include "ApplicationClass.h"

ApplicationClass::ApplicationClass()
{
	m_Direct3D = 0;
	m_ColorShader = 0;
	m_ShaderClass = 0;
	m_CameraClass = 0;
	m_FbxLoader = 0;
	m_InfoUi = 0;
	m_TextClass = 0;

	
	model1_world = XMMatrixIdentity() * XMMatrixScaling(0.1f, 0.1f, 0.1f);
	model2_world = XMMatrixTranslation(5.0f, 5.0f, 0.0f) *XMMatrixScaling(5.0f, 5.0f, 5.0f);
}

ApplicationClass::~ApplicationClass()
{

}

bool ApplicationClass::Initialize(HINSTANCE hinstance, HWND hwnd, bool vsyncEnabled, bool fullScreen, float screenDepth, float screenNear)
{
	bool result;
	
	m_Direct3D = new D3DClass;
	if (!m_Direct3D)
	{
		return false;
	}

	result = m_Direct3D->Initialize(vsyncEnabled, hwnd, fullScreen, screenDepth, screenNear);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	m_ColorShader = new ColorShaderClass;
	if (!m_ColorShader)
	{
		return false;
	}

	result = m_ColorShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the ColorShaderClass.", L"Error", MB_OK);
		return false;
	}

	m_ShaderClass = new TextureShaderClass;
	if (!m_ShaderClass)
	{
		return false;
	}

	result = m_ShaderClass->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the TextureShaderClass.", L"Error", MB_OK);
		return false;
	}

	m_TextClass = new TextClass;
	if (!m_TextClass)
	{
		return false;
	}

	result = m_TextClass->Initialize(m_Direct3D);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the TextClass.", L"Error", MB_OK);
		return false;
	}

	m_InfoUi = new InfoUiClass;
	if (!m_InfoUi)
	{
		return false;
	}

	result = m_InfoUi->Initialize(m_Direct3D);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the InfoUiClass.", L"Error", MB_OK);
		return false;
	}

	m_FbxLoader = new FbxLoader;
	if (!m_FbxLoader)
	{
		return false;
	}

	result = m_FbxLoader->LoadFile(m_Direct3D->GetDevice(), hwnd, "..\\Data\\Models\\Timmy Binary.fbx");//Standing Taunt Battlecry
	if (!result)
	{
		MessageBox(hwnd, L"Could not load Fbx file.", L"Error", MB_OK);
		return false;
	}

	m_Triangle = new TriangleModel;
	if (!m_Triangle)
	{
		return false;
	}

	result = m_Triangle->Initialize(m_Direct3D->GetDevice());
	if (!result)
	{
		MessageBox(hwnd, L"Could not load ModelClass.", L"Error", MB_OK);
		return false;
	}

	m_CameraClass = new CameraClass;
	if (!m_CameraClass)
	{
		return false;
	}

	m_CameraClass->SetPosition(0.0f, 10.0f, -50.0f);

	return result;
}

void ApplicationClass::Shutdown()
{
	if (m_Triangle)
	{
		m_Triangle->Shutdown();
		delete m_Triangle;
		m_Triangle = 0;
	}

	if (m_FbxLoader)
	{
		m_FbxLoader->Shutdown();
		delete m_FbxLoader;
		m_FbxLoader = 0;
	}

	if (m_InfoUi)
	{
		m_InfoUi->Shutdown();
		delete m_InfoUi;
		m_InfoUi = 0;
	}

	if (m_TextClass)
	{
		m_TextClass->Shutdown();
		delete m_TextClass;
		m_TextClass = 0;
	}

	if (m_CameraClass)
	{
		delete m_CameraClass;
		m_CameraClass = 0;
	}

	if (m_ShaderClass)
	{
		m_ShaderClass->Shutdown();
		delete m_ShaderClass;
		m_ShaderClass = 0;
	}

	if (m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
	}

	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}
}

bool ApplicationClass::Frame()
{
	bool result;
	XMFLOAT3 position, rotation;
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;

	m_CameraClass->Render();
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_CameraClass->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	//백 버퍼 초기화
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.2f, 1.0f);

	
	model1_world = model1_world * XMMatrixRotationY(0.05f);//회전
	m_ShaderClass->Render(m_Direct3D->GetDeviceContext(), model1_world, viewMatrix, projectionMatrix);
	m_FbxLoader->Render(m_Direct3D->GetDeviceContext());

	m_ColorShader->Render(m_Direct3D->GetDeviceContext(), 3, model2_world, viewMatrix, projectionMatrix);
	m_Triangle->Render(m_Direct3D->GetDeviceContext());

	m_TextClass->BeginDraw();
	m_InfoUi->Frame(m_TextClass, m_CameraClass->GetPosition(), m_CameraClass->GetRotation());

	m_TextClass->EndDraw();
	m_Direct3D->EndScene();

	return true;
}