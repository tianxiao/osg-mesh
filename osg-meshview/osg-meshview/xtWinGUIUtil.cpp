#include "StdAfx.h"
#include "xtWinGUIUtil.h"

#include <Windows.h>

xtWinGUIUtil::xtWinGUIUtil(void)
{
}


xtWinGUIUtil::~xtWinGUIUtil(void)
{
}


// Open Dialog
char* OpenDialog()
{
	OPENFILENAME ofn;       // common dialog box structure
	wchar_t szFile[260];       // buffer for file name
	HWND hwnd;              // owner window
	HANDLE hf;              // file handle

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile;
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"All\0*.*\0Text\0*.TXT\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Display the Open dialog box. 
	char *filenamebuffer = new char[500];
	filenamebuffer;
	if (GetOpenFileName(&ofn)==TRUE) {
		unsigned int converted = 0;
		wcstombs_s(&converted,filenamebuffer,wcslen(szFile)+1,szFile,_TRUNCATE);
	} else {
		return NULL;
	}
		//hf = CreateFile(ofn.lpstrFile, 
		//				GENERIC_READ,
		//				0,
		//				(LPSECURITY_ATTRIBUTES) NULL,
		//				OPEN_EXISTING,
		//				FILE_ATTRIBUTE_NORMAL,
		//				(HANDLE) NULL);

	return filenamebuffer;
}