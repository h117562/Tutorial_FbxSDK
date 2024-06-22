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
	//std::string fileName = "";
	
	//���� �̸��� �߶󳻱� ����
	//fileName = fileDirectory.substr(fileDirectory.find_last_of('\\') + 1);

	//FBX �Ŵ��� ����
	FbxManager* pManager = FbxManager::Create();

	//���� IO ����� ����(IOSROOT �⺻ ����)
	pIos = FbxIOSettings::Create(pManager, IOSROOT);

	//���� ����
	pManager->SetIOSettings(pIos);

	//FileName�� �ι�° �Ű������� �־� �� �̸��� �����ϴµ� ���� ������ ����
	//Scene ����
	pScene = FbxScene::Create(pManager, "");
	
	//�ҷ��� �����Ϳ� ���� �� �� �ִ� �������̽��� ����
	pImporter = FbxImporter::Create(pManager, "");

	//������ ����� �ε尡 �ƴ��� Ȯ��
	result = pImporter->Initialize(fileDirectory.c_str(), -1, pManager->GetIOSettings());
	if (!result)
	{
		MessageBox(hwnd, L"FbxImport Error", L"Error", MB_OK);
		std::string str = pImporter->GetStatus().GetErrorString();
		std::wstring w_trans = std::wstring(str.begin(), str.end());
		MessageBox(hwnd, w_trans.c_str(), L"Error", MB_OK);

		return false;
	}

	//�ε�� ������ �����͸� Scene�� ��� Importer �ڿ� ��ȯ
	pImporter->Import(pScene);
	pImporter->Destroy();
	pImporter = nullptr;

	//�ﰢ�� ���������� ��ȯ��
	FbxGeometryConverter converter(pManager);
	converter.Triangulate(pScene, true);//true�� ���� ������ ���� 

	FbxDocumentInfo* pDocInfo = pScene->GetDocumentInfo();
	if (pDocInfo)
	{
		FbxString version = pDocInfo->Original_ApplicationVersion.Get();
		std::cout << "FBX file version: " << version << std::endl;
	}

	//�� ������ ��� Ž��
	processNode(pScene->GetRootNode(), pDevice);

	//�޽����� ������ �ε��� ���۸� ����
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
	//����� �Ӽ��� ������
	FbxNodeAttribute* nodeAttribute = pNode->GetNodeAttribute();

	if (nodeAttribute != nullptr)
	{
		//��� Ÿ���� �޽��� ���̷����� ��쿡�� �۾�
		switch (nodeAttribute->GetAttributeType())
		{
		case FbxNodeAttribute::eMesh:
			processMesh(pNode, pDevice);
			break;
		case FbxNodeAttribute::eSkeleton:
			break;
		}
	}

	cout << "child - " << pNode->GetChildCount() << endl;
	//�ڽ� ��尡 �ִ��� Ȯ���ϸ� Ž��
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
	FbxGeometryElementUV* uvElement = pMesh->GetElementUV(0);

	if (pMesh->IsTriangleMesh())
	{
		//�޽��� ��� ���� ������ �޾ƿ� 
		FbxVector4* positions = pMesh->GetControlPoints();

		FbxStringList uvNames;
		pMesh->GetUVSetNames(uvNames);
		const char* uvName = NULL;

		//�ؽ�ó�� �ִ� ���  ����
		int materialCount = pNode->GetMaterialCount();
		cout << "mat count " << materialCount << endl;
		for (int i = 0; i < materialCount; i++)
		{
			bool result;
			FbxSurfaceMaterial* pMaterial = pNode->GetMaterial(i);
			if (!pMaterial)
			{
				return;
			}

			//Diffuse Material�� ã�� �� ���� �ݺ�
			result = GetTextureFromMaterial(pMaterial, pDevice, lMesh);
			if (result)
			{
				/*pMaterial = pNode->GetParent()->GetMaterial(0);
				GetTextureFromMaterial(pMaterial, pDevice, lMesh);*/
				break;
			}
		}

		//��� �ؽ�ó ��ǥ�� �迭�� ����
		int polygonCount = pMesh->GetPolygonCount();
		for (int j = 0; j < polygonCount; j++)
		{
			for (int i = 2; i >= 0; i--)//�ﰢ�� ������ ���� ���� �׸��� ������ ���� �����͸� �迭�� �о�ִ´�.
			{
				bool unmappedUV = true;//���� ����
				Vertex vt;

				//���� �׸��� ������ �޾ƿ�
				UINT index = pMesh->GetPolygonVertex(j, i);

				//���� ��ǥ�� �ʱ�ȭ
				vt.position = DirectX::XMFLOAT3(
					static_cast<float>(positions[index].mData[0]),
					static_cast<float>(positions[index].mData[1]),
					static_cast<float>(positions[index].mData[2]));
				
				//�ؽ�ó ��ǥ�� �ʱ�ȭ
				FbxVector2 fv2;
				pMesh->GetPolygonVertexUV(j, i, uvNames[0], fv2, unmappedUV);
				vt.textureCoord = DirectX::XMFLOAT2(
					static_cast<float>(fv2.mData[0]),
					1.0f - static_cast<float>(fv2.mData[1])//��ǥ�谡 �ٸ��� ������ y �� �ݴ��
				);
				
				//�迭�� �߰�
				lMesh.vertices.emplace_back(vt);
			}
		}

		//�ε��� ������� ����
		int vertexCount = pMesh->GetPolygonVertexCount();
		for (int i = 0; i < vertexCount; i++)
		{
			lMesh.indices.emplace_back(i);
		}

		cout << pMesh->GetName() << endl << endl;
		m_meshes.emplace_back(lMesh);
		pMesh->Destroy();
	}

	return;
}

//Material�� ���� �ؽ�ó ���� ��θ� �˾Ƴ��� �ش� ��θ� ���� �ؽ�ó�� �ε��ϴ� �Լ�
bool FbxLoader::GetTextureFromMaterial(FbxSurfaceMaterial* pMaterial, ID3D11Device* pDevice, Mesh& mesh)
{
	HRESULT result;

	//Diffuse �Ӽ��� ������ 
	FbxProperty prop = pMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
	if (!prop.IsValid()) 
	{
		return false;
	}
	
	//�ؽ�ó ���� Ȯ��
	//�޽� �ϳ��� �ؽ�ó �ϳ��� �ε� ����
	int textureCount = prop.GetSrcObjectCount<FbxTexture>();
	if (textureCount != 1) 
	{
		return false;
	}

	//�ؽ�ó ���� ������ ������
	FbxString tempstr;
	FbxFileTexture* pTexture = prop.GetSrcObject<FbxFileTexture>(0);
	if (!pTexture) 
	{
		return false;
	}
	
	//���� ��θ� �����Ѵ�.
	//������ FbxString�� string���� ��ȯ
	tempstr = pTexture->GetFileName();
	mesh.SetResource(pDevice, tempstr.Buffer());

	pTexture->Destroy();
		
	return true;
}

void FbxLoader::Render(ID3D11DeviceContext* pDeviceContext)
{
	//���� �޽� �ϳ��� ������ ��
	for (Mesh& m : m_meshes)
	{
		m.Render(pDeviceContext);
	}

	return;
}

void FbxLoader::Shutdown()
{
	//����ü �迭���� �����ʹ� ���� ����
	for (Mesh& m : m_meshes)
	{
		m.Shutdown();
	}

	m_meshes.clear();
}
