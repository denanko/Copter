#include "GuiForm.h"
#include "stdio.h"
#include "string.h"
#include <iostream>					// Include our standard header
#include <string>					// Include this to use strings
#include <fstream>					// Include this to use file streams

using namespace std;				// Set our namespace to standard
using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace ZedGraph;
using namespace System::Runtime::InteropServices;
using namespace CopterGUI;
using namespace UcanDotNET;

[STAThreadAttribute]
void Main() {
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	CopterGUI::GuiForm form;
	Application::Run(%form);
}