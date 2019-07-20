#include "stdio.h"
#include "stdio.h"
#include "string.h"
#include <iostream>					// Include our standard header
#include <string>					// Include this to use strings
#include <fstream>					// Include this to use file streams
#include <msclr\marshal_cppstd.h>

//Для отображения в реальном времени
#define K_PELENG	0.00057295779
#define K_PHASE		0.0057295779
#define K_U			0.737

//Для режима калибровки
#define BufferSize 10					//Размер буфера по которому ищутся переходы на фазовой характеристике
#define PhaseCharacteristicStep	40		//Шаг фазовой характеристики в градусах
#define	PelengFilterThreshold	0.15
#define	VoltageFilterThreshold	200
#define FilterThreshold			12

//Коэффициенты масштабирования
#define K			0.18

//defines по отображению
#define START_ROLLING_SHIT		10		//максимум по оси Х при старте, когда время становиться > START_ROLLING_SHIT графики начинают катиться
#define ENEBLE_SYMBOL_STARTING_WITH 4	//при разнице (Xmax - Xmin) < ENEBLE_SYMBOL_STARTING_WITH начинают отображаться символы точек

#pragma once
namespace CopterGUI {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::IO;
	using namespace ZedGraph;
	using namespace std;				// Set our namespace to standard
	using namespace System::Runtime::InteropServices;

	struct U {
		unsigned number;
		double	values[10000];
	};

	static struct U  U1, U2, U3, U4, Time;

