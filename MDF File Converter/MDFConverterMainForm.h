#pragma once

#include "../MDF/MDF.h"

using namespace PsyPhy;
using namespace MDF;

namespace MDFFileConverter {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Runtime::InteropServices;

	/// <summary>
	/// Summary for Form1
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		Form1(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}

	private: System::Windows::Forms::RadioButton^  emtRadioButton1;
	private: System::Windows::Forms::RadioButton^  c3dRadioButton1;
	private: System::Windows::Forms::Button^  button1;
	private: System::Windows::Forms::TextBox^  logTextBox;
	private: System::Windows::Forms::GroupBox^  outputBox;
	private: System::Windows::Forms::OpenFileDialog^  openFileDialog1;
	private: System::Windows::Forms::RadioButton^  txtRadioButton1;
	private: System::Windows::Forms::CheckBox^  keepOnlyCheckBox1;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->outputBox = (gcnew System::Windows::Forms::GroupBox());
			this->txtRadioButton1 = (gcnew System::Windows::Forms::RadioButton());
			this->emtRadioButton1 = (gcnew System::Windows::Forms::RadioButton());
			this->c3dRadioButton1 = (gcnew System::Windows::Forms::RadioButton());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->logTextBox = (gcnew System::Windows::Forms::TextBox());
			this->openFileDialog1 = (gcnew System::Windows::Forms::OpenFileDialog());
			this->keepOnlyCheckBox1 = (gcnew System::Windows::Forms::CheckBox());
			this->outputBox->SuspendLayout();
			this->SuspendLayout();
			// 
			// outputBox
			// 
			this->outputBox->Controls->Add(this->txtRadioButton1);
			this->outputBox->Controls->Add(this->emtRadioButton1);
			this->outputBox->Controls->Add(this->c3dRadioButton1);
			this->outputBox->Location = System::Drawing::Point(618, 15);
			this->outputBox->Margin = System::Windows::Forms::Padding(6);
			this->outputBox->Name = L"outputBox";
			this->outputBox->Padding = System::Windows::Forms::Padding(6);
			this->outputBox->Size = System::Drawing::Size(200, 140);
			this->outputBox->TabIndex = 0;
			this->outputBox->TabStop = false;
			this->outputBox->Text = L"Output Type";
			// 
			// txtRadioButton1
			// 
			this->txtRadioButton1->AutoSize = true;
			this->txtRadioButton1->Location = System::Drawing::Point(47, 98);
			this->txtRadioButton1->Name = L"txtRadioButton1";
			this->txtRadioButton1->Size = System::Drawing::Size(83, 33);
			this->txtRadioButton1->TabIndex = 2;
			this->txtRadioButton1->Text = L"TXT";
			this->txtRadioButton1->UseVisualStyleBackColor = true;
			// 
			// emtRadioButton1
			// 
			this->emtRadioButton1->AutoSize = true;
			this->emtRadioButton1->Location = System::Drawing::Point(47, 67);
			this->emtRadioButton1->Name = L"emtRadioButton1";
			this->emtRadioButton1->Size = System::Drawing::Size(86, 33);
			this->emtRadioButton1->TabIndex = 1;
			this->emtRadioButton1->Text = L"EMT";
			this->emtRadioButton1->UseVisualStyleBackColor = true;
			// 
			// c3dRadioButton1
			// 
			this->c3dRadioButton1->AutoSize = true;
			this->c3dRadioButton1->Checked = true;
			this->c3dRadioButton1->Location = System::Drawing::Point(47, 33);
			this->c3dRadioButton1->Name = L"c3dRadioButton1";
			this->c3dRadioButton1->Size = System::Drawing::Size(81, 33);
			this->c3dRadioButton1->TabIndex = 0;
			this->c3dRadioButton1->TabStop = true;
			this->c3dRadioButton1->Text = L"C3D";
			this->c3dRadioButton1->UseVisualStyleBackColor = true;
			// 
			// button1
			// 
			this->button1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 24, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->button1->Location = System::Drawing::Point(12, 15);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(392, 84);
			this->button1->TabIndex = 1;
			this->button1->Text = L"Select MDF Files";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &Form1::button1_Click);
			// 
			// logTextBox
			// 
			this->logTextBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->logTextBox->Location = System::Drawing::Point(12, 164);
			this->logTextBox->Multiline = true;
			this->logTextBox->Name = L"logTextBox";
			this->logTextBox->ReadOnly = true;
			this->logTextBox->ScrollBars = System::Windows::Forms::ScrollBars::Both;
			this->logTextBox->Size = System::Drawing::Size(805, 450);
			this->logTextBox->TabIndex = 2;
			// 
			// openFileDialog1
			// 
			this->openFileDialog1->DefaultExt = L"mdf";
			this->openFileDialog1->Filter = L"MDF FIles (*.mdf) | *.mdf";
			this->openFileDialog1->Multiselect = true;
			this->openFileDialog1->ReadOnlyChecked = true;
			// 
			// keepOnlyCheckBox1
			// 
			this->keepOnlyCheckBox1->AutoSize = true;
			this->keepOnlyCheckBox1->Location = System::Drawing::Point(57, 118);
			this->keepOnlyCheckBox1->Name = L"keepOnlyCheckBox1";
			this->keepOnlyCheckBox1->Size = System::Drawing::Size(390, 33);
			this->keepOnlyCheckBox1->TabIndex = 3;
			this->keepOnlyCheckBox1->Text = L"Keep only main makers (max 28).";
			this->keepOnlyCheckBox1->UseVisualStyleBackColor = true;
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(14, 29);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(833, 628);
			this->Controls->Add(this->keepOnlyCheckBox1);
			this->Controls->Add(this->logTextBox);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->outputBox);
			this->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->Margin = System::Windows::Forms::Padding(6);
			this->Name = L"Form1";
			this->Text = L"MDF File Converter";
			this->outputBox->ResumeLayout(false);
			this->outputBox->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {
				 Refresh();
				 Enabled = false;
				 if ( ::System::Windows::Forms::DialogResult::OK == openFileDialog1->ShowDialog() ) {
					 DateTime thisDay = DateTime::Now;
					 this->logTextBox->AppendText( thisDay.ToString() );
					 int files = openFileDialog1->FileNames->Length;
					 for ( int i = 0; i < files; i++ ) {

						MDFRecord mdf;
						String^ input = openFileDialog1->FileNames[i];
						logTextBox->AppendText( "\r\n  " + input );
						char* mdf_filename = (char*) Marshal::StringToHGlobalAnsi( input ).ToPointer();
						mdf.ReadDataFile( mdf_filename );
						Marshal::FreeHGlobal(IntPtr( mdf_filename ));

						if ( keepOnlyCheckBox1->Checked ) {
							mdf.KeepOnly( 0, 27 );
							input = input->Replace( ".mdf", "_(1-28).mdf" );
						}

						 if ( c3dRadioButton1->Checked ) {
							 String^ output = input->Replace( ".mdf", ".c3d" );
							 char* c3d_filename = (char*) Marshal::StringToHGlobalAnsi( output ).ToPointer();
							 mdf.WriteC3D( c3d_filename );
							 Marshal::FreeHGlobal(IntPtr( c3d_filename ));
							 logTextBox->AppendText( "\r\n     -> " + output );
						 }
						 if ( emtRadioButton1->Checked ) {
							 String^ marker = input->Replace( ".mdf", "_mrk.emt" );
							 char* mrk_filename = (char*) Marshal::StringToHGlobalAnsi( marker ).ToPointer();
							 mdf.WriteMarkersEMT( mrk_filename );
							 Marshal::FreeHGlobal(IntPtr( mrk_filename ));
							 logTextBox->AppendText( "\r\n     -> " + marker );
							 String^ emg = input->Replace( ".mdf", "_emg.emt" );
							 char* emg_filename = (char*) Marshal::StringToHGlobalAnsi( emg ).ToPointer();
							 mdf.WriteAnalogEMT( emg_filename );
							 Marshal::FreeHGlobal(IntPtr( emg_filename ));
							 logTextBox->AppendText( "\r\n     -> " + emg );
						 }
						 if ( txtRadioButton1->Checked ) {

							 logTextBox->AppendText( "\r\n     -> Up-sampling markers to analog rate." );
							 mdf.FillGaps();

							 logTextBox->AppendText( "\r\n     -> Writing ASCII text file." );
							 String^ text = input->Replace( ".mdf", ".txt" );
							 char *txt_filename = (char*) Marshal::StringToHGlobalAnsi( text ).ToPointer();
							 mdf.WriteASCII( txt_filename );
							 Marshal::FreeHGlobal(IntPtr( txt_filename ));
							 logTextBox->AppendText( "\r\n     -> " + text );

						 }
					 }
					logTextBox->AppendText( "\r\n  Done.\r\n\r\n" );
				 }
				Enabled = true;

			 }
	};
}

