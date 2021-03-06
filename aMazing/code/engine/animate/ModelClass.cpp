#include "ModelClass.h"


ModelClass::ModelClass()
{
	is_inited = false;
	render_time = 0.0f;
	ZeroMemory(&identityMatrix,sizeof(identityMatrix));
	identityMatrix.a1 = 1.0f;
	identityMatrix.b2 = 1.0f;
	identityMatrix.c3 = 1.0f;
	identityMatrix.d4 = 1.0f;
}


ModelClass::~ModelClass()
{
	importer->FreeScene();
}

HRESULT ModelClass::Initialize(ID3D11Device* device,
	ID3D11DeviceContext* context,
	MutableString&& string)
{
	HRESULT hr = E_FAIL;
	hr = BasicObject::Initialize(device, context);
	if (FAILED(hr))
	{
		return hr;
	}
	
	std::string filename = string.getMultiByteString(); 

	unsigned int ppsteps = aiProcess_CalcTangentSpace | // calculate tangents and bitangents if possible
		aiProcess_JoinIdenticalVertices | // join identical vertices/ optimize indexing
		aiProcess_ValidateDataStructure | // perform a full validation of the loader's output
		aiProcess_ImproveCacheLocality | // improve the cache locality of the output vertices
		aiProcess_RemoveRedundantMaterials | // remove redundant materials
		aiProcess_FindDegenerates | // remove degenerated polygons from the import
		aiProcess_FindInvalidData | // detect invalid model data, such as invalid nor vectors
		aiProcess_GenUVCoords | // convert spherical, cylindrical, box and planar mapping to proper UVs
		aiProcess_TransformUVCoords | // preprocess UV transformations (scaling, translation ...)
		aiProcess_FindInstances | // search for instanced meshes and remove them by references to one master
		aiProcess_LimitBoneWeights | // limit bone weights to 4 per SkinVertex
		aiProcess_OptimizeMeshes | // join small meshes, if possible;
		aiProcess_SplitByBoneCount | // split meshes with too many bones. Necessary for our (limited) hardware skinning shader
		0;

	// 使用导入器导入选定的模型文件 
	importer.reset(new Assimp::Importer);
	scene = (aiScene*)importer->ReadFile(filename.c_str(),
		ppsteps |
		aiProcess_GenSmoothNormals |
		aiProcess_SplitLargeMeshes |
		aiProcess_Triangulate |
		aiProcess_ConvertToLeftHanded |
		aiProcess_SortByPType |
		0);
	if (scene == nullptr)
	{
		//failed on loading asset. 
		//if so get error message & output to console.
		std::cout << importer->GetErrorString() << std::endl;
		return E_FAIL;
	}

	sceneAnimator.reset(new SceneAnimator(scene));
	//update the fileLocation
	modelLocation.reset(new std::string(getModelLocation(filename.c_str())));

	hr = boneTransformations.Initialize(device, context, 3);
	if (FAILED(hr))
	{
		return hr;
	}
	//load the texturess of this model.
	loadTextures(device, context, scene, string.getMultiByteString().c_str());

	//load the meshes of this model.
	loadMeshes(scene);
	
	if (vertices.empty() || indices.empty())
	{
		return E_FAIL;
	}
	//send vertices into vertex buffers.
	for (int i = 0; i < vertices.size(); i++)
	{
		auto& currentBuffer = vertexBuffer[i];
		hr = currentBuffer->Initialize(device, context, 
			vertices[i]->data(), 
			vertices[i]->size(),
			indices[i]->data(),
			indices[i]->size());
		if (FAILED(hr))
		{
			return hr;
		}
	}
	return S_OK;
}

void ModelClass::loadTextures(ID3D11Device* device,
	ID3D11DeviceContext* context, 
	const aiScene* pScene,
	const char* modelPath)
{
	if (!pScene->HasMaterials())
	{
		return ;
	}
	//load the materials.
	for (int i = 0; i < pScene->mNumMaterials; i++)
	{
		const aiMaterial* mat = pScene->mMaterials[i];
		int texCount = mat->GetTextureCount(aiTextureType::aiTextureType_DIFFUSE);
		if (texCount > 0)
		{
			aiString filename;
			mat->GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &filename);
			std::string filepath = *modelLocation.get();
			filepath += filename.C_Str();
			std::cout << filepath.c_str() << std::endl;
			textures.push_back(std::unique_ptr<TextureClass>(new TextureClass));
			try
			{
				HRESULT hr = textures.back()->LoadFile(device, context, filepath.c_str());
				if (FAILED(hr))
				{
					return ;
				}
			}
			catch (const std::exception& e)
			{
				printf("%s\n",e.what());
				textures.back()->beChessBoard(device, context);
			}
		}
		else
		{
			textures.push_back(std::unique_ptr<TextureClass>(new TextureClass));
			textures.back()->beChessBoard(device, context);
		}
	}
}

