#include "Test.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace ExamC;

[STAThreadAttribute]
int main(array<String^>^ args)
{
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);
    Application::Run(gcnew Test());
	return 0;
}
