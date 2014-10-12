#pragma once
#include<d3d11.h>
#include<xnamath.h>
#include<cmath>

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