void ModelClass::loadMeshes(const aiScene* pScene)
{
	if (!pScene->HasMeshes())
	{
		return;
	}

	// load the vertices of meshes.
	std::size_t meshBaseVertexIndex = 0;
	for (int i = 0; i < pScene->mNumMeshes; i++)
	{
		const aiMesh* pMesh = pScene->mMeshes[i];
		if (pMesh == nullptr)
		{
			continue;
		}
		vertices.push_back(std::unique_ptr<std::vector<SkinVertex> >(new std::vector<SkinVertex>));
		indices.push_back(std::unique_ptr<std::vector<WORD> >(new std::vector<WORD>)); 
		vertexBuffer.push_back(std::unique_ptr<GPUMutableVerticeBuffer<SkinVertex> >(new GPUMutableVerticeBuffer<SkinVertex>));
		textureIndices.push_back(pMesh->mMaterialIndex);

		std::vector<std::size_t> vertexBoneBindCnt;
		for (int j = 0; j < pMesh->mNumVertices; j++)
		{
			auto& pos = pMesh->mVertices[j];
			auto& nor = pMesh->mNormals[j];
			auto& tex = pMesh->mTextureCoords[0];
			vertexBoneBindCnt.push_back(0);
			vertices.back()->push_back(SkinVertex());
			vertices.back()->back().position = XMFLOAT4{ pos.x, pos.y, pos.z, 1.0f };
			vertices.back()->back().normal = XMFLOAT4{ nor.x, nor.y, nor.z, 1.0f };
			if (tex == nullptr)
			{
				vertices.back()->back().texture = XMFLOAT4{ 1.0f, 1.0f, 0.0f, 0.0f }; 
			}
			else
			{
				vertices.back()->back().texture = XMFLOAT4{ tex[j].x, tex[j].y, 0.0f, 0.0f };
			}
			
		}
		
		for (int k = 0; k < pMesh->mNumBones; k++)
		{
			const auto pBone = pMesh->mBones[k];
			for (int l = 0; l < pBone->mNumWeights; l++)
			{
				const auto weights = pBone->mWeights[l];
				if (weights.mWeight == 0.0f)
				{
					continue; 
				}
				switch (vertexBoneBindCnt[weights.mVertexId])
				{
				case 0:
					vertices.back()->at(weights.mVertexId).setBoneIndex(0, k);
					vertices.back()->at(weights.mVertexId).weights.x = weights.mWeight;
					break;
				case 1:
					vertices.back()->at(weights.mVertexId).setBoneIndex(1, k);
					vertices.back()->at(weights.mVertexId).weights.y = weights.mWeight;
					break;
				case 2:
					vertices.back()->at(weights.mVertexId).setBoneIndex(2, k);
					vertices.back()->at(weights.mVertexId).weights.z = weights.mWeight;
					break;
				case 3:
					vertices.back()->at(weights.mVertexId).setBoneIndex(3, k);
					vertices.back()->at(weights.mVertexId).weights.w = weights.mWeight;
					break;
				default:
					printf("out of range\n");
				}
				vertexBoneBindCnt[weights.mVertexId]++;
			}
		}

		for (int j = 0; j < pMesh->mNumFaces; j++)
		{
			if (pMesh->mFaces[j].mNumIndices != 3)
			{
				continue;
			}
			//push back the indices of meshes.
			for (int k = 0; k < pMesh->mFaces[j].mNumIndices; k++)
			{
				indices.back()->push_back(pMesh->mFaces[j].mIndices[k]);
			}
		}
		meshBaseVertexIndex += pMesh->mNumVertices;
	}
}

std::string ModelClass::getModelLocation(const char* filename)
{
	std::string result;
	result = filename;
	while (!result.empty() && 
		!(result.back() == '/' ||
		result.back() == '\\'))
	{
		result.pop_back();
	}
	return std::move(result);
}

void ModelClass::bindBonesToGPU(ID3D11Device* device,
	ID3D11DeviceContext* context, 
	aiNode* pNode,
	float animationTime)
{
	const aiMatrix4x4 transform = scene->mRootNode->mTransformation;
	BonesBindData boneCbData = {};
	sceneAnimator->calculate(animationTime, 25);
	auto& boneData = sceneAnimator->getBoneMatrices(pNode);
	for (std::size_t cur = 0; cur < boneData.size(); ++cur)
	{
		boneCbData.boneMatrices[cur] = boneData[cur];
	}
	boneTransformations.UpdateData(&boneCbData);
	boneTransformations.UpdateGpu(device, context);
	boneTransformations.BindVertexShader(device, context);
}

bool ModelClass::isInited()const
{
	return is_inited;
}

void ModelClass::Render(ID3D11Device* device,
	ID3D11DeviceContext* context)
{
	BasicObject::UpdatePRS(device, context);

	render_time += 0.01f;
	if (render_time > 20.0f)
	{
		render_time = 0.0f;
	}
	Render(device, context, nullptr);
}

/*
* if pNode is nullptr ,
* then it begin with rootNode
*/
void ModelClass::Render(ID3D11Device* device,
	ID3D11DeviceContext* context,
	aiNode* pNode)
{
	if (pNode == nullptr)
	{
		pNode = scene->mRootNode;
	}
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	for (int i = 0; i < pNode->mNumMeshes; i++)
	{
		std::size_t meshIndex = pNode->mMeshes[i];
		bindBonesToGPU(device, context, pNode, render_time);
		textures[textureIndices[meshIndex]]->bindPS(device, context, 0);
		vertexBuffer[meshIndex]->Render(device, context);
	}
	
	for (int i = 0; i < pNode->mNumChildren; i++)
	{
		Render(device, context, pNode->mChildren[i]);
	}
}