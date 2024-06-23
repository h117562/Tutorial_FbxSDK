#include <directxmath.h>
#include <d3dx11.h>

using namespace DirectX;
class TriangleModel
{
public:
	struct VertexObject
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

	TriangleModel();
	~TriangleModel();

	bool Initialize(ID3D11Device*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

private:
	VertexObject* m_vertices;
	UINT* m_indices;

	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;

	UINT stride = sizeof(VertexObject);
	UINT offset = 0;
};