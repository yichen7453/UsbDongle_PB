// UsbDongle_PB.cpp: �D�n�M���ɡC

#include "stdafx.h"
#include "UsbDongle_GUI.h"

using namespace UsbDongle_PB;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
    //Console::WriteLine(L"Hello World");

	// �إߥ��󱱨���e�A���ҥ� Window ����ı�ƮĪG
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);

	// �إߥD�����ð���
	Application::Run(gcnew UsbDongle_GUI());

    return 0;
}
