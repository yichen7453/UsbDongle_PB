#pragma once

#include <msclr/marshal_cppstd.h>
#include <iostream>
#include <fstream>
#include <conio.h>

#include "libusb.h"

#include "usb_ctl.h"
#include "define.h"

#include "pb_ctl_secure.h"

static uint8_t regs_table[] = {
	0xE0, 0x02,  // AMP
	0xE1, 0x1A,  // VRT 1.60V
	0xE2, 0x10,  // VRB 0.100V + 0.025V * 0x21
	0xE6, 0x12   // DC offset, VCC=3.0V use 0x10, VCC=3.3V use 0x15
};

static uint8_t reset_table[] = {
	0xD2, 0x10,  // sleep mode
	0xD1, 0x80   // soft reset, delay must put here
};

static uint8_t init_table[] = {
	0xD2, 0x10,
	0x30, 0x0A,
	0x31, 0x0A,
	0x32, 0x07,
	0x35, 0x08,
	0x36, 0x8C,
	0x37, 0x64,
	0x38, 0x07,
	0xE0, 0x02,  // AMP
	0xE1, 0x1A,  // VRT 1.60V
	0xE2, 0x10,  // VRB 0.100V + 0.025V * 0x21
	0xE6, 0x12,  // DC offset, VCC=3.0V use 0x10, VCC=3.3V use 0x15
	0x3A, 0x80,
	0x3B, 0x02,
	0xD2, 0x13,
	0xD3, 0x00   // clear status, disable DVR, VRB calibration
};

namespace UsbDongle_PB {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Drawing::Imaging;
	using namespace System::IO;

	using namespace System::Diagnostics;
	using namespace System::Runtime::InteropServices;
	using namespace System::Threading;


	unsigned char dataString[20000];

	bool enroll_save_image;
	bool enroll_preview;
	bool verify_preview;
	bool verify_con;

	bool quality_chk_flag;
	bool get_frame;
	int verify_fail = 0;

	bool start_capture;
	bool start_realtime;
	bool start_enroll;
	bool start_verify;

	char file_path_string[200];

	char databaseFile[200];
	char listViewFile[200];

	char imageFile[100];
	char logo[100];
	char fingerprint_enrollScan[200];
	char fingerprint_enrollProgress[200];
	char fingerprint_enrollFinish[200];
	char fingerprint_verifyScan[200];
	char fingerprint_verifySuccess[200];
	char fingerprint_verifyFail[200];

	char* app_path = (char*)(void*)Marshal::StringToHGlobalAnsi(Application::StartupPath);

	int finger_idx = -1;
	char date_buf[80];
	char datetime_buf[80];
	char finger_idx_buf[10];
	char* filename;

	int	max_samples = 0;
	int num_accepted = 0;
	int num_accepted_prev = 0;

	int sensor_width = 0;
	int sensor_height = 0;