	static long start[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	static long end[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	public ref class GuiForm : public System::Windows::Forms::Form
	{
		/* Added for Copter */


		double avrgFromLeft = 0, avrgFromRight = 0;
		int currGraphLayout = 0x1222, prevGraphLayout = 0x1222;
		int calibrationInProgressFlag = 0, boxesAreDrawedFlag = 0;
		int pvk_soft_ver1, pvk_soft_ver2, pvk_soft_ver3, pvk_soft_ver4, pvk_number;
		int fadingEffectCounter = 0;
		int enableSymbolTrigger = 0;
		int pos = 0;
		long end_of_file = 0;
		char *currFile;
		int algorithState = 0;
		int checkBoxCounter = 0;
		double s0, s1, s2, s3, s4, s5, s6, s7, s8;
		double e0, e1, e2, e3, e4, e5, e6, e7, e8;
		unsigned int peleng_pointer = 0;
		double t = 0;
		unsigned int prevCounter = 0;
		long count = 0;
		char pChannel;
		static RollingPointPairList ^counterRollLine = gcnew RollingPointPairList(50000);
		static RollingPointPairList ^zahvatRollLine = gcnew RollingPointPairList(50000);
		static RollingPointPairList ^zasvetRollLine = gcnew RollingPointPairList(50000);
		static RollingPointPairList ^ARURollLine = gcnew RollingPointPairList(50000);
		static RollingPointPairList ^pelengRollLine = gcnew RollingPointPairList(50000);
		static RollingPointPairList ^phaseRollLine = gcnew RollingPointPairList(50000);
		static RollingPointPairList ^U1RollLine = gcnew RollingPointPairList(50000);
		static RollingPointPairList ^U2RollLine = gcnew RollingPointPairList(50000);
		static RollingPointPairList ^U3RollLine = gcnew RollingPointPairList(50000);
		static RollingPointPairList ^U4RollLine = gcnew RollingPointPairList(50000);
		PointPairList ^u1_avrg = gcnew PointPairList;
		PointPairList ^u2_avrg = gcnew PointPairList;
		PointPairList ^u3_avrg = gcnew PointPairList;
		PointPairList ^u4_avrg = gcnew PointPairList;
		PointPairList ^u1_shifted = gcnew PointPairList;
		PointPairList ^u2_shifted = gcnew PointPairList;
		PointPairList ^u3_shifted = gcnew PointPairList;
		PointPairList ^u4_shifted = gcnew PointPairList;
		double pelengMaxScale = 0, phaseMaxScale = 0, UMaxScale = 0, logicMaxScale = 9;
		int StartOrStop = 0;
		CheckState U2CheckBoxPrev = CheckState::Checked, U3CheckBoxPrev = CheckState::Checked;
		CheckState U1CheckBoxPrev = CheckState::Checked, U4CheckBoxPrev = CheckState::Checked;
		CheckState PhaseCheckBoxPrev = CheckState::Checked, PelengCheckBoxPrev = CheckState::Checked;
		CheckState ZasvetCheckBoxPrev = CheckState::Checked, ARUCheckBoxPrev = CheckState::Checked, ZahvatCheckBoxPrev = CheckState::Checked;
		CheckState CoordsCheckBoxPrev = CheckState::Checked, VoltageCheckBoxPrev = CheckState::Checked;
		CheckState CounterCheckBoxPrev = CheckState::Unchecked;
		//Для масштабирования графиков
		bool manualScaleTrigger = 0, changeScaleTrigger = 0;
		double x_down = 0, x_up = 0, y_down = 0, y_up = 0;
		double xDragStartPoint = 0, yDragStartPoint = 0;
		String^ title = gcnew String("");
		String^ subStr = gcnew String("");

	public:
	private: System::Windows::Forms::TabPage^  tabPage1;
	private: ZedGraph::ZedGraphControl^  zedGraphControl1;
	private: System::Windows::Forms::CheckBox^  checkBox1;
	private: System::Windows::Forms::TabPage^  tabPage5;
	private: ZedGraph::ZedGraphControl^  zedGraphControl5;
	private: System::Windows::Forms::TabControl^  tabControl1;
	private: System::Windows::Forms::ToolStripPanel^  BottomToolStripPanel;
	private: System::Windows::Forms::ToolStripPanel^  TopToolStripPanel;
	private: System::Windows::Forms::ToolStripPanel^  RightToolStripPanel;
	private: System::Windows::Forms::ToolStripPanel^  LeftToolStripPanel;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  label26;
	private: System::Windows::Forms::Label^  label25;
	private: System::Windows::Forms::Label^  label24;
	private: System::Windows::Forms::Label^  label23;
	private: System::Windows::Forms::Label^  label22;
	private: System::Windows::Forms::Label^  label21;
	private: System::Windows::Forms::Label^  label28;
	private: System::Windows::Forms::Label^  label27;
	private: System::Windows::Forms::Panel^  panel1;
	private: System::Windows::Forms::Panel^  panel5;
	private: System::Windows::Forms::CheckBox^  GyroRollCheckBox;
	private: System::Windows::Forms::CheckBox^  RollOutCheckBox;


	private: System::Windows::Forms::CheckBox^  RcRollCheckBox;
	private: System::Windows::Forms::CheckBox^  StabRollCheckBox;


	private: System::Windows::Forms::CheckBox^  RolEnAllCheckBox;



	private: System::Windows::Forms::CheckBox^  PitEnAllCheckBox;


	private: System::Windows::Forms::Panel^  panel2;
	private: System::Windows::Forms::CheckBox^  GyroPitCheckBox;
	private: System::Windows::Forms::CheckBox^  StabPitCheckBox;


	private: System::Windows::Forms::CheckBox^  RcPitCheckBox;

	private: System::Windows::Forms::Button^  stopBtn;
	private: System::Windows::Forms::Button^  saveBtn;
	private: System::Windows::Forms::Button^  openBtn;
	private: System::Windows::Forms::SaveFileDialog^  saveFileDialog1;
	private: System::Windows::Forms::ProgressBar^  progressBar1;
	private: System::Windows::Forms::Button^  pauseBtn;
	private: System::Windows::Forms::CheckBox^  PitOutCheckBox;

	private: System::Windows::Forms::Panel^  panel6;




	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::Label^  label19;
	private: System::Windows::Forms::TextBox^  textBox17;
	private: System::Windows::Forms::TextBox^  textBox18;
	private: System::Windows::Forms::Label^  label20;
	private: System::Windows::Forms::Label^  label17;
private: System::Windows::Forms::TextBox^  YawOutTextBox;

private: System::Windows::Forms::TextBox^  BRTextBox;

	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::Label^  label18;

	private: System::Windows::Forms::Label^  label15;
private: System::Windows::Forms::TextBox^  StabYawTextBox;


	private: System::Windows::Forms::Label^  label4;
private: System::Windows::Forms::TextBox^  BLTextBox;

	private: System::Windows::Forms::Label^  label8;
	private: System::Windows::Forms::Label^  label16;

	private: System::Windows::Forms::Label^  label13;


	private: System::Windows::Forms::Label^  label7;

	private: System::Windows::Forms::Label^  label10;
	private: System::Windows::Forms::Label^  label14;

	private: System::Windows::Forms::Label^  label11;


	private: System::Windows::Forms::Label^  label9;

	private: System::Windows::Forms::Label^  label12;



	private: System::Windows::Forms::Panel^  panel7;


	private: System::Windows::Forms::Label^  softVerLbl;
	private: System::Windows::Forms::Label^  pvkNumberLbl;
	private: System::Windows::Forms::ToolTip^  toolTip1;
	private: System::Windows::Forms::ToolTip^  toolTip2;
	private: System::Windows::Forms::ToolTip^  toolTip3;
	private: System::Windows::Forms::ToolTip^  toolTip4;
	private: System::Windows::Forms::ToolTip^  toolTip5;
	private: System::Windows::Forms::ToolTip^  toolTip6;
	private: System::Windows::Forms::ToolTip^  toolTip7;
	private: System::Windows::Forms::CheckBox^  changeKCheckBox;
	private: System::Windows::Forms::TextBox^  textBox19;
private: System::Windows::Forms::Label^  label3;
private: System::Windows::Forms::TextBox^  RcPitTextBox;

private: System::Windows::Forms::TextBox^  GyroPitTextBox;

private: System::Windows::Forms::TextBox^  PitOutTextBox;
private: System::Windows::Forms::TextBox^  StabPitTextBox;


private: System::Windows::Forms::TextBox^  GyroRollTextBox;
private: System::Windows::Forms::TextBox^  RcRollTextBox;



private: System::Windows::Forms::TextBox^  GyroYawTextBox;


private: System::Windows::Forms::TextBox^  FRTextBox;
private: System::Windows::Forms::TextBox^  RollOutTextBox;
private: System::Windows::Forms::TextBox^  StabRollTextBox;
private: System::Windows::Forms::TextBox^  RcYawTextBox;






private: System::Windows::Forms::TextBox^  FLTextBox;

private: System::Windows::Forms::Panel^  panel3;
private: System::Windows::Forms::CheckBox^  YawEnAllCheckBox;
private: System::Windows::Forms::CheckBox^  GyroYawCheckBox;
private: System::Windows::Forms::CheckBox^  YawOutCheckBox;



private: System::Windows::Forms::CheckBox^  RcYawCheckBox;
private: System::Windows::Forms::CheckBox^  StabYawCheckBox;
private: System::Windows::Forms::CheckBox^  MotorEnAllCheckBox;
private: System::Windows::Forms::CheckBox^  FRCheckBox;
private: System::Windows::Forms::CheckBox^  BRCheckBox;
private: System::Windows::Forms::CheckBox^  FLCheckBox;
private: System::Windows::Forms::CheckBox^  BLCheckBox;
private: System::Windows::Forms::Label^  label29;
private: System::Windows::Forms::TextBox^  RcThrTextBox;

private: System::Windows::Forms::CheckBox^  RcThrCheckBox;
private: System::Windows::Forms::CheckBox^  IntRollCheckBox;
private: System::Windows::Forms::TextBox^  IntRollTextBox;

private: System::Windows::Forms::Label^  label31;
private: System::Windows::Forms::Label^  label32;
private: System::Windows::Forms::TextBox^  IntPitTextBox;

private: System::Windows::Forms::CheckBox^  IntPitCheckBox;
private: System::Windows::Forms::CheckBox^  IntYawCheckBox;
private: System::Windows::Forms::TextBox^  IntYawTextBox;

private: System::Windows::Forms::Label^  label30;


	private: System::Windows::Forms::ToolStripContentPanel^  ContentPanel;



	public: void Load_Graw(double tMax, GraphPane ^currPane, ZedGraphControl ^currZed)
	{

		int scaleFont = 0, axisTitleFont = 0, legendFont = 0;

		if (checkBox1->CheckState == CheckState::Checked)
		{
			//Установка фона панели графиков (не рабочая часть)
			currPane->Fill->Color = System::Drawing::Color::Black;
			//Установка фона панели отображения графиков
			currPane->Chart->Fill = gcnew Fill(Color::Black, Color::Black, 0);
			//Установка границы вывода графиков
			currPane->Chart->Border->Color = System::Drawing::Color::White;
			// Устанавливаем интересующий нас интервал по оси X
			currPane->XAxis->MajorGrid->Color = System::Drawing::Color::Gray;
			currPane->XAxis->Color = System::Drawing::Color::Gray;
			currPane->YAxis->MajorGrid->Color = System::Drawing::Color::Gray;
			currPane->YAxis->Color = System::Drawing::Color::Gray;
			//Цвет фона легенды
			currPane->Legend->Fill = gcnew Fill(Color::Black, Color::Black, 0);
			//Цвет текста легенды
			currPane->Legend->FontSpec->FontColor = Color::White;
			//Цвет подписи осей
			currPane->YAxis->Title->FontSpec->FontColor = Color::White;
			//Цвет рамки легенды
			currPane->Legend->Border->Color = Color::White;
			//Цвет подписи делений на осях
			currPane->XAxis->Scale->FontSpec->FontColor = Color::White;
			currPane->YAxis->Scale->FontSpec->FontColor = Color::White;

		}
		else
		{
			currPane->CurveList->Clear();
			currPane->GraphObjList->Clear();
			//Установка фона панели графиков (не рабочая часть)
			currPane->Fill->Color = System::Drawing::Color::White;
			//Установка фона панели отображения графиков
			currPane->Chart->Fill = gcnew Fill(Color::White, Color::White, 0);
			//Установка границы вывода графиков
			currPane->Chart->Border->Color = System::Drawing::Color::Black;
			// Устанавливаем интересующий нас интервал по оси X
			currPane->XAxis->MajorGrid->Color = System::Drawing::Color::Gray;
			currPane->XAxis->Color = System::Drawing::Color::Gray;
			currPane->YAxis->MajorGrid->Color = System::Drawing::Color::Gray;
			currPane->YAxis->Color = System::Drawing::Color::Gray;
			//Цвет фона легенды
			currPane->Legend->Fill->Color = Color::White;
			//Цвет текста легенды
			currPane->Legend->FontSpec->FontColor = Color::Black;
			//Цвет подписи осей
			currPane->YAxis->Title->FontSpec->FontColor = Color::Black;
			//Цвет рамки легенды
			currPane->Legend->Border->Color = Color::Black;
			//Цвет подписи делений на осях
			currPane->XAxis->Scale->FontSpec->FontColor = Color::Black;
			currPane->YAxis->Scale->FontSpec->FontColor = Color::Black;
		}
		axisTitleFont = 22;
		scaleFont = 18;
		legendFont = 24;

		if (currGraphLayout == 0x0222 || currGraphLayout == 0x1022 || currGraphLayout == 0x1202 || currGraphLayout == 0x1220)
		{
			axisTitleFont = 18;
			scaleFont = 14;
			legendFont = 18;
		}
		else if (currGraphLayout == 0x0022 || currGraphLayout == 0x1002 || currGraphLayout == 0x1200 || currGraphLayout == 0x0220 || currGraphLayout == 0x0202 || currGraphLayout == 0x1020)
		{
			axisTitleFont = 14;
			scaleFont = 12;
			legendFont = 14;
		}
		else if (currGraphLayout == 0x0002 || currGraphLayout == 0x1000 || currGraphLayout == 0x0200 || currGraphLayout == 0x0020)
		{
			axisTitleFont = 10;
			scaleFont = 6;
			legendFont = 8;
		}

		// Получим панель для рисования
		//Запрет на самосогласования и выход за установленные границы
		currPane->XAxis->Scale->MaxGrace = 0;
		currPane->XAxis->Scale->MinGrace = 0;
		currPane->YAxis->Scale->MaxGrace = 0;
		currPane->YAxis->Scale->MinGrace = 0;
		currPane->Margin->Left = 0;
		currPane->Margin->Right = 0;
		// Подписи к графику и к осям 
		// Установим размеры шрифтов для подписей по осям
		currPane->YAxis->Title->IsVisible = true;
		currPane->XAxis->Title->IsVisible = false;
		currPane->XAxis->Title->FontSpec->Size = axisTitleFont;
		currPane->YAxis->Title->FontSpec->Size = axisTitleFont;
		//Шрифт для делений масштаба
		currPane->XAxis->Scale->FontSpec->Size = scaleFont;
		currPane->YAxis->Scale->FontSpec->Size = scaleFont;
		currPane->YAxis->MinSpace = 90;
		//currPane->YAxis->Scale->Align = AlignP::Center;
		// Установим размеры шрифта для легенды
		currPane->Legend->FontSpec->Size = legendFont;
		currPane->Legend->Position = LegendPos::InsideTopLeft;
		//currPane->Legend->IsReverse = 1;
		// Установим размеры шрифта для общего заголовка
		currPane->Title->IsVisible = false;

		// Устанавливаем интересующий нас интервал по оси X
		currPane->XAxis->Scale->Min = 0;
		currPane->XAxis->Scale->Max = START_ROLLING_SHIT;
		//Ручная установка шага оси Х -  1 В 
		//myPane->XAxis->Scale->MinorStep = 0.1;
		//myPane->XAxis->Scale->MajorStep = 1;
		// Устанавливаем интересующий нас интервал по оси Y - значения в мА от -10 до 100 мА
		//myPane->YAxis->Scale->Min = 0;
		//myPane->YAxis->Scale->Max = 200;
		//myPane->YAxis->Scale->MinorStep = 5;
		//myPane->YAxis->Scale->MajorStep = 20;
		//Установка оси "Y" ровно по оси "Х" 0.0
		//myPane->YAxis->Cross = 0.0;
		//Устанавливаем метки только возле осей!
		currPane->XAxis->MajorTic->IsOpposite = true;
		currPane->XAxis->MinorTic->IsOpposite = true;
		currPane->YAxis->MajorTic->IsOpposite = true;
		currPane->YAxis->MinorTic->IsOpposite = true;
		//Рисуем сетку по X
		currPane->XAxis->MajorGrid->IsVisible = true;
		currPane->XAxis->MajorGrid->DashOn = 10;
		currPane->XAxis->MajorGrid->DashOff = 10;
		//Рисуем сетку по Y
		currPane->YAxis->MajorGrid->IsVisible = true;
		currPane->YAxis->MajorGrid->DashOn = 10;
		currPane->YAxis->MajorGrid->DashOff = 10;
		// Вызываем метод AxisChange (), чтобы обновить данные об осях. 
		// В противном случае на рисунке будет показана только часть графика, 
		// которая умещается в интервалы по осям, установленные по умолчанию
		currZed->AxisChange();
		// Обновляем график
		currZed->Invalidate();
	}

	public:	void FileRead(char *fileLocation, int State)
	{
		//Переменные для графиков
		Graphics ^g_m120 = zedGraphControl1->CreateGraphics();
		GraphPane^ m120_pane1 = gcnew GraphPane;
		GraphPane^ m120_pane2 = gcnew GraphPane;
		GraphPane^ m120_pane3 = gcnew GraphPane;
		GraphPane^ m120_pane4 = gcnew GraphPane;
		MasterPane ^m120_masterPane = zedGraphControl1->MasterPane;
		float lineWidth = 2;
		String^ s;
		LineItem ^myCurve1;
		LineItem ^myCurve2;

		if (algorithState == 0)		//Начальные настройки графиков
		{
			algorithState++;

			m120_masterPane->PaneList->Clear();

			//	m120_masterPane->Margin->Top = 0;
			//	m120_masterPane->Margin->Bottom = 0;
				//Прорисовываем оформление по-умолчанию для всех подграфиков
			Load_Graw(0, m120_pane1, zedGraphControl1);
			Load_Graw(0, m120_pane2, zedGraphControl1);
			Load_Graw(0, m120_pane3, zedGraphControl1);
			Load_Graw(0, m120_pane4, zedGraphControl1);
			//Прорисовываем оформление для отдельных подграфиков
		//	m120_pane1->YAxis->Title->FontSpec->Size = 48;
		//	m120_pane1->XAxis->Scale->FontSpec->Size = 36;
		//	m120_pane1->YAxis->Scale->FontSpec->Size = 36;
		//	m120_pane1->Legend->FontSpec->Size = 48;
			m120_pane1->YAxis->Title->Text = "Логика";
			m120_pane1->Margin->Bottom = 0;
			m120_pane1->Margin->Top = 0;
			m120_pane2->Legend->IsVisible = false;
			m120_pane2->YAxis->Title->Text = "Пеленг, град";
			m120_pane3->Legend->IsVisible = false;
			m120_pane3->YAxis->Title->Text = "Фаза пеленга, град";
			m120_pane4->YAxis->Title->Text = "Напряжение, мВ";
			//Очищаем кривые (для работы кнопки Clear
			if (State)
			{
				counterRollLine->Clear();
				zahvatRollLine->Clear();
				zasvetRollLine->Clear();
				ARURollLine->Clear();
				pelengRollLine->Clear();
				phaseRollLine->Clear();
				U1RollLine->Clear();
				U2RollLine->Clear();
				U3RollLine->Clear();
				U4RollLine->Clear();

				u1_avrg->Clear();
				u2_avrg->Clear();
				u3_avrg->Clear();
				u4_avrg->Clear();
				u1_shifted->Clear();
				u2_shifted->Clear();
				u3_shifted->Clear();
				u4_shifted->Clear();

				//И сбрасываем время
				t = 0;
			}

			Color BWColor;
			SymbolType symbol;
			if (checkBox1->CheckState == CheckState::Checked)
			{

				BWColor = Color::White;
			}
			else
			{

				BWColor = Color::Black;
			}

			if (enableSymbolTrigger)
			{
				symbol = SymbolType::Circle;
			}
			else
			{
				symbol = SymbolType::None;
			}

			//Заполняем первый подграфик кривыми
			s = "Захват ";
			myCurve1 = m120_pane1->AddCurve(s, zahvatRollLine, Color::Red, symbol);
			myCurve1->Line->Width = lineWidth;
			myCurve1->Symbol->Fill->Type = FillType::Solid;
			myCurve1->Symbol->Size = 9;
			s = "АРУ";
			myCurve1 = m120_pane1->AddCurve(s, ARURollLine, Color::Cyan, symbol);
			myCurve1->Line->Width = lineWidth;
			myCurve1->Symbol->Fill->Type = FillType::Solid;
			myCurve1->Symbol->Size = 9;
			s = "Засвет ";
			myCurve1 = m120_pane1->AddCurve(s, zasvetRollLine, BWColor, symbol);

			myCurve1->Line->Width = lineWidth;
			myCurve1->Symbol->Fill->Type = FillType::Solid;
			myCurve1->Symbol->Size = 9;
			s = "Счетчик ";
			myCurve1 = m120_pane1->AddCurve(s, counterRollLine, Color::LightCoral, symbol);
			myCurve1->Line->Width = lineWidth;
			myCurve1->Symbol->Fill->Type = FillType::Solid;
			myCurve1->Symbol->Size = 9;
			myCurve1->IsVisible = false;
			//Заполняем второй подграфик кривыми
			s = "Пеленг";
			myCurve1 = m120_pane2->AddCurve(s, pelengRollLine, BWColor, symbol);
			myCurve1->Line->Width = lineWidth;
			myCurve1->Symbol->Fill->Type = FillType::Solid;
			myCurve1->Symbol->Size = 9;
			//Заполняем третий подграфик кривыми
			s = "Фаза";
			myCurve1 = m120_pane3->AddCurve(s, phaseRollLine, BWColor, symbol);
			myCurve1->Line->Width = lineWidth;
			myCurve1->Symbol->Fill->Type = FillType::Solid;
			myCurve1->Symbol->Size = 9;
			//Заполняем четвертый подграфик кривыми
			s = "U1  ";
			myCurve1 = m120_pane4->AddCurve(s, U1RollLine, Color::Red, symbol);
			myCurve1->Line->Width = lineWidth;
			myCurve1->Symbol->Fill->Type = FillType::Solid;
			myCurve1->Symbol->Size = 9;
			s = "U2  ";
			myCurve1 = m120_pane4->AddCurve(s, U2RollLine, Color::Cyan, symbol);
			myCurve1->Line->Width = lineWidth;
			myCurve1->Symbol->Fill->Type = FillType::Solid;
			myCurve1->Symbol->Size = 9;
			s = "U3  ";
			myCurve1 = m120_pane4->AddCurve(s, U3RollLine, BWColor, symbol);
			myCurve1->Line->Width = lineWidth;
			myCurve1->Symbol->Fill->Type = FillType::Solid;
			myCurve1->Symbol->Size = 9;
			s = "U4  ";
			myCurve1 = m120_pane4->AddCurve(s, U4RollLine, Color::LawnGreen, symbol);
			myCurve1->Line->Width = lineWidth;
			myCurve1->Symbol->Fill->Type = FillType::Solid;
			myCurve1->Symbol->Size = 9;
			//Добавляем подграфики на полотно
			m120_masterPane->Add(m120_pane1);
			m120_masterPane->Add(m120_pane2);
			m120_masterPane->Add(m120_pane3);
			m120_masterPane->Add(m120_pane4);
			//Выставляем графики один под другим
			//m120_masterPane->SetLayout(g_m120, PaneLayout::SingleColumn);
			zedGraphControl1->IsSynchronizeXAxes = true;
			m120_masterPane->IsCommonScaleFactor = true;
			m120_masterPane->IsFontsScaled = true;
			m120_masterPane->InnerPaneGap = 0;
			m120_masterPane->CommonScaleFactor();
			zedGraphControl1->AxisChange();
			zedGraphControl1->Invalidate();

			if (State == 0 && !enableSymbolTrigger)
			{
				ScaleGraphs(zedGraphControl1);
			}

			float lineWidth = 2.0;
			String^ s;

			LineItem ^myCurve1;
			LineItem ^myCurve2;
			//Заполняем графики для СПКС
			Graphics ^g_secondTab = zedGraphControl5->CreateGraphics();
			GraphPane^ pane1 = gcnew GraphPane;
			GraphPane^ pane2 = gcnew GraphPane;
			MasterPane ^masterPane = zedGraphControl5->MasterPane;

			masterPane->PaneList->Clear();
			//Прорисовываем оформление по-умолчанию для подграфиков
			Load_Graw(0, pane1, zedGraphControl5);
			Load_Graw(0, pane2, zedGraphControl5);
			//Заполняем первый подграфик кривыми
			s = "U1";

			myCurve1 = pane1->AddCurve(s, u1_avrg, Color::Red, symbol);
			myCurve1->Line->Width = lineWidth;
			myCurve1->Symbol->Fill->Type = FillType::Solid;
			myCurve1->Symbol->Size = 9;
			s = "U2";
			myCurve1 = pane1->AddCurve(s, u2_avrg, Color::Cyan, symbol);
			myCurve1->Line->Width = lineWidth;
			myCurve1->Symbol->Fill->Type = FillType::Solid;
			myCurve1->Symbol->Size = 9;
			s = "U3";
			myCurve1 = pane1->AddCurve(s, u3_avrg, BWColor, symbol);
			myCurve1->Line->Width = lineWidth;
			myCurve1->Symbol->Fill->Type = FillType::Solid;
			myCurve1->Symbol->Size = 9;
			s = "U4";
			myCurve1 = pane1->AddCurve(s, u4_avrg, Color::LawnGreen, symbol);
			myCurve1->Line->Width = lineWidth;
			myCurve1->Symbol->Fill->Type = FillType::Solid;
			myCurve1->Symbol->Size = 9;
			//Заполняем второй подграфик кривыми
			s = "U1";
			myCurve1 = pane2->AddCurve(s, u1_shifted, Color::Red, symbol);
			s = "U2";
			myCurve1 = pane2->AddCurve(s, u2_shifted, Color::Cyan, symbol);
			s = "U3";
			myCurve1 = pane2->AddCurve(s, u3_shifted, BWColor, symbol);
			s = "U4";
			myCurve1 = pane2->AddCurve(s, u4_shifted, Color::LawnGreen, symbol);
			//Добавляем подграфики на полотно
			masterPane->Add(pane1);
			masterPane->Add(pane2);

			masterPane->SetLayout(g_secondTab, PaneLayout::SingleColumn);
			masterPane->InnerPaneGap = 0;
			zedGraphControl5->IsSynchronizeXAxes = false;

			zedGraphControl5->AxisChange();
			zedGraphControl5->Invalidate();
			if (State == 0 && !enableSymbolTrigger)
			{
				ScaleGraphs(zedGraphControl5);
			}

			startBtn->BackgroundImageLayout = ImageLayout::Stretch;
			stopBtn->BackgroundImageLayout = ImageLayout::Stretch;
			pauseBtn->BackgroundImageLayout = ImageLayout::Stretch;
			startBtn->Height = 40;
			startBtn->Width = 40;
			startBtn->Left = 7;
			startBtn->Top = 2;
			pauseBtn->Height = 40;
			pauseBtn->Width = 40;
			pauseBtn->Left = 58;
			pauseBtn->Top = 2;
		}
		else if (algorithState == 1)		//Включаем или выключаем вывод графиков согласно выделенных CheckBox
		{

		}
	}

	public:	void SaveFile(char *fileLocation)
	{
		string file = fileLocation;
		string outString = "";
		int j = 0;

		title = gcnew String(file.c_str());
		title = title->Replace("\\", "\\\\");
		subStr = title->Substring(title->Length - 4);

		if (subStr != ".txt")
		{
			title += ".txt";
		}
		msclr::interop::marshal_context context;
		file = context.marshal_as<std::string>(title);
		ofstream fout(file);

		progressBar1->Maximum = zahvatRollLine->Count;
		progressBar1->Value = 0;
		Cursor->Current = Cursors::WaitCursor;

		outString = "t\tCapture\tAGR\tSaturation\tPeleng, grad\tPhase, grad\tU1, мВ\tU2, мВ\tU3, мВ\tU4, mV\tCounter\t\t";
		outString += context.marshal_as<std::string>(pvkNumberLbl->Text);
		outString += context.marshal_as<std::string>(softVerLbl->Text);
		outString += "\n";
		fout << outString;
		for (long i = 0; i < zahvatRollLine->Count; i++)
		{
			progressBar1->Value++;
			outString = to_string(zahvatRollLine[i]->X);
			outString += '\t';
			outString += to_string((int)zahvatRollLine[i]->Y);
			outString += '\t';
			outString += to_string((int)ARURollLine[i]->Y);
			outString += '\t';
			outString += to_string((int)zasvetRollLine[i]->Y);
			outString += '\t';
			outString += to_string(pelengRollLine[i]->Y);
			outString += '\t';
			outString += to_string(phaseRollLine[i]->Y);
			outString += '\t';
			j = 0;
			while (U1RollLine[j]->X <= zahvatRollLine[i]->X)
			{
				if (j < U1RollLine->Count - 1)
				{
					j++;
				}
				else
				{
					j = U1RollLine->Count - 1;
					break;
				}
			}
			outString += to_string(U1RollLine[j]->Y);
			outString += '\t';
			outString += to_string(U2RollLine[j]->Y);
			outString += '\t';
			outString += to_string(U3RollLine[j]->Y);
			outString += '\t';
			outString += to_string(U4RollLine[j]->Y);
			outString += '\t';
			outString += to_string(counterRollLine[i]->Y);
			outString += '\n';

			fout << outString;
		}

		fout.close();

		progressBar1->Value = progressBar1->Maximum;
	}


	public:	void ReadFile(char *fileLocation)
	{
		//For M-120
		int time_pos, capture_pos, lighted_pos, aru_pos, cycCount_pos, peleng_pos, phase_pos, time2_pos, canMsg2_pos, u1_pos, u2_pos, u3_pos, u4_pos, counter_pos, end_pos;
		int pvkNumber_pos, pvkVersion_pos;
		double time = 0, capture = 0, aru = 0, lighted = 0, peleng = 0, phase = 0, u1 = 0, u2 = 0, u3 = 0, u4 = 0, counter = 0;

		ifstream fin;				// Here we create an ifstream object and we will call it "fin", like "cin"
		string strLine = "";		// Lets create a string to hold a line of text from the file
		string strWord = "";		// This will hold a word of text

		fin.open(fileLocation);
		string ssss = fileLocation;
		String^ title = gcnew String(ssss.c_str());
		this->Text = title;
		//GuiForm
		if (fin.fail())										// But before we start reading .. we need to check IF there is even a file there....
		{													// We do that by calling a function from "fin" called fail().  It tells us if the file was opened or not.
			strWord = "ERROR: File couldn't be found!\n";	// Now, Print out an error message.  This is very important, especially when getting into huge projects.
		}

		end_of_file = 0;
		while (getline(fin, strLine))							// "fin" works just like "cin", it will read 1 word at a time.  The loop will continue until we reach the end of the file.
		{
			end_of_file++;
		}
		fin.close();
		progressBar1->Maximum = end_of_file - 1;
		progressBar1->Value = 0;

		Cursor->Current = Cursors::WaitCursor;

		fin.open(fileLocation);
		getline(fin, strLine);		//Get first line with graph header
		ParseHeader(strLine);
		pvkNumber_pos = strLine.find("ПВК №", 0);
		pvkVersion_pos = strLine.find("v.", pvkNumber_pos);
		if (pvkNumber_pos != -1 && pvkVersion_pos != -1)
		{
			ssss = strLine.substr(pvkNumber_pos, pvkVersion_pos - pvkNumber_pos);
			String^ HWnum = gcnew String(ssss.c_str());
			pvkNumberLbl->Text = HWnum;

			ssss = strLine.substr(pvkVersion_pos, strLine.length() - pvkVersion_pos);
			String^ SWver = gcnew String(ssss.c_str());
			softVerLbl->Text = SWver;
		}
		pelengMaxScale = phaseMaxScale = UMaxScale = 0;
		t = 0;

		while (getline(fin, strLine))							// "fin" works just like "cin", it will read 1 word at a time.  The loop will continue until we reach the end of the file.
		{
			progressBar1->Value++;
			string strLineCuted = strLine.substr(0, 3);		//берем первые три символа (№сообщения)
			if (strLineCuted != "Err")
			{
				//Считываем первое число в строке - время.
				time_pos = strLine.find('\t', 0);
				capture_pos = strLine.find('\t', time_pos + 1);		//Второе число в файле - номер CAN посылки
				aru_pos = strLine.find('\t', capture_pos + 1);		//Четвортое число в файле - хз
				lighted_pos = strLine.find('\t', aru_pos + 1);		//Третье число в файле - захват

			//	cycCount_pos = strLine.find('\t', aru_pos + 1);		//Пятое число в файле - АРУ
				peleng_pos = strLine.find('\t', lighted_pos + 1);	//Шестое число в файле - счетчик
				phase_pos = strLine.find('\t', peleng_pos + 1);		//Седьмое число в файле - пеленг

				u1_pos = strLine.find('\t', phase_pos + 1);			//U1
				u2_pos = strLine.find('\t', u1_pos + 1);			//U2
				u3_pos = strLine.find('\t', u2_pos + 1);			//U3
				u4_pos = strLine.find('\t', u3_pos + 1);

				if (capture_pos && peleng_pos != -1 && phase_pos != -1 && time_pos != -1)
				{
					if (u1_pos != -1 && u2_pos != -1 && u3_pos != -1)
					{
						t = stod(strLine.substr(0, time_pos));
						capture = stod(strLine.substr(time_pos, capture_pos));
						aru = stod(strLine.substr(capture_pos, aru_pos));
						lighted = stod(strLine.substr(aru_pos, lighted_pos));
						peleng = stod(strLine.substr(lighted_pos, peleng_pos));
						phase = stod(strLine.substr(peleng_pos, phase_pos));
						u1 = stod(strLine.substr(phase_pos, u1_pos));
						u2 = stod(strLine.substr(u1_pos, u2_pos));
						u3 = stod(strLine.substr(u2_pos, u3_pos));
						u4 = stod(strLine.substr(u3_pos, u4_pos));

						if (u4_pos != -1)
						{
							counter = stod(strLine.substr(u4_pos, strLine.length()));
							counterRollLine->Add(t, counter);
						}

						zahvatRollLine->Add(t, capture);
						zasvetRollLine->Add(t, lighted);
						ARURollLine->Add(t, aru);
						pelengRollLine->Add(t, peleng);
						phaseRollLine->Add(t, phase);
						U1RollLine->Add(t, u1);
						U2RollLine->Add(t, u2);
						U3RollLine->Add(t, u3);
						U4RollLine->Add(t, u4);

						if (peleng > pelengMaxScale)
						{
							pelengMaxScale = peleng;
						}
						if (phase > phaseMaxScale)
						{
							phaseMaxScale = phase;
						}
						if (u1 > UMaxScale)
						{
							UMaxScale = u1;
						}
						if (u2 > UMaxScale)
						{
							UMaxScale = u2;
						}
						if (u3 > UMaxScale)
						{
							UMaxScale = u3;
						}
						if (u4 > UMaxScale)
						{
							UMaxScale = u4;
						}
					}
				}
			}
		}
		zedGraphControl1->AxisChange();
		zedGraphControl1->Invalidate();

		ScaleGraphs(zedGraphControl1);
		//ScaleGraphs(zedGraphControl5);

		tabControl1->SelectTab(0);
	}


	public:	void FindTransitions(void)
	{
		double time_prev2 = 0, timeBuffer[BufferSize], phaseBuffer[BufferSize];
		int middleOfBuffer = BufferSize >> 1;
		int bufferPointer = 0, intervalCounter = 0;
		long i = 0;

		for (int i = 0; i < BufferSize; i++)
		{
			timeBuffer[i] = 0;
			phaseBuffer[i] = 0;
		}
		for (i = 0; i < zedGraphControl1->MasterPane->PaneList[2]->CurveList[0]->Points->Count; i++)
		{
			progressBar1->Value++;
			for (int j = 0; j < BufferSize - 1; j++)
			{
				phaseBuffer[j] = phaseBuffer[j + 1];
				timeBuffer[j] = timeBuffer[j + 1];
			}
			phaseBuffer[BufferSize - 1] = zedGraphControl1->MasterPane->PaneList[2]->CurveList[0]->Points[i]->Y;
			timeBuffer[BufferSize - 1] = zedGraphControl1->MasterPane->PaneList[2]->CurveList[0]->Points[i]->X;

			if (phaseBuffer[0] != 0)
			{	//Отсекает первые итерации, когда все элементы массива еще не прогрузились
				avrgFromLeft = (phaseBuffer[0] + phaseBuffer[1] + phaseBuffer[2] + phaseBuffer[3] + phaseBuffer[4]) / 5;
				avrgFromRight = (phaseBuffer[5] + phaseBuffer[6] + phaseBuffer[7] + phaseBuffer[8] + phaseBuffer[9]) / 5;
				if (abs(avrgFromRight - avrgFromLeft) > (PhaseCharacteristicStep / 2))
				{
					intervalCounter++;
					for (int j = 0; j < BufferSize; j++)
					{
						phaseBuffer[j] = 0;
					}		//Обнуляем массив, чтобы не было повторного срабатывания алгоритма в этой области
					switch (intervalCounter)
					{
					case 1:
						s0 = start[0];
						/*textBox1->Text = (0).ToString("F2");
						textBox2->Text = timeBuffer[0].ToString("F2");
						textBox3->Text = timeBuffer[BufferSize - 1].ToString("F2");*/
						e0 = end[0] = i - middleOfBuffer;
						s1 = start[1] = i + middleOfBuffer;
						break;
					case 2:
					/*	textBox4->Text = timeBuffer[0].ToString("F2");
						textBox5->Text = timeBuffer[BufferSize - 1].ToString("F2");*/
						e1 = end[1] = i - middleOfBuffer;
						s2 = start[2] = i + middleOfBuffer;
						break;
					case 3:
					//	textBox6->Text = timeBuffer[0].ToString("F2");
					//	textBox7->Text = timeBuffer[BufferSize - 1].ToString("F2");
						e2 = end[2] = i - middleOfBuffer;
						s3 = start[3] = i + middleOfBuffer;
						break;
					case 4:
					//	textBox8->Text = timeBuffer[0].ToString("F2");
					//	textBox9->Text = timeBuffer[BufferSize - 1].ToString("F2");
						e3 = end[3] = i - middleOfBuffer;
						s4 = start[4] = i + middleOfBuffer;
						break;
					case 5:
					//	textBox10->Text = timeBuffer[0].ToString("F2");
					//	textBox11->Text = timeBuffer[BufferSize - 1].ToString("F2");
						e4 = end[4] = i - middleOfBuffer;
						s5 = start[5] = i + middleOfBuffer;
						break;
					case 6:
					//	textBox12->Text = timeBuffer[0].ToString("F2");
					//	textBox13->Text = timeBuffer[BufferSize - 1].ToString("F2");
						e5 = end[5] = i - middleOfBuffer;
						s6 = start[6] = i + middleOfBuffer;
						break;
					case 7:
					//	textBox14->Text = timeBuffer[0].ToString("F2");
					//	textBox15->Text = timeBuffer[BufferSize - 1].ToString("F2");
						e6 = end[6] = i - middleOfBuffer;
						s7 = start[7] = i + middleOfBuffer;
						break;
					case 8:
					//	textBox16->Text = timeBuffer[0].ToString("F2");
						textBox17->Text = timeBuffer[BufferSize - 1].ToString("F2");
						textBox18->Text = (timeBuffer[BufferSize - 1] + 3).ToString("F2");
						e7 = end[7] = i - middleOfBuffer;
						s8 = start[8] = i + middleOfBuffer;
						e8 = end[8] = i + 100;
						break;
					default: break;
					}
				}
			}
			ReDrawBoxes(zedGraphControl1);
		}

	}


	public:	void Calibrate(void)
	{
		int i = 0, j = 0, k = 0, n = 0;
		float U1_avg[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		float U2_avg[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		float U3_avg[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		float U4_avg[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		float sum1 = 0, sum2 = 0, sum3 = 0, sum4 = 0, fi_1, fi_2, faza, pel;
		float sum = 0, K1 = 0, K2 = 0, K3 = 0, K4 = 0;

		//Clear the graphs
		u1_avrg->Clear();
		u2_avrg->Clear();
		u3_avrg->Clear();
		u4_avrg->Clear();
		u1_shifted->Clear();
		u2_shifted->Clear();
		u3_shifted->Clear();
		u4_shifted->Clear();
		//Смещаем площадки для графика
		for (i = start[0]; i < end[0]; i++)
		{

			u1_shifted->Add(zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X, zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->Y);
			if ((end[2] - start[2]) > (i - start[0]))
				u2_shifted->Add(zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X, zedGraphControl1->MasterPane->PaneList[3]->CurveList[1]->Points[i + start[2]]->Y);
			if ((end[4] - start[4]) > (i - start[0]))
				u3_shifted->Add(zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X, zedGraphControl1->MasterPane->PaneList[3]->CurveList[2]->Points[i + start[4]]->Y);
			if ((end[6] - start[6]) > (i - start[0]))
				u4_shifted->Add(zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X, zedGraphControl1->MasterPane->PaneList[3]->CurveList[3]->Points[i + start[6]]->Y);
		}
		for (i = start[1]; i < end[1]; i++)
		{
			u1_shifted->Add(zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X, zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->Y);
			if ((end[3] - start[3]) > (i - start[1]))
				u2_shifted->Add(zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X, zedGraphControl1->MasterPane->PaneList[3]->CurveList[1]->Points[i + start[3] - start[1]]->Y);
			if ((end[5] - start[5]) > (i - start[1]))
				u3_shifted->Add(zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X, zedGraphControl1->MasterPane->PaneList[3]->CurveList[2]->Points[i + start[5] - start[1]]->Y);
			if ((end[7] - start[7]) > (i - start[1]))
				u4_shifted->Add(zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X, zedGraphControl1->MasterPane->PaneList[3]->CurveList[3]->Points[i + start[7] - start[1]]->Y);
		}
		for (i = start[2]; i < end[2]; i++)
		{
			u1_shifted->Add(zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X, zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->Y);
			if ((end[4] - start[4]) > (i - start[2]))
				u2_shifted->Add(zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X, zedGraphControl1->MasterPane->PaneList[3]->CurveList[1]->Points[i + start[4] - start[2]]->Y);
			if ((end[6] - start[6]) > (i - start[2]))
				u3_shifted->Add(zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X, zedGraphControl1->MasterPane->PaneList[3]->CurveList[2]->Points[i + start[6] - start[2]]->Y);
			if ((end[8] - start[8]) > (i - start[2]))
				u4_shifted->Add(zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X, zedGraphControl1->MasterPane->PaneList[3]->CurveList[3]->Points[i + start[8] - start[2]]->Y);
		}
		for (i = start[3]; i < end[3]; i++)
		{
			u1_shifted->Add(zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X, zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->Y);
			if ((end[5] - start[5]) > (i - start[3]))
				u2_shifted->Add(zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X, zedGraphControl1->MasterPane->PaneList[3]->CurveList[1]->Points[i + start[5] - start[3]]->Y);
			if ((end[7] - start[7]) > (i - start[3]))
				u3_shifted->Add(zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X, zedGraphControl1->MasterPane->PaneList[3]->CurveList[2]->Points[i + start[7] - start[3]]->Y);
			if ((end[1] - start[1]) > (i - start[3]))
				u4_shifted->Add(zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X, zedGraphControl1->MasterPane->PaneList[3]->CurveList[3]->Points[i + start[1] - start[3]]->Y);
		}
		for (i = start[4]; i < end[4]; i++)
		{
			u1_shifted->Add(zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X, zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->Y);
			if ((end[6] - start[6]) > (i - start[4]))
				u2_shifted->Add(zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X, zedGraphControl1->MasterPane->PaneList[3]->CurveList[1]->Points[i + start[6] - start[4]]->Y);
			if ((end[8] - start[8]) > (i - start[4]))
				u3_shifted->Add(zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X, zedGraphControl1->MasterPane->PaneList[3]->CurveList[2]->Points[i + start[8] - start[4]]->Y);
			if ((end[2] - start[2]) > (i - start[4]))
				u4_shifted->Add(zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X, zedGraphControl1->MasterPane->PaneList[3]->CurveList[3]->Points[i + start[2] - start[4]]->Y);
		}
		for (i = start[5]; i < end[5]; i++)
		{
			u1_shifted->Add(zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X, zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->Y);
			if ((end[7] - start[7]) > (i - start[5]))
				u2_shifted->Add(zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X, zedGraphControl1->MasterPane->PaneList[3]->CurveList[1]->Points[i + start[7] - start[5]]->Y);
			if ((end[1] - start[1]) > (i - start[5]))
				u3_shifted->Add(zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X, zedGraphControl1->MasterPane->PaneList[3]->CurveList[2]->Points[i + start[1] - start[5]]->Y);
			if ((end[3] - start[3]) > (i - start[5]))
				u4_shifted->Add(zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X, zedGraphControl1->MasterPane->PaneList[3]->CurveList[3]->Points[i + start[3] - start[5]]->Y);
		}
		for (i = start[6]; i < end[6]; i++)
		{
			u1_shifted->Add(zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X, zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->Y);
			if ((end[8] - start[8]) > (i - start[6]))
				u2_shifted->Add(zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X, zedGraphControl1->MasterPane->PaneList[3]->CurveList[1]->Points[i + start[8] - start[6]]->Y);
			if ((end[2] - start[2]) > (i - start[6]))
				u3_shifted->Add(zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X, zedGraphControl1->MasterPane->PaneList[3]->CurveList[2]->Points[i + start[2] - start[6]]->Y);
			if ((end[4] - start[4]) > (i - start[6]))
				u4_shifted->Add(zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X, zedGraphControl1->MasterPane->PaneList[3]->CurveList[3]->Points[i + start[4] - start[6]]->Y);
		}
		for (i = start[7]; i < end[7]; i++)
		{
			u1_shifted->Add(zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X, zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->Y);
			if ((end[1] - start[1]) > (i - start[7]))
				u2_shifted->Add(zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X, zedGraphControl1->MasterPane->PaneList[3]->CurveList[1]->Points[i + start[1] - start[7]]->Y);
			if ((end[3] - start[3]) > (i - start[7]))
				u3_shifted->Add(zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X, zedGraphControl1->MasterPane->PaneList[3]->CurveList[2]->Points[i + start[3] - start[7]]->Y);
			if ((end[5] - start[5]) > (i - start[7]))
				u4_shifted->Add(zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X, zedGraphControl1->MasterPane->PaneList[3]->CurveList[3]->Points[i + start[5] - start[7]]->Y);
		}
		for (i = start[8]; i < end[8]; i++)
		{
			u1_shifted->Add(zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X, zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->Y);
			if ((end[2] - start[2]) > (i - start[8]))
				u2_shifted->Add(zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X, zedGraphControl1->MasterPane->PaneList[3]->CurveList[1]->Points[i + start[2] - start[8]]->Y);
			if ((end[4] - start[4]) > (i - start[8]))
				u3_shifted->Add(zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X, zedGraphControl1->MasterPane->PaneList[3]->CurveList[2]->Points[i + start[4] - start[8]]->Y);
			if ((end[6] - start[6]) > (i - start[8]))
				u4_shifted->Add(zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X, zedGraphControl1->MasterPane->PaneList[3]->CurveList[3]->Points[i + start[6] - start[8]]->Y);
		}

		for (j = 0; j < 9; j++)
		{
			// СЧИТАЕМ СРЕДНЕЕ ЗНАЧЕНИЕ
			for (i = start[j]; i < end[j]; i++)
			{
				U1_avg[j] = U1_avg[j] + zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->Y;
			}
			U1_avg[j] = U1_avg[j] / (end[j] - start[j]);

			k = j + 2;
			if (k == 9)
				k = 1;
			if (k == 10)
				k = 2;
			for (i = start[k]; i < end[k]; i++)
			{
				U2_avg[j] = U2_avg[j] + zedGraphControl1->MasterPane->PaneList[3]->CurveList[1]->Points[i]->Y;
			}
			U2_avg[j] = U2_avg[j] / (end[k] - start[k]);

			k = k + 2;
			if (k == 9)
				k = 1;
			if (k == 10)
				k = 2;
			for (i = start[k]; i < end[k]; i++)
			{
				U3_avg[j] = U3_avg[j] + zedGraphControl1->MasterPane->PaneList[3]->CurveList[2]->Points[i]->Y;
			}
			U3_avg[j] = U3_avg[j] / (end[k] - start[k]);

			k = k + 2;
			if (k == 9)
				k = 1;
			if (k == 10)
				k = 2;
			for (i = start[k]; i < end[k]; i++)
			{
				U4_avg[j] = U4_avg[j] + zedGraphControl1->MasterPane->PaneList[3]->CurveList[3]->Points[i]->Y;
			}
			U4_avg[j] = U4_avg[j] / (end[k] - start[k]);
		}

		for (i = 0; i < 9; i++)
		{
			sum1 = sum1 + U1_avg[i];
			sum2 = sum2 + U2_avg[i];
			sum3 = sum3 + U3_avg[i];
			sum4 = sum4 + U4_avg[i];
			u1_avrg->Add(i, U1_avg[i]);
			u2_avrg->Add(i, U2_avg[i]);
			u3_avrg->Add(i, U3_avg[i]);
			u4_avrg->Add(i, U4_avg[i]);
		}

		sum = (sum1 + sum2 + sum3 + sum4) / 4;
		K1 = sum / sum1;
		K2 = sum / sum2;
		K3 = sum / sum3;
		K4 = sum / sum4;

		fi_1 = sum1 + sum4 - sum2 - sum3;
		fi_2 = sum3 + sum4 - sum1 - sum2;
		faza = atan2(fi_1, fi_2) * 57.29;
		if (changeKCheckBox->CheckState == CheckState::Checked)
		{
			try
			{
				String^ s = gcnew String("");
				s = textBox19->Text;
				if (s == "")
				{
					s = "0";
				}
				s = s->Replace(",", System::Globalization::CultureInfo::CurrentCulture->NumberFormat->NumberDecimalSeparator);
				s = s->Replace(".", System::Globalization::CultureInfo::CurrentCulture->NumberFormat->NumberDecimalSeparator);

				pel = sqrt(fi_1 * fi_1 + fi_2 * fi_2) / (sum1 + sum4 + sum2 + sum3) * System::Convert::ToDouble(s) * 57.29;
			}
			catch (System::FormatException^ e)
			{
				MessageBox::Show("Please input correct value for K.", "Wrong number format", MessageBoxButtons::OK, MessageBoxIcon::Warning);
			}
		}
		else
		{
			pel = sqrt(fi_1 * fi_1 + fi_2 * fi_2) / (sum1 + sum4 + sum2 + sum3) * K * 57.29;
		}

		label21->Text = "U1_0 = " + (sum1).ToString("F1");
		label22->Text = "U2_0 = " + (sum2).ToString("F1");
		label23->Text = "U3_0 = " + (sum3).ToString("F1");
		label24->Text = "U4_0 = " + (sum4).ToString("F1");

		label1->Text = "Phase error = " + faza.ToString("F2");
		label2->Text = "Peleng error = " + pel.ToString("F4");
		label25->Text = "gamma_0 = " + (faza / 57.29).ToString("F3");
		label26->Text = "phi_0 = " + (pel / 57.29 * 1000).ToString("F3");

		zedGraphControl5->AxisChange();
		zedGraphControl5->Invalidate();

		ScaleGraphs(zedGraphControl5);

		tabControl1->SelectTab(1);
	}

	public:		void ReDrawBoxes(ZedGraphControl ^currZed)
	{
		double start = 0, width = 0;
		bool a = true, b, c, d;
		//index = currZed->MasterPane->PaneList[2]->GraphObjList->FindLast;
		//(textBox1->Text != "") ? (start = System::Convert::ToDouble(textBox1->Text)) : (start = 0);
		//(textBox1->Text != "" && textBox2->Text != "") ? (width = System::Convert::ToDouble(textBox2->Text) - System::Convert::ToDouble(textBox1->Text)) : (width = 0);
		(width > 0) ? (width = width) : (width = 0);
		(width == 0) ? (a &= false) : (a &= true);
		currZed->MasterPane->PaneList[2]->GraphObjList->Clear();
		BoxObj ^box = gcnew BoxObj(start, 350, width, 350, Color::LightGray, Color::LightGray);// Color.FromArgb(225, 245, 225));
		box->ZOrder = ZOrder::F_BehindGrid;
		currZed->MasterPane->PaneList[2]->GraphObjList->Add(box);
		TextObj ^text = gcnew TextObj("1", start, 0);
		text->FontSpec = gcnew FontSpec("1", 28, Color::Black, false, false, false);
		currZed->MasterPane->PaneList[2]->GraphObjList->Add(text);

		//(textBox3->Text != "") ? (start = System::Convert::ToDouble(textBox3->Text)) : (start = 0);
	//	(textBox3->Text != "" && textBox4->Text != "") ? (width = System::Convert::ToDouble(textBox4->Text) - System::Convert::ToDouble(textBox3->Text)) : (width = 0);
		(width > 0) ? (width = width) : (width = 0);
		(width == 0) ? (a &= false) : (a &= true);
		box = gcnew BoxObj(start, 350, width, 350, Color::LightGray, Color::LightGray);// Color.FromArgb(225, 245, 225));
		box->ZOrder = ZOrder::F_BehindGrid;
		currZed->MasterPane->PaneList[2]->GraphObjList->Add(box);
		if (start != 0)
		{
			text = gcnew TextObj("2", start, 0);
			text->FontSpec = gcnew FontSpec("2", 28, Color::Black, false, false, false);
			currZed->MasterPane->PaneList[2]->GraphObjList->Add(text);
		}
		//(textBox5->Text != "") ? (start = System::Convert::ToDouble(textBox5->Text)) : (start = 0);
	//	(textBox5->Text != "" && textBox6->Text != "") ? (width = System::Convert::ToDouble(textBox6->Text) - System::Convert::ToDouble(textBox5->Text)) : (width = 0);
		(width > 0) ? (width = width) : (width = 0);
		(width == 0) ? (a &= false) : (a &= true);
		box = gcnew BoxObj(start, 350, width, 350, Color::LightGray, Color::LightGray);// Color.FromArgb(225, 245, 225));
		box->ZOrder = ZOrder::F_BehindGrid;//.D_BehindAxis;//.E_BehindAxis;
		currZed->MasterPane->PaneList[2]->GraphObjList->Add(box);
		if (start != 0)
		{
			text = gcnew TextObj("3", start, 0);
			text->FontSpec = gcnew FontSpec("3", 28, Color::Black, false, false, false);
			currZed->MasterPane->PaneList[2]->GraphObjList->Add(text);
		}

		//(textBox7->Text != "") ? (start = System::Convert::ToDouble(textBox7->Text)) : (start = 0);
	//	(textBox7->Text != "" && textBox8->Text != "") ? (width = System::Convert::ToDouble(textBox8->Text) - System::Convert::ToDouble(textBox7->Text)) : (width = 0);
		(width > 0) ? (width = width) : (width = 0);
		(width == 0) ? (a &= false) : (a &= true);
		box = gcnew BoxObj(start, 350, width, 350, Color::LightGray, Color::LightGray);// Color.FromArgb(225, 245, 225));
		box->ZOrder = ZOrder::F_BehindGrid;
		currZed->MasterPane->PaneList[2]->GraphObjList->Add(box);
		if (start != 0)
		{
			text = gcnew TextObj("4", start, 0);
			text->FontSpec = gcnew FontSpec("4", 28, Color::Black, false, false, false);
			currZed->MasterPane->PaneList[2]->GraphObjList->Add(text);
		}
	//	(textBox9->Text != "") ? (start = System::Convert::ToDouble(textBox9->Text)) : (start = 0);
		//(textBox9->Text != "" && textBox10->Text != "") ? (width = System::Convert::ToDouble(textBox10->Text) - System::Convert::ToDouble(textBox9->Text)) : (width = 0);
		(width > 0) ? (width = width) : (width = 0);
		(width == 0) ? (a &= false) : (a &= true);
		box = gcnew BoxObj(start, 350, width, 350, Color::LightGray, Color::LightGray);// Color.FromArgb(225, 245, 225));
		box->ZOrder = ZOrder::F_BehindGrid;//.D_BehindAxis;//.E_BehindAxis;
		currZed->MasterPane->PaneList[2]->GraphObjList->Add(box);
		if (start != 0)
		{
			text = gcnew TextObj("5", start, 0);
			text->FontSpec = gcnew FontSpec("5", 28, Color::Black, false, false, false);
			currZed->MasterPane->PaneList[2]->GraphObjList->Add(text);
		}
	//	(textBox11->Text != "") ? (start = System::Convert::ToDouble(textBox11->Text)) : (start = 0);
	//	(textBox11->Text != "" && textBox12->Text != "") ? (width = System::Convert::ToDouble(textBox12->Text) - System::Convert::ToDouble(textBox11->Text)) : (width = 0);
		(width > 0) ? (width = width) : (width = 0);
		(width == 0) ? (a &= false) : (a &= true);
		box = gcnew BoxObj(start, 350, width, 350, Color::LightGray, Color::LightGray);// Color.FromArgb(225, 245, 225));
		box->ZOrder = ZOrder::F_BehindGrid;
		currZed->MasterPane->PaneList[2]->GraphObjList->Add(box);
		if (start != 0)
		{
			text = gcnew TextObj("6", start, 0);
			text->FontSpec = gcnew FontSpec("6", 28, Color::Black, false, false, false);
			currZed->MasterPane->PaneList[2]->GraphObjList->Add(text);
		}
	//	(textBox13->Text != "") ? (start = System::Convert::ToDouble(textBox13->Text)) : (start = 0);
	//	(textBox13->Text != "" && textBox14->Text != "") ? (width = System::Convert::ToDouble(textBox14->Text) - System::Convert::ToDouble(textBox13->Text)) : (width = 0);
		(width > 0) ? (width = width) : (width = 0);
		(width == 0) ? (a &= false) : (a &= true);
		box = gcnew BoxObj(start, 350, width, 350, Color::LightGray, Color::LightGray);// Color.FromArgb(225, 245, 225));
		box->ZOrder = ZOrder::F_BehindGrid;//.D_BehindAxis;//.E_BehindAxis;
		currZed->MasterPane->PaneList[2]->GraphObjList->Add(box);
		if (start != 0)
		{
			text = gcnew TextObj("7", start, 0);
			text->FontSpec = gcnew FontSpec("7", 28, Color::Black, false, false, false);
			currZed->MasterPane->PaneList[2]->GraphObjList->Add(text);
		}

	//	(textBox15->Text != "") ? (start = System::Convert::ToDouble(textBox15->Text)) : (start = 0);
	//	(textBox15->Text != "" && textBox16->Text != "") ? (width = System::Convert::ToDouble(textBox16->Text) - System::Convert::ToDouble(textBox15->Text)) : (width = 0);
		(width > 0) ? (width = width) : (width = 0);
		(width == 0) ? (a &= false) : (a &= true);
		box = gcnew BoxObj(start, 350, width, 350, Color::LightGray, Color::LightGray);// Color.FromArgb(225, 245, 225));
		box->ZOrder = ZOrder::F_BehindGrid;
		currZed->MasterPane->PaneList[2]->GraphObjList->Add(box);
		if (start != 0)
		{
			text = gcnew TextObj("8", start, 0);
			text->FontSpec = gcnew FontSpec("8", 28, Color::Black, false, false, false);
			currZed->MasterPane->PaneList[2]->GraphObjList->Add(text);
		}

		(textBox17->Text != "") ? (start = System::Convert::ToDouble(textBox17->Text)) : (start = 0);
		(textBox17->Text != "" && textBox18->Text != "") ? (width = System::Convert::ToDouble(textBox18->Text) - System::Convert::ToDouble(textBox17->Text)) : (width = 0);
		(width > 0) ? (width = width) : (width = 0);
		(width == 0) ? (a &= false) : (a &= true);
		box = gcnew BoxObj(start, 350, width, 350, Color::LightGray, Color::LightGray);// Color.FromArgb(225, 245, 225));
		box->ZOrder = ZOrder::F_BehindGrid;//.D_BehindAxis;//.E_BehindAxis;
		currZed->MasterPane->PaneList[2]->GraphObjList->Add(box);
		if (start != 0)
		{
			text = gcnew TextObj("9", start, 0);
			text->FontSpec = gcnew FontSpec("9", 28, Color::Black, false, false, false);
			currZed->MasterPane->PaneList[2]->GraphObjList->Add(text);
		}
		currZed->IsSynchronizeXAxes = true;
		currZed->AxisChange();
		currZed->Invalidate();

	/*	if (a && textBox1->Text != "" && textBox2->Text != "" && textBox3->Text != "" && textBox4->Text != "" && textBox5->Text != ""
			&& textBox6->Text != "" && textBox7->Text != "" && textBox8->Text != "" && textBox9->Text != "" && textBox10->Text != ""
			&& textBox11->Text != "" && textBox12->Text != "" && textBox13->Text != "" && textBox14->Text != "" && textBox15->Text != ""
			&& textBox16->Text != "" && textBox17->Text != "")
		{
			boxesAreDrawedFlag = 1;
		}
		else
		{
			boxesAreDrawedFlag = 0;
		}*/
	}


				void ScaleGraphs(ZedGraphControl^  sender)
				{
					int i = 0;
					// Координаты, пересчитанные в систему координат графика
					double graphX, graphY, minY, maxY, tmp = 0;

					for (i = 0; i < sender->MasterPane->PaneList->Count; i++)
					{
						// minY = maxY = 0;
						minY = 999;
						maxY = -999;
						for (int j = 0; j < sender->MasterPane->PaneList[i]->CurveList->Count; j++)
						{
							for (int k = 0; k < sender->MasterPane->PaneList[i]->CurveList[j]->Points->Count; k++)
							{
								if (i == 0)
								{
									if (RcPitCheckBox->CheckState == CheckState::Unchecked && j == 0)
									{
										break;
									}
									if (GyroPitCheckBox->CheckState == CheckState::Unchecked && j == 1)
									{
										break;
									}
									if (StabPitCheckBox->CheckState == CheckState::Unchecked && j == 2)
									{
										break;
									}
									if (PitOutCheckBox->CheckState == CheckState::Unchecked && j == 3)
									{
										break;
									}
								}
								if (i == 3)
								{
									if (RcRollCheckBox->CheckState == CheckState::Unchecked && j == 0)
									{
										break;
									}
									if (GyroRollCheckBox->CheckState == CheckState::Unchecked && j == 1)
									{
										break;
									}
									if (StabRollCheckBox->CheckState == CheckState::Unchecked && j == 2)
									{
										break;
									}
									if (RollOutCheckBox->CheckState == CheckState::Unchecked && j == 3)
									{
										break;
									}
								}
								if (sender->MasterPane->PaneList[i]->CurveList[j]->Points[k]->Y < minY)
								{
									minY = sender->MasterPane->PaneList[i]->CurveList[j]->Points[k]->Y;
								}
								if (sender->MasterPane->PaneList[i]->CurveList[j]->Points[k]->Y > maxY)
								{
									maxY = sender->MasterPane->PaneList[i]->CurveList[j]->Points[k]->Y;
								}
								t = sender->MasterPane->PaneList[i]->CurveList[j]->Points[k]->X;
							}
						}
						if (minY == maxY)
						{
							minY -= 1;
							maxY += 1;
						}
						if (minY > maxY)
						{
							tmp = minY;
							minY = maxY;
							maxY = tmp;
						}
						sender->MasterPane->PaneList[i]->YAxis->Scale->Min = 1.3 * minY;
						if (minY > 0)
						{
							sender->MasterPane->PaneList[i]->YAxis->Scale->Min = 0.7 * minY;
						}
						sender->MasterPane->PaneList[i]->YAxis->Scale->Max = 1.3 * maxY;
						if (maxY < 0)
						{
							sender->MasterPane->PaneList[i]->YAxis->Scale->Max = 0.7 * maxY;
						}
						sender->MasterPane->PaneList[i]->XAxis->Scale->Min = 0;
						sender->MasterPane->PaneList[i]->XAxis->Scale->Max = (t > START_ROLLING_SHIT) ? (t) : (START_ROLLING_SHIT);
						sender->MasterPane->PaneList[i]->XAxis->Scale->MinorStep = (int)t / 50;
						sender->MasterPane->PaneList[i]->XAxis->Scale->MajorStep = (int)t / 10;
					}

					manualScaleTrigger = false;

					sender->AxisChange();
					sender->Invalidate();
				}

	public:	void ParseHeader(string headerStr) {
		int startPos = 0, endPos = 0;
		while (endPos != std::string::npos) {
			endPos = headerStr.find_first_of('\t', endPos + 1);
			if (endPos != -1)
			{
				string strLineCuted = headerStr.substr(startPos + 1, (endPos - startPos - 1));
				startPos = endPos;
				if (strLineCuted == "rcthr") {
					RcThrCheckBox->Enabled = true;
					RcThrTextBox->Enabled = true;
				}
				else if (strLineCuted == "rcpit") {
					RcPitCheckBox->Enabled = true;
					RcPitTextBox->Enabled = true;
				}
				else if (strLineCuted == "gyropit") {
					GyroPitCheckBox->Enabled = true;
					GyroPitTextBox->Enabled = true;
				}
				else if (strLineCuted == "intpit") {
					IntPitCheckBox->Enabled = true;
					IntPitTextBox->Enabled = true;
				}
				else if (strLineCuted == "stabpit") {
					StabPitCheckBox->Enabled = true;
					StabPitTextBox->Enabled = true;
				}
				else if (strLineCuted == "pitout") {
					PitOutCheckBox->Enabled = true;
					PitOutTextBox->Enabled = true;
				}
				else if (strLineCuted == "rcroll") {
					RcRollCheckBox->Enabled = true;
					RcRollTextBox->Enabled = true;
				}
				else if (strLineCuted == "gyroroll") {
					GyroRollCheckBox->Enabled = true;
					GyroRollTextBox->Enabled = true;
				}
				else if (strLineCuted == "introll") {
					IntRollCheckBox->Enabled = true;
					IntRollTextBox->Enabled = true;
				}
				else if (strLineCuted == "stabroll") {
					StabRollCheckBox->Enabled = true;
					StabRollTextBox->Enabled = true;
				}
				else if (strLineCuted == "rollout") {
					RollOutCheckBox->Enabled = true;
					RollOutTextBox->Enabled = true;
				}
				else if (strLineCuted == "rcyaw") {
					RcYawCheckBox->Enabled = true;
					RcYawTextBox->Enabled = true;
				}
				else if (strLineCuted == "gyroyaw") {
					GyroYawCheckBox->Enabled = true;
					GyroYawTextBox->Enabled = true;
				}
				else if (strLineCuted == "intyaw") {
					IntYawCheckBox->Enabled = true;
					IntYawTextBox->Enabled = true;
				}
				else if (strLineCuted == "stabyaw") {
					StabYawCheckBox->Enabled = true;
					StabYawTextBox->Enabled = true;
				}
				else if (strLineCuted == "yawout") {
					YawOutCheckBox->Enabled = true;
					YawOutTextBox->Enabled = true;
				}
				else if (strLineCuted == "FR") {
					FRCheckBox->Enabled = true;
					FRTextBox->Enabled = true;
				}
				else if (strLineCuted == "FL") {
					FLCheckBox->Enabled = true;
					FLTextBox->Enabled = true;
				}
				else if (strLineCuted == "BR") {
					BRCheckBox->Enabled = true;
					BRTextBox->Enabled = true;
				}
				else if (strLineCuted == "BL") {
					BLCheckBox->Enabled = true;
					BLTextBox->Enabled = true;
				}
			}
		}
	}
				void startBtnClickHandler(void)
				{

				}


				void stopBtnClickHandler(void)
				{
					algorithState = 0;
					FileRead(0, 1);
					calibrationInProgressFlag = 0;
					StartOrStop = 3;
					pelengMaxScale = phaseMaxScale = UMaxScale = 0;
					if (boxesAreDrawedFlag)
					{
						/*textBox1->Text = "";
						textBox2->Text = "";
						textBox3->Text = "";
						textBox4->Text = "";
						textBox5->Text = "";
						textBox6->Text = "";
						textBox7->Text = "";
						textBox8->Text = "";
						textBox9->Text = "";
						textBox10->Text = "";
						textBox11->Text = "";
						textBox12->Text = "";
						textBox13->Text = "";
						textBox14->Text = "";
						textBox15->Text = "";
						textBox16->Text = "";*/
						textBox17->Text = "";
						textBox18->Text = "";
						ReDrawBoxes(zedGraphControl1);
						boxesAreDrawedFlag = 0;
					}
				}

				void saveBtnClickHandler(void)
				{
					Stream^ myStream;
					if (saveFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
					{
						if (saveFileDialog1->FileName != ".txt")
						{
							// if ((myStream = saveFileDialog1->OpenFile()) != nullptr)
							// {
							//	 char* fileLocation = (char*)(void*)Marshal::StringToHGlobalAnsi(saveFileDialog1->FileName);
							/* for (int i = 0; fileLocation[i] != '\0'; i++)
							{

							}*/
							//	 SaveFile(fileLocation);
							//	 myStream->Close();
							// }
							char* fileLocation = (char*)(void*)Marshal::StringToHGlobalAnsi(saveFileDialog1->FileName);
							SaveFile(fileLocation);
						}
					}
				}


				void openBtnClickHandler(void)
				{
					if (openFileDialog1->ShowDialog() == System::Windows::Forms::DialogResult::OK)
					{
						System::IO::StreamReader ^ sr = gcnew
							System::IO::StreamReader(openFileDialog1->FileName);
						string tmp;
						char* fileLocation = (char*)(void*)Marshal::StringToHGlobalAnsi(openFileDialog1->FileName);
						algorithState = 0;

						StartOrStop = 3;
						startBtn->Height = 40;
						startBtn->Width = 40;
						startBtn->Left = 7;
						startBtn->Top = 2;
						pauseBtn->Height = 40;
						pauseBtn->Width = 40;
						pauseBtn->Left = 58;
						pauseBtn->Top = 2;

						FileRead(0, 1);
						ReadFile(fileLocation);
					}
				}


				void calibrationBtnClickHandler(void)
				{
					if (boxesAreDrawedFlag)
					{
						Calibrate();
					}
					else
					{
						//No data on graph
						MessageBox::Show("Please select transitions first.", "Nothing to calibrate", MessageBoxButtons::OK, MessageBoxIcon::Warning);
					}
				}


				void findTransitionBtnClickHandler(void)
				{
					if (zedGraphControl1->MasterPane->PaneList->Count == 0 || zedGraphControl1->MasterPane->PaneList[0]->CurveList->Count == 0
						|| zedGraphControl1->MasterPane->PaneList[1]->CurveList[0]->Points->Count == 0)
					{
						//No data on graph
						MessageBox::Show("Please load the data first.", "No suitable data to calculate trasitions", MessageBoxButtons::OK, MessageBoxIcon::Warning);
					}
					else
					{
						if (boxesAreDrawedFlag)
						{
							/*textBox1->Text = "";
							textBox2->Text = "";
							textBox3->Text = "";
							textBox4->Text = "";
							textBox5->Text = "";
							textBox6->Text = "";
							textBox7->Text = "";
							textBox8->Text = "";
							textBox9->Text = "";
							textBox10->Text = "";
							textBox11->Text = "";
							textBox12->Text = "";
							textBox13->Text = "";
							textBox14->Text = "";
							textBox15->Text = "";
							textBox16->Text = "";*/
							textBox17->Text = "";
							textBox18->Text = "";
							ReDrawBoxes(zedGraphControl1);
							boxesAreDrawedFlag = 0;
						}
						else
						{
							progressBar1->Maximum = zedGraphControl1->MasterPane->PaneList[2]->CurveList[0]->Points->Count;
							progressBar1->Value = 0;
							Cursor->Current = Cursors::WaitCursor;
							FindTransitions();
							boxesAreDrawedFlag = 1;
						}
						checkBoxCounter = 0;
						calibrationInProgressFlag = 1;
					}
				}


				void nightModeBtnClickHandler(void)
				{
					int panePointer = 0, prevAlgorithState = 0;
					if (checkBox1->CheckState == CheckState::Checked)
					{
						zedGraphControl1->BackColor = Color::Black;
						zedGraphControl5->BackColor = Color::Black;
						prevAlgorithState = algorithState;
						algorithState = 0;
						FileRead(0, 0);
						algorithState = prevAlgorithState;
						//Все для ночного режима
						this->BackColor = Color::Black;		//цвет фона базового окна

						panel1->BackColor = Color::Black;
						panel1->ForeColor = Color::WhiteSmoke;
						panel2->BackColor = Color::Black;
						panel2->ForeColor = Color::WhiteSmoke;
						panel5->BackColor = Color::Black;
						panel5->ForeColor = Color::WhiteSmoke;
						tabControl1->BackColor = Color::Black;
						tabControl1->TabPages[0]->BackColor = Color::Black;
						tabControl1->TabPages[1]->BackColor = Color::Black;
						tabControl1->ForeColor = Color::WhiteSmoke;
						checkBox1->BackColor = Color::Black;
						checkBox1->ForeColor = Color::WhiteSmoke;
						button1->BackColor = Color::Black;
						button1->ForeColor = Color::WhiteSmoke;
						startBtn->BackColor = Color::Black;
						startBtn->ForeColor = Color::WhiteSmoke;
						pauseBtn->BackColor = Color::Black;
						pauseBtn->ForeColor = Color::WhiteSmoke;
						stopBtn->BackColor = Color::Black;
						stopBtn->ForeColor = Color::WhiteSmoke;
						openBtn->BackColor = Color::Black;
						openBtn->ForeColor = Color::WhiteSmoke;
						saveBtn->BackColor = Color::Black;
						saveBtn->ForeColor = Color::WhiteSmoke;
						progressBar1->BackColor = Color::Black;
						progressBar1->ForeColor = Color::WhiteSmoke;
					}
					else
					{
						zedGraphControl1->BackColor = Color::White;
						zedGraphControl5->BackColor = Color::White;

						prevAlgorithState = algorithState;
						algorithState = 0;
						FileRead(0, 0);
						algorithState = prevAlgorithState;
						//Все для дневного режима
						this->BackColor = System::Windows::Forms::Form::DefaultBackColor;

						panel1->BackColor = Color::White;
						panel1->ForeColor = Color::Black;
						panel2->BackColor = Color::White;
						panel2->ForeColor = Color::Black;
						panel5->BackColor = Color::White;
						panel5->ForeColor = Color::Black;
						tabControl1->BackColor = Color::White;
						tabControl1->TabPages[0]->BackColor = Color::White;
						tabControl1->TabPages[1]->BackColor = Color::White;
						tabControl1->ForeColor = Color::Black;
						checkBox1->BackColor = Color::White;
						checkBox1->ForeColor = Color::Black;
						button1->BackColor = Color::White;
						button1->ForeColor = Color::Black;
						startBtn->BackColor = Color::White;
						startBtn->ForeColor = Color::Black;
						pauseBtn->BackColor = Color::White;
						pauseBtn->ForeColor = Color::Black;
						stopBtn->BackColor = Color::White;
						stopBtn->ForeColor = Color::Black;
						openBtn->BackColor = Color::White;
						openBtn->ForeColor = Color::Black;
						saveBtn->BackColor = Color::White;
						saveBtn->ForeColor = Color::Black;
						progressBar1->BackColor = System::Windows::Forms::Form::DefaultBackColor;
						progressBar1->ForeColor = Color::Black;
					}
				}

	public:
		GuiForm(void)
		{
			InitializeComponent();

		}
	protected:
		/// <summary>
		/// Освободить все используемые ресурсы.
		/// </summary>
		~GuiForm()
		{
			if (components)
			{
				delete components;
			}
		}

	protected:
	private: System::Windows::Forms::Button^  button1;
	private: System::Windows::Forms::Button^  startBtn;


	private: System::Windows::Forms::OpenFileDialog^  openFileDialog1;
	private: System::ComponentModel::IContainer^  components;

	private:
		/// <summary>
		/// Требуется переменная конструктора.
		/// </summary>


#pragma region Windows Form Designer generated code
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(GuiForm::typeid));
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->startBtn = (gcnew System::Windows::Forms::Button());
			this->openFileDialog1 = (gcnew System::Windows::Forms::OpenFileDialog());
			this->BottomToolStripPanel = (gcnew System::Windows::Forms::ToolStripPanel());
			this->TopToolStripPanel = (gcnew System::Windows::Forms::ToolStripPanel());
			this->RightToolStripPanel = (gcnew System::Windows::Forms::ToolStripPanel());
			this->LeftToolStripPanel = (gcnew System::Windows::Forms::ToolStripPanel());
			this->ContentPanel = (gcnew System::Windows::Forms::ToolStripContentPanel());
			this->tabPage5 = (gcnew System::Windows::Forms::TabPage());
			this->changeKCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->textBox19 = (gcnew System::Windows::Forms::TextBox());
			this->label28 = (gcnew System::Windows::Forms::Label());
			this->label27 = (gcnew System::Windows::Forms::Label());
			this->label26 = (gcnew System::Windows::Forms::Label());
			this->label25 = (gcnew System::Windows::Forms::Label());
			this->label24 = (gcnew System::Windows::Forms::Label());
			this->label23 = (gcnew System::Windows::Forms::Label());
			this->label22 = (gcnew System::Windows::Forms::Label());
			this->label21 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->zedGraphControl5 = (gcnew ZedGraph::ZedGraphControl());
			this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
			this->tabPage1 = (gcnew System::Windows::Forms::TabPage());
			this->zedGraphControl1 = (gcnew ZedGraph::ZedGraphControl());
			this->softVerLbl = (gcnew System::Windows::Forms::Label());
			this->pvkNumberLbl = (gcnew System::Windows::Forms::Label());
			this->panel7 = (gcnew System::Windows::Forms::Panel());
			this->RolEnAllCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->PitEnAllCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->panel6 = (gcnew System::Windows::Forms::Panel());
			this->label29 = (gcnew System::Windows::Forms::Label());
			this->RcThrTextBox = (gcnew System::Windows::Forms::TextBox());
			this->RcThrCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->MotorEnAllCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->FRCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->BRCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->FLCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->BLCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->label17 = (gcnew System::Windows::Forms::Label());
			this->BRTextBox = (gcnew System::Windows::Forms::TextBox());
			this->label15 = (gcnew System::Windows::Forms::Label());
			this->BLTextBox = (gcnew System::Windows::Forms::TextBox());
			this->label13 = (gcnew System::Windows::Forms::Label());
			this->FRTextBox = (gcnew System::Windows::Forms::TextBox());
			this->label11 = (gcnew System::Windows::Forms::Label());
			this->FLTextBox = (gcnew System::Windows::Forms::TextBox());
			this->label19 = (gcnew System::Windows::Forms::Label());
			this->textBox17 = (gcnew System::Windows::Forms::TextBox());
			this->textBox18 = (gcnew System::Windows::Forms::TextBox());
			this->label20 = (gcnew System::Windows::Forms::Label());
			this->YawOutTextBox = (gcnew System::Windows::Forms::TextBox());
			this->label18 = (gcnew System::Windows::Forms::Label());
			this->StabYawTextBox = (gcnew System::Windows::Forms::TextBox());
			this->label16 = (gcnew System::Windows::Forms::Label());
			this->GyroYawTextBox = (gcnew System::Windows::Forms::TextBox());
			this->label14 = (gcnew System::Windows::Forms::Label());
			this->RcYawTextBox = (gcnew System::Windows::Forms::TextBox());
			this->label12 = (gcnew System::Windows::Forms::Label());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->GyroRollTextBox = (gcnew System::Windows::Forms::TextBox());
			this->RcRollTextBox = (gcnew System::Windows::Forms::TextBox());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->label10 = (gcnew System::Windows::Forms::Label());
			this->RollOutTextBox = (gcnew System::Windows::Forms::TextBox());
			this->StabRollTextBox = (gcnew System::Windows::Forms::TextBox());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->RcPitTextBox = (gcnew System::Windows::Forms::TextBox());
			this->GyroPitTextBox = (gcnew System::Windows::Forms::TextBox());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->PitOutTextBox = (gcnew System::Windows::Forms::TextBox());
			this->StabPitTextBox = (gcnew System::Windows::Forms::TextBox());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->panel5 = (gcnew System::Windows::Forms::Panel());
			this->IntRollCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->IntRollTextBox = (gcnew System::Windows::Forms::TextBox());
			this->label31 = (gcnew System::Windows::Forms::Label());
			this->GyroRollCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->RollOutCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->RcRollCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->StabRollCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->panel2 = (gcnew System::Windows::Forms::Panel());
			this->label32 = (gcnew System::Windows::Forms::Label());
			this->IntPitTextBox = (gcnew System::Windows::Forms::TextBox());
			this->IntPitCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->PitOutCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->GyroPitCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->StabPitCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->RcPitCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->checkBox1 = (gcnew System::Windows::Forms::CheckBox());
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->progressBar1 = (gcnew System::Windows::Forms::ProgressBar());
			this->pauseBtn = (gcnew System::Windows::Forms::Button());
			this->saveBtn = (gcnew System::Windows::Forms::Button());
			this->openBtn = (gcnew System::Windows::Forms::Button());
			this->stopBtn = (gcnew System::Windows::Forms::Button());
			this->saveFileDialog1 = (gcnew System::Windows::Forms::SaveFileDialog());
			this->toolTip1 = (gcnew System::Windows::Forms::ToolTip(this->components));
			this->toolTip2 = (gcnew System::Windows::Forms::ToolTip(this->components));
			this->toolTip3 = (gcnew System::Windows::Forms::ToolTip(this->components));
			this->toolTip4 = (gcnew System::Windows::Forms::ToolTip(this->components));
			this->toolTip5 = (gcnew System::Windows::Forms::ToolTip(this->components));
			this->toolTip6 = (gcnew System::Windows::Forms::ToolTip(this->components));
			this->toolTip7 = (gcnew System::Windows::Forms::ToolTip(this->components));
			this->panel3 = (gcnew System::Windows::Forms::Panel());
			this->IntYawCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->IntYawTextBox = (gcnew System::Windows::Forms::TextBox());
			this->label30 = (gcnew System::Windows::Forms::Label());
			this->YawEnAllCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->GyroYawCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->YawOutCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->RcYawCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->StabYawCheckBox = (gcnew System::Windows::Forms::CheckBox());
			this->tabPage5->SuspendLayout();
			this->tabControl1->SuspendLayout();
			this->tabPage1->SuspendLayout();
			this->panel6->SuspendLayout();
			this->panel5->SuspendLayout();
			this->panel2->SuspendLayout();
			this->panel1->SuspendLayout();
			this->panel3->SuspendLayout();
			this->SuspendLayout();
			// 
			// button1
			// 
			this->button1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->button1->BackColor = System::Drawing::Color::Transparent;
			this->button1->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"button1.BackgroundImage")));
			this->button1->FlatAppearance->BorderSize = 0;
			this->button1->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->button1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->button1->Location = System::Drawing::Point(1065, 2);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(40, 40);
			this->button1->TabIndex = 3;
			this->toolTip2->SetToolTip(this->button1, L"Calibration (C)");
			this->button1->UseVisualStyleBackColor = false;
			this->button1->Click += gcnew System::EventHandler(this, &GuiForm::button1_Click);
			// 
			// startBtn
			// 
			this->startBtn->BackColor = System::Drawing::Color::Transparent;
			this->startBtn->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"startBtn.BackgroundImage")));
			this->startBtn->FlatAppearance->BorderSize = 0;
			this->startBtn->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->startBtn->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->startBtn->Location = System::Drawing::Point(7, 2);
			this->startBtn->Margin = System::Windows::Forms::Padding(0);
			this->startBtn->Name = L"startBtn";
			this->startBtn->Size = System::Drawing::Size(40, 40);
			this->startBtn->TabIndex = 3;
			this->toolTip3->SetToolTip(this->startBtn, L"Start CAN (P)");
			this->startBtn->UseVisualStyleBackColor = false;
			this->startBtn->Click += gcnew System::EventHandler(this, &GuiForm::startBtn_Click);
			// 
			// openFileDialog1
			// 
			this->openFileDialog1->FileName = L"openFileDialog1";
			this->openFileDialog1->Filter = L"\"txt-files (*.txt)|*.txt|All files (*.*)|*.*\";";
			this->openFileDialog1->FileOk += gcnew System::ComponentModel::CancelEventHandler(this, &GuiForm::openFileDialog1_FileOk);
			// 
			// BottomToolStripPanel
			// 
			this->BottomToolStripPanel->Location = System::Drawing::Point(0, 0);
			this->BottomToolStripPanel->Name = L"BottomToolStripPanel";
			this->BottomToolStripPanel->Orientation = System::Windows::Forms::Orientation::Horizontal;
			this->BottomToolStripPanel->RowMargin = System::Windows::Forms::Padding(3, 0, 0, 0);
			this->BottomToolStripPanel->Size = System::Drawing::Size(0, 0);
			// 
			// TopToolStripPanel
			// 
			this->TopToolStripPanel->Location = System::Drawing::Point(0, 0);
			this->TopToolStripPanel->Name = L"TopToolStripPanel";
			this->TopToolStripPanel->Orientation = System::Windows::Forms::Orientation::Horizontal;
			this->TopToolStripPanel->RowMargin = System::Windows::Forms::Padding(3, 0, 0, 0);
			this->TopToolStripPanel->Size = System::Drawing::Size(0, 0);
			// 
			// RightToolStripPanel
			// 
			this->RightToolStripPanel->Location = System::Drawing::Point(0, 0);
			this->RightToolStripPanel->Name = L"RightToolStripPanel";
			this->RightToolStripPanel->Orientation = System::Windows::Forms::Orientation::Horizontal;
			this->RightToolStripPanel->RowMargin = System::Windows::Forms::Padding(3, 0, 0, 0);
			this->RightToolStripPanel->Size = System::Drawing::Size(0, 0);
			// 
			// LeftToolStripPanel
			// 
			this->LeftToolStripPanel->Location = System::Drawing::Point(0, 0);
			this->LeftToolStripPanel->Name = L"LeftToolStripPanel";
			this->LeftToolStripPanel->Orientation = System::Windows::Forms::Orientation::Horizontal;
			this->LeftToolStripPanel->RowMargin = System::Windows::Forms::Padding(3, 0, 0, 0);
			this->LeftToolStripPanel->Size = System::Drawing::Size(0, 0);
			// 
			// ContentPanel
			// 
			this->ContentPanel->Size = System::Drawing::Size(150, 125);
			// 
			// tabPage5
			// 
			this->tabPage5->Controls->Add(this->changeKCheckBox);
			this->tabPage5->Controls->Add(this->textBox19);
			this->tabPage5->Controls->Add(this->label28);
			this->tabPage5->Controls->Add(this->label27);
			this->tabPage5->Controls->Add(this->label26);
			this->tabPage5->Controls->Add(this->label25);
			this->tabPage5->Controls->Add(this->label24);
			this->tabPage5->Controls->Add(this->label23);
			this->tabPage5->Controls->Add(this->label22);
			this->tabPage5->Controls->Add(this->label21);
			this->tabPage5->Controls->Add(this->label2);
			this->tabPage5->Controls->Add(this->label1);
			this->tabPage5->Controls->Add(this->zedGraphControl5);
			this->tabPage5->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->tabPage5->Location = System::Drawing::Point(4, 25);
			this->tabPage5->Name = L"tabPage5";
			this->tabPage5->Padding = System::Windows::Forms::Padding(3);
			this->tabPage5->Size = System::Drawing::Size(1026, 608);
			this->tabPage5->TabIndex = 4;
			this->tabPage5->Text = L"Calculated data";
			this->tabPage5->UseVisualStyleBackColor = true;
			// 
			// changeKCheckBox
			// 
			this->changeKCheckBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->changeKCheckBox->AutoSize = true;
			this->changeKCheckBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->changeKCheckBox->Location = System::Drawing::Point(1038, 353);
			this->changeKCheckBox->Name = L"changeKCheckBox";
			this->changeKCheckBox->Size = System::Drawing::Size(80, 24);
			this->changeKCheckBox->TabIndex = 203;
			this->changeKCheckBox->Text = L"Set K =";
			this->changeKCheckBox->UseVisualStyleBackColor = true;
			this->changeKCheckBox->CheckedChanged += gcnew System::EventHandler(this, &GuiForm::changeKCheckBox_CheckedChanged);
			// 
			// textBox19
			// 
			this->textBox19->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->textBox19->Enabled = false;
			this->textBox19->Location = System::Drawing::Point(1124, 354);
			this->textBox19->Name = L"textBox19";
			this->textBox19->Size = System::Drawing::Size(51, 22);
			this->textBox19->TabIndex = 202;
			// 
			// label28
			// 
			this->label28->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label28->AutoSize = true;
			this->label28->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Underline, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label28->Location = System::Drawing::Point(1013, 157);
			this->label28->Name = L"label28";
			this->label28->Size = System::Drawing::Size(186, 20);
			this->label28->TabIndex = 12;
			this->label28->Text = L"Step 2 - optic calibraition:";
			// 
			// label27
			// 
			this->label27->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label27->AutoSize = true;
			this->label27->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Underline, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label27->Location = System::Drawing::Point(1013, 3);
			this->label27->Name = L"label27";
			this->label27->Size = System::Drawing::Size(182, 20);
			this->label27->TabIndex = 11;
			this->label27->Text = L"Step 1 - diod calibraition:";
			// 
			// label26
			// 
			this->label26->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label26->AutoSize = true;
			this->label26->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label26->Location = System::Drawing::Point(1013, 245);
			this->label26->Name = L"label26";
			this->label26->Size = System::Drawing::Size(61, 20);
			this->label26->TabIndex = 10;
			this->label26->Text = L"phi_0 =";
			// 
			// label25
			// 
			this->label25->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label25->AutoSize = true;
			this->label25->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label25->Location = System::Drawing::Point(1013, 267);
			this->label25->Name = L"label25";
			this->label25->Size = System::Drawing::Size(93, 20);
			this->label25->TabIndex = 9;
			this->label25->Text = L"gamma_0 =";
			// 
			// label24
			// 
			this->label24->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label24->AutoSize = true;
			this->label24->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label24->Location = System::Drawing::Point(1013, 106);
			this->label24->Name = L"label24";
			this->label24->Size = System::Drawing::Size(61, 20);
			this->label24->TabIndex = 8;
			this->label24->Text = L"U4_0 =";
			// 
			// label23
			// 
			this->label23->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label23->AutoSize = true;
			this->label23->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label23->Location = System::Drawing::Point(1013, 82);
			this->label23->Name = L"label23";
			this->label23->Size = System::Drawing::Size(65, 20);
			this->label23->TabIndex = 7;
			this->label23->Text = L"U3_0 = ";
			// 
			// label22
			// 
			this->label22->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label22->AutoSize = true;
			this->label22->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label22->Location = System::Drawing::Point(1013, 57);
			this->label22->Name = L"label22";
			this->label22->Size = System::Drawing::Size(65, 20);
			this->label22->TabIndex = 6;
			this->label22->Text = L"U2_0 = ";
			// 
			// label21
			// 
			this->label21->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label21->AutoSize = true;
			this->label21->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label21->Location = System::Drawing::Point(1013, 33);
			this->label21->Name = L"label21";
			this->label21->Size = System::Drawing::Size(65, 20);
			this->label21->TabIndex = 5;
			this->label21->Text = L"U1_0 = ";
			// 
			// label2
			// 
			this->label2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label2->AutoSize = true;
			this->label2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label2->Location = System::Drawing::Point(1013, 186);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(108, 20);
			this->label2->TabIndex = 4;
			this->label2->Text = L"Peleng error =";
			// 
			// label1
			// 
			this->label1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label1->Location = System::Drawing::Point(1013, 209);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(104, 20);
			this->label1->TabIndex = 3;
			this->label1->Text = L"Phase error =";
			// 
			// zedGraphControl5
			// 
			this->zedGraphControl5->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->zedGraphControl5->AutoScroll = true;
			this->zedGraphControl5->AutoSize = true;
			this->zedGraphControl5->Location = System::Drawing::Point(3, 0);
			this->zedGraphControl5->Name = L"zedGraphControl5";
			this->zedGraphControl5->ScrollGrace = 0;
			this->zedGraphControl5->ScrollMaxX = 0;
			this->zedGraphControl5->ScrollMaxY = 0;
			this->zedGraphControl5->ScrollMaxY2 = 0;
			this->zedGraphControl5->ScrollMinX = 0;
			this->zedGraphControl5->ScrollMinY = 0;
			this->zedGraphControl5->ScrollMinY2 = 0;
			this->zedGraphControl5->Size = System::Drawing::Size(1004, 519);
			this->zedGraphControl5->TabIndex = 2;
			this->zedGraphControl5->Load += gcnew System::EventHandler(this, &GuiForm::zedGraphControl5_Load);
			// 
			// tabControl1
			// 
			this->tabControl1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->tabControl1->Controls->Add(this->tabPage1);
			this->tabControl1->Controls->Add(this->tabPage5);
			this->tabControl1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->tabControl1->Location = System::Drawing::Point(0, 47);
			this->tabControl1->Name = L"tabControl1";
			this->tabControl1->SelectedIndex = 0;
			this->tabControl1->Size = System::Drawing::Size(1034, 637);
			this->tabControl1->TabIndex = 11;
			// 
			// tabPage1
			// 
			this->tabPage1->Controls->Add(this->zedGraphControl1);
			this->tabPage1->Location = System::Drawing::Point(4, 25);
			this->tabPage1->Name = L"tabPage1";
			this->tabPage1->Padding = System::Windows::Forms::Padding(3);
			this->tabPage1->Size = System::Drawing::Size(1026, 608);
			this->tabPage1->TabIndex = 0;
			this->tabPage1->Text = L"Input data";
			this->tabPage1->UseVisualStyleBackColor = true;
			// 
			// zedGraphControl1
			// 
			this->zedGraphControl1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->zedGraphControl1->AutoScroll = true;
			this->zedGraphControl1->AutoSize = true;
			this->zedGraphControl1->BackColor = System::Drawing::Color::Transparent;
			this->zedGraphControl1->IsShowContextMenu = false;
			this->zedGraphControl1->Location = System::Drawing::Point(3, 2);
			this->zedGraphControl1->Margin = System::Windows::Forms::Padding(3, 2, 3, 2);
			this->zedGraphControl1->Name = L"zedGraphControl1";
			this->zedGraphControl1->ScrollGrace = 0;
			this->zedGraphControl1->ScrollMaxX = 0;
			this->zedGraphControl1->ScrollMaxY = 0;
			this->zedGraphControl1->ScrollMaxY2 = 0;
			this->zedGraphControl1->ScrollMinX = 0;
			this->zedGraphControl1->ScrollMinY = 0;
			this->zedGraphControl1->ScrollMinY2 = 0;
			this->zedGraphControl1->Size = System::Drawing::Size(1023, 602);
			this->zedGraphControl1->TabIndex = 0;
			// 
			// softVerLbl
			// 
			this->softVerLbl->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->softVerLbl->AutoSize = true;
			this->softVerLbl->Location = System::Drawing::Point(1162, 645);
			this->softVerLbl->Name = L"softVerLbl";
			this->softVerLbl->Size = System::Drawing::Size(16, 13);
			this->softVerLbl->TabIndex = 204;
			this->softVerLbl->Text = L"v.";
			// 
			// pvkNumberLbl
			// 
			this->pvkNumberLbl->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->pvkNumberLbl->AutoSize = true;
			this->pvkNumberLbl->Location = System::Drawing::Point(1058, 645);
			this->pvkNumberLbl->Name = L"pvkNumberLbl";
			this->pvkNumberLbl->Size = System::Drawing::Size(43, 13);
			this->pvkNumberLbl->TabIndex = 203;
			this->pvkNumberLbl->Text = L"ПВК №";
			// 
			// panel7
			// 
			this->panel7->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->panel7->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->panel7->Location = System::Drawing::Point(1040, 534);
			this->panel7->Name = L"panel7";
			this->panel7->Size = System::Drawing::Size(175, 85);
			this->panel7->TabIndex = 202;
			// 
			// RolEnAllCheckBox
			// 
			this->RolEnAllCheckBox->AutoSize = true;
			this->RolEnAllCheckBox->Enabled = false;
			this->RolEnAllCheckBox->Location = System::Drawing::Point(43, 0);
			this->RolEnAllCheckBox->Name = L"RolEnAllCheckBox";
			this->RolEnAllCheckBox->Size = System::Drawing::Size(72, 17);
			this->RolEnAllCheckBox->TabIndex = 16;
			this->RolEnAllCheckBox->Text = L"Enable all";
			this->RolEnAllCheckBox->UseVisualStyleBackColor = true;
			// 
			// PitEnAllCheckBox
			// 
			this->PitEnAllCheckBox->AutoSize = true;
			this->PitEnAllCheckBox->Enabled = false;
			this->PitEnAllCheckBox->Location = System::Drawing::Point(43, 0);
			this->PitEnAllCheckBox->Name = L"PitEnAllCheckBox";
			this->PitEnAllCheckBox->Size = System::Drawing::Size(72, 17);
			this->PitEnAllCheckBox->TabIndex = 10;
			this->PitEnAllCheckBox->Text = L"Enable all";
			this->PitEnAllCheckBox->UseVisualStyleBackColor = true;
			// 
			// panel6
			// 
			this->panel6->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->panel6->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->panel6->Controls->Add(this->label29);
			this->panel6->Controls->Add(this->RcThrTextBox);
			this->panel6->Controls->Add(this->RcThrCheckBox);
			this->panel6->Controls->Add(this->MotorEnAllCheckBox);
			this->panel6->Controls->Add(this->FRCheckBox);
			this->panel6->Controls->Add(this->BRCheckBox);
			this->panel6->Controls->Add(this->FLCheckBox);
			this->panel6->Controls->Add(this->BLCheckBox);
			this->panel6->Controls->Add(this->label17);
			this->panel6->Controls->Add(this->BRTextBox);
			this->panel6->Controls->Add(this->label15);
			this->panel6->Controls->Add(this->BLTextBox);
			this->panel6->Controls->Add(this->label13);
			this->panel6->Controls->Add(this->FRTextBox);
			this->panel6->Controls->Add(this->label11);
			this->panel6->Controls->Add(this->FLTextBox);
			this->panel6->Location = System::Drawing::Point(1036, 392);
			this->panel6->Name = L"panel6";
			this->panel6->Size = System::Drawing::Size(175, 100);
			this->panel6->TabIndex = 164;
			// 
			// label29
			// 
			this->label29->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label29->AutoSize = true;
			this->label29->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label29->Location = System::Drawing::Point(105, 13);
			this->label29->Name = L"label29";
			this->label29->Size = System::Drawing::Size(14, 16);
			this->label29->TabIndex = 202;
			this->label29->Text = L"x";
			// 
			// RcThrTextBox
			// 
			this->RcThrTextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->RcThrTextBox->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->RcThrTextBox->Enabled = false;
			this->RcThrTextBox->Location = System::Drawing::Point(119, 15);
			this->RcThrTextBox->Name = L"RcThrTextBox";
			this->RcThrTextBox->Size = System::Drawing::Size(44, 13);
			this->RcThrTextBox->TabIndex = 203;
			this->RcThrTextBox->Text = L"1";
			this->RcThrTextBox->UseWaitCursor = true;
			// 
			// RcThrCheckBox
			// 
			this->RcThrCheckBox->AutoSize = true;
			this->RcThrCheckBox->Enabled = false;
			this->RcThrCheckBox->Location = System::Drawing::Point(6, 15);
			this->RcThrCheckBox->Name = L"RcThrCheckBox";
			this->RcThrCheckBox->Size = System::Drawing::Size(76, 17);
			this->RcThrCheckBox->TabIndex = 201;
			this->RcThrCheckBox->Text = L"RC throttle";
			this->RcThrCheckBox->UseVisualStyleBackColor = true;
			// 
			// MotorEnAllCheckBox
			// 
			this->MotorEnAllCheckBox->AutoSize = true;
			this->MotorEnAllCheckBox->Enabled = false;
			this->MotorEnAllCheckBox->Location = System::Drawing::Point(43, 0);
			this->MotorEnAllCheckBox->Name = L"MotorEnAllCheckBox";
			this->MotorEnAllCheckBox->Size = System::Drawing::Size(72, 17);
			this->MotorEnAllCheckBox->TabIndex = 196;
			this->MotorEnAllCheckBox->Text = L"Enable all";
			this->MotorEnAllCheckBox->UseVisualStyleBackColor = true;
			// 
			// FRCheckBox
			// 
			this->FRCheckBox->AutoSize = true;
			this->FRCheckBox->Enabled = false;
			this->FRCheckBox->Location = System::Drawing::Point(6, 48);
			this->FRCheckBox->Name = L"FRCheckBox";
			this->FRCheckBox->Size = System::Drawing::Size(40, 17);
			this->FRCheckBox->TabIndex = 200;
			this->FRCheckBox->Text = L"FR";
			this->FRCheckBox->UseVisualStyleBackColor = true;
			// 
			// BRCheckBox
			// 
			this->BRCheckBox->AutoSize = true;
			this->BRCheckBox->Enabled = false;
			this->BRCheckBox->Location = System::Drawing::Point(6, 81);
			this->BRCheckBox->Name = L"BRCheckBox";
			this->BRCheckBox->Size = System::Drawing::Size(41, 17);
			this->BRCheckBox->TabIndex = 199;
			this->BRCheckBox->Text = L"BR";
			this->BRCheckBox->UseVisualStyleBackColor = true;
			// 
			// FLCheckBox
			// 
			this->FLCheckBox->AutoSize = true;
			this->FLCheckBox->Enabled = false;
			this->FLCheckBox->Location = System::Drawing::Point(6, 31);
			this->FLCheckBox->Name = L"FLCheckBox";
			this->FLCheckBox->Size = System::Drawing::Size(38, 17);
			this->FLCheckBox->TabIndex = 198;
			this->FLCheckBox->Text = L"FL";
			this->FLCheckBox->UseVisualStyleBackColor = true;
			// 
			// BLCheckBox
			// 
			this->BLCheckBox->AutoSize = true;
			this->BLCheckBox->Enabled = false;
			this->BLCheckBox->Location = System::Drawing::Point(6, 65);
			this->BLCheckBox->Name = L"BLCheckBox";
			this->BLCheckBox->Size = System::Drawing::Size(39, 17);
			this->BLCheckBox->TabIndex = 197;
			this->BLCheckBox->Text = L"BL";
			this->BLCheckBox->UseVisualStyleBackColor = true;
			// 
			// label17
			// 
			this->label17->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label17->AutoSize = true;
			this->label17->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label17->Location = System::Drawing::Point(105, 64);
			this->label17->Name = L"label17";
			this->label17->Size = System::Drawing::Size(14, 16);
			this->label17->TabIndex = 195;
			this->label17->Text = L"x";
			// 
			// BRTextBox
			// 
			this->BRTextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->BRTextBox->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->BRTextBox->Enabled = false;
			this->BRTextBox->Location = System::Drawing::Point(119, 82);
			this->BRTextBox->Name = L"BRTextBox";
			this->BRTextBox->Size = System::Drawing::Size(44, 13);
			this->BRTextBox->TabIndex = 193;
			this->BRTextBox->Text = L"1";
			// 
			// label15
			// 
			this->label15->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label15->AutoSize = true;
			this->label15->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label15->Location = System::Drawing::Point(105, 47);
			this->label15->Name = L"label15";
			this->label15->Size = System::Drawing::Size(14, 16);
			this->label15->TabIndex = 191;
			this->label15->Text = L"x";
			// 
			// BLTextBox
			// 
			this->BLTextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->BLTextBox->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->BLTextBox->Enabled = false;
			this->BLTextBox->Location = System::Drawing::Point(119, 66);
			this->BLTextBox->Name = L"BLTextBox";
			this->BLTextBox->Size = System::Drawing::Size(44, 13);
			this->BLTextBox->TabIndex = 189;
			this->BLTextBox->Text = L"1";
			// 
			// label13
			// 
			this->label13->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label13->AutoSize = true;
			this->label13->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label13->Location = System::Drawing::Point(105, 80);
			this->label13->Name = L"label13";
			this->label13->Size = System::Drawing::Size(14, 16);
			this->label13->TabIndex = 187;
			this->label13->Text = L"x";
			// 
			// FRTextBox
			// 
			this->FRTextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->FRTextBox->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->FRTextBox->Enabled = false;
			this->FRTextBox->Location = System::Drawing::Point(119, 50);
			this->FRTextBox->Name = L"FRTextBox";
			this->FRTextBox->Size = System::Drawing::Size(44, 13);
			this->FRTextBox->TabIndex = 185;
			this->FRTextBox->Text = L"1";
			// 
			// label11
			// 
			this->label11->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label11->AutoSize = true;
			this->label11->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label11->Location = System::Drawing::Point(105, 30);
			this->label11->Name = L"label11";
			this->label11->Size = System::Drawing::Size(14, 16);
			this->label11->TabIndex = 183;
			this->label11->Text = L"x";
			// 
			// FLTextBox
			// 
			this->FLTextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->FLTextBox->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->FLTextBox->Enabled = false;
			this->FLTextBox->Location = System::Drawing::Point(119, 33);
			this->FLTextBox->Name = L"FLTextBox";
			this->FLTextBox->Size = System::Drawing::Size(44, 13);
			this->FLTextBox->TabIndex = 181;
			this->FLTextBox->Text = L"1";
			// 
			// label19
			// 
			this->label19->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label19->AutoSize = true;
			this->label19->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label19->Location = System::Drawing::Point(1139, 513);
			this->label19->Name = L"label19";
			this->label19->Size = System::Drawing::Size(35, 16);
			this->label19->TabIndex = 199;
			this->label19->Text = L"End:";
			// 
			// textBox17
			// 
			this->textBox17->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->textBox17->Location = System::Drawing::Point(1095, 508);
			this->textBox17->Name = L"textBox17";
			this->textBox17->Size = System::Drawing::Size(44, 20);
			this->textBox17->TabIndex = 198;
			// 
			// textBox18
			// 
			this->textBox18->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->textBox18->Location = System::Drawing::Point(1174, 508);
			this->textBox18->Name = L"textBox18";
			this->textBox18->Size = System::Drawing::Size(44, 20);
			this->textBox18->TabIndex = 197;
			// 
			// label20
			// 
			this->label20->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label20->AutoSize = true;
			this->label20->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label20->Location = System::Drawing::Point(1044, 513);
			this->label20->Name = L"label20";
			this->label20->Size = System::Drawing::Size(51, 16);
			this->label20->TabIndex = 196;
			this->label20->Text = L"9: Start:";
			// 
			// YawOutTextBox
			// 
			this->YawOutTextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->YawOutTextBox->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->YawOutTextBox->Enabled = false;
			this->YawOutTextBox->Location = System::Drawing::Point(119, 83);
			this->YawOutTextBox->Name = L"YawOutTextBox";
			this->YawOutTextBox->Size = System::Drawing::Size(44, 13);
			this->YawOutTextBox->TabIndex = 194;
			this->YawOutTextBox->Text = L"1";
			// 
			// label18
			// 
			this->label18->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label18->AutoSize = true;
			this->label18->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label18->Location = System::Drawing::Point(105, 80);
			this->label18->Name = L"label18";
			this->label18->Size = System::Drawing::Size(14, 16);
			this->label18->TabIndex = 192;
			this->label18->Text = L"x";
			// 
			// StabYawTextBox
			// 
			this->StabYawTextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->StabYawTextBox->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->StabYawTextBox->Enabled = false;
			this->StabYawTextBox->Location = System::Drawing::Point(119, 66);
			this->StabYawTextBox->Name = L"StabYawTextBox";
			this->StabYawTextBox->Size = System::Drawing::Size(44, 13);
			this->StabYawTextBox->TabIndex = 190;
			this->StabYawTextBox->Text = L"1";
			// 
			// label16
			// 
			this->label16->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label16->AutoSize = true;
			this->label16->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label16->Location = System::Drawing::Point(105, 64);
			this->label16->Name = L"label16";
			this->label16->Size = System::Drawing::Size(14, 16);
			this->label16->TabIndex = 188;
			this->label16->Text = L"x";
			// 
			// GyroYawTextBox
			// 
			this->GyroYawTextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->GyroYawTextBox->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->GyroYawTextBox->Enabled = false;
			this->GyroYawTextBox->Location = System::Drawing::Point(119, 34);
			this->GyroYawTextBox->Name = L"GyroYawTextBox";
			this->GyroYawTextBox->Size = System::Drawing::Size(44, 13);
			this->GyroYawTextBox->TabIndex = 186;
			this->GyroYawTextBox->Text = L"1";
			// 
			// label14
			// 
			this->label14->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label14->AutoSize = true;
			this->label14->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label14->Location = System::Drawing::Point(105, 32);
			this->label14->Name = L"label14";
			this->label14->Size = System::Drawing::Size(14, 16);
			this->label14->TabIndex = 184;
			this->label14->Text = L"x";
			// 
			// RcYawTextBox
			// 
			this->RcYawTextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->RcYawTextBox->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->RcYawTextBox->Enabled = false;
			this->RcYawTextBox->Location = System::Drawing::Point(119, 17);
			this->RcYawTextBox->Name = L"RcYawTextBox";
			this->RcYawTextBox->Size = System::Drawing::Size(44, 13);
			this->RcYawTextBox->TabIndex = 182;
			this->RcYawTextBox->Text = L"1";
			// 
			// label12
			// 
			this->label12->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label12->AutoSize = true;
			this->label12->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label12->Location = System::Drawing::Point(105, 15);
			this->label12->Name = L"label12";
			this->label12->Size = System::Drawing::Size(14, 16);
			this->label12->TabIndex = 180;
			this->label12->Text = L"x";
			// 
			// label8
			// 
			this->label8->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label8->AutoSize = true;
			this->label8->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label8->Location = System::Drawing::Point(105, 32);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(14, 16);
			this->label8->TabIndex = 172;
			this->label8->Text = L"x";
			// 
			// GyroRollTextBox
			// 
			this->GyroRollTextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->GyroRollTextBox->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->GyroRollTextBox->Enabled = false;
			this->GyroRollTextBox->Location = System::Drawing::Point(119, 34);
			this->GyroRollTextBox->Name = L"GyroRollTextBox";
			this->GyroRollTextBox->Size = System::Drawing::Size(44, 13);
			this->GyroRollTextBox->TabIndex = 173;
			this->GyroRollTextBox->Text = L"1";
			// 
			// RcRollTextBox
			// 
			this->RcRollTextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->RcRollTextBox->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->RcRollTextBox->Enabled = false;
			this->RcRollTextBox->Location = System::Drawing::Point(119, 17);
			this->RcRollTextBox->Name = L"RcRollTextBox";
			this->RcRollTextBox->Size = System::Drawing::Size(44, 13);
			this->RcRollTextBox->TabIndex = 174;
			this->RcRollTextBox->Text = L"1";
			// 
			// label7
			// 
			this->label7->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label7->AutoSize = true;
			this->label7->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label7->Location = System::Drawing::Point(105, 15);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(14, 16);
			this->label7->TabIndex = 175;
			this->label7->Text = L"x";
			// 
			// label10
			// 
			this->label10->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label10->AutoSize = true;
			this->label10->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label10->Location = System::Drawing::Point(105, 79);
			this->label10->Name = L"label10";
			this->label10->Size = System::Drawing::Size(14, 16);
			this->label10->TabIndex = 176;
			this->label10->Text = L"x";
			// 
			// RollOutTextBox
			// 
			this->RollOutTextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->RollOutTextBox->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->RollOutTextBox->Enabled = false;
			this->RollOutTextBox->Location = System::Drawing::Point(119, 81);
			this->RollOutTextBox->Name = L"RollOutTextBox";
			this->RollOutTextBox->Size = System::Drawing::Size(44, 13);
			this->RollOutTextBox->TabIndex = 177;
			this->RollOutTextBox->Text = L"1";
			// 
			// StabRollTextBox
			// 
			this->StabRollTextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->StabRollTextBox->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->StabRollTextBox->Enabled = false;
			this->StabRollTextBox->Location = System::Drawing::Point(119, 65);
			this->StabRollTextBox->Name = L"StabRollTextBox";
			this->StabRollTextBox->Size = System::Drawing::Size(44, 13);
			this->StabRollTextBox->TabIndex = 178;
			this->StabRollTextBox->Text = L"1";
			// 
			// label9
			// 
			this->label9->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label9->AutoSize = true;
			this->label9->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label9->Location = System::Drawing::Point(105, 63);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(14, 16);
			this->label9->TabIndex = 179;
			this->label9->Text = L"x";
			// 
			// label3
			// 
			this->label3->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label3->AutoSize = true;
			this->label3->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label3->Location = System::Drawing::Point(105, 13);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(14, 16);
			this->label3->TabIndex = 164;
			this->label3->Text = L"x";
			// 
			// RcPitTextBox
			// 
			this->RcPitTextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->RcPitTextBox->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->RcPitTextBox->Enabled = false;
			this->RcPitTextBox->Location = System::Drawing::Point(119, 15);
			this->RcPitTextBox->Name = L"RcPitTextBox";
			this->RcPitTextBox->Size = System::Drawing::Size(44, 13);
			this->RcPitTextBox->TabIndex = 165;
			this->RcPitTextBox->Text = L"1";
			this->RcPitTextBox->UseWaitCursor = true;
			// 
			// GyroPitTextBox
			// 
			this->GyroPitTextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->GyroPitTextBox->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->GyroPitTextBox->Enabled = false;
			this->GyroPitTextBox->Location = System::Drawing::Point(119, 32);
			this->GyroPitTextBox->Name = L"GyroPitTextBox";
			this->GyroPitTextBox->Size = System::Drawing::Size(44, 13);
			this->GyroPitTextBox->TabIndex = 166;
			this->GyroPitTextBox->Text = L"1";
			// 
			// label5
			// 
			this->label5->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label5->AutoSize = true;
			this->label5->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label5->Location = System::Drawing::Point(105, 30);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(14, 16);
			this->label5->TabIndex = 167;
			this->label5->Text = L"x";
			// 
			// label6
			// 
			this->label6->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label6->AutoSize = true;
			this->label6->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label6->Location = System::Drawing::Point(105, 62);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(14, 16);
			this->label6->TabIndex = 168;
			this->label6->Text = L"x";
			// 
			// PitOutTextBox
			// 
			this->PitOutTextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->PitOutTextBox->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->PitOutTextBox->Enabled = false;
			this->PitOutTextBox->Location = System::Drawing::Point(119, 81);
			this->PitOutTextBox->Name = L"PitOutTextBox";
			this->PitOutTextBox->Size = System::Drawing::Size(44, 13);
			this->PitOutTextBox->TabIndex = 169;
			this->PitOutTextBox->Text = L"1";
			// 
			// StabPitTextBox
			// 
			this->StabPitTextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->StabPitTextBox->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->StabPitTextBox->Enabled = false;
			this->StabPitTextBox->Location = System::Drawing::Point(119, 64);
			this->StabPitTextBox->Name = L"StabPitTextBox";
			this->StabPitTextBox->Size = System::Drawing::Size(44, 13);
			this->StabPitTextBox->TabIndex = 170;
			this->StabPitTextBox->Text = L"1";
			// 
			// label4
			// 
			this->label4->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label4->AutoSize = true;
			this->label4->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label4->Location = System::Drawing::Point(105, 79);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(14, 16);
			this->label4->TabIndex = 171;
			this->label4->Text = L"x";
			// 
			// panel5
			// 
			this->panel5->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->panel5->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->panel5->Controls->Add(this->IntRollCheckBox);
			this->panel5->Controls->Add(this->IntRollTextBox);
			this->panel5->Controls->Add(this->label31);
			this->panel5->Controls->Add(this->RolEnAllCheckBox);
			this->panel5->Controls->Add(this->GyroRollCheckBox);
			this->panel5->Controls->Add(this->RollOutCheckBox);
			this->panel5->Controls->Add(this->RcRollCheckBox);
			this->panel5->Controls->Add(this->StabRollCheckBox);
			this->panel5->Controls->Add(this->RcRollTextBox);
			this->panel5->Controls->Add(this->GyroRollTextBox);
			this->panel5->Controls->Add(this->StabRollTextBox);
			this->panel5->Controls->Add(this->RollOutTextBox);
			this->panel5->Controls->Add(this->label7);
			this->panel5->Controls->Add(this->label8);
			this->panel5->Controls->Add(this->label9);
			this->panel5->Controls->Add(this->label10);
			this->panel5->Location = System::Drawing::Point(1036, 180);
			this->panel5->Name = L"panel5";
			this->panel5->Size = System::Drawing::Size(175, 100);
			this->panel5->TabIndex = 95;
			// 
			// IntRollCheckBox
			// 
			this->IntRollCheckBox->AutoSize = true;
			this->IntRollCheckBox->Enabled = false;
			this->IntRollCheckBox->Location = System::Drawing::Point(6, 49);
			this->IntRollCheckBox->Name = L"IntRollCheckBox";
			this->IntRollCheckBox->Size = System::Drawing::Size(54, 17);
			this->IntRollCheckBox->TabIndex = 180;
			this->IntRollCheckBox->Text = L"Int roll";
			this->IntRollCheckBox->UseVisualStyleBackColor = true;
			// 
			// IntRollTextBox
			// 
			this->IntRollTextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->IntRollTextBox->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->IntRollTextBox->Enabled = false;
			this->IntRollTextBox->Location = System::Drawing::Point(119, 49);
			this->IntRollTextBox->Name = L"IntRollTextBox";
			this->IntRollTextBox->Size = System::Drawing::Size(44, 13);
			this->IntRollTextBox->TabIndex = 182;
			this->IntRollTextBox->Text = L"1";
			// 
			// label31
			// 
			this->label31->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label31->AutoSize = true;
			this->label31->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label31->Location = System::Drawing::Point(105, 47);
			this->label31->Name = L"label31";
			this->label31->Size = System::Drawing::Size(14, 16);
			this->label31->TabIndex = 181;
			this->label31->Text = L"x";
			// 
			// GyroRollCheckBox
			// 
			this->GyroRollCheckBox->AutoSize = true;
			this->GyroRollCheckBox->Enabled = false;
			this->GyroRollCheckBox->Location = System::Drawing::Point(6, 34);
			this->GyroRollCheckBox->Name = L"GyroRollCheckBox";
			this->GyroRollCheckBox->Size = System::Drawing::Size(64, 17);
			this->GyroRollCheckBox->TabIndex = 24;
			this->GyroRollCheckBox->Text = L"Gyro roll";
			this->GyroRollCheckBox->UseVisualStyleBackColor = true;
			// 
			// RollOutCheckBox
			// 
			this->RollOutCheckBox->AutoSize = true;
			this->RollOutCheckBox->Enabled = false;
			this->RollOutCheckBox->Location = System::Drawing::Point(6, 81);
			this->RollOutCheckBox->Name = L"RollOutCheckBox";
			this->RollOutCheckBox->Size = System::Drawing::Size(62, 17);
			this->RollOutCheckBox->TabIndex = 23;
			this->RollOutCheckBox->Text = L"Roll out";
			this->RollOutCheckBox->UseVisualStyleBackColor = true;
			// 
			// RcRollCheckBox
			// 
			this->RcRollCheckBox->AutoSize = true;
			this->RcRollCheckBox->Enabled = false;
			this->RcRollCheckBox->Location = System::Drawing::Point(6, 17);
			this->RcRollCheckBox->Name = L"RcRollCheckBox";
			this->RcRollCheckBox->Size = System::Drawing::Size(57, 17);
			this->RcRollCheckBox->TabIndex = 22;
			this->RcRollCheckBox->Text = L"RC roll";
			this->RcRollCheckBox->UseVisualStyleBackColor = true;
			// 
			// StabRollCheckBox
			// 
			this->StabRollCheckBox->AutoSize = true;
			this->StabRollCheckBox->Enabled = false;
			this->StabRollCheckBox->Location = System::Drawing::Point(6, 65);
			this->StabRollCheckBox->Name = L"StabRollCheckBox";
			this->StabRollCheckBox->Size = System::Drawing::Size(64, 17);
			this->StabRollCheckBox->TabIndex = 21;
			this->StabRollCheckBox->Text = L"Stab roll";
			this->StabRollCheckBox->UseVisualStyleBackColor = true;
			// 
			// panel2
			// 
			this->panel2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->panel2->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->panel2->Controls->Add(this->label32);
			this->panel2->Controls->Add(this->IntPitTextBox);
			this->panel2->Controls->Add(this->IntPitCheckBox);
			this->panel2->Controls->Add(this->PitEnAllCheckBox);
			this->panel2->Controls->Add(this->label5);
			this->panel2->Controls->Add(this->label3);
			this->panel2->Controls->Add(this->PitOutCheckBox);
			this->panel2->Controls->Add(this->GyroPitTextBox);
			this->panel2->Controls->Add(this->RcPitTextBox);
			this->panel2->Controls->Add(this->GyroPitCheckBox);
			this->panel2->Controls->Add(this->StabPitCheckBox);
			this->panel2->Controls->Add(this->label6);
			this->panel2->Controls->Add(this->RcPitCheckBox);
			this->panel2->Controls->Add(this->StabPitTextBox);
			this->panel2->Controls->Add(this->label4);
			this->panel2->Controls->Add(this->PitOutTextBox);
			this->panel2->Location = System::Drawing::Point(1036, 74);
			this->panel2->Name = L"panel2";
			this->panel2->Size = System::Drawing::Size(175, 100);
			this->panel2->TabIndex = 92;
			// 
			// label32
			// 
			this->label32->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label32->AutoSize = true;
			this->label32->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label32->Location = System::Drawing::Point(105, 46);
			this->label32->Name = L"label32";
			this->label32->Size = System::Drawing::Size(14, 16);
			this->label32->TabIndex = 174;
			this->label32->Text = L"x";
			// 
			// IntPitTextBox
			// 
			this->IntPitTextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->IntPitTextBox->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->IntPitTextBox->Enabled = false;
			this->IntPitTextBox->Location = System::Drawing::Point(119, 48);
			this->IntPitTextBox->Name = L"IntPitTextBox";
			this->IntPitTextBox->Size = System::Drawing::Size(44, 13);
			this->IntPitTextBox->TabIndex = 173;
			this->IntPitTextBox->Text = L"1";
			// 
			// IntPitCheckBox
			// 
			this->IntPitCheckBox->AutoSize = true;
			this->IntPitCheckBox->Enabled = false;
			this->IntPitCheckBox->Location = System::Drawing::Point(6, 48);
			this->IntPitCheckBox->Name = L"IntPitCheckBox";
			this->IntPitCheckBox->Size = System::Drawing::Size(64, 17);
			this->IntPitCheckBox->TabIndex = 172;
			this->IntPitCheckBox->Text = L"Int pitch";
			this->IntPitCheckBox->UseVisualStyleBackColor = true;
			// 
			// PitOutCheckBox
			// 
			this->PitOutCheckBox->AutoSize = true;
			this->PitOutCheckBox->Enabled = false;
			this->PitOutCheckBox->Location = System::Drawing::Point(6, 81);
			this->PitOutCheckBox->Name = L"PitOutCheckBox";
			this->PitOutCheckBox->Size = System::Drawing::Size(68, 17);
			this->PitOutCheckBox->TabIndex = 4;
			this->PitOutCheckBox->Text = L"Pitch out";
			this->PitOutCheckBox->UseVisualStyleBackColor = true;
			// 
			// GyroPitCheckBox
			// 
			this->GyroPitCheckBox->AutoSize = true;
			this->GyroPitCheckBox->Enabled = false;
			this->GyroPitCheckBox->Location = System::Drawing::Point(6, 32);
			this->GyroPitCheckBox->Name = L"GyroPitCheckBox";
			this->GyroPitCheckBox->Size = System::Drawing::Size(74, 17);
			this->GyroPitCheckBox->TabIndex = 2;
			this->GyroPitCheckBox->Text = L"Gyro pitch";
			this->GyroPitCheckBox->UseVisualStyleBackColor = true;
			// 
			// StabPitCheckBox
			// 
			this->StabPitCheckBox->AutoSize = true;
			this->StabPitCheckBox->Enabled = false;
			this->StabPitCheckBox->Location = System::Drawing::Point(6, 64);
			this->StabPitCheckBox->Name = L"StabPitCheckBox";
			this->StabPitCheckBox->Size = System::Drawing::Size(74, 17);
			this->StabPitCheckBox->TabIndex = 1;
			this->StabPitCheckBox->Text = L"Stab pitch";
			this->StabPitCheckBox->UseVisualStyleBackColor = true;
			// 
			// RcPitCheckBox
			// 
			this->RcPitCheckBox->AutoSize = true;
			this->RcPitCheckBox->Enabled = false;
			this->RcPitCheckBox->Location = System::Drawing::Point(6, 15);
			this->RcPitCheckBox->Name = L"RcPitCheckBox";
			this->RcPitCheckBox->Size = System::Drawing::Size(67, 17);
			this->RcPitCheckBox->TabIndex = 0;
			this->RcPitCheckBox->Text = L"RC pitch";
			this->RcPitCheckBox->UseVisualStyleBackColor = true;
			// 
			// checkBox1
			// 
			this->checkBox1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->checkBox1->Appearance = System::Windows::Forms::Appearance::Button;
			this->checkBox1->BackColor = System::Drawing::Color::Transparent;
			this->checkBox1->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"checkBox1.BackgroundImage")));
			this->checkBox1->FlatAppearance->BorderSize = 0;
			this->checkBox1->FlatAppearance->CheckedBackColor = System::Drawing::Color::Transparent;
			this->checkBox1->FlatAppearance->MouseDownBackColor = System::Drawing::Color::Transparent;
			this->checkBox1->FlatAppearance->MouseOverBackColor = System::Drawing::Color::Transparent;
			this->checkBox1->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->checkBox1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->checkBox1->Location = System::Drawing::Point(1165, 2);
			this->checkBox1->Name = L"checkBox1";
			this->checkBox1->Size = System::Drawing::Size(40, 40);
			this->checkBox1->TabIndex = 88;
			this->toolTip1->SetToolTip(this->checkBox1, L"Night mode (B)");
			this->checkBox1->UseVisualStyleBackColor = true;
			this->checkBox1->CheckedChanged += gcnew System::EventHandler(this, &GuiForm::OnCheckedChanged);
			// 
			// panel1
			// 
			this->panel1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->panel1->Controls->Add(this->progressBar1);
			this->panel1->Controls->Add(this->pauseBtn);
			this->panel1->Controls->Add(this->saveBtn);
			this->panel1->Controls->Add(this->openBtn);
			this->panel1->Controls->Add(this->stopBtn);
			this->panel1->Controls->Add(this->button1);
			this->panel1->Controls->Add(this->startBtn);
			this->panel1->Controls->Add(this->checkBox1);
			this->panel1->Location = System::Drawing::Point(0, 0);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(1211, 44);
			this->panel1->TabIndex = 12;
			// 
			// progressBar1
			// 
			this->progressBar1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->progressBar1->BackColor = System::Drawing::SystemColors::Control;
			this->progressBar1->Location = System::Drawing::Point(334, 3);
			this->progressBar1->Name = L"progressBar1";
			this->progressBar1->Size = System::Drawing::Size(725, 40);
			this->progressBar1->TabIndex = 93;
			// 
			// pauseBtn
			// 
			this->pauseBtn->BackColor = System::Drawing::Color::Transparent;
			this->pauseBtn->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pauseBtn.BackgroundImage")));
			this->pauseBtn->FlatAppearance->BorderSize = 0;
			this->pauseBtn->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->pauseBtn->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->pauseBtn->Location = System::Drawing::Point(58, 2);
			this->pauseBtn->Margin = System::Windows::Forms::Padding(0);
			this->pauseBtn->Name = L"pauseBtn";
			this->pauseBtn->Size = System::Drawing::Size(40, 40);
			this->pauseBtn->TabIndex = 92;
			this->toolTip4->SetToolTip(this->pauseBtn, L"Pause CAN");
			this->pauseBtn->UseVisualStyleBackColor = false;
			this->pauseBtn->Click += gcnew System::EventHandler(this, &GuiForm::pauseBtn_Click);
			// 
			// saveBtn
			// 
			this->saveBtn->BackColor = System::Drawing::Color::Transparent;
			this->saveBtn->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"saveBtn.BackgroundImage")));
			this->saveBtn->FlatAppearance->BorderSize = 0;
			this->saveBtn->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->saveBtn->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->saveBtn->Location = System::Drawing::Point(288, 2);
			this->saveBtn->Name = L"saveBtn";
			this->saveBtn->Size = System::Drawing::Size(40, 40);
			this->saveBtn->TabIndex = 91;
			this->toolTip7->SetToolTip(this->saveBtn, L"Save file (S)");
			this->saveBtn->UseVisualStyleBackColor = false;
			this->saveBtn->Click += gcnew System::EventHandler(this, &GuiForm::saveBtn_Click);
			// 
			// openBtn
			// 
			this->openBtn->BackColor = System::Drawing::Color::Transparent;
			this->openBtn->FlatAppearance->BorderSize = 0;
			this->openBtn->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->openBtn->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->openBtn->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"openBtn.Image")));
			this->openBtn->Location = System::Drawing::Point(200, 2);
			this->openBtn->Name = L"openBtn";
			this->openBtn->Size = System::Drawing::Size(40, 40);
			this->openBtn->TabIndex = 90;
			this->toolTip6->SetToolTip(this->openBtn, L"Open file (O)");
			this->openBtn->UseVisualStyleBackColor = false;
			this->openBtn->Click += gcnew System::EventHandler(this, &GuiForm::openBtn_Click_1);
			// 
			// stopBtn
			// 
			this->stopBtn->BackColor = System::Drawing::Color::Transparent;
			this->stopBtn->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"stopBtn.BackgroundImage")));
			this->stopBtn->FlatAppearance->BorderSize = 0;
			this->stopBtn->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->stopBtn->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->stopBtn->Location = System::Drawing::Point(110, 2);
			this->stopBtn->Name = L"stopBtn";
			this->stopBtn->Size = System::Drawing::Size(40, 40);
			this->stopBtn->TabIndex = 89;
			this->toolTip5->SetToolTip(this->stopBtn, L"Stop CAN (Clear graphs)");
			this->stopBtn->UseVisualStyleBackColor = false;
			this->stopBtn->Click += gcnew System::EventHandler(this, &GuiForm::stopBtn_Click_1);
			// 
			// saveFileDialog1
			// 
			this->saveFileDialog1->FileOk += gcnew System::ComponentModel::CancelEventHandler(this, &GuiForm::saveFileDialog1_FileOk);
			// 
			// toolTip1
			// 
			this->toolTip1->AutomaticDelay = 300;
			// 
			// panel3
			// 
			this->panel3->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->panel3->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->panel3->Controls->Add(this->IntYawCheckBox);
			this->panel3->Controls->Add(this->IntYawTextBox);
			this->panel3->Controls->Add(this->label30);
			this->panel3->Controls->Add(this->YawEnAllCheckBox);
			this->panel3->Controls->Add(this->GyroYawCheckBox);
			this->panel3->Controls->Add(this->YawOutCheckBox);
			this->panel3->Controls->Add(this->RcYawCheckBox);
			this->panel3->Controls->Add(this->StabYawCheckBox);
			this->panel3->Controls->Add(this->YawOutTextBox);
			this->panel3->Controls->Add(this->label12);
			this->panel3->Controls->Add(this->label14);
			this->panel3->Controls->Add(this->label18);
			this->panel3->Controls->Add(this->StabYawTextBox);
			this->panel3->Controls->Add(this->label16);
			this->panel3->Controls->Add(this->RcYawTextBox);
			this->panel3->Controls->Add(this->GyroYawTextBox);
			this->panel3->Location = System::Drawing::Point(1036, 286);
			this->panel3->Name = L"panel3";
			this->panel3->Size = System::Drawing::Size(175, 100);
			this->panel3->TabIndex = 96;
			// 
			// IntYawCheckBox
			// 
			this->IntYawCheckBox->AutoSize = true;
			this->IntYawCheckBox->Enabled = false;
			this->IntYawCheckBox->Location = System::Drawing::Point(6, 50);
			this->IntYawCheckBox->Name = L"IntYawCheckBox";
			this->IntYawCheckBox->Size = System::Drawing::Size(60, 17);
			this->IntYawCheckBox->TabIndex = 195;
			this->IntYawCheckBox->Text = L"Int yaw";
			this->IntYawCheckBox->UseVisualStyleBackColor = true;
			// 
			// IntYawTextBox
			// 
			this->IntYawTextBox->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->IntYawTextBox->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->IntYawTextBox->Enabled = false;
			this->IntYawTextBox->Location = System::Drawing::Point(119, 50);
			this->IntYawTextBox->Name = L"IntYawTextBox";
			this->IntYawTextBox->Size = System::Drawing::Size(44, 13);
			this->IntYawTextBox->TabIndex = 197;
			this->IntYawTextBox->Text = L"1";
			// 
			// label30
			// 
			this->label30->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label30->AutoSize = true;
			this->label30->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label30->Location = System::Drawing::Point(105, 48);
			this->label30->Name = L"label30";
			this->label30->Size = System::Drawing::Size(14, 16);
			this->label30->TabIndex = 196;
			this->label30->Text = L"x";
			// 
			// YawEnAllCheckBox
			// 
			this->YawEnAllCheckBox->AutoSize = true;
			this->YawEnAllCheckBox->Enabled = false;
			this->YawEnAllCheckBox->Location = System::Drawing::Point(43, 0);
			this->YawEnAllCheckBox->Name = L"YawEnAllCheckBox";
			this->YawEnAllCheckBox->Size = System::Drawing::Size(72, 17);
			this->YawEnAllCheckBox->TabIndex = 16;
			this->YawEnAllCheckBox->Text = L"Enable all";
			this->YawEnAllCheckBox->UseVisualStyleBackColor = true;
			// 
			// GyroYawCheckBox
			// 
			this->GyroYawCheckBox->AutoSize = true;
			this->GyroYawCheckBox->Enabled = false;
			this->GyroYawCheckBox->Location = System::Drawing::Point(6, 34);
			this->GyroYawCheckBox->Name = L"GyroYawCheckBox";
			this->GyroYawCheckBox->Size = System::Drawing::Size(70, 17);
			this->GyroYawCheckBox->TabIndex = 24;
			this->GyroYawCheckBox->Text = L"Gyro yaw";
			this->GyroYawCheckBox->UseVisualStyleBackColor = true;
			// 
			// YawOutCheckBox
			// 
			this->YawOutCheckBox->AutoSize = true;
			this->YawOutCheckBox->Enabled = false;
			this->YawOutCheckBox->Location = System::Drawing::Point(6, 82);
			this->YawOutCheckBox->Name = L"YawOutCheckBox";
			this->YawOutCheckBox->Size = System::Drawing::Size(65, 17);
			this->YawOutCheckBox->TabIndex = 23;
			this->YawOutCheckBox->Text = L"Yaw out";
			this->YawOutCheckBox->UseVisualStyleBackColor = true;
			// 
			// RcYawCheckBox
			// 
			this->RcYawCheckBox->AutoSize = true;
			this->RcYawCheckBox->Enabled = false;
			this->RcYawCheckBox->Location = System::Drawing::Point(6, 17);
			this->RcYawCheckBox->Name = L"RcYawCheckBox";
			this->RcYawCheckBox->Size = System::Drawing::Size(63, 17);
			this->RcYawCheckBox->TabIndex = 22;
			this->RcYawCheckBox->Text = L"RC yaw";
			this->RcYawCheckBox->UseVisualStyleBackColor = true;
			// 
			// StabYawCheckBox
			// 
			this->StabYawCheckBox->AutoSize = true;
			this->StabYawCheckBox->Enabled = false;
			this->StabYawCheckBox->Location = System::Drawing::Point(6, 66);
			this->StabYawCheckBox->Name = L"StabYawCheckBox";
			this->StabYawCheckBox->Size = System::Drawing::Size(70, 17);
			this->StabYawCheckBox->TabIndex = 21;
			this->StabYawCheckBox->Text = L"Stab yaw";
			this->StabYawCheckBox->UseVisualStyleBackColor = true;
			// 
			// GuiForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1211, 684);
			this->Controls->Add(this->panel3);
			this->Controls->Add(this->label19);
			this->Controls->Add(this->softVerLbl);
			this->Controls->Add(this->textBox17);
			this->Controls->Add(this->textBox18);
			this->Controls->Add(this->panel1);
			this->Controls->Add(this->label20);
			this->Controls->Add(this->pvkNumberLbl);
			this->Controls->Add(this->tabControl1);
			this->Controls->Add(this->panel7);
			this->Controls->Add(this->panel6);
			this->Controls->Add(this->panel2);
			this->Controls->Add(this->panel5);
			this->KeyPreview = true;
			this->Name = L"GuiForm";
			this->Text = L"GuiForm";
			this->WindowState = System::Windows::Forms::FormWindowState::Maximized;
			this->Closed += gcnew System::EventHandler(this, &GuiForm::GuiForm_Closed);
			this->Load += gcnew System::EventHandler(this, &GuiForm::GuiForm_Load);
			this->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &GuiForm::GuiForm_KeyUp);
			this->tabPage5->ResumeLayout(false);
			this->tabPage5->PerformLayout();
			this->tabControl1->ResumeLayout(false);
			this->tabPage1->ResumeLayout(false);
			this->tabPage1->PerformLayout();
			this->panel6->ResumeLayout(false);
			this->panel6->PerformLayout();
			this->panel5->ResumeLayout(false);
			this->panel5->PerformLayout();
			this->panel2->ResumeLayout(false);
			this->panel2->PerformLayout();
			this->panel1->ResumeLayout(false);
			this->panel3->ResumeLayout(false);
			this->panel3->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}

		/*
		//
		// CAN
		//
		this->CANsrv = (gcnew UcanDotNET::USBcanServer());
		this->zedGraphControl1->MouseDownEvent += gcnew ZedGraphControl::ZedMouseEventHandler(this, &GuiForm::MouseDown);
		this->zedGraphControl1->MouseUpEvent += gcnew ZedGraphControl::ZedMouseEventHandler(this, &GuiForm::MouseUp);
		this->zedGraphControl1->MouseMoveEvent += gcnew ZedGraphControl::ZedMouseEventHandler(this, &GuiForm::MouseMove);
		this->zedGraphControl5->MouseDownEvent += gcnew ZedGraphControl::ZedMouseEventHandler(this, &GuiForm::MouseDown);
		this->zedGraphControl5->MouseUpEvent += gcnew ZedGraphControl::ZedMouseEventHandler(this, &GuiForm::MouseUp);
		this->zedGraphControl5->MouseMoveEvent += gcnew ZedGraphControl::ZedMouseEventHandler(this, &GuiForm::MouseMove);
			*/
