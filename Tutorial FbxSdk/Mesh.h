#include <vector>
#include <iostream>
#include <fbxsdk.h>
#include <d3dx11.h>
#include <DirectXMath.h>
using namespace std;


struct Vertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 textureCoord;
};

class Mesh
{
public:
	Mesh();

	bool InitializeBuffer(ID3D11Device* pDevice);
	void Render(ID3D11DeviceContext* pDeviceContext);
	void Shutdown();

	void SetResource(ID3D11Device* pDevice, string filePath);
	ID3D11ShaderResourceView* CheckResource();
public:
	std::vector<Vertex> vertices;
	std::vector<UINT> indices;
	ID3D11ShaderResourceView* diffuseTexture = NULL;

private:
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;
};