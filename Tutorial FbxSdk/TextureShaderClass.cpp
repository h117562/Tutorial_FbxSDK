#include "TextureShaderClass.h"

TextureShaderClass::TextureShaderClass()
{

}

TextureShaderClass::~TextureShaderClass()
{
}


bool TextureShaderClass::Initialize(ID3D11Device* pDevice, HWND hwnd) {


	HRESULT result;
	ID3DBlob* errorMessage;
	ID3DBlob* vertexShaderBuffer;
	ID3DBlob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;

	const wchar_t* vsFilename = L"../VertexShader.hlsl";
	const wchar_t* psFilename = L"../PixelShader.hlsl";

	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "main", "vs_4_0",
		D3DCOMPILE_ENABLE_STRICTNESS, 0, NULL, &vertexShaderBuffer, &errorMessage, NULL);

	if (FAILED(result))
	{
		//���Ϸκ��� �����Ͽ� �������� ��� ���� �޽����� ���Ϸ� ����
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		//���� ���� �޽����� ���� ��� ������ ã�� �� ���ٴ� �ǹ�
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Vs Texture Shader File", MB_OK);
		}

		return false;
	}

	result = D3DX11CompileFromFile(psFilename, NULL, NULL, "main", "ps_4_0",
		D3DCOMPILE_ENABLE_STRICTNESS, 0, NULL, &pixelShaderBuffer, &errorMessage, NULL);

	if (FAILED(result))
	{
		//���Ϸκ��� �����Ͽ� �������� ��� ���� �޽����� ���Ϸ� ����
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		//���� ���� �޽����� ���� ��� ������ ã�� �� ���ٴ� �ǹ�
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Ps Texture Shader File", MB_OK);
		}

		return false;
	}

	//���� ���̴� ����
	result = pDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	//�ȼ� ���̴� ����
	result = pDevice->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	//���̴��� �Է� �����͸� ����
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0; //D3D11_APPEND_ALIGNED_ELEMENT �̶� ���� �⺻��
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	//�Է� �������� ����
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	//���� �Է� ���̾ƿ� ���� (�ȼ� ���̴��� �Ұ���)
	result = pDevice->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
	{
		return false;
	}

	//����, �ȼ� ���̴� ���� ����
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	//��Ʈ���� ���� ����
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	//��Ʈ���� ���� ����
	result = pDevice->CreateBuffer(&matrixBufferDesc, nullptr, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//���÷� State ����
	D3D11_SAMPLER_DESC sampDesc;

	//���� �ʱ�ȭ
	ZeroMemory(&sampDesc, sizeof(sampDesc));

	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;//�ؽ�ó ���͸� ��� (���� ����)
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;//�ؽ�ó ��ǥ�� 0���� 1���̸� �ݺ��ϴ� ����� ������
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;//�� ��쿡�� 1�� ������ �������� ��ǥ�� ���
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;//D3D11_TEXTURE_ADDRESS_CLAMP�� �ʰ��ϰų� �̸��� ��� 0 �Ǵ� 1�� ����
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//���÷� State ����
	result = pDevice->CreateSamplerState(&sampDesc, &m_samplerState);
	if (FAILED(result))
	{
		return false;
	}


	return true;
}

bool TextureShaderClass::Render(ID3D11DeviceContext* pDeviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	bool result;


	result = UpdateShaderBuffers(pDeviceContext, worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	//��ǲ ��������� ���۸� Ȱ��ȭ�Ͽ� ������ �� �� �ֵ��� ����
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pDeviceContext->IASetInputLayout(m_layout);
	pDeviceContext->VSSetShader(m_vertexShader, NULL, 0);
	pDeviceContext->PSSetShader(m_pixelShader, NULL, 0);
	pDeviceContext->PSSetSamplers(0, 1, &m_samplerState);

	return true;
}


void TextureShaderClass::Shutdown()
{
	//��Ʈ���� ���� ����
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	//���̾ƿ� ����
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	//�ȼ� ���̴� ����
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	//���� ���̴� ����
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	return;
}

//���� ������ �ؽ�Ʈ ���Ϸ� ����
void TextureShaderClass::OutputShaderErrorMessage(ID3DBlob* errorMessage, HWND hwnd, CONST WCHAR* fileName)
{
	char* compileErrors;
	unsigned long long bufferSize, i;
	std::ofstream fout;

	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("TextureShader-error.txt");

	// Write out the error message.

	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", fileName, MB_OK);

	return;
}

//���̴����� ����ϴ� ���۸� ������Ʈ
bool TextureShaderClass::UpdateShaderBuffers(ID3D11DeviceContext* deviceContext, 
	XMMATRIX modelPosition, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBuffer* dataPtr = 0;

	//��� ��ġ
	modelPosition = XMMatrixTranspose(modelPosition);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	//��Ʈ���� ���۸� ���� (GPU ���� �����͸� ������) 
	result = deviceContext->Map(m_matrixBuffer, NULL, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	dataPtr = (MatrixBuffer*)mappedResource.pData;

	//�Ű������� ������ �����ͷ� ������Ʈ
	dataPtr->mWorld = modelPosition;
	dataPtr->mView = viewMatrix;
	dataPtr->mProjection = projectionMatrix;
	
	//��Ʈ���� ���۸� ����� (GPU �� �����͸� ����)
	deviceContext->Unmap(m_matrixBuffer, NULL);

	deviceContext->VSSetConstantBuffers(startNumber, bufferCount, &m_matrixBuffer);

	return true;
}