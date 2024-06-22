#ifndef _COLOR_SHADER_CLASS_H_
#define _COLOR_SHADER_CLASS_H_

#include <d3dx11.h>
#include <d3dcompiler.h>
#include <directxmath.h>

#include <string>

using namespace DirectX;

class ColorShaderClass {
private:
	struct MatrixBuffer {
		XMMATRIX mWorld;
		XMMATRIX mView;
		XMMATRIX mProjection;
	};
public:
	ColorShaderClass();
	~ColorShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX);

private:
	bool InitializeShader(ID3D11Device*, HWND, CONST WCHAR*, CONST WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND);

	bool SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX);
	void RenderShader(ID3D11DeviceContext*);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
};

#endif