#pragma endregion
	private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e)
	{
		calibrationBtnClickHandler();
	}
	private: System::Void button2_Click(System::Object^  sender, System::EventArgs^  e)
	{
		findTransitionBtnClickHandler();
	}
	private: System::Void openFileDialog1_FileOk(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e)
	{
	}
	private: System::Void GuiForm_Load(System::Object^  sender, System::EventArgs^  e)
	{
		FileRead(0, 1);
		textBox19->Text = "" + K;
	}
	private: System::Void GuiForm_Closed(System::Object^  sender, System::EventArgs^  e)
	{
		//	 CANsrv->Shutdown(254, true);
	}
	private: System::Void GuiForm_KeyUp(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e)
	{
		if (e->KeyCode == Keys::P)
		{
			startBtnClickHandler();
		}
		else if (e->KeyCode == Keys::R)
		{
			stopBtnClickHandler();
			StartOrStop = 1;
			tabControl1->SelectTab(0);
		}
		else if (e->KeyCode == Keys::O)
		{
			openBtnClickHandler();
		}
		else if (e->KeyCode == Keys::S)
		{
			saveBtnClickHandler();
		}
		else if (e->KeyCode == Keys::C)
		{
			calibrationBtnClickHandler();
		}
		else if (e->KeyCode == Keys::B)
		{
			if (checkBox1->CheckState == CheckState::Checked)
			{
				checkBox1->CheckState = CheckState::Unchecked;
			}
			else
			{
				checkBox1->CheckState = CheckState::Checked;
			}
			nightModeBtnClickHandler();
		}
		else if (e->KeyCode == Keys::F)
		{
			findTransitionBtnClickHandler();
		}
		else if (e->KeyCode == Keys::E)
		{
			zedGraphControl1->SaveAsBitmap();
		}
		else if (e->KeyCode == Keys::I)
		{
			zedGraphControl1->Copy(true);
		}
	}
	private: System::Void splitContainer1_Panel1_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) {
	}
	private: System::Void zedGraphControl5_Load(System::Object^  sender, System::EventArgs^  e) {
	}
	private: System::Void zedGraphControl2_Load(System::Object^  sender, System::EventArgs^  e) {
	}

	private: System::Void Mouse(System::Object^  sender, MouseEventArgs^ e)
	{
	}
			 //Прерывание при нажатии кнопки на полотне графика
	private: bool MouseMove(ZedGraphControl^  sender, MouseEventArgs^ e)
	{
		int i = 0;
		if (manualScaleTrigger)
		{
			if (e->Button == System::Windows::Forms::MouseButtons::Middle)
			{
				double xOffset = 0, yOffset = 0;
				PointF^ point = gcnew PointF(e->X, e->Y);
				GraphPane^ paneSender = gcnew GraphPane;
				// Координаты, пересчитанные в систему координат графика
				double graphX, graphY;

				paneSender = sender->MasterPane->FindPane(*point);
				paneSender->ReverseTransform(*point, graphX, graphY);

				xOffset = xDragStartPoint - graphX;
				yOffset = yDragStartPoint - graphY;

				//Меняем масштаб по Y только для активного графика
				paneSender->YAxis->Scale->Min += yOffset;
				paneSender->YAxis->Scale->Max += yOffset;
				//Меняем масштаб по Х для всех графиков на полотне
				for (i = 0; i < sender->MasterPane->PaneList->Count; i++)
				{
					sender->MasterPane->PaneList[i]->XAxis->Scale->Min += xOffset;
					sender->MasterPane->PaneList[i]->XAxis->Scale->Max += xOffset;

				}
				sender->AxisChange();
				sender->Invalidate();
			}
		}
		return 1;
	}

			 //Прерывание при нажатии кнопки на полотне графика
	private: bool MouseDown(ZedGraphControl^  sender, MouseEventArgs^ e)
	{
		PointF^ point = gcnew PointF(e->X, e->Y);
		GraphPane^ paneSender = gcnew GraphPane;
		// Координаты, пересчитанные в систему координат графика
		double graphX, graphY;

		if (e->Button == System::Windows::Forms::MouseButtons::Left)
		{
			// Пересчитать координаты из системы координат, связанной с контролом zedGraph 
			// в систему координат, связанную с графиком
			paneSender = sender->MasterPane->FindPane(*point);
			if (paneSender)
			{
				paneSender->ReverseTransform(*point, graphX, graphY);
				x_down = graphX;
				y_down = graphY;
			}
		}
		else if (e->Button == System::Windows::Forms::MouseButtons::Middle)
		{
			paneSender = sender->MasterPane->FindPane(*point);
			paneSender->ReverseTransform(*point, graphX, graphY);
			xDragStartPoint = graphX;
			yDragStartPoint = graphY;
		}
		return 1;
	}
			 //Прерывание при отпускании кнопки на полотне графика
	private: bool MouseUp(ZedGraphControl^  sender, MouseEventArgs^ e)
	{
		int i = 0;
		PointF^ point = gcnew PointF(e->X, e->Y);
		GraphPane^ paneSender = gcnew GraphPane;
		// Координаты, пересчитанные в систему координат графика
		double graphX, graphY, minY = -0.001, maxY = 0.001;
		if (e->Button == System::Windows::Forms::MouseButtons::Left)
		{
			// Пересчитать координаты из системы координат, связанной с контролом zedGraph 
			// в систему координат, связанную с графиком
			paneSender = sender->MasterPane->FindPane(*point);
			if (paneSender)
			{
				paneSender->ReverseTransform(*point, graphX, graphY);
				x_up = graphX;
				y_up = graphY;

				if (x_down < x_up)
				{
					manualScaleTrigger = true;

					//В достаточно крупном масштабе отображаем точки на линиях
					if (x_up - x_down < ENEBLE_SYMBOL_STARTING_WITH)
					{
						if (enableSymbolTrigger == 0)
						{
							enableSymbolTrigger = 1;
							algorithState = 0;
							FileRead(0, 0);
						}
					}
					//Проверка что y_down меньше y_up (если окошко тянули сверху вниз)
					if (y_down > y_up)
					{
						double tmp = y_down;
						y_down = y_up;
						y_up = tmp;
					}
					for (i = 0; i < sender->MasterPane->PaneList->Count; i++)
					{
						sender->MasterPane->PaneList[i]->XAxis->Scale->Min = x_down;
						sender->MasterPane->PaneList[i]->XAxis->Scale->Max = x_up;
						if ((x_up - x_down) < 10)
						{
							sender->MasterPane->PaneList[i]->XAxis->Scale->MinorStep = (double)((int)(x_up - x_down) * 1000 / 50) / 1000;
							sender->MasterPane->PaneList[i]->XAxis->Scale->MajorStep = (double)((int)(x_up - x_down) * 1000 / 10) / 1000;
						}
						else
						{
							sender->MasterPane->PaneList[i]->XAxis->Scale->MinorStep = (int)(x_up - x_down) / 50;
							sender->MasterPane->PaneList[i]->XAxis->Scale->MajorStep = (int)(x_up - x_down) / 10;
						}
					}
					paneSender->YAxis->Scale->Min = y_down;
					paneSender->YAxis->Scale->Max = y_up;
					sender->AxisChange();
					sender->Invalidate();
				}
				else if (x_down > x_up)
				{
					if (enableSymbolTrigger)
					{
						enableSymbolTrigger = 0;
						algorithState = 0;
						FileRead(0, 0);
					}
					ScaleGraphs(sender);
				}
			}
		}
		else if (e->Button == System::Windows::Forms::MouseButtons::Right)
		{
			// Пересчитать координаты из системы координат, связанной с контролом zedGraph 
			// в систему координат, связанную с графиком
			paneSender = sender->MasterPane->FindPane(*point);
			if (paneSender)
			{
				paneSender->ReverseTransform(*point, graphX, graphY);
				x_up = graphX;
				y_up = graphY;

				if (calibrationInProgressFlag)
				{
				/*	if (boxesAreDrawedFlag)
					{
						i = 0;
						if (textBox1->Text == "")
						{
							textBox1->Text = graphX.ToString("F2");
							while (zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X < graphX)
							{
								i++;
							}
							start[0] = i;
						}
						if (textBox2->Text == "")
						{
							textBox2->Text = graphX.ToString("F2");
							while (zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X < graphX)
							{
								i++;
							}
							end[0] = i;
						}
						if (textBox3->Text == "")
						{
							textBox3->Text = graphX.ToString("F2");
							while (zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X < graphX)
							{
								i++;
							}
							start[1] = i;
						}
						if (textBox4->Text == "")
						{
							textBox4->Text = graphX.ToString("F2");
							while (zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X < graphX)
							{
								i++;
							}
							end[1] = i;
						}
						if (textBox5->Text == "")
						{
							textBox5->Text = graphX.ToString("F2");
							while (zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X < graphX)
							{
								i++;
							}
							start[2] = i;
						}
						if (textBox6->Text == "")
						{
							textBox6->Text = graphX.ToString("F2");
							while (zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X < graphX)
							{
								i++;
							}
							end[2] = i;
						}
						if (textBox7->Text == "")
						{
							textBox7->Text = graphX.ToString("F2");
							while (zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X < graphX)
							{
								i++;
							}
							start[3] = i;
						}
						if (textBox8->Text == "")
						{
							textBox8->Text = graphX.ToString("F2");
							while (zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X < graphX)
							{
								i++;
							}
							end[3] = i;
						}
						if (textBox9->Text == "")
						{
							textBox9->Text = graphX.ToString("F2");
							while (zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X < graphX)
							{
								i++;
							}
							start[4] = i;
						}
						if (textBox10->Text == "")
						{
							textBox10->Text = graphX.ToString("F2");
							while (zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X < graphX)
							{
								i++;
							}
							end[4] = i;
						}
						if (textBox11->Text == "")
						{
							textBox11->Text = graphX.ToString("F2");
							while (zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X < graphX)
							{
								i++;
							}
							start[5] = i;
						}
						if (textBox12->Text == "")
						{
							textBox12->Text = graphX.ToString("F2");
							while (zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X < graphX)
							{
								i++;
							}
							end[5] = i;
						}
						if (textBox13->Text == "")
						{
							textBox13->Text = graphX.ToString("F2");
							while (zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X < graphX)
							{
								i++;
							}
							start[6] = i;
						}
						if (textBox14->Text == "")
						{
							textBox14->Text = graphX.ToString("F2");
							while (zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X < graphX)
							{
								i++;
							}
							end[6] = i;
						}
						if (textBox15->Text == "")
						{
							textBox15->Text = graphX.ToString("F2");
							while (zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X < graphX)
							{
								i++;
							}
							start[7] = i;
						}
						if (textBox16->Text == "")
						{
							textBox16->Text = graphX.ToString("F2");
							while (zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X < graphX)
							{
								i++;
							}
							end[7] = i;
						}
						if (textBox17->Text == "")
						{
							textBox17->Text = graphX.ToString("F2");
							while (zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X < graphX)
							{
								i++;
							}
							start[8] = i;
						}
						if (textBox18->Text == "")
						{
							textBox18->Text = graphX.ToString("F2");
							while (zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X < graphX)
							{
								i++;
							}
							end[8] = i;
						}
					}
					else
					{
						i = 0;
						checkBoxCounter++;
						switch (checkBoxCounter)
						{
						case 1:
							textBox1->Text = graphX.ToString("F2");
							while (zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X < graphX)
							{
								i++;
							}
							start[0] = i;
							break;
						case 2:
							textBox2->Text = graphX.ToString("F2");
							while (zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X < graphX)
							{
								i++;
							}
							end[0] = i;
							break;
						case 3:
							textBox3->Text = graphX.ToString("F2");
							while (zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X < graphX)
							{
								i++;
							}
							start[1] = i;
							break;
						case 4:
							textBox4->Text = graphX.ToString("F2");
							while (zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X < graphX)
							{
								i++;
							}
							end[1] = i;
							break;
						case 5:
							textBox5->Text = graphX.ToString("F2");
							while (zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X < graphX)
							{
								i++;
							}
							start[2] = i;
							break;
						case 6:
							textBox6->Text = graphX.ToString("F2");
							while (zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X < graphX)
							{
								i++;
							}
							end[2] = i;
							break;
						case 7:
							textBox7->Text = graphX.ToString("F2");
							while (zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X < graphX)
							{
								i++;
							}
							start[3] = i;
							break;
						case 8:
							textBox8->Text = graphX.ToString("F2");
							while (zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X < graphX)
							{
								i++;
							}
							end[3] = i;
							break;
						case 9:
							textBox9->Text = graphX.ToString("F2");
							while (zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X < graphX)
							{
								i++;
							}
							start[4] = i;
							break;
						case 10:
							textBox10->Text = graphX.ToString("F2");
							while (zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X < graphX)
							{
								i++;
							}
							end[4] = i;
							break;
						case 11:
							textBox11->Text = graphX.ToString("F2");
							while (zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X < graphX)
							{
								i++;
							}
							start[5] = i;
							break;
						case 12:
							textBox12->Text = graphX.ToString("F2");
							while (zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X < graphX)
							{
								i++;
							}
							end[5] = i;
							break;
						case 13:
							textBox13->Text = graphX.ToString("F2");
							while (zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X < graphX)
							{
								i++;
							}
							start[6] = i;
							break;
						case 14:
							textBox14->Text = graphX.ToString("F2");
							while (zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X < graphX)
							{
								i++;
							}
							end[6] = i;
							break;
						case 15:
							textBox15->Text = graphX.ToString("F2");
							while (zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X < graphX)
							{
								i++;
							}
							start[7] = i;
							break;
						case 16:
							textBox16->Text = graphX.ToString("F2");
							while (zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X < graphX)
							{
								i++;
							}
							end[7] = i;
							break;
						case 17:
							textBox17->Text = graphX.ToString("F2");
							while (zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X < graphX)
							{
								i++;
							}
							start[8] = i;
							break;
						case 18:
							textBox18->Text = graphX.ToString("F2");
							while (zedGraphControl1->MasterPane->PaneList[3]->CurveList[0]->Points[i]->X < graphX)
							{
								i++;
							}
							end[8] = i;
							algorithState = 1;
							break;
						default: break;
						}
					}
					ReDrawBoxes(zedGraphControl1);*/
				}
			}
		}

		return 1;
	}

	private: System::Void startBtn_Click(System::Object^  sender, System::EventArgs^  e)
	{
		startBtnClickHandler();
	}
	private: System::Void pauseBtn_Click(System::Object^  sender, System::EventArgs^  e) {
	}
	private: System::Void stopBtn_Click_1(System::Object^  sender, System::EventArgs^  e)
	{
		stopBtnClickHandler();
	}
			 //Open button
	private: System::Void openBtn_Click_1(System::Object^  sender, System::EventArgs^  e)
	{
		openBtnClickHandler();
	}
			 //Save button
	private: System::Void saveBtn_Click(System::Object^  sender, System::EventArgs^  e)
	{
		saveBtnClickHandler();
	}
	private: System::Void OnCheckedChanged(System::Object^  sender, System::EventArgs^  e) {

		nightModeBtnClickHandler();
	}
	private: System::Void saveFileDialog1_FileOk(System::Object^  sender, System::ComponentModel::CancelEventArgs^  e) {
	}
	private: System::Void SendLitBtn_Click(System::Object^  sender, System::EventArgs^  e) {
	
	}
	private: System::Void changeKCheckBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
		if (changeKCheckBox->CheckState == CheckState::Checked)
		{
			textBox19->Enabled = true;
		}
		else
		{
			textBox19->Enabled = false;
		}
	}
	};
}
