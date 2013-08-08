#pragma once

class xtOpenmeshText
{
public:
	xtOpenmeshText(void);
	~xtOpenmeshText(void);

	void RunTest()
	{
		TestOppositeFaceHandel();
		TestDeleteFaceTrue();
	}

private:
	void TestOppositeFaceHandel();
	void TestDeleteFaceTrue();
};

