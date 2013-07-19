#pragma once
class Vec2
{
public:
	Vec2()
	{
		this->x = 0.0;
		this->y = 0.0;
	}

	Vec2(float x, float y) 
	{
		this->x = x;
		this->y = y;
	}
	~Vec2(void);

	float x;
	float y;
};

