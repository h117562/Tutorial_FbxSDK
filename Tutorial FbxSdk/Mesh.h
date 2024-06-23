#include <vector>
#include <string>
#include <fbxsdk.h>
#include <d3dx11.h>
#include <DirectXMath.h>

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

	void SetResource(ID3D11Device* pDevice, std::string filePath);
public:
	std::vector<Vertex> vertices;
	std::vector<UINT> indices;
	ID3D11ShaderResourceView* diffuseTexture;

private:
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
};