//Demo .NET integration with OSG using powerful C++/CLI language
//
//NOTE: Please update project include and library path to where your OSG is at
//
//Author: Hesicong
//Homepage: www.hesicong.net
//Email: hesicong2005 at 163.com
//QQ: 38288890
//

#include "stdafx.h"
#include "osgNETDemoForm.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace osgNETDemo;

int main(array<System::String ^> ^args)
{
	//Run program in console mode
	//Some OSG info will print in console.
	osgNETDemoForm^ form=gcnew osgNETDemoForm();
	Application::Run(form);
}
