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
	
	//���� ���� ��ġ ����
	m_originPath = fileDirectory;

	//�ε�� ������ �����͸� Scene�� ��� Importer �ڿ� ��ȯ
	pImporter->Import(pScene);
	pImporter->Destroy();
	pImporter = nullptr;

	//���� ������ ���°� �簢������ �Ǿ� �ֱ⵵ �Ͽ� �ϴ� �ﰢ�� ���������� ��ȯ��
	FbxGeometryConverter converter(pManager);
	converter.Triangulate(pScene, true);//true�� ���� ������ ���� 

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

	if (pMesh->IsTriangleMesh())
	{
		//�޽��� ��� ���� ������ �޾ƿ� 
		FbxVector4* positions = pMesh->GetControlPoints();

		//UVSet �̸����� �ҷ���
		FbxStringList uvNames;
		pMesh->GetUVSetNames(uvNames);
		
		//��� �ؽ�ó ��ǥ�� �迭�� ����
		int polygonCount = pMesh->GetPolygonCount();
		for (int j = 0; j < polygonCount; j++)
		{
			for (int i = 0; i < 3; i++)//�ﰢ�� ������ ���� ���� �׸��� ������ ���� �����͸� �迭�� �о�ִ´�.
			{
				bool unmappedUV;//���� ���� (����*) false�� ���Ϲް� �Ǹ� ������ �Ǿ������� �ǹ�
				Vertex vt;

				//���� �׸��� ������ �޾ƿ�
				UINT index = pMesh->GetPolygonVertex(j, i);

				//���� ��ǥ�� ����
				vt.position = DirectX::XMFLOAT3(
					static_cast<float>(positions[index].mData[0]),
					static_cast<float>(positions[index].mData[1]),
					static_cast<float>(positions[index].mData[2]));
				
				//�ؽ�ó ��ǥ�� ����
				FbxVector2 fv2;
				pMesh->GetPolygonVertexUV(j, i, uvNames[0], fv2, unmappedUV);
				vt.textureCoord = DirectX::XMFLOAT2(
					static_cast<float>(fv2.mData[0]),
					1.0f - static_cast<float>(fv2.mData[1])
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

		//�ؽ�ó�� �ִ� ��� ����
		int materialCount = pNode->GetMaterialCount();
		for (int index = 0; index < materialCount; index++)
		{
			FbxSurfaceMaterial* pMaterial = pNode->GetMaterial(index);
			if (!pMaterial)
			{
				break;
			}

			bool result;
			result = GetTextureFromMaterial(pMaterial, pDevice, lMesh);
			if (result)
			{
				break;
			}
		}

		m_meshes.emplace_back(lMesh);
		pMesh->Destroy();
	}

	return;
}

//Material�� ���� �ؽ�ó ���� ��θ� �˾Ƴ��� �ش� ��θ� ���� �ؽ�ó�� �ε��ϴ� �Լ�
bool FbxLoader::GetTextureFromMaterial(FbxSurfaceMaterial* pMaterial, ID3D11Device* pDevice, Mesh& mesh)
{
	bool result;

	//Diffuse �Ӽ��� ������ 
	FbxProperty prop = pMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
	if (!prop.IsValid())
	{
		return false;
	}

	//�ؽ�ó ���� Ȯ��
	int textureCount = prop.GetSrcObjectCount<FbxFileTexture>();
	if (textureCount == 0)
	{
		//Diffuse ������ 0�ε� Normal�� ���������� ������ ��찡 ���� ���� �׷��� NormalMap �� ���� ��� ���� �޽��� �ؽ�ó�� ������ �����ϰ� �Ͽ���
		FbxProperty temp = pMaterial->FindProperty(FbxSurfaceMaterial::sNormalMap);
		if (temp.IsValid())
		{
			int tempCount = temp.GetSrcObjectCount<FbxFileTexture>();
			if (tempCount > 0)
			{
				result = mesh.SetResource(pDevice, m_prevTexturePath);//�ؽ�ó ��θ� �����Ͽ� ���̴� ���ҽ��� ����
				if (!result)
				{
					return false;
				}
			}

			return true;
		}

		return false;
	}

	//�ؽ�ó ���� ������ ������
	FbxString filepath;
	std::string tmpstr;
	FbxFileTexture* pTexture = prop.GetSrcObject<FbxFileTexture>();
	if (!pTexture) {

		return false;
	}
	
	//���� ��θ� ����
	filepath = pTexture->GetRelativeFileName(); //GetFileName()�Լ��� ������ \\�� /�� �����Ǵ� ������ ����
	
	//���� ���� ��ġ�� �ؽ�ó ��ġ ���ڿ��� ��ħ
	tmpstr = m_originPath.substr(0, m_originPath.find_last_of("\\"));
	tmpstr.append("\\");
	tmpstr.append(filepath.Buffer());

	result = mesh.SetResource(pDevice, tmpstr);
	if (!result)
	{
		return false;
	}

	//���� �޽��� �ؽ�ó ��θ� �����ϰ� FileTexture ����
	m_prevTexturePath = tmpstr;
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

