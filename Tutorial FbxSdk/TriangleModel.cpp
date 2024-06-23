#include "TriangleModel.h"

TriangleModel::TriangleModel()
{
	m_vertices = 0;
	m_indices = 0;

	m_vertexBuffer = 0;
	m_indexBuffer = 0;
}

TriangleModel::~TriangleModel()
{

}

bool TriangleModel::Initialize(ID3D11Device* device)
{
	HRESULT result;

	m_vertices = new VertexObject[3];
	m_indices = new UINT[3];

	//�ﰢ���� ������ �����ϴ� �κ�
	//�÷��� ����������
	m_vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	m_vertices[0].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	m_vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_vertices[1].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	m_vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);
	m_vertices[2].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	m_indices[0] = 0;
	m_indices[1] = 1;
	m_indices[2] = 2;


	int vertexCount;

	vertexCount = 3;
	//������ �ε����� ����


	//���� ���ۿ� ���� ������ �Է��Ѵ�.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexObject) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = m_vertices;

	//Desc�� ������� ���۸� �����Ѵ�.
	result = device->CreateBuffer(&vertexBufferDesc, &initData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//�ε��� ���ۿ� ���� ������ �Է��Ѵ�.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(UINT) * vertexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	initData.pSysMem = &m_indices[0];

	//Desc�� ������� ���۸� �����Ѵ�.
	result = device->CreateBuffer(&indexBufferDesc, &initData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void TriangleModel::Render(ID3D11DeviceContext* deviceContext)
{
	const int indexCounts = 3;

	//��ǲ ��������� ���۸� Ȱ��ȭ�Ͽ� ������ �� �� �ֵ��� ����
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->DrawIndexed(indexCounts, 0, 0);
}

void TriangleModel::Shutdown()
{
	//����, �ε��� ���� ����
	m_vertexBuffer->Release();
	m_indexBuffer->Release();
	m_vertexBuffer = 0;
	m_indexBuffer = 0;

}