	/// <summary>
	/// UsbDongle_GUI 的摘要
	/// </summary>
	public ref class UsbDongle_GUI : public System::Windows::Forms::Form
	{
	public:
		UsbDongle_GUI(void)
		{
			InitializeComponent();
			
			strcpy(file_path_string, app_path);
			strcat(file_path_string, "\\res");
			strcat(file_path_string, "\\logo.png");
			sprintf(logo, file_path_string);
			logoImage = gcnew Bitmap(Marshal::PtrToStringAnsi((IntPtr)logo));
			pictureBox1->Image = logoImage;

			time_t		now = time(0);
			struct tm	tstruct;
			tstruct = *localtime(&now);

			strftime(date_buf, sizeof(date_buf), "%Y%m%d", &tstruct);
			//printf("date => %s\n", date_buf);

			enrollProgressImage = get_enroll_progress_image();

			template_path = Application::StartupPath + "\\Template\\";
			image_path = Application::StartupPath + "\\Fingerprint Image\\";
		}

	protected:
		/// <summary>
		/// 清除任何使用中的資源。
		/// </summary>
		~UsbDongle_GUI()
		{
			if (components)
			{
				delete components;
			}
		}

	protected:


	protected:

	private:
		/// <summary>
		/// 設計工具所需的變數。
		/// </summary>
		struct libusb_device_handle *dev_handle = NULL;
		int deviceLocation = 0;

		uint16_t pixel_cnt = 0;
		int frame_cnt = 0;
		int loopcnt = 0;
		unsigned int start;

		MemoryStream^ memStream;
		array<Byte>^ byteArray;
		array<Byte>^ deleteHeadArray;
		unsigned char* p;
		uint8_t toggle = 0x02;

		array<System::String^>^ fileList;
		array<System::Drawing::Bitmap^>^ enrollProgressImage;
		array<System::String^>^ filePath;

		Bitmap^ logoImage;
		Bitmap^ enrollScan;
		Bitmap^ enrollProgress;
		Bitmap^ enrollFinish;
		Bitmap^ verifyScan;
		Bitmap^ verifySuccess;
		Bitmap^ verifyFail;

		System::String^ sensorType;
		System::String^ template_path;
		System::String^ image_path;
		System::String^ template_type_path;
		System::String^ image_type_path;

		int max_enrollment_samples = 0;
		int max_fingerprint_area = 0;
		int set_enrollment_samples = 8;
		int enrollment_image_quality = 0;
		int enrollment_fingerprint_area = 0;
		int verify_image_quality = 0;
		int verify_fingerprint_area = 0;

	private: System::ComponentModel::IContainer^  components;
	private: System::Windows::Forms::TabControl^  tabControl1;
	private: System::Windows::Forms::TabPage^  tabPage1;
	private: System::Windows::Forms::TabPage^  tabPage2;
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::ComboBox^  cboDevices;
	private: System::Windows::Forms::TreeView^  tvInfo;
	private: System::Windows::Forms::Button^  btnRealTime;


	private: System::Windows::Forms::Button^  btnRead_Reg;
	private: System::Windows::Forms::Button^  btnWrite_Reg;
	private: System::Windows::Forms::Label^  lb_info;


	private: System::Windows::Forms::Timer^  realtime_timer;
	private: System::Windows::Forms::PictureBox^  pictureBox1;
	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::Button^  btnCancel;
	private: System::Windows::Forms::Button^  btnVerify;
	private: System::Windows::Forms::Button^  btnDeleteAll;



	private: System::Windows::Forms::Button^  btnEnroll;

	private: System::Windows::Forms::Button^  btnCapture;
	private: System::Windows::Forms::Timer^  capture_timer;
	private: System::Windows::Forms::GroupBox^  groupBox3;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Button^  btnSave;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label_area;
	private: System::Windows::Forms::Label^  label_quality;
	private: System::Windows::Forms::Label^  lb_message;
	private: System::Windows::Forms::Timer^  enroll_timer;
	private: System::Windows::Forms::GroupBox^  groupBox4;
	private: System::Windows::Forms::CheckBox^  cb_enroll_save_image;
	private: System::Windows::Forms::CheckBox^  cb_enroll_preview;
	private: System::Windows::Forms::TextBox^  tb_enroll_name;
	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::GroupBox^  groupBox5;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::Label^  lb_enroll_return_coverage;
	private: System::Windows::Forms::TextBox^  tb_enroll_image_quality;
	private: System::Windows::Forms::TextBox^  tb_enroll_return_coverage;
	private: System::Windows::Forms::TextBox^  tb_enroll_fingerprint_area;
	private: System::Windows::Forms::Label^  label10;
	private: System::Windows::Forms::Label^  lb_enroll_fingerprint_area;
	private: System::Windows::Forms::Label^  lb_enroll_count;
	private: System::Windows::Forms::Label^  lb_enroll_image_quality;
	private: System::Windows::Forms::TextBox^  tb_enroll_count;
	private: System::Windows::Forms::Label^  label8;
private: System::Windows::Forms::GroupBox^  groupBox6;
private: System::Windows::Forms::GroupBox^  groupBox7;
private: System::Windows::Forms::Label^  label9;
private: System::Windows::Forms::Label^  label7;
private: System::Windows::Forms::Label^  lb_verify_fingerprint_area;
private: System::Windows::Forms::CheckBox^  cb_verify_con;
private: System::Windows::Forms::Label^  lb_verify_image_quality;
private: System::Windows::Forms::CheckBox^  cb_verify_preview;
private: System::Windows::Forms::TextBox^  tb_verify_fingerprint_area;
private: System::Windows::Forms::TextBox^  tb_verify_image_quality;
private: System::Windows::Forms::PictureBox^  pb_png;
private: System::Windows::Forms::PictureBox^  pb_gif;
private: System::Windows::Forms::Timer^  verify_timer;
private: System::Windows::Forms::TextBox^  textBox2;
private: System::Windows::Forms::ComboBox^  cboSensorType;


private: System::Windows::Forms::Button^  btnOpen_device;





#pragma region Windows Form Designer generated code
		/// <summary>
		/// 此為設計工具支援所需的方法 - 請勿使用程式碼編輯器
		/// 修改這個方法的內容。
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(UsbDongle_GUI::typeid));
			this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
			this->tabPage1 = (gcnew System::Windows::Forms::TabPage());
			this->tvInfo = (gcnew System::Windows::Forms::TreeView());
			this->tabPage2 = (gcnew System::Windows::Forms::TabPage());
			this->pb_gif = (gcnew System::Windows::Forms::PictureBox());
			this->pb_png = (gcnew System::Windows::Forms::PictureBox());
			this->groupBox6 = (gcnew System::Windows::Forms::GroupBox());
			this->groupBox7 = (gcnew System::Windows::Forms::GroupBox());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->lb_verify_fingerprint_area = (gcnew System::Windows::Forms::Label());
			this->cb_verify_con = (gcnew System::Windows::Forms::CheckBox());
			this->lb_verify_image_quality = (gcnew System::Windows::Forms::Label());
			this->cb_verify_preview = (gcnew System::Windows::Forms::CheckBox());
			this->tb_verify_fingerprint_area = (gcnew System::Windows::Forms::TextBox());
			this->tb_verify_image_quality = (gcnew System::Windows::Forms::TextBox());
			this->groupBox4 = (gcnew System::Windows::Forms::GroupBox());
			this->cb_enroll_save_image = (gcnew System::Windows::Forms::CheckBox());
			this->cb_enroll_preview = (gcnew System::Windows::Forms::CheckBox());
			this->tb_enroll_name = (gcnew System::Windows::Forms::TextBox());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->groupBox5 = (gcnew System::Windows::Forms::GroupBox());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->lb_enroll_return_coverage = (gcnew System::Windows::Forms::Label());
			this->tb_enroll_image_quality = (gcnew System::Windows::Forms::TextBox());
			this->tb_enroll_return_coverage = (gcnew System::Windows::Forms::TextBox());
			this->tb_enroll_fingerprint_area = (gcnew System::Windows::Forms::TextBox());
			this->label10 = (gcnew System::Windows::Forms::Label());
			this->lb_enroll_fingerprint_area = (gcnew System::Windows::Forms::Label());
			this->lb_enroll_count = (gcnew System::Windows::Forms::Label());
			this->lb_enroll_image_quality = (gcnew System::Windows::Forms::Label());
			this->tb_enroll_count = (gcnew System::Windows::Forms::TextBox());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->lb_message = (gcnew System::Windows::Forms::Label());
			this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
			this->label_area = (gcnew System::Windows::Forms::Label());
			this->label_quality = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->btnSave = (gcnew System::Windows::Forms::Button());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->cboSensorType = (gcnew System::Windows::Forms::ComboBox());
			this->btnCancel = (gcnew System::Windows::Forms::Button());
			this->btnVerify = (gcnew System::Windows::Forms::Button());
			this->btnDeleteAll = (gcnew System::Windows::Forms::Button());
			this->btnEnroll = (gcnew System::Windows::Forms::Button());
			this->btnCapture = (gcnew System::Windows::Forms::Button());
			this->btnOpen_device = (gcnew System::Windows::Forms::Button());
			this->btnRealTime = (gcnew System::Windows::Forms::Button());
			this->lb_info = (gcnew System::Windows::Forms::Label());
			this->btnRead_Reg = (gcnew System::Windows::Forms::Button());
			this->btnWrite_Reg = (gcnew System::Windows::Forms::Button());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->cboDevices = (gcnew System::Windows::Forms::ComboBox());
			this->realtime_timer = (gcnew System::Windows::Forms::Timer(this->components));
			this->capture_timer = (gcnew System::Windows::Forms::Timer(this->components));
			this->enroll_timer = (gcnew System::Windows::Forms::Timer(this->components));
			this->verify_timer = (gcnew System::Windows::Forms::Timer(this->components));
			this->textBox2 = (gcnew System::Windows::Forms::TextBox());
			this->tabControl1->SuspendLayout();
			this->tabPage1->SuspendLayout();
			this->tabPage2->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pb_gif))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pb_png))->BeginInit();
			this->groupBox6->SuspendLayout();
			this->groupBox4->SuspendLayout();
			this->groupBox3->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->BeginInit();
			this->groupBox2->SuspendLayout();
			this->groupBox1->SuspendLayout();
			this->SuspendLayout();
			// 
			// tabControl1
			// 
			this->tabControl1->Controls->Add(this->tabPage1);
			this->tabControl1->Controls->Add(this->tabPage2);
			this->tabControl1->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 11.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->tabControl1->Location = System::Drawing::Point(6, 67);
			this->tabControl1->Name = L"tabControl1";
			this->tabControl1->SelectedIndex = 0;
			this->tabControl1->Size = System::Drawing::Size(752, 553);
			this->tabControl1->TabIndex = 1;
			this->tabControl1->SelectedIndexChanged += gcnew System::EventHandler(this, &UsbDongle_GUI::tabControl_SelectedIndexChanged);
			// 
			// tabPage1
			// 
			this->tabPage1->BackColor = System::Drawing::SystemColors::Control;
			this->tabPage1->Controls->Add(this->tvInfo);
			this->tabPage1->Location = System::Drawing::Point(4, 28);
			this->tabPage1->Name = L"tabPage1";
			this->tabPage1->Padding = System::Windows::Forms::Padding(3);
			this->tabPage1->Size = System::Drawing::Size(744, 521);
			this->tabPage1->TabIndex = 0;
			this->tabPage1->Text = L"Device Info";
			// 
			// tvInfo
			// 
			this->tvInfo->Location = System::Drawing::Point(6, 9);
			this->tvInfo->Name = L"tvInfo";
			this->tvInfo->Size = System::Drawing::Size(716, 485);
			this->tvInfo->TabIndex = 0;
			// 
			// tabPage2
			// 
			this->tabPage2->BackColor = System::Drawing::SystemColors::Control;
			this->tabPage2->Controls->Add(this->pb_gif);
			this->tabPage2->Controls->Add(this->pb_png);
			this->tabPage2->Controls->Add(this->groupBox6);
			this->tabPage2->Controls->Add(this->groupBox4);
			this->tabPage2->Controls->Add(this->lb_message);
			this->tabPage2->Controls->Add(this->groupBox3);
			this->tabPage2->Controls->Add(this->groupBox2);
			this->tabPage2->Controls->Add(this->lb_info);
			this->tabPage2->Controls->Add(this->btnRead_Reg);
			this->tabPage2->Controls->Add(this->btnWrite_Reg);
			this->tabPage2->Location = System::Drawing::Point(4, 28);
			this->tabPage2->Name = L"tabPage2";
			this->tabPage2->Padding = System::Windows::Forms::Padding(3);
			this->tabPage2->Size = System::Drawing::Size(744, 521);
			this->tabPage2->TabIndex = 1;
			this->tabPage2->Text = L"Main";
			// 
			// pb_gif
			// 
			this->pb_gif->BackColor = System::Drawing::Color::White;
			this->pb_gif->Location = System::Drawing::Point(175, 65);
			this->pb_gif->Name = L"pb_gif";
			this->pb_gif->Size = System::Drawing::Size(140, 140);
			this->pb_gif->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->pb_gif->TabIndex = 25;
			this->pb_gif->TabStop = false;
			this->pb_gif->Visible = false;
			// 
			// pb_png
			// 
			this->pb_png->BackColor = System::Drawing::Color::Transparent;
			this->pb_png->Location = System::Drawing::Point(321, 30);
			this->pb_png->Name = L"pb_png";
			this->pb_png->Size = System::Drawing::Size(140, 140);
			this->pb_png->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->pb_png->TabIndex = 24;
			this->pb_png->TabStop = false;
			this->pb_png->Visible = false;
			// 
			// groupBox6
			// 
			this->groupBox6->Controls->Add(this->groupBox7);
			this->groupBox6->Controls->Add(this->label9);
			this->groupBox6->Controls->Add(this->label7);
			this->groupBox6->Controls->Add(this->lb_verify_fingerprint_area);
			this->groupBox6->Controls->Add(this->cb_verify_con);
			this->groupBox6->Controls->Add(this->lb_verify_image_quality);
			this->groupBox6->Controls->Add(this->cb_verify_preview);
			this->groupBox6->Controls->Add(this->tb_verify_fingerprint_area);
			this->groupBox6->Controls->Add(this->tb_verify_image_quality);
			this->groupBox6->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->groupBox6->Location = System::Drawing::Point(518, 271);
			this->groupBox6->Name = L"groupBox6";
			this->groupBox6->Size = System::Drawing::Size(221, 156);
			this->groupBox6->TabIndex = 22;
			this->groupBox6->TabStop = false;
			this->groupBox6->Text = L"Verify Parameter";
			// 
			// groupBox7
			// 
			this->groupBox7->BackColor = System::Drawing::Color::White;
			this->groupBox7->Location = System::Drawing::Point(1, 84);
			this->groupBox7->Name = L"groupBox7";
			this->groupBox7->Size = System::Drawing::Size(225, 2);
			this->groupBox7->TabIndex = 13;
			this->groupBox7->TabStop = false;
			// 
			// label9
			// 
			this->label9->AutoSize = true;
			this->label9->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label9->Location = System::Drawing::Point(10, 126);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(88, 18);
			this->label9->TabIndex = 18;
			this->label9->Text = L"Area     :";
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label7->Location = System::Drawing::Point(10, 99);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(88, 18);
			this->label7->TabIndex = 17;
			this->label7->Text = L"Quality  :";
			// 
			// lb_verify_fingerprint_area
			// 
			this->lb_verify_fingerprint_area->AutoSize = true;
			this->lb_verify_fingerprint_area->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 10, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->lb_verify_fingerprint_area->Location = System::Drawing::Point(158, 125);
			this->lb_verify_fingerprint_area->Name = L"lb_verify_fingerprint_area";
			this->lb_verify_fingerprint_area->Size = System::Drawing::Size(48, 18);
			this->lb_verify_fingerprint_area->TabIndex = 22;
			this->lb_verify_fingerprint_area->Text = L"(0-0)";
			// 
			// cb_verify_con
			// 
			this->cb_verify_con->AutoSize = true;
			this->cb_verify_con->Enabled = false;
			this->cb_verify_con->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 11, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->cb_verify_con->Location = System::Drawing::Point(11, 60);
			this->cb_verify_con->Name = L"cb_verify_con";
			this->cb_verify_con->Size = System::Drawing::Size(181, 23);
			this->cb_verify_con->TabIndex = 18;
			this->cb_verify_con->Text = L"Continuous verify";
			this->cb_verify_con->UseVisualStyleBackColor = true;
			// 
			// lb_verify_image_quality
			// 
			this->lb_verify_image_quality->AutoSize = true;
			this->lb_verify_image_quality->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 10, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->lb_verify_image_quality->Location = System::Drawing::Point(158, 99);
			this->lb_verify_image_quality->Name = L"lb_verify_image_quality";
			this->lb_verify_image_quality->Size = System::Drawing::Size(48, 18);
			this->lb_verify_image_quality->TabIndex = 21;
			this->lb_verify_image_quality->Text = L"(0-0)";
			// 
			// cb_verify_preview
			// 
			this->cb_verify_preview->AutoSize = true;
			this->cb_verify_preview->Enabled = false;
			this->cb_verify_preview->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 11, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->cb_verify_preview->Location = System::Drawing::Point(11, 30);
			this->cb_verify_preview->Name = L"cb_verify_preview";
			this->cb_verify_preview->Size = System::Drawing::Size(145, 23);
			this->cb_verify_preview->TabIndex = 17;
			this->cb_verify_preview->Text = L"Preview image";
			this->cb_verify_preview->UseVisualStyleBackColor = true;
			// 
			// tb_verify_fingerprint_area
			// 
			this->tb_verify_fingerprint_area->BackColor = System::Drawing::Color::White;
			this->tb_verify_fingerprint_area->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->tb_verify_fingerprint_area->Enabled = false;
			this->tb_verify_fingerprint_area->Location = System::Drawing::Point(104, 125);
			this->tb_verify_fingerprint_area->MaxLength = 2;
			this->tb_verify_fingerprint_area->Name = L"tb_verify_fingerprint_area";
			this->tb_verify_fingerprint_area->Size = System::Drawing::Size(48, 21);
			this->tb_verify_fingerprint_area->TabIndex = 20;
			this->tb_verify_fingerprint_area->Text = L"0";
			this->tb_verify_fingerprint_area->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// tb_verify_image_quality
			// 
			this->tb_verify_image_quality->BackColor = System::Drawing::Color::White;
			this->tb_verify_image_quality->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->tb_verify_image_quality->Enabled = false;
			this->tb_verify_image_quality->Location = System::Drawing::Point(104, 99);
			this->tb_verify_image_quality->MaxLength = 3;
			this->tb_verify_image_quality->Name = L"tb_verify_image_quality";
			this->tb_verify_image_quality->Size = System::Drawing::Size(48, 21);
			this->tb_verify_image_quality->TabIndex = 19;
			this->tb_verify_image_quality->Text = L"0";
			this->tb_verify_image_quality->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// groupBox4
			// 
			this->groupBox4->Controls->Add(this->cb_enroll_save_image);
			this->groupBox4->Controls->Add(this->cb_enroll_preview);
			this->groupBox4->Controls->Add(this->tb_enroll_name);
			this->groupBox4->Controls->Add(this->label6);
			this->groupBox4->Controls->Add(this->groupBox5);
			this->groupBox4->Controls->Add(this->label4);
			this->groupBox4->Controls->Add(this->label5);
			this->groupBox4->Controls->Add(this->lb_enroll_return_coverage);
			this->groupBox4->Controls->Add(this->tb_enroll_image_quality);
			this->groupBox4->Controls->Add(this->tb_enroll_return_coverage);
			this->groupBox4->Controls->Add(this->tb_enroll_fingerprint_area);
			this->groupBox4->Controls->Add(this->label10);
			this->groupBox4->Controls->Add(this->lb_enroll_fingerprint_area);
			this->groupBox4->Controls->Add(this->lb_enroll_count);
			this->groupBox4->Controls->Add(this->lb_enroll_image_quality);
			this->groupBox4->Controls->Add(this->tb_enroll_count);
			this->groupBox4->Controls->Add(this->label8);
			this->groupBox4->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->groupBox4->Location = System::Drawing::Point(289, 271);
			this->groupBox4->Name = L"groupBox4";
			this->groupBox4->Size = System::Drawing::Size(225, 248);
			this->groupBox4->TabIndex = 16;
			this->groupBox4->TabStop = false;
			this->groupBox4->Text = L"Enroll Parameter";
			// 
			// cb_enroll_save_image
			// 
			this->cb_enroll_save_image->Enabled = false;
			this->cb_enroll_save_image->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 11, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->cb_enroll_save_image->Location = System::Drawing::Point(11, 62);
			this->cb_enroll_save_image->Name = L"cb_enroll_save_image";
			this->cb_enroll_save_image->Size = System::Drawing::Size(181, 23);
			this->cb_enroll_save_image->TabIndex = 17;
			this->cb_enroll_save_image->Text = L"Save enroll image";
			this->cb_enroll_save_image->UseVisualStyleBackColor = true;
			// 
			// cb_enroll_preview
			// 
			this->cb_enroll_preview->Enabled = false;
			this->cb_enroll_preview->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 11, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->cb_enroll_preview->Location = System::Drawing::Point(11, 33);
			this->cb_enroll_preview->Name = L"cb_enroll_preview";
			this->cb_enroll_preview->Size = System::Drawing::Size(145, 23);
			this->cb_enroll_preview->TabIndex = 16;
			this->cb_enroll_preview->Text = L"Preview image";
			this->cb_enroll_preview->UseVisualStyleBackColor = true;
			// 
			// tb_enroll_name
			// 
			this->tb_enroll_name->BackColor = System::Drawing::Color::White;
			this->tb_enroll_name->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->tb_enroll_name->Enabled = false;
			this->tb_enroll_name->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->tb_enroll_name->Location = System::Drawing::Point(104, 99);
			this->tb_enroll_name->MaxLength = 10;
			this->tb_enroll_name->Name = L"tb_enroll_name";
			this->tb_enroll_name->ShortcutsEnabled = false;
			this->tb_enroll_name->Size = System::Drawing::Size(110, 21);
			this->tb_enroll_name->TabIndex = 14;
			this->tb_enroll_name->Text = L"0";
			this->tb_enroll_name->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label6->Location = System::Drawing::Point(8, 102);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(88, 18);
			this->label6->TabIndex = 13;
			this->label6->Text = L"Name     :";
			// 
			// groupBox5
			// 
			this->groupBox5->BackColor = System::Drawing::Color::White;
			this->groupBox5->Location = System::Drawing::Point(0, 182);
			this->groupBox5->Name = L"groupBox5";
			this->groupBox5->Size = System::Drawing::Size(225, 2);
			this->groupBox5->TabIndex = 12;
			this->groupBox5->TabStop = false;
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label4->Location = System::Drawing::Point(8, 191);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(88, 18);
			this->label4->TabIndex = 0;
			this->label4->Text = L"Quality  :";
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label5->Location = System::Drawing::Point(8, 216);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(88, 18);
			this->label5->TabIndex = 1;
			this->label5->Text = L"Area     :";
			// 
			// lb_enroll_return_coverage
			// 
			this->lb_enroll_return_coverage->AutoSize = true;
			this->lb_enroll_return_coverage->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 10, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->lb_enroll_return_coverage->Location = System::Drawing::Point(158, 152);
			this->lb_enroll_return_coverage->Name = L"lb_enroll_return_coverage";
			this->lb_enroll_return_coverage->Size = System::Drawing::Size(48, 18);
			this->lb_enroll_return_coverage->TabIndex = 11;
			this->lb_enroll_return_coverage->Text = L"(0-0)";
			// 
			// tb_enroll_image_quality
			// 
			this->tb_enroll_image_quality->BackColor = System::Drawing::Color::White;
			this->tb_enroll_image_quality->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->tb_enroll_image_quality->Enabled = false;
			this->tb_enroll_image_quality->Location = System::Drawing::Point(104, 189);
			this->tb_enroll_image_quality->MaxLength = 3;
			this->tb_enroll_image_quality->Name = L"tb_enroll_image_quality";
			this->tb_enroll_image_quality->Size = System::Drawing::Size(48, 21);
			this->tb_enroll_image_quality->TabIndex = 2;
			this->tb_enroll_image_quality->Text = L"0";
			this->tb_enroll_image_quality->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// tb_enroll_return_coverage
			// 
			this->tb_enroll_return_coverage->BackColor = System::Drawing::Color::White;
			this->tb_enroll_return_coverage->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->tb_enroll_return_coverage->Enabled = false;
			this->tb_enroll_return_coverage->Location = System::Drawing::Point(104, 151);
			this->tb_enroll_return_coverage->MaxLength = 3;
			this->tb_enroll_return_coverage->Name = L"tb_enroll_return_coverage";
			this->tb_enroll_return_coverage->Size = System::Drawing::Size(48, 21);
			this->tb_enroll_return_coverage->TabIndex = 10;
			this->tb_enroll_return_coverage->Text = L"0";
			this->tb_enroll_return_coverage->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// tb_enroll_fingerprint_area
			// 
			this->tb_enroll_fingerprint_area->BackColor = System::Drawing::Color::White;
			this->tb_enroll_fingerprint_area->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->tb_enroll_fingerprint_area->Enabled = false;
			this->tb_enroll_fingerprint_area->Location = System::Drawing::Point(104, 215);
			this->tb_enroll_fingerprint_area->MaxLength = 2;
			this->tb_enroll_fingerprint_area->Name = L"tb_enroll_fingerprint_area";
			this->tb_enroll_fingerprint_area->Size = System::Drawing::Size(48, 21);
			this->tb_enroll_fingerprint_area->TabIndex = 3;
			this->tb_enroll_fingerprint_area->Text = L"0";
			this->tb_enroll_fingerprint_area->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// label10
			// 
			this->label10->AutoSize = true;
			this->label10->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label10->Location = System::Drawing::Point(8, 152);
			this->label10->Name = L"label10";
			this->label10->Size = System::Drawing::Size(88, 18);
			this->label10->TabIndex = 9;
			this->label10->Text = L"Coverage :";
			// 
			// lb_enroll_fingerprint_area
			// 
			this->lb_enroll_fingerprint_area->AutoSize = true;
			this->lb_enroll_fingerprint_area->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 10, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->lb_enroll_fingerprint_area->Location = System::Drawing::Point(158, 216);
			this->lb_enroll_fingerprint_area->Name = L"lb_enroll_fingerprint_area";
			this->lb_enroll_fingerprint_area->Size = System::Drawing::Size(48, 18);
			this->lb_enroll_fingerprint_area->TabIndex = 8;
			this->lb_enroll_fingerprint_area->Text = L"(0-0)";
			// 
			// lb_enroll_count
			// 
			this->lb_enroll_count->AutoSize = true;
			this->lb_enroll_count->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->lb_enroll_count->Location = System::Drawing::Point(158, 127);
			this->lb_enroll_count->Name = L"lb_enroll_count";
			this->lb_enroll_count->Size = System::Drawing::Size(48, 18);
			this->lb_enroll_count->TabIndex = 6;
			this->lb_enroll_count->Text = L"(0-0)";
			// 
			// lb_enroll_image_quality
			// 
			this->lb_enroll_image_quality->AutoSize = true;
			this->lb_enroll_image_quality->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 10, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->lb_enroll_image_quality->Location = System::Drawing::Point(158, 191);
			this->lb_enroll_image_quality->Name = L"lb_enroll_image_quality";
			this->lb_enroll_image_quality->Size = System::Drawing::Size(48, 18);
			this->lb_enroll_image_quality->TabIndex = 7;
			this->lb_enroll_image_quality->Text = L"(0-0)";
			// 
			// tb_enroll_count
			// 
			this->tb_enroll_count->BackColor = System::Drawing::Color::White;
			this->tb_enroll_count->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->tb_enroll_count->Enabled = false;
			this->tb_enroll_count->Location = System::Drawing::Point(104, 125);
			this->tb_enroll_count->MaxLength = 2;
			this->tb_enroll_count->Name = L"tb_enroll_count";
			this->tb_enroll_count->ShortcutsEnabled = false;
			this->tb_enroll_count->Size = System::Drawing::Size(48, 21);
			this->tb_enroll_count->TabIndex = 5;
			this->tb_enroll_count->Text = L"0";
			this->tb_enroll_count->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// label8
			// 
			this->label8->AutoSize = true;
			this->label8->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label8->Location = System::Drawing::Point(8, 127);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(88, 18);
			this->label8->TabIndex = 4;
			this->label8->Text = L"Count    :";
			// 
			// lb_message
			// 
			this->lb_message->BackColor = System::Drawing::Color::White;
			this->lb_message->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 15, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->lb_message->ForeColor = System::Drawing::Color::DodgerBlue;
			this->lb_message->Location = System::Drawing::Point(55, 30);
			this->lb_message->Name = L"lb_message";
			this->lb_message->Size = System::Drawing::Size(390, 26);
			this->lb_message->TabIndex = 15;
			this->lb_message->Text = L"message";
			this->lb_message->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			this->lb_message->Visible = false;
			// 
			// groupBox3
			// 
			this->groupBox3->Controls->Add(this->label_area);
			this->groupBox3->Controls->Add(this->label_quality);
			this->groupBox3->Controls->Add(this->label3);
			this->groupBox3->Controls->Add(this->btnSave);
			this->groupBox3->Controls->Add(this->label2);
			this->groupBox3->Controls->Add(this->pictureBox1);
			this->groupBox3->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->groupBox3->Location = System::Drawing::Point(491, 3);
			this->groupBox3->Name = L"groupBox3";
			this->groupBox3->Size = System::Drawing::Size(248, 266);
			this->groupBox3->TabIndex = 12;
			this->groupBox3->TabStop = false;
			this->groupBox3->Text = L"Show Image";
			// 
			// label_area
			// 
			this->label_area->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label_area->Location = System::Drawing::Point(202, 236);
			this->label_area->Name = L"label_area";
			this->label_area->Size = System::Drawing::Size(40, 20);
			this->label_area->TabIndex = 16;
			this->label_area->Text = L"0";
			this->label_area->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// label_quality
			// 
			this->label_quality->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label_quality->Location = System::Drawing::Point(201, 213);
			this->label_quality->Name = L"label_quality";
			this->label_quality->Size = System::Drawing::Size(40, 20);
			this->label_quality->TabIndex = 15;
			this->label_quality->Text = L"0";
			this->label_quality->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label3->Location = System::Drawing::Point(123, 236);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(80, 18);
			this->label3->TabIndex = 14;
			this->label3->Text = L"Area    :";
			// 
			// btnSave
			// 
			this->btnSave->Enabled = false;
			this->btnSave->Font = (gcnew System::Drawing::Font(L"新細明體", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->btnSave->Location = System::Drawing::Point(13, 213);
			this->btnSave->Name = L"btnSave";
			this->btnSave->Size = System::Drawing::Size(100, 40);
			this->btnSave->TabIndex = 13;
			this->btnSave->Text = L"Save";
			this->btnSave->UseVisualStyleBackColor = true;
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label2->Location = System::Drawing::Point(122, 213);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(80, 18);
			this->label2->TabIndex = 13;
			this->label2->Text = L"Quality :";
			// 
			// pictureBox1
			// 
			this->pictureBox1->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->pictureBox1->Location = System::Drawing::Point(38, 27);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(180, 180);
			this->pictureBox1->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->pictureBox1->TabIndex = 10;
			this->pictureBox1->TabStop = false;
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->cboSensorType);
			this->groupBox2->Controls->Add(this->btnCancel);
			this->groupBox2->Controls->Add(this->btnVerify);
			this->groupBox2->Controls->Add(this->btnDeleteAll);
			this->groupBox2->Controls->Add(this->btnEnroll);
			this->groupBox2->Controls->Add(this->btnCapture);
			this->groupBox2->Controls->Add(this->btnOpen_device);
			this->groupBox2->Controls->Add(this->btnRealTime);
			this->groupBox2->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->groupBox2->Location = System::Drawing::Point(7, 271);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(277, 247);
			this->groupBox2->TabIndex = 11;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"Main Function";
			// 
			// cboSensorType
			// 
			this->cboSensorType->BackColor = System::Drawing::Color::White;
			this->cboSensorType->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->cboSensorType->FormattingEnabled = true;
			this->cboSensorType->ItemHeight = 20;
			this->cboSensorType->Items->AddRange(gcnew cli::array< System::Object^  >(3) { L"301B", L"301C", L"701" });
			this->cboSensorType->Location = System::Drawing::Point(7, 37);
			this->cboSensorType->Name = L"cboSensorType";
			this->cboSensorType->Size = System::Drawing::Size(129, 28);
			this->cboSensorType->TabIndex = 14;
			this->cboSensorType->SelectedIndexChanged += gcnew System::EventHandler(this, &UsbDongle_GUI::cboSensorType_SelectedIndexChanged);
			// 
			// btnCancel
			// 
			this->btnCancel->BackColor = System::Drawing::SystemColors::Control;
			this->btnCancel->Enabled = false;
			this->btnCancel->FlatAppearance->BorderColor = System::Drawing::Color::White;
			this->btnCancel->Font = (gcnew System::Drawing::Font(L"新細明體", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(136)));
			this->btnCancel->Location = System::Drawing::Point(142, 199);
			this->btnCancel->Name = L"btnCancel";
			this->btnCancel->Size = System::Drawing::Size(130, 40);
			this->btnCancel->TabIndex = 13;
			this->btnCancel->Text = L"Cancel";
			this->btnCancel->UseVisualStyleBackColor = true;
			this->btnCancel->Click += gcnew System::EventHandler(this, &UsbDongle_GUI::btnCancel_Click);
			// 
			// btnVerify
			// 
			this->btnVerify->BackColor = System::Drawing::SystemColors::Control;
			this->btnVerify->Enabled = false;
			this->btnVerify->FlatAppearance->BorderColor = System::Drawing::Color::White;
			this->btnVerify->Font = (gcnew System::Drawing::Font(L"新細明體", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(136)));
			this->btnVerify->Location = System::Drawing::Point(142, 142);
			this->btnVerify->Name = L"btnVerify";
			this->btnVerify->Size = System::Drawing::Size(130, 40);
			this->btnVerify->TabIndex = 12;
			this->btnVerify->Text = L"Verify";
			this->btnVerify->UseVisualStyleBackColor = true;
			this->btnVerify->Click += gcnew System::EventHandler(this, &UsbDongle_GUI::btnVerify_Click);
			// 
			// btnDeleteAll
			// 
			this->btnDeleteAll->BackColor = System::Drawing::SystemColors::Control;
			this->btnDeleteAll->Enabled = false;
			this->btnDeleteAll->FlatAppearance->BorderColor = System::Drawing::Color::White;
			this->btnDeleteAll->Font = (gcnew System::Drawing::Font(L"新細明體", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(136)));
			this->btnDeleteAll->Location = System::Drawing::Point(6, 199);
			this->btnDeleteAll->Name = L"btnDeleteAll";
			this->btnDeleteAll->Size = System::Drawing::Size(130, 40);
			this->btnDeleteAll->TabIndex = 11;
			this->btnDeleteAll->Text = L"Delete All";
			this->btnDeleteAll->UseVisualStyleBackColor = true;
			this->btnDeleteAll->Click += gcnew System::EventHandler(this, &UsbDongle_GUI::btnDeleteAll_Click);
			// 
			// btnEnroll
			// 
			this->btnEnroll->BackColor = System::Drawing::SystemColors::Control;
			this->btnEnroll->Enabled = false;
			this->btnEnroll->FlatAppearance->BorderColor = System::Drawing::Color::White;
			this->btnEnroll->Font = (gcnew System::Drawing::Font(L"新細明體", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(136)));
			this->btnEnroll->Location = System::Drawing::Point(6, 142);
			this->btnEnroll->Name = L"btnEnroll";
			this->btnEnroll->Size = System::Drawing::Size(130, 40);
			this->btnEnroll->TabIndex = 10;
			this->btnEnroll->Text = L"Enroll";
			this->btnEnroll->UseVisualStyleBackColor = true;
			this->btnEnroll->Click += gcnew System::EventHandler(this, &UsbDongle_GUI::btnEnroll_Click);
			// 
			// btnCapture
			// 
			this->btnCapture->BackColor = System::Drawing::SystemColors::Control;
			this->btnCapture->Enabled = false;
			this->btnCapture->FlatAppearance->BorderColor = System::Drawing::Color::White;
			this->btnCapture->Font = (gcnew System::Drawing::Font(L"新細明體", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(136)));
			this->btnCapture->Location = System::Drawing::Point(6, 85);
			this->btnCapture->Name = L"btnCapture";
			this->btnCapture->Size = System::Drawing::Size(130, 40);
			this->btnCapture->TabIndex = 9;
			this->btnCapture->Text = L"Capture";
			this->btnCapture->UseVisualStyleBackColor = true;
			this->btnCapture->Click += gcnew System::EventHandler(this, &UsbDongle_GUI::btnCapture_Click);
			// 
			// btnOpen_device
			// 
			this->btnOpen_device->BackColor = System::Drawing::SystemColors::Control;
			this->btnOpen_device->FlatAppearance->BorderColor = System::Drawing::Color::White;
			this->btnOpen_device->Font = (gcnew System::Drawing::Font(L"新細明體", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(136)));
			this->btnOpen_device->Location = System::Drawing::Point(141, 30);
			this->btnOpen_device->Name = L"btnOpen_device";
			this->btnOpen_device->Size = System::Drawing::Size(130, 40);
			this->btnOpen_device->TabIndex = 8;
			this->btnOpen_device->Text = L"Open Device";
			this->btnOpen_device->UseVisualStyleBackColor = true;
			this->btnOpen_device->Click += gcnew System::EventHandler(this, &UsbDongle_GUI::btnOpen_Device_Click);
			// 
			// btnRealTime
			// 
			this->btnRealTime->BackColor = System::Drawing::SystemColors::Control;
			this->btnRealTime->Enabled = false;
			this->btnRealTime->FlatAppearance->BorderColor = System::Drawing::Color::White;
			this->btnRealTime->Font = (gcnew System::Drawing::Font(L"新細明體", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(136)));
			this->btnRealTime->Location = System::Drawing::Point(142, 85);
			this->btnRealTime->Name = L"btnRealTime";
			this->btnRealTime->Size = System::Drawing::Size(130, 40);
			this->btnRealTime->TabIndex = 7;
			this->btnRealTime->Text = L"RealTime";
			this->btnRealTime->UseVisualStyleBackColor = true;
			this->btnRealTime->Click += gcnew System::EventHandler(this, &UsbDongle_GUI::btnRealtime_Click);
			// 
			// lb_info
			// 
			this->lb_info->BackColor = System::Drawing::Color::White;
			this->lb_info->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->lb_info->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->lb_info->Location = System::Drawing::Point(8, 9);
			this->lb_info->Name = L"lb_info";
			this->lb_info->Size = System::Drawing::Size(476, 257);
			this->lb_info->TabIndex = 0;
			this->lb_info->Text = L"Welcome to use fingerprint tool";
			this->lb_info->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// btnRead_Reg
			// 
			this->btnRead_Reg->BackColor = System::Drawing::SystemColors::Control;
			this->btnRead_Reg->FlatAppearance->BorderColor = System::Drawing::Color::White;
			this->btnRead_Reg->Font = (gcnew System::Drawing::Font(L"新細明體", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(136)));
			this->btnRead_Reg->Location = System::Drawing::Point(564, 433);
			this->btnRead_Reg->Name = L"btnRead_Reg";
			this->btnRead_Reg->Size = System::Drawing::Size(130, 40);
			this->btnRead_Reg->TabIndex = 6;
			this->btnRead_Reg->Text = L"Read Register";
			this->btnRead_Reg->UseVisualStyleBackColor = true;
			this->btnRead_Reg->Click += gcnew System::EventHandler(this, &UsbDongle_GUI::btnRead_Reg_Click);
			// 
			// btnWrite_Reg
			// 
			this->btnWrite_Reg->BackColor = System::Drawing::SystemColors::Control;
			this->btnWrite_Reg->FlatAppearance->BorderColor = System::Drawing::Color::White;
			this->btnWrite_Reg->Font = (gcnew System::Drawing::Font(L"新細明體", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(136)));
			this->btnWrite_Reg->Location = System::Drawing::Point(564, 479);
			this->btnWrite_Reg->Name = L"btnWrite_Reg";
			this->btnWrite_Reg->Size = System::Drawing::Size(130, 40);
			this->btnWrite_Reg->TabIndex = 5;
			this->btnWrite_Reg->Text = L"Write Register";
			this->btnWrite_Reg->UseVisualStyleBackColor = true;
			this->btnWrite_Reg->Click += gcnew System::EventHandler(this, &UsbDongle_GUI::btnWrite_Reg_Click);
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->cboDevices);
			this->groupBox1->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 11.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->groupBox1->Location = System::Drawing::Point(8, 3);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(752, 58);
			this->groupBox1->TabIndex = 2;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Usb Device";
			// 
			// cboDevices
			// 
			this->cboDevices->Enabled = false;
			this->cboDevices->Font = (gcnew System::Drawing::Font(L"Source Code Pro Medium", 11, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->cboDevices->FormattingEnabled = true;
			this->cboDevices->Location = System::Drawing::Point(7, 23);
			this->cboDevices->Name = L"cboDevices";
			this->cboDevices->Size = System::Drawing::Size(739, 27);
			this->cboDevices->TabIndex = 0;
			// 
			// realtime_timer
			// 
			this->realtime_timer->Tick += gcnew System::EventHandler(this, &UsbDongle_GUI::realtime_timer_Tick);
			// 
			// capture_timer
			// 
			this->capture_timer->Tick += gcnew System::EventHandler(this, &UsbDongle_GUI::capture_timer_Tick);
			// 
			// enroll_timer
			// 
			this->enroll_timer->Tick += gcnew System::EventHandler(this, &UsbDongle_GUI::enroll_timer_Tick);
			// 
			// verify_timer
			// 
			this->verify_timer->Tick += gcnew System::EventHandler(this, &UsbDongle_GUI::verify_timer_Tick);
			// 
			// textBox2
			// 
			this->textBox2->Location = System::Drawing::Point(818, 39);
			this->textBox2->Name = L"textBox2";
			this->textBox2->Size = System::Drawing::Size(100, 22);
			this->textBox2->TabIndex = 13;
			// 
			// UsbDongle_GUI
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(767, 621);
			this->Controls->Add(this->textBox2);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->tabControl1);
			this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
			this->Name = L"UsbDongle_GUI";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"UsbDongle_PB";
			this->Load += gcnew System::EventHandler(this, &UsbDongle_GUI::UsbDongle_GUI_Load);
			this->tabControl1->ResumeLayout(false);
			this->tabPage1->ResumeLayout(false);
			this->tabPage2->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pb_gif))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pb_png))->EndInit();
			this->groupBox6->ResumeLayout(false);
			this->groupBox6->PerformLayout();
			this->groupBox4->ResumeLayout(false);
			this->groupBox4->PerformLayout();
			this->groupBox3->ResumeLayout(false);
			this->groupBox3->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->EndInit();
			this->groupBox2->ResumeLayout(false);
			this->groupBox1->ResumeLayout(false);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

	private: System::Void UsbDongle_GUI_Load(System::Object^  sender, System::EventArgs^  e) {
				 int r;

				 r = libusb_init(NULL);
				 if (r < 0)
				 	 printf("failed to initialise libusb\n");

				 findDevice("06CB", "0040");
	}

	private: System::Void cboSensorType_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {

				 if (cboSensorType->SelectedIndex == 0) {
					 sensorType = "301B";
					 sensor_width = 128;
					 sensor_height = 128;
					 printf("width = %d\n", sensor_width);
					 printf("height = %d\n", sensor_height);
				 }
				 else if (cboSensorType->SelectedIndex == 1) {
					 sensorType = "301C";
					 sensor_width = 120;
					 sensor_height = 120;
					 printf("width = %d\n", sensor_width);
					 printf("height = %d\n", sensor_height);
				 }
				 else if (cboSensorType->SelectedIndex == 2) {
					 sensorType = "701";
				 }
	}

	private: System::Void btnOpen_Device_Click(System::Object^  sender, System::EventArgs^  e) {

				 int res = -1;
	
				 res = libusb_claim_interface(dev_handle, 0);
				 if (res < 0) {
					 printf("Cannot Clain Interface\n");
				 }
				 else {
					 printf("Claim interface success. (%d)\n", res);

					 res = usb_sensor_reset(dev_handle);
					 if (res < 0) {
						 printf("control transfer OUT error %d\n", res);
					 }
					 else {

						 lb_info->Text = "Open Device Success.";

						 set_adjust_parameter(sensor_width, sensor_height);
						 printf("width = %d\n", sensor_width);
						 printf("height = %d\n", sensor_height);

						 setSensorType(128, 128);

						 control_item_enable(true);
						 btnOpen_device->Enabled = false;
						 btnSave->Enabled = false;

						 tb_enroll_name->Text = "Your Name";
						 tb_enroll_count->Text = Convert::ToString(set_enrollment_samples);
						 tb_enroll_image_quality->Text = Convert::ToString(enrollment_image_quality);
						 tb_enroll_fingerprint_area->Text = Convert::ToString(enrollment_fingerprint_area);
						 tb_enroll_return_coverage->Text = "0";

						 lb_enroll_count->Text = "(0-" + max_enrollment_samples + ")";
						 lb_enroll_image_quality->Text = "(0-100)";
						 lb_enroll_fingerprint_area->Text = "(0-" + max_fingerprint_area + ")";
						 lb_enroll_return_coverage->Text = "(0-100)";

						 tb_verify_image_quality->Text = Convert::ToString(verify_image_quality);
						 tb_verify_fingerprint_area->Text = Convert::ToString(verify_fingerprint_area);
						 lb_verify_image_quality->Text = "(0-100)";
						 lb_verify_fingerprint_area->Text = "(0-" + max_fingerprint_area + ")";

						 template_type_path = template_path + sensorType;

						 if (!System::IO::Directory::Exists(template_type_path))
						 {
							 System::IO::Directory::CreateDirectory(template_type_path);
						 }
					 }
				 }			 
	}

	private: System::Void btnCapture_Click(System::Object^  sender, System::EventArgs^  e) {
				 printf("btnCapture_Click...\n");

				 int res;
				 uint16_t frameSize;

				 textBox2->Focus();

				 lb_info->ForeColor = Color::Black;
				 pb_gif->Visible = false;

				 if (btnCapture->Text == "Capture") {

					 res = usb_sensor_init(dev_handle);
					 if (res < 0) {
						 printf("control transfer OUT error %d\n", res);
					 }
					 else {
						 //printf("control transfer OUT ok, cmd (0x%.02X)\n", FPD_CMD_WRITE_REG);
						 printf("Init sensor success.\n");
					 }

					 res = usb_sensor_start_stream(dev_handle);
					 if (res < 0) {
						 printf("control transfer OUT error %d\n", res);
					 }
					 else {
						 printf("control transfer OUT ok, cmd(0x%.02X)\n", FPD_CMD_START_STREAM);

						 printf("Press finger to capture fingerprint on device\n");
						 lb_info->TextAlign = ContentAlignment::MiddleCenter;
						 lb_info->Text = "Place finger to capture fingerprint \n\non device.";
						 lb_info->Refresh();

						 lb_message->Visible = false;

						 control_item_enable(false);
						 btnCapture->Text = "Capturing";
						 btnCancel->Enabled = true;

						 quality_chk_init();

						 start_capture = true;
						 get_frame = true;
						 pictureBox1->Image = nullptr;

						 memStream = gcnew MemoryStream();

						 capture_timer->Interval = 33;
						 capture_timer->Start();
					 }
				 }
	}

	private: System::Void capture_timer_Tick(System::Object^  sender, System::EventArgs^  e) {
				 pb_image_t* image = 0;
				 uint8_t image_quality = 0;
				 uint16_t fingerprint_area = 0;

				 int res;
				 uint16_t frameSize;

				 unsigned char data[PKT_SIZE];
				 unsigned char convertData[4162];
				 unsigned char new_data[20000];
				 int transferred;

				 unsigned char *image_data;
				 uint16_t data_len;

				 int tag = 0;

				 bool full_image;

				 res = libusb_bulk_transfer(dev_handle, 0x81, data, PKT_SIZE, &transferred, 0);
				 printf("GUI -> data length = %d\n", data_len);				 
				 
				 if (res < 0) {
					 printf("bulk IN error(%d)\n", res);
				 }
				 else {
					 
					 if (transferred > 0) {
						 if ((data[0] & 0x02) != 0x02) {
							 if ((data[0] & 0x01) != toggle) {

								 toggle = (data[0] & 0x01);
								 pixel_cnt = 0;
								 frame_cnt = 0;

								 array<Byte>^ r = memStream->ToArray();
								 Console::WriteLine("MemoryStream Length = " + r->Length);								 

								 if (r->Length == 16384) {
										 Marshal::Copy(r, 0, (IntPtr)new_data, r->Length);

										 image = pb_image_create_mr(128, 128, 508, 508, new_data, PB_IMPRESSION_TYPE_LIVE_SCAN_PLAIN);
										 if (!image) {
											 printf("ERROR: pb_image_create_mr() failed.\n");
										 }

										 quality_chk(image, &image_quality, &fingerprint_area);

										 if (image_quality > 50 && fingerprint_area > 24) {

											 label_quality->Text = "" + image_quality;
											 label_area->Text = "" + fingerprint_area;

											 pictureBox1->Image = ToGrayBitmap(r, 128, 128);

											 pb_gif->Visible = true;
											 pb_png->Visible = true;
											 pb_png->Parent = pb_gif;
											 pb_png->Location = System::Drawing::Point(0, 0);
											 pb_png->Image = enrollProgressImage[enrollProgressImage->Length - 1];

											 res = usb_sensor_stop_stream(dev_handle);
											 if (res < 0) {
												 printf("control transfer OUT error %d\n", r);
											 }
											 else {
												 printf("control transfer OUT ok, cmd(%.02x)\n", FPD_CMD_STOP_STREAM);
											 }

											 capture_timer->Stop();

											 lb_message->Visible = true;
											 lb_message->Text = "Capture Successfully";

											 lb_info->TextAlign = ContentAlignment::BottomCenter;
											 lb_info->Text = "You can click the save button to\n";
											 lb_info->Text += "save the fingerprint image.\n";
											 control_item_enable(true);
											 btnOpen_device->Enabled = false;
											 btnCapture->Text = "Capture";
											 start_capture = false;
											 quality_chk_deinit();											 
										 }										 
								 }
								 memStream = gcnew MemoryStream();					 
							 }

							 for (int y = 0; y < transferred; y++) {
								 if (y % 64 != 0) {
									 convertData[y - tag] = data[y];
								 }
								 else {
									 tag++;
								 }
							 }

							 for (int i = 0; i < transferred - tag; i++) {
								 convertData[i] = 255 - convertData[i];
							 }

							 byteArray = gcnew array<Byte>(transferred - tag);
							 Marshal::Copy((IntPtr)convertData, byteArray, 0, transferred - tag);

							 memStream->Write(byteArray, 0, transferred - tag);
						 }

						 pixel_cnt += data_len;
						 ++frame_cnt;
						 loopcnt++;
					 }					 
				 }				 
	}

	private: System::Void btnRealtime_Click(System::Object^  sender, System::EventArgs^  e) {
			
				 int res;

				 textBox2->Focus();

				 lb_info->ForeColor = Color::Black;
				 pb_gif->Visible = false;
				 lb_message->Visible = false;

				 if (btnRealTime->Text == "RealTime") {

					 btnRealTime->Enabled = true;
					 btnRealTime->Text = "Stop";

					 res = usb_sensor_init(dev_handle);
					 if (res < 0) {
						 printf("control transfer OUT error %d\n", res);
					 }
					 else {
						 //printf("control transfer OUT ok, cmd (0x%.02X)\n", FPD_CMD_WRITE_REG);
						 printf("Init sensor success.\n");
					 }
					
					 res = usb_sensor_start_stream(dev_handle);
					 if (res < 0) {
						 printf("control transfer OUT error %d\n", res);
					 }
					 else {
						 //printf("control transfer OUT ok, cmd(0x%.02X)\n", FPD_CMD_START_STREAM);

						 printf("Press finger to show real-time image on device\n");
						 lb_info->TextAlign = ContentAlignment::MiddleCenter;
						 lb_info->Text = "Press finger to show real-time\n\n image on device.";
						 lb_info->Refresh();

						 //lb_open_message_visible(false);
						 //lb_enroll_message_visible(false);

						 control_item_enable(false);
						 btnRealTime->Enabled = true;
						 btnRealTime->Text = "Stop";

						 quality_chk_init();

						 get_frame = true;
						 pictureBox1->Image = nullptr;

						 realtime_timer->Interval = 33;
						 realtime_timer->Start();

						 start_realtime = true;

						 pixel_cnt = 0;
						 loopcnt = 0;

						 memStream = gcnew MemoryStream();
					 }

					 //start = clock();			 
				 }
				 else {

					 res = usb_sensor_stop_stream(dev_handle);
					 if (res < 0) {
						 printf("control transfer OUT error %d\n", res);
					 }
					 else {
						 //printf("control transfer OUT ok, cmd(%.02x)\n", FPD_CMD_STOP_STREAM);

						 realtime_timer->Stop();

						 lb_info->TextAlign = ContentAlignment::MiddleCenter;
						 //label1->Text = "Welcome to use fingerprint tool";
						 lb_info->Text = "You can click the save button to\n\n";
						 lb_info->Text += "save the fingerprint image.";

						 control_item_enable(true);
						 btnOpen_device->Enabled = false;
						 btnRealTime->Text = "RealTime";

						 start_realtime = false;

						 printf("test_tag false\n");
					 }
				 }
	}

	private: System::Void realtime_timer_Tick(System::Object^  sender, System::EventArgs^  e) {
				 int r;

				 pb_image_t* image = 0;
				 uint8_t image_quality = 0;
				 uint16_t fingerprint_area = 0;

				 unsigned char data[PKT_SIZE];
				 unsigned char convertData[20000];
				 unsigned char new_data[20000];
				 int transferred;

				 int tag = 0;

				 r = libusb_bulk_transfer(dev_handle, 0x81, data, PKT_SIZE, &transferred, 0);
				 
				 if (r < 0) {
					 printf("bulk IN error(%d)\n", r);
				 } else {					 
					 if (transferred > 0) {						 
						 if ((data[0] & 0x02) != 0x02) {												 
							 if ((data[0] & 0x01) != toggle) {									
								 //unsigned int stop = clock();
								 //Console::WriteLine(stop - start + "ms");

								 toggle = (data[0] & 0x01);
								 pixel_cnt = 0;
								 frame_cnt = 0;

								 array<Byte>^ r = memStream->ToArray();
								 //Console::WriteLine("MemoryStream Length = " + r->Length);

								 if (r->Length == 16384) {	

										 Marshal::Copy(r, 0, (IntPtr)new_data, r->Length);

										 image = pb_image_create_mr(128, 128, 508, 508, new_data, PB_IMPRESSION_TYPE_LIVE_SCAN_PLAIN);
										 if (!image) {
											 printf("ERROR: pb_image_create_mr() failed.\n");
										 }

										 quality_chk(image, &image_quality, &fingerprint_area);

										 label_quality->Text = "" + image_quality;
										 label_area->Text = "" + fingerprint_area;

										 pictureBox1->Image = ToGrayBitmap(r, 128, 128);
								 }			
								 memStream = gcnew MemoryStream();
								 
								 //start = clock();

								 //Console::WriteLine(" +++++++++++++++++ End +++++++++++++++++");								 
							 }				

							 for (int y = 0; y < transferred; y++) {
								 if (y % 64 != 0) {
									 convertData[y - tag] = data[y];
								 }
								 else {
									 tag++;
								 }
							 }

							 for (int i = 0; i < transferred - tag; i++) {
								 convertData[i] = 255 - convertData[i];
							 }

							 byteArray = gcnew array<Byte>(transferred - tag);
							 Marshal::Copy((IntPtr)convertData, byteArray, 0, transferred - tag);							 
						
							 memStream->Write(byteArray, 0, transferred - tag);
						 }

						 pixel_cnt += transferred;
						 ++frame_cnt;
						 loopcnt++;									
					 }

					 //printf("bulk in ok (%d)\n", transferred);
					 //printf("\n");
				 }
	}

	private: System::Void btnEnroll_Click(System::Object^  sender, System::EventArgs^  e) {
				 printf("btnEnroll_Click...\n\n");

				 pb_gif->Visible = false;

				 textBox2->Focus();

				 int res;

				 label_quality->Text = "" + 0;
				 label_area->Text = "" + 0;

				 if (cb_enroll_preview->CheckState == CheckState::Checked) {
					 enroll_preview = true;
					 pictureBox1->Image = nullptr;
				 }
				 else {
					 enroll_preview = false;
					 pictureBox1->Image = logoImage;
				 }

				 if (cb_enroll_save_image->CheckState == CheckState::Checked) {
					 enroll_save_image = true;
					
					 image_type_path = image_path + sensorType;
					 if (!System::IO::Directory::Exists(image_type_path))
					 {
						 System::IO::Directory::CreateDirectory(image_type_path);
					 }

					 cb_enroll_preview->CheckState = CheckState::Checked;
					 enroll_preview = true;
					 pictureBox1->Image = nullptr;
				 }
				 else {
					 enroll_save_image = false;
				 }

				 if (btnEnroll->Text == "Enroll") {
				     
					 res = usb_sensor_init(dev_handle);
					 if (res < 0) {
						 printf("control transfer OUT error %d\n", res);
					 }
					 else {
						 //printf("control transfer OUT ok, cmd (0x%.02X)\n", FPD_CMD_WRITE_REG);
						 printf("Init sensor success.\n");
					 }

					 res = usb_sensor_start_stream(dev_handle);
					 if (res < 0) {
						 printf("control transfer OUT error %d\n", res);
					 }
					 else {

						 lb_message->Visible = true;
						 lb_message->Text = "Enrolling ....";

						 printf("Place finger to enroll fingerprint on device.\n");

						 lb_info->ForeColor = Color::Black;
						 lb_info->TextAlign = ContentAlignment::BottomCenter;
						 lb_info->Text = "Place finger on device, lift it off, \n then repeat.\n";

						 if (tb_enroll_count->Text == "" || Convert::ToInt32(tb_enroll_count->Text) > max_enrollment_samples) {
							 tb_enroll_count->Text = Convert::ToString(set_enrollment_samples);
						 }

						 if (tb_enroll_image_quality->Text == "" || Convert::ToInt32(tb_enroll_image_quality->Text) > 100) {
							 tb_enroll_image_quality->Text = Convert::ToString(enrollment_image_quality);
						 }

						 if (tb_enroll_fingerprint_area->Text == "" || Convert::ToInt32(tb_enroll_fingerprint_area->Text) > max_fingerprint_area) {
							 tb_enroll_fingerprint_area->Text = Convert::ToString(enrollment_fingerprint_area);
						 }

						 if (tb_enroll_return_coverage->Text == "" || Convert::ToInt32(tb_enroll_return_coverage->Text) > 100) {
							 tb_enroll_return_coverage->Text = "0";
						 }
						 else if (Convert::ToInt32(tb_enroll_return_coverage->Text) > 0){
							 tb_enroll_count->Text = "32";
						 }

						 control_item_enable(false);
						 btnEnroll->Text = "Enrolling..";
						 btnCancel->Enabled = true;
						 
						 lib_init();

						 set_enroll_count(Convert::ToInt32(tb_enroll_count->Text));

						 max_samples = get_enroll_count();

						 IntPtr convertString = Marshal::StringToHGlobalAnsi(template_type_path);
						 char* nativeChar = static_cast<char*>(convertString.ToPointer());

						 if (tb_enroll_name->Text == "" || tb_enroll_name->Text == "Your Name") {
							 tb_enroll_name->Text = "Your Name";
							 filename = "Finger";

							 set_archive_path(nativeChar, filename);
							 finger_idx = enroll_setup();
						 }
						 else {
							 filename = (char*)(void*)Marshal::StringToHGlobalAnsi(tb_enroll_name->Text);
							 set_archive_path(nativeChar, filename);
							 finger_idx = enroll_setup();
						 }
						 sprintf(finger_idx_buf, "%02d", finger_idx);

						 quality_chk_flag = true;
						 start_enroll = true;
						 num_accepted_prev = 0;
						 num_accepted = 0;

						 enroll_timer->Interval = 33;
						 enroll_timer->Start();

						 printf("ENROLLMENT - starting capturing and enrollment\n");
						 printf("----------------------------------------------\n");

						 strcpy(file_path_string, app_path);
						 strcat(file_path_string, "\\res");
						 strcat(file_path_string, "\\fingerprint_enroll_scan.gif");
						 sprintf(fingerprint_enrollScan, file_path_string);
						 enrollScan = gcnew Bitmap(Marshal::PtrToStringAnsi((IntPtr)fingerprint_enrollScan));
						 pb_gif->Visible = true;
						 pb_gif->Image = enrollScan;

						 strcpy(file_path_string, app_path);
						 strcat(file_path_string, "\\res");
						 strcat(file_path_string, "\\enroll");
						 strcat(file_path_string, "\\fingerprint_enroll_progress-00.png");
						 sprintf(fingerprint_enrollProgress, file_path_string);
						 enrollProgress = gcnew Bitmap(Marshal::PtrToStringAnsi((IntPtr)fingerprint_enrollProgress));
						 pb_png->Visible = true;
						 pb_png->Parent = pb_gif;
						 pb_png->Location = System::Drawing::Point(0, 0);
						 pb_png->Image = enrollProgress;

						 memStream = gcnew MemoryStream();						 
					 }
				 }
	}

	private: System::Void enroll_timer_Tick(System::Object^  sender, System::EventArgs^  e) {
				 pb_image_t* image = 0;
				 uint8_t image_quality = 0;
				 uint16_t fingerprint_area = 0;

				 uint8_t coverage_area = 0;
				 int template_size = 0;
				 int file_index = -1;

				 int res;
				 uint16_t frameSize;

				 unsigned char data[PKT_SIZE];
				 unsigned char convertData[20000];
				 unsigned char new_data[20000];
				 int transferred;

				 int tag = 0;

				 res = libusb_bulk_transfer(dev_handle, 0x81, data, PKT_SIZE, &transferred, 0);

				 if (res < 0) {
					 printf("bulk IN error(%d)\n", res);
				 }
				 else {
					 if (transferred > 0) {
						 if ((data[0] & 0x02) != 0x02) {
							 if ((data[0] & 0x01) != toggle) {								

								 toggle = (data[0] & 0x01);
								 pixel_cnt = 0;
								 frame_cnt = 0;

								 array<Byte>^ r = memStream->ToArray();
								 Console::WriteLine("MemoryStream Length = " + r->Length);

								 if (r->Length == 16384) {

										 Marshal::Copy(r, 0, (IntPtr)new_data, r->Length);

										 image = pb_image_create_mr(128, 128, 508, 508, new_data, PB_IMPRESSION_TYPE_LIVE_SCAN_PLAIN);
										 if (!image) {
											 printf("ERROR: pb_image_create_mr() failed.\n");
										 }

										 quality_chk(image, &image_quality, &fingerprint_area);

										 if (quality_chk_flag) {
											 if (image_quality > Convert::ToInt32(tb_enroll_image_quality->Text) &&
												 fingerprint_area > Convert::ToInt32(tb_enroll_fingerprint_area->Text)) {

												 label_quality->Text = "" + image_quality;
												 label_area->Text = "" + fingerprint_area;

												 num_accepted = enroll_finger(image, &coverage_area);

												 printf("  PBGUI : num_accepted = %d\n", num_accepted);
												 printf("  PBGUI : coverage_area => %d %%\n", coverage_area);

												 double count_percent = ((double)num_accepted / (double)max_samples) * 100;
												 double coverage_percent = ((double)coverage_area / (double)Convert::ToInt32(tb_enroll_return_coverage->Text)) * 100;
												 double count_progressRatio = (double)(enrollProgressImage->Length - 2) / (double)max_samples;
												 double count_progressIndex = (double)count_progressRatio * num_accepted;
												 double coverage_progressRatio = (double)enrollProgressImage->Length / ((double)Convert::ToInt32(tb_enroll_return_coverage->Text) / 10);
												 double coverage_progressIndex = (double)coverage_progressRatio * (double)(coverage_area / 10);
												 printf("coverage_progressIndex : %f\n", (double)coverage_progressIndex);

												 if (Convert::ToInt32(tb_enroll_return_coverage->Text) == 0) {
													 if (num_accepted > num_accepted_prev) {
														 num_accepted_prev = num_accepted;

														 //label1->Text += (int)round(count_percent) + "%\n";
														 //label1->Text += "\n";
														 lb_info->Text = "Place finger on device, lift it off, \n then repeat.\n";

														 if ((int)round(count_progressIndex) >= enrollProgressImage->Length) {
															 pb_png->Image = enrollProgressImage[enrollProgressImage->Length - 2];
														 }
														 else {
															 pb_png->Image = enrollProgressImage[(int)round(count_progressIndex)];
														 }

													 }
													 else {
														 lb_info->Text = "Move finger more !\n";
														 //label1->Text += (int)round(count_percent) + "%";
														 //label1->Text += "\n";
													 }
												 }
												 else {
													 if (num_accepted > num_accepted_prev) {
														 num_accepted_prev = num_accepted;
														 if (coverage_percent > 100) {
															 coverage_percent = 100;
														 }

														 //label1->Text += (int)round(coverage_percent) + "%\n";
														 //label1->Text += "\n";

														 if ((int)round(coverage_progressIndex) >= enrollProgressImage->Length) {
															 printf("enroll progress index = %d", (int)round(coverage_progressIndex));
															 pb_png->Image = enrollProgressImage[enrollProgressImage->Length - 2];
														 }
														 else {
															 printf("enroll progress index = %d", (int)round(coverage_progressIndex));
															 pb_png->Image = enrollProgressImage[(int)round(coverage_progressIndex)];
														 }

													 }
													 else {
														 lb_info->Text = "Move finger more !\n";
														 //label1->Text += (int)round(coverage_percent) + "%";
														 //label1->Text += "\n";
													 }
												 }

												 if (enroll_preview) {
													 pictureBox1->Image = ToGrayBitmap(r, 128, 128);
													 
													 if (enroll_save_image) {
														 char imagePath[200];
														 char string[200];

														 strcpy(string, (char*)(void*)Marshal::StringToHGlobalAnsi(image_type_path));
														 strcat(string, "\\");
														 strcat(string, date_buf);
														 strcat(string, "_");
														 strcat(string, filename);
														 strcat(string, "_");
														 strcat(string, finger_idx_buf);
														 strcat(string, "_%d.png");
														 sprintf(imagePath, string, num_accepted);
														 pictureBox1->Image->Save(Marshal::PtrToStringAnsi((IntPtr)imagePath), ImageFormat::Png);
													 }
													 
												 }
												 quality_chk_flag = false;
											 }
										 }
	
									 if (fingerprint_area == 0) {
										 quality_chk_flag = true;
									 }

									 if (Convert::ToInt32(tb_enroll_return_coverage->Text) == 0) {
										 if (num_accepted == max_samples) {

											 res = usb_sensor_stop_stream(dev_handle);
											 if (res < 0) {
												 printf("control transfer OUT error %d\n", r);
											 }
											 else {
												 printf("control transfer OUT ok, cmd(%.02x)\n", FPD_CMD_STOP_STREAM);
											 }

											 enroll_timer->Stop();

											 enroll_finish(&template_size);

											 lib_deinit();
						
											 pb_png->Image = enrollProgressImage[enrollProgressImage->Length - 1];

											 lb_message->Visible = true;
											 lb_message->Text = "Enroll Successfully\n";

											 //label1->TextAlign = ContentAlignment::MiddleLeft;
											 //label1->Text = " Template information \n";
											 lb_info->Text = "Template Name : " + Marshal::PtrToStringAnsi((IntPtr)get_output_file_name()) + "\n";

											 //lb_enroll_message_visible(true);								
											 //lb_enroll_parameter->Text = Marshal::PtrToStringAnsi((IntPtr)get_output_file_name()) + "\n";
											 //lb_enroll_parameter->Text += num_accepted + "\n";
											 //lb_enroll_parameter->Text += template_size + " bytes\n";
											 //lb_enroll_parameter->Text += coverage_area + "%";

											 control_item_enable(true);
											 btnOpen_device->Enabled = false;
											 btnSave->Enabled = false;
											 btnEnroll->Text = "Enroll";

											 get_frame = false;
											 start_enroll = false;
										 }
									 }
									 else {
										 if (coverage_area >= Convert::ToInt32(tb_enroll_return_coverage->Text)) {

											 res = usb_sensor_stop_stream(dev_handle);
											 if (res < 0) {
												 printf("control transfer OUT error %d\n", r);
											 }
											 else {
												 printf("control transfer OUT ok, cmd(%.02x)\n", FPD_CMD_STOP_STREAM);
											 }

											 enroll_timer->Stop();

											 enroll_finish(&template_size);

											 lib_deinit();

											 //pb_png->Image = enrollProgressImage[enrollProgressImage->Length - 1];

											 lb_message->Visible = true;
											 lb_message->Text = "Enroll Successfully\n";

											 //label1->TextAlign = ContentAlignment::MiddleLeft;
											 //label1->Text = " Template information \n";
											 lb_info->Text = "Template Name : " + Marshal::PtrToStringAnsi((IntPtr)get_output_file_name()) + "\n";

											 //lb_enroll_message_visible(true);								 								
											 //lb_enroll_parameter->Text = Marshal::PtrToStringAnsi((IntPtr)get_output_file_name()) + "\n";
											 //lb_enroll_parameter->Text += num_accepted + "\n";
											 //lb_enroll_parameter->Text += template_size + " bytes\n";
											 //lb_enroll_parameter->Text += coverage_area + "%";

											 control_item_enable(true);
											 btnOpen_device->Enabled = false;
											 btnSave->Enabled = false;
											 btnEnroll->Text = "Enroll";

											 get_frame = false;
											 start_enroll = false;
										 }
									 }
								 }
								 memStream = gcnew MemoryStream();
							 }

							 for (int y = 0; y < transferred; y++) {
								 if (y % 64 != 0) {
									 convertData[y - tag] = data[y];
								 }
								 else {
									 tag++;
								 }
							 }

							 for (int i = 0; i < transferred - tag; i++) {
								 convertData[i] = 255 - convertData[i];
							 }

							 byteArray = gcnew array<Byte>(transferred - tag);
							 Marshal::Copy((IntPtr)convertData, byteArray, 0, transferred - tag);

							 memStream->Write(byteArray, 0, transferred - tag);
						 }

						 pixel_cnt += transferred;
						 ++frame_cnt;
						 loopcnt++;
					 }

					 printf("bulk in ok (%d)\n", transferred);
					 printf("\n");
				 }
	}

	private: System::Void btnVerify_Click(System::Object^  sender, System::EventArgs^  e) {
				 printf("btnVerify_Click...\n\n");

				 int res;
				 uint16_t frameSize;

				 pb_gif->Visible = false;

				 textBox2->Focus();

				 label_quality->Text = "  " + 0;
				 label_area->Text = "  " + 0;

				 if (cb_verify_preview->CheckState == CheckState::Checked) {
					 verify_preview = true;
					 pictureBox1->Image = nullptr;
				 }
				 else {
					 verify_preview = false;
					 pictureBox1->Image = logoImage;
				 }

				 if (cb_verify_con->CheckState == CheckState::Checked) {
					 verify_con = true;
				 }
				 else {
					 verify_con = false;
				 }

				 if (btnVerify->Text == "Verify") {
					 fileList = list_files();
					 if (fileList->Length <= 0) {
						 //lb_open_message_visible(false);
						 //lb_enroll_message_visible(false);
						 lb_message->Visible = false;

						 lb_info->TextAlign = ContentAlignment::MiddleCenter;
						 lb_info->Text = "The database no templates to verify.";
						 lb_info->ForeColor = Color::Red;
					 }
					 else {

						 res = usb_sensor_init(dev_handle);
						 if (res < 0) {
							 printf("control transfer OUT error %d\n", res);
						 }
						 else {
							 //printf("control transfer OUT ok, cmd (0x%.02X)\n", FPD_CMD_WRITE_REG);
							 printf("Init sensor success.\n");
						 }

						 res = usb_sensor_start_stream(dev_handle);
						 if (res < 0) {
							 printf("control transfer OUT error %d\n", res);
						 }
						 else {
							 printf("Place finger to verify template on device.\n");

							 lb_message->Visible = true;
							 lb_message->Text = "Verifying ....";

							 //lb_open_message_visible(false);
							 //lb_enroll_message_visible(false);

							 control_item_enable(false);
							 btnVerify->Text = "Verifying..";
							 btnCancel->Enabled = true;

							 lb_info->TextAlign = ContentAlignment::BottomCenter;
							 lb_info->Text = "Place your fingertip on the device \nto verify your identity.\n";
							 lb_info->Refresh();

							 if (tb_verify_image_quality->Text == "" || Convert::ToInt32(tb_verify_image_quality->Text) > 100) {
								 //if ((int)dWidth == 120) {
									 tb_verify_image_quality->Text = Convert::ToString(verify_image_quality);
								 //}
							 }

							 if (tb_verify_fingerprint_area->Text == "" || Convert::ToInt32(tb_verify_fingerprint_area->Text) > max_fingerprint_area_301c) {
								 tb_verify_fingerprint_area->Text = Convert::ToString(verify_fingerprint_area);
							 }

							 lib_init();

							 IntPtr convertString = Marshal::StringToHGlobalAnsi(template_type_path);
							 char* nativeChar = static_cast<char*>(convertString.ToPointer());

							 set_archive_path(nativeChar, 0);

							 verify_setup();

							 quality_chk_flag = true;
							 get_frame = true;

							 verify_timer->Interval = 33;
							 verify_timer->Start();

							 verify_fail = 0;

							 start_verify = true;

							 printf("verify - starting capturing and verify\n");
							 printf("----------------------------------------------\n");

							 pb_gif->Visible = true;
							 strcpy(file_path_string, app_path);
							 strcat(file_path_string, "\\res");
							 strcat(file_path_string, "\\fingerprint_verify_scan.gif");
							 sprintf(fingerprint_verifyScan, file_path_string);
							 verifyScan = gcnew Bitmap(Marshal::PtrToStringAnsi((IntPtr)fingerprint_verifyScan));
							 pb_gif->Image = verifyScan;

							 pb_png->Visible = true;
							 pb_png->Parent = pb_gif;
							 pb_png->Location = System::Drawing::Point(0, 0);
							 strcpy(file_path_string, app_path);
							 strcat(file_path_string, "\\res");
							 strcat(file_path_string, "\\verify");
							 strcat(file_path_string, "\\fingerprint_verify_ok.png");
							 sprintf(fingerprint_verifySuccess, file_path_string);
							 verifySuccess = gcnew Bitmap(Marshal::PtrToStringAnsi((IntPtr)fingerprint_verifySuccess));
							 pb_png->Image = nullptr;

							 strcpy(file_path_string, app_path);
							 strcat(file_path_string, "\\res");
							 strcat(file_path_string, "\\verify");
							 strcat(file_path_string, "\\fingerprint_verify_fail.png");
							 sprintf(fingerprint_verifyFail, file_path_string);
							 verifyFail = gcnew Bitmap(Marshal::PtrToStringAnsi((IntPtr)fingerprint_verifyFail));

							 memStream = gcnew MemoryStream();
						 }
					 }
				 }
	}

	private: System::Void verify_timer_Tick(System::Object^  sender, System::EventArgs^  e) {

				 int cover = 0;
				 int res = -2;

				 pb_image_t*	image = 0;

				 uint8_t image_quality = 0;
				 uint16_t fingerprint_area = 0;
				 int matching_index = -1;

				 int usb_res;
				 uint16_t frameSize;

				 unsigned char data[PKT_SIZE];
				 unsigned char convertData[20000];
				 unsigned char new_data[20000];
				 int transferred;

				 int tag = 0;

				 usb_res = libusb_bulk_transfer(dev_handle, 0x81, data, PKT_SIZE, &transferred, 0);

				 if (usb_res < 0) {
					 printf("bulk IN error(%d)\n", res);
				 }
				 else {
					 if (transferred > 0) {
						 if ((data[0] & 0x02) != 0x02) {
							 if ((data[0] & 0x01) != toggle) {
								
								 toggle = (data[0] & 0x01);
								 pixel_cnt = 0;
								 frame_cnt = 0;

								 array<Byte>^ r = memStream->ToArray();
								 //Console::WriteLine("MemoryStream Length = " + r->Length);

								 if (r->Length == 16384) {
								
									 	 Marshal::Copy(r, 0, (IntPtr)new_data, r->Length);

										 image = pb_image_create_mr(128, 128, 508, 508, new_data, PB_IMPRESSION_TYPE_LIVE_SCAN_PLAIN);
										 if (!image) {
											 printf("ERROR: pb_image_create_mr() failed.\n");
										 }

										 quality_chk(image, &image_quality, &fingerprint_area);

										 if (quality_chk_flag) {
											 if (image_quality > Convert::ToInt32(tb_enroll_image_quality->Text) &&
												 fingerprint_area > Convert::ToInt32(tb_enroll_fingerprint_area->Text)) {

												 label_quality->Text = "" + image_quality;
												 label_area->Text = "" + fingerprint_area;

												 res = verify_finger(image);

												 if (verify_preview) {
													 pictureBox1->Image = ToGrayBitmap(r, 128, 128);
													 /*
													 if (enroll_save_image) {
													 char imagePath[200];
													 char string[200];

													 strcpy(string, (char*)(void*)Marshal::StringToHGlobalAnsi(image_type_path));
													 strcat(string, "\\");
													 strcat(string, date_buf);
													 strcat(string, "_");
													 strcat(string, filename);
													 strcat(string, "_");
													 strcat(string, finger_idx_buf);
													 strcat(string, "_%d.png");
													 sprintf(imagePath, string, num_accepted);
													 b->Save(Marshal::PtrToStringAnsi((IntPtr)imagePath), ImageFormat::Png);
													 }
													 */
												 }
												 quality_chk_flag = false;
											 }
										 }

										 if (fingerprint_area == 0) {
											 quality_chk_flag = true;
											 pb_gif->Image = enrollProgressImage[enrollProgressImage->Length - 1];
											 pb_png->Image = nullptr;
										 }

										 if (verify_con) {
											 if (res == -1) {
												 lb_message->Visible = true;
												 lb_message->Text = "Verify Failure";
												 lb_info->Text = "\nPlace finger to verify template\n\n";

												 pb_png->Image = verifyFail;
											 }
											 else if (res >= 0) {
												 verify_finish();

												 verify_fail = 0;

												 lb_message->Visible = true;
												 lb_message->Text = "Verify Successfully";
												 lb_info->Text = "\nPlace finger to verify template\n";
												 lb_info->Text += "\nTemplate: " + fileList[res];

												 pb_png->Image = verifySuccess;
											 }
										 }
										 else {
											 if (res == -1) {
												 verify_fail++;

												 lb_message->Visible = true;
												 lb_message->Text = "Verify Failure";
												 lb_info->Text = "Verify fail " + verify_fail + " times, \n";
												 lb_info->Text += "place finger to verify template.\n";

												 pb_png->Image = verifyFail;

												 if (verify_preview) {
													 btnSave->Enabled = true;
												 }
												 else {
													 btnSave->Enabled = false;
												 }
											 }
											 else if (res >= 0) {
												 usb_res = usb_sensor_stop_stream(dev_handle);
												 if (res < 0) {
													 printf("control transfer OUT error %d\n", usb_res);
												 }
												 else {
													 printf("control transfer OUT ok, cmd(%.02x)\n", FPD_CMD_STOP_STREAM);
												 }

												 verify_timer->Stop();

												 verify_finish();

												 lib_deinit();
												 quality_chk_deinit();

												 lb_message->Visible = true;
												 lb_message->Text = "Verify Successfully";
												 lb_info->Text = "Template: " + fileList[res] + "\n";

												 pb_png->Image = verifySuccess;

												 control_item_enable(true);
												 btnOpen_device->Enabled = false;
												 if (verify_preview) {
													 btnSave->Enabled = true;
												 }
												 else {
													 btnSave->Enabled = false;
												 }
												 btnVerify->Text = "Verify";

												 start_verify = false;
											 }

											 if (verify_fail == 10) {
												 usb_res = usb_sensor_stop_stream(dev_handle);

												 verify_timer->Stop();

												 lib_deinit();
												 quality_chk_deinit();

												 lb_info->Text = "The verify of failures has reached 10,\n";
												 lb_info->Text += "so verify function stop.\n";

												 control_item_enable(true);
												 btnOpen_device->Enabled = false;
												 if (verify_preview) {
													 btnSave->Enabled = true;
												 }
												 else {
													 btnSave->Enabled = false;
												 }
												 btnVerify->Text = "Verify";

												 start_verify = false;
											 }
										 }
								 }
								 memStream = gcnew MemoryStream();
							 }

							 for (int y = 0; y < transferred; y++) {
								 if (y % 64 != 0) {
									 convertData[y - tag] = data[y];
								 }
								 else {
									 tag++;
								 }
							 }

							 for (int i = 0; i < transferred - tag; i++) {
								 convertData[i] = 255 - convertData[i];
							 }

							 byteArray = gcnew array<Byte>(transferred - tag);
							 Marshal::Copy((IntPtr)convertData, byteArray, 0, transferred - tag);

							 memStream->Write(byteArray, 0, transferred - tag);
						 }

						 pixel_cnt += transferred;
						 ++frame_cnt;
						 loopcnt++;
					 }
					 printf("bulk in ok (%d)\n", transferred);
					 printf("\n");
				 }
	}

	private: System::Void btnDeleteAll_Click(System::Object^  sender, System::EventArgs^  e) {
				 delete_files();

				 //lb_open_message_visible(false);
				 //lb_enroll_message_visible(false);

				 pb_gif->Visible = false;

				 lb_message->Visible = true;
				 lb_message->Text = "Delete Successfully";

				 lb_info->TextAlign = ContentAlignment::MiddleCenter;
				 lb_info->Text = "The database template already clear.";
	}

	private: System::Void btnCancel_Click(System::Object^  sender, System::EventArgs^  e) {

				 int res;
				 uint16_t frameSize;
				 pictureBox1->Image = logoImage;

				 lb_message->Visible = false;

				 //lb_open_message_visible(false);
				 //lb_enroll_message_visible(false);

				 lb_info->TextAlign = ContentAlignment::MiddleCenter;
				 lb_info->Text = "Welcome to use fingerprint tool";

				 textBox2->Focus();

				 label_quality->Text = "  " + 0;
				 label_area->Text = "  " + 0;

				 if (start_capture) {
					 res = usb_sensor_stop_stream(dev_handle);
					 if (res < 0) {
						 printf("control transfer OUT error %d\n", res);
					 }
					 else {
						 printf("control transfer OUT ok, cmd(%.02x)\n", FPD_CMD_STOP_STREAM);
						 capture_timer->Stop();
						 start_capture = false;

						 control_item_enable(true);
						 btnOpen_device->Enabled = false;
						 btnSave->Enabled = false;
						 btnCapture->Text = "Capture";
					 }					 
				 }
				 else if (start_enroll) {
					 res = usb_sensor_stop_stream(dev_handle);
					 if (res < 0) {
						 printf("control transfer OUT error %d\n", res);
					 }
					 else {
						 printf("control transfer OUT ok, cmd(%.02x)\n", FPD_CMD_STOP_STREAM);
						 enroll_timer->Stop();
						 start_enroll = false;

						 control_item_enable(true);
						 btnOpen_device->Enabled = false;
						 btnSave->Enabled = false;
						 btnEnroll->Text = "Enroll";

						 pb_gif->Visible = false;

						 if (enroll_save_image) {
							 char imagePath[200];
							 char string[200];
							 strcpy(string, (char*)(void*)Marshal::StringToHGlobalAnsi(image_type_path));
							 strcat(string, "\\");
							 strcat(string, date_buf);
							 strcat(string, "_");
							 strcat(string, filename);
							 strcat(string, "_");
							 strcat(string, finger_idx_buf);
							 strcat(string, "_%d.png");

							 for (int i = 1; i <= max_samples; i++)
							 {
								 sprintf(imagePath, string, i);
								 remove(imagePath);
							 }
						 }
					 }
				 }
				 else if (start_verify) {
					 res = usb_sensor_stop_stream(dev_handle);
					 if (res < 0) {
						 printf("control transfer OUT error %d\n", res);
					 }
					 else {
						 printf("control transfer OUT ok, cmd(%.02x)\n", FPD_CMD_STOP_STREAM);
					 }
					 verify_timer->Stop();
					 start_verify = false;

					 control_item_enable(true);
					 btnOpen_device->Enabled = false;
					 btnSave->Enabled = false;
					 btnVerify->Text = "Verify";

					 lb_message->Visible = false;
					 pb_gif->Visible = false;
				 }
				 else {					

					 control_item_enable(false);
					 btnOpen_device->Enabled = true;

					 tb_enroll_name->Text = "0";
					 tb_enroll_count->Text = "0";
					 tb_enroll_image_quality->Text = "0";
					 tb_enroll_fingerprint_area->Text = "0";
					 tb_enroll_return_coverage->Text = "0";

					 lb_enroll_count->Text = "(0-0)";
					 lb_enroll_image_quality->Text = "(0-0)";
					 lb_enroll_fingerprint_area->Text = "(0-0)";
					 lb_enroll_return_coverage->Text = "(0-0)";

					 tb_verify_image_quality->Text = "0";
					 tb_verify_fingerprint_area->Text = "0";
					 lb_verify_image_quality->Text = "(0-0)";
					 lb_verify_fingerprint_area->Text = "(0-0)";

					 lb_message->Visible = false;
					 pb_gif->Visible = false;

					 sensorType = "";
				 }
	}

	private: System::Void btnRead_Reg_Click(System::Object^  sender, System::EventArgs^  e) {
				 int r;
				 uint8_t RegData;

				 r = libusb_control_transfer(dev_handle, CTRL_IN, FPD_CMD_READ_REG, 0, 0x0031, &RegData, 1, 0);
				 if (r < 0) {
					 printf("control transfer IN error(%d)\n", r);
				 }
				 else {
					 printf("control transfer IN ok, cmd(0x%.02X)\n", FPD_CMD_READ_REG);
					 printf("RegData(0x%.02X)\n", RegData);
				 }
	}

	private: Void set_adjust_parameter(int width, int height) {
				 if (width == 128 && height == 128) {
					 //sensorType = "301B";
					 max_enrollment_samples = max_enrollment_samples_301b;
					 max_fingerprint_area = max_fingerprint_area_301b;
					 set_enrollment_samples = 8;
					 enrollment_image_quality = enrollment_image_quality_301b;
					 enrollment_fingerprint_area = enrollment_fingerprint_area_301b;
					 verify_image_quality = verify_image_quality_301b;
					 verify_fingerprint_area = verify_fingerprint_area_301b;
				 }
				 else if (width == 120 && height == 120) {
					 sensorType = "301C";
					 max_enrollment_samples = max_enrollment_samples_301c;
					 max_fingerprint_area = max_fingerprint_area_301c;
					 set_enrollment_samples = 8;
					 enrollment_image_quality = enrollment_image_quality_301c;
					 enrollment_fingerprint_area = enrollment_fingerprint_area_301c;
					 verify_image_quality = verify_image_quality_301c;
					 verify_fingerprint_area = verify_fingerprint_area_301b;
				 }
				 else if (width == 176 && height == 176) {
					 sensorType = "501";
					 max_enrollment_samples = max_enrollment_samples_501;
					 max_fingerprint_area = max_fingerprint_area_501;
					 set_enrollment_samples = 5;
					 enrollment_image_quality = enrollment_image_quality_501;
					 enrollment_fingerprint_area = enrollment_fingerprint_area_501;
					 verify_image_quality = verify_image_quality_301c;
					 verify_fingerprint_area = verify_fingerprint_area_301b;
				 }
	}

	public: static Bitmap^ ToGrayBitmap(array<Byte>^ rawValues, int width, int height) {
				Bitmap^ bmp = gcnew Bitmap(width, height, System::Drawing::Imaging::PixelFormat::Format8bppIndexed);
				System::Drawing::Rectangle rect = System::Drawing::Rectangle(0, 0, bmp->Width, bmp->Height);
				BitmapData^ bmpData = bmp->LockBits(rect, System::Drawing::Imaging::ImageLockMode::WriteOnly, bmp->PixelFormat);

				int stride = bmpData->Stride;
				int offset = stride - width;
				IntPtr iptr = bmpData->Scan0;
				int scanBytes = stride * height;

				int posScan = 0, posReal = 0;

				array<Byte>^ pixelValues = gcnew array<Byte>(scanBytes);

				for (int x = 0; x < height; x++) {
					for (int y = 0; y < width; y++) {
						pixelValues[posScan++] = rawValues[posReal++];
					}
					posScan += offset;
				}

				Marshal::Copy(pixelValues, 0, iptr, scanBytes);
				bmp->UnlockBits(bmpData);


				ColorPalette^ temPalette;

				Bitmap^ tempBmp = gcnew Bitmap(1, 1, System::Drawing::Imaging::PixelFormat::Format8bppIndexed);
				temPalette = tempBmp->Palette;

				for (int i = 0; i < 256; i++) {
					temPalette->Entries[i] = Color::FromArgb(i, i, i);
				}

				bmp->Palette = temPalette;

				return bmp;
	}

	
	private: Void printHex(uint8_t * buffer, uint32_t length) {
				 unsigned int i;

				 if (buffer == 0 || length == 0) {
					 printf("\r\n");
				 }
				 else {
					 for (i = 0; i < length; i++) {
						 printf("%02x ", buffer[i]);
					 }
				 }
	}

	private: Void findDevice(String^ vid, String^ pid) {
				 libusb_device **devs;
				 int cnt;
				 unsigned char iProduct[200];

				 String^ cvProduct;
				 
				 bool getdevice = false;

				 cboDevices->Sorted = false;
				 cboDevices->Items->Clear();	

				 cnt = libusb_get_device_list(NULL, &devs);
				 //printf("Libusb_get_device_list = %d\n", cnt);

				 for (deviceLocation = 0; deviceLocation < cnt; deviceLocation++) {
					 struct libusb_device_descriptor desc;
					 int r = libusb_get_device_descriptor(devs[deviceLocation], &desc);
					 if (r < 0) {
						 fprintf(stderr, "failed to get device descriptor");
						 return;
					 }
					 printf("%04X:%04X\n", desc.idVendor, desc.idProduct);

					 if ((desc.idVendor.ToString("X4") == vid) && (desc.idProduct.ToString("X4") == pid)) {
						 
						 //dev_handle = libusb_open_device_with_vid_pid(NULL, 0x06cb, 0x0040);
						 dev_handle = libusb_open_device_with_vid_pid(NULL, desc.idVendor, desc.idProduct);

						 libusb_get_string_descriptor_ascii(dev_handle, desc.iProduct, iProduct, 200);	

						 cvProduct = Marshal::PtrToStringAnsi((IntPtr)iProduct);

						 String^ sAdd = String::Format("Vid : 0x{0:X4}, Pid : 0x{1:X4} - {2}",
														desc.idVendor,
														desc.idProduct,
														cvProduct);
						 cboDevices->Items->Add(sAdd);

						 cboDevices->SelectedIndex = cboDevices->Items->Count > 0 ? 0 : -1;

						 getdevice = true;
						 break;
					 }					 				
				 }

				 if (!getdevice) {
					 tvInfo->Nodes->Clear();
					 tvInfo->Nodes->Add("No USB devices found.");
					 tvInfo->Nodes->Add("A device must be installed which uses the LibUsb-Win32 driver.");
					 tvInfo->Nodes->Add("Or");
					 tvInfo->Nodes->Add("The LibUsb-Win32 kernel service must be enabled.");
				 }

				 if (cboDevices->SelectedIndex >= 0)
				 {
					 tvInfo->Nodes->Clear();
					 Console::WriteLine("Location = " + deviceLocation);
					 addDevice(devs[deviceLocation], cboDevices->Text);
				 }

				 libusb_free_device_list(devs, 1);
	}

	private: Void addDevice(libusb_device *dev, String^ display) {
				 unsigned char descriptorType[200];
				 unsigned char iManufacturer[200];
				 unsigned char iProduct[200];
				 unsigned char serialNumber[200];

				 libusb_device_descriptor desc;

				 TreeNode^ tvDevice = tvInfo->Nodes->Add(display);

				 int r = libusb_get_device_descriptor(dev, &desc);

				 if (r < 0)
					printf("failed to get device descriptor\n");

				 tvDevice->Nodes->Add("Length : " + desc.bLength);
				 //libusb_get_string_descriptor_ascii(dev_handle, desc.bDescriptorType, descriptorType, 200);
				 //tvDevice->Nodes->Add("DescriptorType : " + Marshal::PtrToStringAnsi((IntPtr)descriptorType));
				 //printf("Device DescriptorType : %d\n", desc.bDescriptorType);
				 tvDevice->Nodes->Add("DescriptorType : " + desc.bDescriptorType);
				 tvDevice->Nodes->Add("BcdUSB : 0x" + desc.bcdDevice.ToString("X4"));
				 tvDevice->Nodes->Add("Class : 0x" + desc.bDeviceClass.ToString("X2"));
				 tvDevice->Nodes->Add("SubClass : 0x" + desc.bDeviceSubClass.ToString("X2"));
				 tvDevice->Nodes->Add("Protocol :0x" + desc.bDeviceProtocol.ToString("X2"));
				 tvDevice->Nodes->Add("MaxPacketSize : " + desc.bMaxPacketSize0);
				 tvDevice->Nodes->Add("VenderID : 0x" + desc.idVendor.ToString("X4"));
				 tvDevice->Nodes->Add("ProductID : 0x" + desc.idProduct.ToString("X4"));
				 tvDevice->Nodes->Add("BcdDevice : 0x" + desc.bcdDevice.ToString("x4"));
				 tvDevice->Nodes->Add("Configuration : " + desc.bNumConfigurations);
				 libusb_get_string_descriptor_ascii(dev_handle, desc.iManufacturer, iManufacturer, 200);
				 tvDevice->Nodes->Add("Manufacturer : " + Marshal::PtrToStringAnsi((IntPtr)iManufacturer));
				 libusb_get_string_descriptor_ascii(dev_handle, desc.iProduct, iProduct, 200);
				 tvDevice->Nodes->Add("Product : " + Marshal::PtrToStringAnsi((IntPtr)iProduct));
				 libusb_get_string_descriptor_ascii(dev_handle, desc.iSerialNumber, serialNumber, 200);
				 tvDevice->Nodes->Add("SerialNumber : " + Marshal::PtrToStringAnsi((IntPtr)serialNumber));
								
				 libusb_config_descriptor *config;

				 libusb_get_config_descriptor(dev, 0, &config);

				 TreeNode^ tvConfig = tvDevice->Nodes->Add("Config " + config->bNumInterfaces);

				 tvConfig->Nodes->Add("Length : " + config->bLength);
				 //libusb_get_string_descriptor_ascii(dev_handle, config->bDescriptorType, descriptorType, 200);
				 //tvConfig->Nodes->Add("DescriptorType : " + Marshal::PtrToStringAnsi((IntPtr)descriptorType));
				 //printf("Config DescriptorType : %d\n", config->bDescriptorType);
				 tvConfig->Nodes->Add("DescriptorType : " + config->bDescriptorType);
				 tvConfig->Nodes->Add("TotalLength : " + config->wTotalLength);
				 tvConfig->Nodes->Add("InterfaceCount : " + config->bNumInterfaces);
				 tvConfig->Nodes->Add("ConfigID : " + config->iConfiguration);
				 tvConfig->Nodes->Add("Attributes : 0x" + config->bmAttributes.ToString("X2"));
				 tvConfig->Nodes->Add("MaxPower : " + config->MaxPower);

				 const libusb_interface *inter;
				 const libusb_interface_descriptor *interdesc;
				 const libusb_endpoint_descriptor *epdesc;

				 TreeNode^ tvInterfaces = tvConfig;
				 for (int i = 0; i < (int)config->bNumInterfaces; i++) {
					 inter = &config->interface[i];
					 
					 for (int j = 0; j < inter->num_altsetting; j++) {
						 interdesc = &inter->altsetting[j];
						 TreeNode^ tvInterface = tvInterfaces->Nodes->Add("Interface [" + interdesc->bInterfaceNumber + "," + interdesc->bAlternateSetting + "]");
						 tvInterface->Nodes->Add("Length : " + interdesc->bLength);
						 //libusb_get_string_descriptor_ascii(dev_handle, interdesc->bDescriptorType, descriptorType, 200);
						 //tvInterface->Nodes->Add("DescriptorType : " + Marshal::PtrToStringAnsi((IntPtr)descriptorType));
						 //printf("Interface DescriptorType : %d\n", interdesc->bDescriptorType);
						 tvInterface->Nodes->Add("DescriptorType : " + interdesc->bDescriptorType);
						 tvInterface->Nodes->Add("InterfaceID : " + interdesc->bInterfaceNumber);
						 tvInterface->Nodes->Add("AlternateID : " + interdesc->bAlternateSetting);
						 tvInterface->Nodes->Add("EndpointCount : " + interdesc->bNumEndpoints);
						 tvInterface->Nodes->Add("Class : 0x" + interdesc->bInterfaceClass.ToString("X2"));
						 tvInterface->Nodes->Add("SubClass : 0x" + interdesc->bInterfaceSubClass.ToString("X2"));
						 tvInterface->Nodes->Add("Protocol : 0x" + interdesc->bInterfaceProtocol.ToString("X2"));

						 TreeNode^ tvEndpoints = tvInterface;
						 for (int k = 0; k < (int)interdesc->bNumEndpoints; k++) {
							 epdesc = &interdesc->endpoint[k];
							 TreeNode^ tvEndpoint = tvEndpoints->Nodes->Add("Endpoint 0x" + epdesc->bEndpointAddress.ToString("x2"));
							 tvEndpoint->Nodes->Add("Length : " + epdesc->bLength);
							 //libusb_get_string_descriptor_ascii(dev_handle, epdesc->bDescriptorType, descriptorType, 200);
							 //tvEndpoint->Nodes->Add("DescriptorType : " + Marshal::PtrToStringAnsi((IntPtr)descriptorType));
							 //printf("Endpoint DescriptorType : %d\n", epdesc->bDescriptorType);
							 tvEndpoint->Nodes->Add("DescriptorType : " + epdesc->bDescriptorType);
							 tvEndpoint->Nodes->Add("EndpointID : 0x" + epdesc->bEndpointAddress.ToString("X2"));
							 tvEndpoint->Nodes->Add("Attributes : 0x" + epdesc->bmAttributes.ToString("X2"));
							 tvEndpoint->Nodes->Add("MaxPacketSize : " + epdesc->wMaxPacketSize);
							 tvEndpoint->Nodes->Add("Interval : " + epdesc->bInterval);
							 tvEndpoint->Nodes->Add("Refresh : " + epdesc->bRefresh);
							 tvEndpoint->Nodes->Add("SynchAddress : 0x" + epdesc->bSynchAddress.ToString("X2"));							 			
						 }
					 }
				 }
				 libusb_free_config_descriptor(config);				 
	 }   


	 private: array<System::Drawing::Bitmap^>^ get_enroll_progress_image() {
				  array<System::Drawing::Bitmap^>^ progressList = gcnew array<System::Drawing::Bitmap^>(39);

				  char path[200];
				  char pathString[200];

				  struct _finddata_t c_file;
				  intptr_t hFile = 0;
				  int i = 0;

				  strcpy(file_path_string, app_path);
				  strcat(file_path_string, "\\res");
				  strcat(file_path_string, "\\enroll");
				  strcat(file_path_string, "\\*.png");

				  strcpy(pathString, app_path);
				  strcat(pathString, "\\res");
				  strcat(pathString, "\\enroll");
				  strcat(pathString, "\\%s");

				  hFile = _findfirst(file_path_string, &c_file);
				  if (hFile != -1) {
					  do {
						  sprintf(path, pathString, c_file.name);
						  progressList[i] = gcnew Bitmap(Marshal::PtrToStringAnsi((IntPtr)path));
						  i++;
					  } while (_findnext(hFile, &c_file) == 0);
				  }
				  _findclose(hFile);

				  return progressList;
	 }

	 private: array<System::String^>^ list_files() {
				  System::String^ DS_file = ".DS_Store";

				  //array<System::String^>^ file = Directory::GetFiles(Marshal::PtrToStringAnsi((IntPtr)databaseFile));
				  array<System::String^>^ file = Directory::GetFiles(template_type_path);
				  array<System::String^>^ fileList = gcnew array<System::String^>(file->Length);

				  for (int i = 0; i < file->Length; i++) {
					  if (DS_file == Path::GetFileName(file[i])) {
						  printf("delete file name => %s\n", file[i]);
						  remove((char*)(void*)Marshal::StringToHGlobalAnsi(file[i]));
					  }
					  else {
						  fileList[i] = Path::GetFileName(file[i]);
					  }
				  }
				  printf("file length => %d\n", file->Length);
				  return fileList;
	 }

	 private: void delete_files() {
				 array<System::String^>^ file = Directory::GetFiles(template_type_path);
				 for (int i = 0; i < file->Length; i++) {
					 remove((char*)(void*)Marshal::StringToHGlobalAnsi(file[i]));
				 }
	 }

	 private: void control_item_enable(bool flag) {
				  if (flag) {
					  btnOpen_device->Enabled = true;
					  btnCapture->Enabled = true;
					  btnRealTime->Enabled = true;
					  btnEnroll->Enabled = true;
					  btnVerify->Enabled = true;
					  btnDeleteAll->Enabled = true;
					  btnCancel->Enabled = true;
					  btnSave->Enabled = true;
					  cb_enroll_preview->Enabled = true;
					  cb_enroll_save_image->Enabled = true;
					  tb_enroll_name->Enabled = true;
					  tb_enroll_count->Enabled = true;
					  tb_enroll_return_coverage->Enabled = true;
					  tb_enroll_image_quality->Enabled = true;
					  tb_enroll_fingerprint_area->Enabled = true;
					  cb_verify_preview->Enabled = true;
					  cb_verify_con->Enabled = true;
					  tb_verify_image_quality->Enabled = true;
					  tb_verify_fingerprint_area->Enabled = true;
				  }
				  else {
					  btnOpen_device->Enabled = false;
					  btnCapture->Enabled = false;
					  btnRealTime->Enabled = false;
					  btnEnroll->Enabled = false;
					  btnVerify->Enabled = false;
					  btnDeleteAll->Enabled = false;
					  btnCancel->Enabled = false;
					  btnSave->Enabled = false;
					  cb_enroll_preview->Enabled = false;
					  cb_enroll_save_image->Enabled = false;
					  tb_enroll_name->Enabled = false;
					  tb_enroll_count->Enabled = false;
					  tb_enroll_return_coverage->Enabled = false;
					  tb_enroll_image_quality->Enabled = false;
					  tb_enroll_fingerprint_area->Enabled = false;
					  cb_verify_preview->Enabled = false;
					  cb_verify_con->Enabled = false;
					  tb_verify_image_quality->Enabled = false;
					  tb_verify_fingerprint_area->Enabled = false;
				  }
	 }

	private: System::Void tabControl_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {

				 if (tabControl1->SelectedTab == tabPage2) {
					 cboSensorType->SelectedIndex = cboSensorType->Items->Count > 0 ? 0 : -1;
				 }
	}
private: System::Void btnWrite_Reg_Click(System::Object^  sender, System::EventArgs^  e) {
}
};
}
