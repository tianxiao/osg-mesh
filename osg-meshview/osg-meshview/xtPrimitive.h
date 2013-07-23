#pragma once

class xtIndexTria3
{
public:
	xtIndexTria3(int a, int b, int c) {
		this->a[0] = a;
		this->a[1] = b;
		this->a[2] = c;
	}

	int a[3];
};

class xtIndexCquad4
{
public:
	xtIndexCquad4(int a, int b, int c, int d) {
		this->a[0] = a;
		this->a[1] = b;
		this->a[2] = c;
		this->a[3] = d;
	}

	int a[4];
};

struct xtEdge
{
	int a, b;
};