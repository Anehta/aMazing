#pragma once
#include<d3d11.h>
#include<xnamath.h>
#include<cmath>
#include<string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include"CommonTemplates.hpp"
#include"CommonDef.hpp"

//��ֵval����������
float increaseByScale(float val,float scale,float upperBound,float bias);

//��ֵval������˥��
float fadeByScale(float val,float scale,float lowerBound,float bias);

//��ֵval����ֵ����
float increaseByValue(float val, float increasement, float upperBound);

//��ֵval������ֵ����
float fadeByValue(float val, float decreasement, float lowerBound);

//����ά������λ��
XMFLOAT3 normalize(XMFLOAT3 src);

class aiStringLess
{
public:
	bool operator () (const aiString& aiA,const aiString& aiB) const;
};