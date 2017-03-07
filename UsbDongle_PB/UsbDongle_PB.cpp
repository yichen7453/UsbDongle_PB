// UsbDongle_PB.cpp: 主要專案檔。

#include "stdafx.h"
#include "UsbDongle_GUI.h"

using namespace UsbDongle_PB;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
    //Console::WriteLine(L"Hello World");

	// 建立任何控制項之前，先啟用 Window 的視覺化效果
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);

	// 建立主視窗並執行
	Application::Run(gcnew UsbDongle_GUI());

    return 0;
}
