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
		//파일로부터 컴파일에 실패했을 경우 오류 메시지를 파일로 생성
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		//만약 오류 메시지가 없을 경우 파일을 찾을 수 없다는 의미
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
		//파일로부터 컴파일에 실패했을 경우 오류 메시지를 파일로 생성
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		//만약 오류 메시지가 없을 경우 파일을 찾을 수 없다는 의미
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Ps Texture Shader File", MB_OK);
		}

		return false;
	}

	//정점 쉐이더 생성
	result = pDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	//픽셀 쉐이더 생성
	result = pDevice->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	//쉐이더의 입력 데이터를 정의
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0; //D3D11_APPEND_ALIGNED_ELEMENT 이랑 같음 기본값
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	//입력 데이터의 개수
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	//정점 입력 레이아웃 생성 (픽셀 쉐이더는 불가능)
	result = pDevice->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
	{
		return false;
	}

	//정점, 픽셀 쉐이더 버퍼 해제
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	//매트릭스 버퍼 설정
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	//매트릭스 버퍼 생성
	result = pDevice->CreateBuffer(&matrixBufferDesc, nullptr, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//샘플러 State 설정
	D3D11_SAMPLER_DESC sampDesc;

	//변수 초기화
	ZeroMemory(&sampDesc, sizeof(sampDesc));

	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;//텍스처 필터링 모드 (선형 보간)
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;//텍스처 좌표가 0에서 1사이를 반복하는 방법을 결정함
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;//이 경우에는 1로 나누어 나머지를 좌표로 사용
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;//D3D11_TEXTURE_ADDRESS_CLAMP는 초과하거나 미만인 경우 0 또는 1로 고정
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//샘플러 State 생성
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

	//인풋 어셈블러에서 버퍼를 활성화하여 렌더링 할 수 있도록 설정
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pDeviceContext->IASetInputLayout(m_layout);
	pDeviceContext->VSSetShader(m_vertexShader, NULL, 0);
	pDeviceContext->PSSetShader(m_pixelShader, NULL, 0);
	pDeviceContext->PSSetSamplers(0, 1, &m_samplerState);

	return true;
}


void TextureShaderClass::Shutdown()
{
	//매트릭스 버퍼 해제
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	//레이아웃 해제
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	//픽셀 쉐이더 해제
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	//정점 쉐이더 해제
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	return;
}

//에러 내용을 텍스트 파일로 생성
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

//쉐이더에서 사용하는 버퍼를 업데이트
bool TextureShaderClass::UpdateShaderBuffers(ID3D11DeviceContext* deviceContext, 
	XMMATRIX modelPosition, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBuffer* dataPtr = 0;

	//행렬 전치
	modelPosition = XMMatrixTranspose(modelPosition);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	//매트릭스 버퍼를 매핑 (GPU 에서 데이터를 가져옴) 
	result = deviceContext->Map(m_matrixBuffer, NULL, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	dataPtr = (MatrixBuffer*)mappedResource.pData;

	//매개변수로 가져온 데이터로 업데이트
	dataPtr->mWorld = modelPosition;
	dataPtr->mView = viewMatrix;
	dataPtr->mProjection = projectionMatrix;
	
	//매트릭스 버퍼를 언매핑 (GPU 로 데이터를 전달)
	deviceContext->Unmap(m_matrixBuffer, NULL);

	deviceContext->VSSetConstantBuffers(startNumber, bufferCount, &m_matrixBuffer);

	return true;
}