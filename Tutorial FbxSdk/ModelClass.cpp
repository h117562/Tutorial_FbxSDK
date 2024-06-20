#include "ModelClass.h"

ModelClass::ModelClass()
{
	m_triangle = 0;
	m_indices = 0;

	m_vertexBuffer = 0;
	m_indexBuffer = 0;
}

ModelClass::~ModelClass()
{

}

bool ModelClass::Initialize(ID3D11Device* device)
{
	HRESULT result;

	m_triangle = new VertexObject[3];
	m_indices = new UINT[3];

	//삼각형의 정점을 저장하는 부분
	//컬러는 빨간색으로
	m_triangle[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	m_triangle[0].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	m_triangle[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_triangle[1].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	m_triangle[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);
	m_triangle[2].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	m_indices[0] = 0;
	m_indices[1] = 1;
	m_indices[2] = 2;


	int vertexCount, indexCount;

	vertexCount = 3;
	indexCount = 3;
	//정점과 인덱스의 갯수


	//정점 버퍼에 대한 설명을 입력한다.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexObject) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = m_triangle;

	//Desc를 기반으로 버퍼를 생성한다.
	result = device->CreateBuffer(&vertexBufferDesc, &initData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//인덱스 버퍼에 대한 설명을 입력한다.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(UINT) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	initData.pSysMem = &m_indices[0];


	//Desc를 기반으로 버퍼를 생성한다.
	result = device->CreateBuffer(&indexBufferDesc, &initData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}


	return true;
}


void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	//인풋 어셈블러에서 버퍼를 활성화하여 렌더링 할 수 있도록 설정
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	//정점 버퍼에서 렌더링하는 유형을 설정함 (삼각형)
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}



void ModelClass::Shutdown()
{
	//정점, 인덱스 버퍼 해제
	m_vertexBuffer->Release();
	m_indexBuffer->Release();
	m_vertexBuffer = 0;
	m_indexBuffer = 0;

}

int ModelClass::GetIndexCount()
{
	return 3;//정점의 개수 반환
}