#include "Mesh.h"

Mesh::Mesh()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
}

bool Mesh::InitializeBuffer(ID3D11Device* pDevice)
{
	HRESULT hr;

	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * vertices.size();
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = vertices.data();

	hr = pDevice->CreateBuffer(&vertexBufferDesc, &initData, &m_vertexBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(UINT) * indices.size();
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	initData.pSysMem = indices.data();

	hr = pDevice->CreateBuffer(&indexBufferDesc, &initData, &m_indexBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

bool Mesh::SetResource(ID3D11Device* pDevice, std::string str)
{
	HRESULT result;

	int len = str.length();
	wchar_t* textureName = new wchar_t[len + 1] {};
	if (textureName)
	{
		//멀티바이트 문자로 변환
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, textureName, len);// -1이면 함수는 종료 null 문자를 포함하여 전체 입력 문자열을 처리

		//Diffuse 텍스처 파일 경로를 통해 텍스처 리소스 생성
		result = D3DX11CreateShaderResourceViewFromFile(pDevice, textureName, 0, 0, &m_diffuseTexture, 0);
		if (FAILED(result))
		{
			delete[] textureName;

			m_diffuseTexture = NULL;

			return false;
		}
	}

	delete[] textureName;

	return true;
}


void Mesh::Render(ID3D11DeviceContext* pDeviceContext)
{
	pDeviceContext->PSSetShaderResources(0, 1, &m_diffuseTexture);
	pDeviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	pDeviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	pDeviceContext->DrawIndexed(indices.size(), 0, 0);

	return;
}

void Mesh::Shutdown()
{
	if (m_diffuseTexture)
	{
		m_diffuseTexture->Release();
		m_diffuseTexture = nullptr;
	}

	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}

	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = nullptr;
	}

	return;
}
