#include "FbxLoader.h"

FbxLoader::FbxLoader()
{

}

bool FbxLoader::LoadFile(ID3D11Device* pDevice, HWND hwnd, std::string fileDirectory)
{
	bool result;

	FbxIOSettings* pIos;
	FbxImporter* pImporter;
	FbxScene* pScene;

	//FBX 매니저 생성
	FbxManager* pManager = FbxManager::Create();

	//파일 IO 입출력 설정(IOSROOT 기본 설정)
	pIos = FbxIOSettings::Create(pManager, IOSROOT);

	//설정 적용
	pManager->SetIOSettings(pIos);

	//FileName을 두번째 매개변수에 넣어 씬 이름을 저장하는데 딱히 쓸데는 없다
	//Scene 생성
	pScene = FbxScene::Create(pManager, "");
	
	//불러온 데이터에 접근 할 수 있는 인터페이스를 제공
	pImporter = FbxImporter::Create(pManager, "");

	//파일이 제대로 로드가 됐는지 확인
	result = pImporter->Initialize(fileDirectory.c_str(), -1, pManager->GetIOSettings());
	if (!result)
	{
		MessageBox(hwnd, L"FbxImport Error", L"Error", MB_OK);
		std::string str = pImporter->GetStatus().GetErrorString();
		std::wstring w_trans = std::wstring(str.begin(), str.end());
		MessageBox(hwnd, w_trans.c_str(), L"Error", MB_OK);

		return false;
	}

	//로드된 파일의 데이터를 Scene에 담고 Importer 자원 반환
	pImporter->Import(pScene);
	pImporter->Destroy();
	pImporter = nullptr;

	//보통 폴리곤 형태가 사각형으로 되어 있기도 하여 일단 삼각형 폴리곤으로 변환함
	FbxGeometryConverter converter(pManager);
	converter.Triangulate(pScene, true);//true시 원본 데이터 유지 

	//모델 파일의 노드 탐색
	processNode(pScene->GetRootNode(), pDevice);

	//메쉬마다 정점과 인덱스 버퍼를 생성
	for (Mesh& m : m_meshes)
	{
		result = m.InitializeBuffer(pDevice);
		if (!result)
		{
			MessageBox(hwnd, L"Could not initialize Vertex Buffer", L"Error", MB_OK);
			return false;
		}
	}

	return true;
}

void FbxLoader::processNode(FbxNode* pNode, ID3D11Device* pDevice)
{
	//노드의 속성을 가져옴
	FbxNodeAttribute* nodeAttribute = pNode->GetNodeAttribute();

	if (nodeAttribute != nullptr)
	{
		//노드 타입이 메쉬나 스켈레톤일 경우에만 작업
		switch (nodeAttribute->GetAttributeType())
		{
		case FbxNodeAttribute::eMesh:
			processMesh(pNode, pDevice);
			break;
		case FbxNodeAttribute::eSkeleton:
			break;
		}
	}

	//자식 노드가 있는지 확인하며 탐색
	if (pNode->GetChildCount() > 0)
	{
		for (int i = 0; i < pNode->GetChildCount(); i++)
		{
			processNode(pNode->GetChild(i), pDevice);
		}
	}
	
	return;
}

void FbxLoader::processMesh(FbxNode* pNode, ID3D11Device* pDevice)
{
	FbxMesh* pMesh = pNode->GetMesh();
	Mesh lMesh;

	if (pMesh->IsTriangleMesh())
	{
		//메쉬내 모든 정점 정보를 받아옴 
		FbxVector4* positions = pMesh->GetControlPoints();

		//UVSet 이름들을 불러옴
		FbxStringList uvNames;
		pMesh->GetUVSetNames(uvNames);
		
		//모든 텍스처 좌표를 배열에 저장
		int polygonCount = pMesh->GetPolygonCount();
		for (int j = 0; j < polygonCount; j++)
		{
			for (int i = 0; i < 3; i++)//삼각형 폴리곤 기준 정점 그리는 순서에 따라 데이터를 배열에 밀어넣는다.
			{
				bool unmappedUV;//매핑 여부 (주의*) false를 리턴받게 되면 매핑이 되어있음을 의미
				Vertex vt;

				//정점 그리는 순서를 받아옴
				UINT index = pMesh->GetPolygonVertex(j, i);

				//정점 좌표를 저장
				vt.position = DirectX::XMFLOAT3(
					static_cast<float>(positions[index].mData[0]),
					static_cast<float>(positions[index].mData[1]),
					static_cast<float>(positions[index].mData[2]));
				
				//텍스처 좌표를 저장
				FbxVector2 fv2;
				pMesh->GetPolygonVertexUV(j, i, uvNames[0], fv2, unmappedUV);
				vt.textureCoord = DirectX::XMFLOAT2(
					static_cast<float>(fv2.mData[0]),
					1.0f - static_cast<float>(fv2.mData[1])
				);
				
				//배열에 추가
				lMesh.vertices.emplace_back(vt);
			}
		}

		//인덱스 순서대로 나열
		int vertexCount = pMesh->GetPolygonVertexCount();
		for (int i = 0; i < vertexCount; i++)
		{
			lMesh.indices.emplace_back(i);
		}

		//텍스처가 있는 경우  저장
		int materialCount = pNode->GetMaterialCount();
		if (materialCount > 0)
		{
			FbxSurfaceMaterial* pMaterial = pNode->GetMaterial(0);
			if (!pMaterial)
			{
				return;
			}
		
			GetTextureFromMaterial(pMaterial, pDevice, lMesh);
		}

		m_meshes.emplace_back(lMesh);
		pMesh->Destroy();
	}

	return;
}

//Material을 통해 텍스처 파일 경로를 알아내고 해당 경로를 통해 텍스처를 로드하는 함수
bool FbxLoader::GetTextureFromMaterial(FbxSurfaceMaterial* pMaterial, ID3D11Device* pDevice, Mesh& mesh)
{
	HRESULT result;

	//Diffuse 속성을 가져옴 
	FbxProperty prop = pMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
	if (!prop.IsValid()) 
	{
		return false;
	}

	//텍스처 보유 확인
	int textureCount = prop.GetSrcObjectCount<FbxTexture>();
	if (!textureCount) 
	{
		return false;
	}

	//텍스처 파일 정보를 가져옴
	FbxString tempstr;
	FbxFileTexture* pTexture = prop.GetSrcObject<FbxFileTexture>();
	if (!pTexture) {

		return false;
	}
	
	//파일 경로를 저장한다.
	//저장한 FbxString을 string으로 변환
	tempstr = pTexture->GetFileName();
	mesh.SetResource(pDevice, tempstr.Buffer());

	pTexture->Destroy();

	return true;
}

void FbxLoader::Render(ID3D11DeviceContext* pDeviceContext)
{
	//모델의 메쉬 하나씩 렌더링 함
	for (Mesh& m : m_meshes)
	{
		m.Render(pDeviceContext);
	}

	return;
}

void FbxLoader::Shutdown()
{
	//구조체 배열안의 포인터는 직접 해제
	for (Mesh& m : m_meshes)
	{
		m.Shutdown();
	}

	m_meshes.clear();
}

