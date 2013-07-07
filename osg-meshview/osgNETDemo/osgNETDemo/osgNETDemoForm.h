#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::Runtime::InteropServices;
using namespace System::Threading;

namespace osgNETDemo {

	/// <summary>
	/// osgNETDemoForm 摘要
	///
	/// 警告: 如果更改此类的名称，则需要更改
	///          与此类所依赖的所有 .resx 文件关联的托管资源编译器工具的
	///          “资源文件名”属性。否则，
	///          设计器将不能与此窗体的关联
	///          本地化资源正确交互。
	/// </summary>
	public ref class osgNETDemoForm : public System::Windows::Forms::Form
	{
	public:
		osgNETDemoForm(void)
		{
			InitializeComponent();
			//
			//TODO: 在此处添加构造函数代码
			//
		}

	protected:
		/// <summary>
		/// 清理所有正在使用的资源。
		/// </summary>
		~osgNETDemoForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::MenuStrip^  mainMenu;
	protected: 

	private: System::Windows::Forms::ToolStripMenuItem^  fileToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  openToolStripMenuItem;



	private: System::Windows::Forms::ToolStripMenuItem^  exitToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  helpToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  aboutToolStripMenuItem;




	private: System::Windows::Forms::StatusStrip^  osgStatus;
	private: System::Windows::Forms::ToolStripStatusLabel^  frameRateToolStripStatusLabel;
	private: System::Windows::Forms::SplitContainer^  splitContainer1;
	private: System::Windows::Forms::Label^  osgRenderTarget;
	private: System::Windows::Forms::Button^  btnRight;
	private: System::Windows::Forms::Button^  btnLeft;
	private: System::Windows::Forms::Button^  btnDown;
	private: System::Windows::Forms::Button^  btnUp;









	protected: 


	private:
		/// <summary>
		/// 必需的设计器变量。
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// 设计器支持所需的方法 - 不要
		/// 使用代码编辑器修改此方法的内容。
		/// </summary>
		void InitializeComponent(void)
		{
			this->mainMenu = (gcnew System::Windows::Forms::MenuStrip());
			this->fileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->openToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->exitToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->helpToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->aboutToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->osgStatus = (gcnew System::Windows::Forms::StatusStrip());
			this->frameRateToolStripStatusLabel = (gcnew System::Windows::Forms::ToolStripStatusLabel());
			this->splitContainer1 = (gcnew System::Windows::Forms::SplitContainer());
			this->osgRenderTarget = (gcnew System::Windows::Forms::Label());
			this->btnRight = (gcnew System::Windows::Forms::Button());
			this->btnLeft = (gcnew System::Windows::Forms::Button());
			this->btnDown = (gcnew System::Windows::Forms::Button());
			this->btnUp = (gcnew System::Windows::Forms::Button());
			this->mainMenu->SuspendLayout();
			this->osgStatus->SuspendLayout();
			this->splitContainer1->Panel1->SuspendLayout();
			this->splitContainer1->Panel2->SuspendLayout();
			this->splitContainer1->SuspendLayout();
			this->SuspendLayout();
			// 
			// mainMenu
			// 
			this->mainMenu->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->fileToolStripMenuItem, 
				this->helpToolStripMenuItem});
			this->mainMenu->Location = System::Drawing::Point(0, 0);
			this->mainMenu->Name = L"mainMenu";
			this->mainMenu->Size = System::Drawing::Size(777, 24);
			this->mainMenu->TabIndex = 0;
			this->mainMenu->Text = L"menuStrip1";
			// 
			// fileToolStripMenuItem
			// 
			this->fileToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->openToolStripMenuItem, 
				this->exitToolStripMenuItem});
			this->fileToolStripMenuItem->Name = L"fileToolStripMenuItem";
			this->fileToolStripMenuItem->Size = System::Drawing::Size(37, 20);
			this->fileToolStripMenuItem->Text = L"&File";
			// 
			// openToolStripMenuItem
			// 
			this->openToolStripMenuItem->Name = L"openToolStripMenuItem";
			this->openToolStripMenuItem->Size = System::Drawing::Size(103, 22);
			this->openToolStripMenuItem->Text = L"&Open";
			this->openToolStripMenuItem->Click += gcnew System::EventHandler(this, &osgNETDemoForm::openToolStripMenuItem_Click);
			// 
			// exitToolStripMenuItem
			// 
			this->exitToolStripMenuItem->Name = L"exitToolStripMenuItem";
			this->exitToolStripMenuItem->Size = System::Drawing::Size(103, 22);
			this->exitToolStripMenuItem->Text = L"E&xit";
			this->exitToolStripMenuItem->Click += gcnew System::EventHandler(this, &osgNETDemoForm::exitToolStripMenuItem_Click);
			// 
			// helpToolStripMenuItem
			// 
			this->helpToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->aboutToolStripMenuItem});
			this->helpToolStripMenuItem->Name = L"helpToolStripMenuItem";
			this->helpToolStripMenuItem->Size = System::Drawing::Size(44, 20);
			this->helpToolStripMenuItem->Text = L"&Help";
			// 
			// aboutToolStripMenuItem
			// 
			this->aboutToolStripMenuItem->Name = L"aboutToolStripMenuItem";
			this->aboutToolStripMenuItem->Size = System::Drawing::Size(165, 22);
			this->aboutToolStripMenuItem->Text = L"Go to Homepage";
			this->aboutToolStripMenuItem->Click += gcnew System::EventHandler(this, &osgNETDemoForm::aboutToolStripMenuItem_Click);
			// 
			// osgStatus
			// 
			this->osgStatus->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->frameRateToolStripStatusLabel});
			this->osgStatus->Location = System::Drawing::Point(0, 536);
			this->osgStatus->Name = L"osgStatus";
			this->osgStatus->Size = System::Drawing::Size(777, 22);
			this->osgStatus->TabIndex = 4;
			this->osgStatus->Text = L"statusStrip1";
			// 
			// frameRateToolStripStatusLabel
			// 
			this->frameRateToolStripStatusLabel->Name = L"frameRateToolStripStatusLabel";
			this->frameRateToolStripStatusLabel->Size = System::Drawing::Size(0, 17);
			// 
			// splitContainer1
			// 
			this->splitContainer1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->splitContainer1->Location = System::Drawing::Point(0, 24);
			this->splitContainer1->Name = L"splitContainer1";
			// 
			// splitContainer1.Panel1
			// 
			this->splitContainer1->Panel1->Controls->Add(this->osgRenderTarget);
			// 
			// splitContainer1.Panel2
			// 
			this->splitContainer1->Panel2->Controls->Add(this->btnRight);
			this->splitContainer1->Panel2->Controls->Add(this->btnLeft);
			this->splitContainer1->Panel2->Controls->Add(this->btnDown);
			this->splitContainer1->Panel2->Controls->Add(this->btnUp);
			this->splitContainer1->Size = System::Drawing::Size(777, 512);
			this->splitContainer1->SplitterDistance = 688;
			this->splitContainer1->TabIndex = 5;
			// 
			// osgRenderTarget
			// 
			this->osgRenderTarget->BackColor = System::Drawing::Color::White;
			this->osgRenderTarget->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->osgRenderTarget->Dock = System::Windows::Forms::DockStyle::Fill;
			this->osgRenderTarget->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->osgRenderTarget->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 16, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(134)));
			this->osgRenderTarget->Location = System::Drawing::Point(0, 0);
			this->osgRenderTarget->Name = L"osgRenderTarget";
			this->osgRenderTarget->Size = System::Drawing::Size(688, 512);
			this->osgRenderTarget->TabIndex = 6;
			this->osgRenderTarget->Text = L"osgNETDemo\r\n\r\nwww.hesicong.net";
			this->osgRenderTarget->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// btnRight
			// 
			this->btnRight->Location = System::Drawing::Point(2, 198);
			this->btnRight->Name = L"btnRight";
			this->btnRight->Size = System::Drawing::Size(79, 59);
			this->btnRight->TabIndex = 3;
			this->btnRight->Text = L"Right";
			this->btnRight->UseVisualStyleBackColor = true;
			this->btnRight->Click += gcnew System::EventHandler(this, &osgNETDemoForm::btnRight_Click);
			// 
			// btnLeft
			// 
			this->btnLeft->Location = System::Drawing::Point(3, 133);
			this->btnLeft->Name = L"btnLeft";
			this->btnLeft->Size = System::Drawing::Size(79, 59);
			this->btnLeft->TabIndex = 2;
			this->btnLeft->Text = L"Left";
			this->btnLeft->UseVisualStyleBackColor = true;
			this->btnLeft->Click += gcnew System::EventHandler(this, &osgNETDemoForm::btnLeft_Click);
			// 
			// btnDown
			// 
			this->btnDown->Location = System::Drawing::Point(2, 68);
			this->btnDown->Name = L"btnDown";
			this->btnDown->Size = System::Drawing::Size(79, 59);
			this->btnDown->TabIndex = 1;
			this->btnDown->Text = L"Down";
			this->btnDown->UseVisualStyleBackColor = true;
			this->btnDown->Click += gcnew System::EventHandler(this, &osgNETDemoForm::btnDown_Click);
			// 
			// btnUp
			// 
			this->btnUp->Location = System::Drawing::Point(3, 3);
			this->btnUp->Name = L"btnUp";
			this->btnUp->Size = System::Drawing::Size(79, 59);
			this->btnUp->TabIndex = 0;
			this->btnUp->Text = L"Up";
			this->btnUp->UseVisualStyleBackColor = true;
			this->btnUp->Click += gcnew System::EventHandler(this, &osgNETDemoForm::btnUp_Click);
			// 
			// osgNETDemoForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(777, 558);
			this->Controls->Add(this->splitContainer1);
			this->Controls->Add(this->osgStatus);
			this->Controls->Add(this->mainMenu);
			this->MainMenuStrip = this->mainMenu;
			this->Name = L"osgNETDemoForm";
			this->Text = L"osgNETDemoForm (http://www.hesicong.net)";
			this->Load += gcnew System::EventHandler(this, &osgNETDemoForm::osgNETDemoForm_Load);
			this->mainMenu->ResumeLayout(false);
			this->mainMenu->PerformLayout();
			this->osgStatus->ResumeLayout(false);
			this->osgStatus->PerformLayout();
			this->splitContainer1->Panel1->ResumeLayout(false);
			this->splitContainer1->Panel2->ResumeLayout(false);
			this->splitContainer1->ResumeLayout(false);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion


#pragma region osgStuff

		//Note:
		//Here we can not use osg::ref_ptr due to C++/CLI limit. I have wrote a managed smart_ptr, but for simple, I use pointer instead.
		//Memory leak may occur if you do not manullly clear the resource.
		osg::Group* osgRoot;
		osg::StateSet* polygonStateSet;
		osgViewer::Viewer* osgViewer;

		Object^ dummyViewerObject;		//Managed dummy viewer object. Used to prevent modifying viewer by one more thread.
		System::Threading::Thread^ OSGthread;	//Thread OSG rendering loop will run in.

		void InitOSG();
		void RunOSG();
		void OSGThread();

	private: System::Void ViewRotate(double angleDeg, osg::Vec3 axis)
			 {
				 Monitor::Enter(dummyViewerObject);	//Prevent modify scene while viewer is rendering.
				 {
					 osgGA::TrackballManipulator* trackball=(osgGA::TrackballManipulator*)osgViewer->getCameraManipulator();
					 osg::Quat q=trackball->getRotation();
					 osg::Vec3 v;
					 osg::Quat qq(angleDeg/180.0*osg::PI , axis);
					 q=qq*q;
					 trackball->setRotation(q);
				 }
				 Monitor::Exit(dummyViewerObject);
			 }

#pragma endregion osgStuff

	private: System::Void openToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
				 osgViewer->setDone(true);
				 OpenFileDialog^ openFileDialog=gcnew OpenFileDialog();
				 if(openFileDialog->ShowDialog(this)!=::DialogResult::OK)
				 {
					 MessageBox::Show("You do not select a file, render will stop.");
					 return;
				 }

				 String^ fileName=openFileDialog->FileName;

				 //Convert managed string to unmanaged.
				 //NOTE: For demo purpose, here only ANSI characters is supported.
				 char* stringPointer = (char*) Marshal::StringToHGlobalAnsi(fileName ).ToPointer();
				 osgRoot=(osg::Group*)osgDB::readNodeFile(stringPointer);
				 Marshal::FreeHGlobal( IntPtr(stringPointer));

				 osgViewer->setSceneData(osgRoot);
				 RunOSG();
			 }

	private: System::Void osgNETDemoForm_Load(System::Object^  sender, System::EventArgs^  e) {
				 InitOSG();
			 }
	private: System::Void exitToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
				 Application::Exit();
			 }
	private: System::Void aboutToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
				 System::Diagnostics::Process::Start("IExplore.exe",   "http://www.hesicong.net");
			 }
	private: System::Void btnUp_Click(System::Object^  sender, System::EventArgs^  e) {
				 ViewRotate(-5, osg::Vec3(1,0,0));
			 }
	private: System::Void btnDown_Click(System::Object^  sender, System::EventArgs^  e) {
				 ViewRotate(5, osg::Vec3(1,0,0));
			 }
	private: System::Void btnLeft_Click(System::Object^  sender, System::EventArgs^  e) {
				 ViewRotate(5, osg::Vec3(0,1,0));
			 }
	private: System::Void btnRight_Click(System::Object^  sender, System::EventArgs^  e) {
				 ViewRotate(-5, osg::Vec3(0,1,0));
			 }
	};
}
