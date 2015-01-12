#include"CommonFunction.hpp"
//��ֵval����������
float CommonFunction::increaseByScale(float val, float scale, float upperBound, float bias)
{
	if (val > upperBound - bias)
	{
		return upperBound;
	}
	return val * scale;
}

//��ֵval������˥��
float CommonFunction::fadeByScale(float val, float scale, float lowerBound, float bias)
{
	if (val > 0 && val < lowerBound + bias)
	{
		return lowerBound;
	}
	if (val < 0 && val > lowerBound - bias)
	{
		return lowerBound;
	}
	return val * scale;
}

//��ֵval������ֵ����
float CommonFunction::fadeByValue(float val, float decreasement, float lowerBound)
{
	val -= decreasement;
	if (val < lowerBound)
	{
		return lowerBound;
	}
	return val;
}

//��ֵval����ֵ����
float CommonFunction::increaseByValue(float val, float increasement, float upperBound)
{
	val += increasement;
	if (val > upperBound)
	{
		return upperBound;
	}
	return val;
}

//����ά������λ��
XMFLOAT3 CommonFunction::normalize(XMFLOAT3 src)
{
	float length = src.x * src.x;
	length += src.y * src.y;
	length += src.z * src.z;
	length = std::sqrt(length);
	if (length == 0.0f)
		return src;
	src.x /= length;
	src.y /= length;
	src.z /= length;
	return src;
}