#pragma comment(lib, "user32.lib")


//these files are needed to use the opencv libraries
#include <opencv2/opencv.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>
#include <iostream>
#include <stdio.h>
#include <Windows.h>
#include <winuser.h>
#include <conio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/videoio.hpp>

namespace FaceProject {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace std;
	using namespace cv;

	IplImage* frame = 0;
	CvCapture* capture = 0;
	int camBusy = 0;
	Mat captureFrame;
	Mat grayscaleFrame;
	VideoCapture captureDevice;
	CascadeClassifier nose_cascade, righteye_cascade, lefteye_cascade ;
	int b1 = 0;
	int leftclick = 0, rightclick = 0;
	int turnon = 0, turnoff = 0;
	int c = 0;
	int p1, p2, p3, p4;
	cv::Point r1, r2, r3, r4;
	int x, y, w, z;
	POINT pt;

	/// <summary>
	/// Summary for MyForm
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void)
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
		~MyForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::PictureBox^  pictureBox1;
	protected:
	private: System::Windows::Forms::Button^  button1;
	private: System::Windows::Forms::Button^  button2;
	private: System::ComponentModel::BackgroundWorker^  backgroundWorker1;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::NotifyIcon^  notifyIcon1;

	private: System::ComponentModel::IContainer^  components;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(MyForm::typeid));
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->backgroundWorker1 = (gcnew System::ComponentModel::BackgroundWorker());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->notifyIcon1 = (gcnew System::Windows::Forms::NotifyIcon(this->components));
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->BeginInit();
			this->SuspendLayout();
			// 
			// pictureBox1
			// 
			this->pictureBox1->Location = System::Drawing::Point(13, 13);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(640, 480);
			this->pictureBox1->TabIndex = 0;
			this->pictureBox1->TabStop = false;
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(13, 499);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(75, 23);
			this->button1->TabIndex = 1;
			this->button1->Text = L"Start Cam";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &MyForm::button1_Click);
			// 
			// button2
			// 
			this->button2->Location = System::Drawing::Point(94, 499);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(98, 23);
			this->button2->TabIndex = 2;
			this->button2->Text = L"Tracking";
			this->button2->UseVisualStyleBackColor = true;
			this->button2->Click += gcnew System::EventHandler(this, &MyForm::button2_Click);
			// 
			// backgroundWorker1
			// 
			this->backgroundWorker1->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &MyForm::backgroundWorker1_DoWork);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(216, 504);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(418, 13);
			this->label1->TabIndex = 3;
			this->label1->Text = L"To turn on tracking deep blink with your left eye, to turn off face away from you"
				L"r camera";
			// 
			// notifyIcon1
			// 
			this->notifyIcon1->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"notifyIcon1.Icon")));
			this->notifyIcon1->Text = L"notifyIcon1";
			this->notifyIcon1->Visible = true;
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(667, 528);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->button2);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->pictureBox1);
			this->Name = L"MyForm";
			this->Text = L"MyForm";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void backgroundWorker1_DoWork(System::Object^  sender, System::ComponentModel::DoWorkEventArgs^  e) {
		//this loads the haar-cascade files to be usable in the program
		nose_cascade.load("D:/nose.xml");
		righteye_cascade.load("D:/haarcascade_righteye_2splits.xml");
		lefteye_cascade.load("D:/haarcascade_lefteye_2splits.xml");
		//starts a loop so that a live image feed can appear
		while (true)
		{
			//takes the capturedevice(webcam) and puts it into a frame
			captureDevice >> captureFrame;
			//flips the image so that the image is a mirror
			cv::flip(captureFrame, captureFrame, 1);
			//changes the image to a greyscale image
			cvtColor(captureFrame, grayscaleFrame, CV_BGR2GRAY);
			equalizeHist(grayscaleFrame, grayscaleFrame);

			//create a vector array to store the face found
			std::vector<Rect> nose, lefteye, righteye;
			//code to track the different facial features
			nose_cascade.detectMultiScale(grayscaleFrame, nose, 1.5, 3, CV_HAAR_FIND_BIGGEST_OBJECT | CV_HAAR_SCALE_IMAGE, cv::Size(30, 30));
			righteye_cascade.detectMultiScale(grayscaleFrame, righteye, 2.5, 3, CV_HAAR_FIND_BIGGEST_OBJECT | CV_HAAR_SCALE_IMAGE, cv::Size(30, 30));
			lefteye_cascade.detectMultiScale(grayscaleFrame, lefteye, 2.5, 3, CV_HAAR_FIND_BIGGEST_OBJECT | CV_HAAR_SCALE_IMAGE, cv::Size(30, 30));
			//if it tracks a nose then it does this
			for (int i = 0; i < nose.size(); i++)
			{
				//creates two points that will be used to draw the rectangle
				cv::Point pt1(nose[i].x + nose[i].width, nose[i].y + nose[i].height);
				cv::Point pt2(nose[i].x, nose[i].y);
				//draws the rectangle around the nose
				cv::rectangle(captureFrame, pt1, pt2, cvScalar(0, 255, 0, 0), 1, 8, 0);
				//draws a bigger rectangle if it has turned on
				if (b1 == 1) {
					while (c != 1) {
						//doing the points here will make sure the rectangle stays in a set position
						p1 = nose[i].x + nose[i].width + 10;
						p2 = nose[i].y + nose[i].height + 10;
						p4 = nose[i].y - 5;
						p3 = nose[i].x - 10;
						r1 = cv::Point(p1, p2);
						r2 = cv::Point(p3, p2);
						r3 = cv::Point(p1, p4);
						r4 = cv::Point(p3, p4);
						c = 1;
					}
					//draws a rectangle
					rectangle(captureFrame, r1, r4, cvScalar(0, 255, 0, 0), 1, 8, 0);
					//here it controls the mouse depending on the position it moves
					if ((p1 < nose[i].x + nose[i].width) && ((nose[i].x + nose[i].width - p1) < 30)) {
						line(captureFrame, r1, r3, cvScalar(0, 255, 255, 0), 1, 8, 0);
						//calculates how far it is from the outer rectangle, moves more the further it is
						x = x + (nose[i].x + nose[i].width - p1);
						SetCursorPos(x, y);
					}
					if ((p2 < nose[i].y + nose[i].height) && ((nose[i].y + nose[i].height - p2) < 30)) {
						line(captureFrame, r1, r2, cvScalar(0, 255, 255, 0), 1, 8, 0);
						y = y + (nose[i].y + nose[i].height - p2);
						SetCursorPos(x, y);
					}
					if ((p3 > nose[i].x) && ((p3 - nose[i].x) < 30)) {
						line(captureFrame, r2, r4, cvScalar(0, 255, 255, 0), 1, 8, 0);
						x = x - (p3 - nose[i].x);
						SetCursorPos(x, y);

					}
					if ((p4 > nose[i].y) && ((p4 - nose[i].y) < 30)) {
						line(captureFrame, r3, r4, cvScalar(0, 255, 255, 0), 1, 8, 0);
						y = y - (p4 - nose[i].y);
						SetCursorPos(x, y);
					}

					//tracks the right eye
					for (int i = 0; i < righteye.size(); i++)
					{
						cv::Point ept1(righteye[i].x + righteye[i].width, righteye[i].y + righteye[i].height);
						cv::Point ept2(righteye[i].x, righteye[i].y);

						rectangle(captureFrame, ept1, ept2, cvScalar(0, 255, 0, 0), 1, 8, 0);

					}
					for (int i = 0; i < lefteye.size(); i++)
					{
						cv::Point ept1(lefteye[i].x + lefteye[i].width, lefteye[i].y + lefteye[i].height);
						cv::Point ept2(lefteye[i].x, lefteye[i].y);

						rectangle(captureFrame, ept1, ept2, cvScalar(0, 255, 0, 0), 1, 8, 0);

					}
					//if eye is visible the counter resets to 0 as the cascade is not there 100% of the time
					if (lefteye.size() > 0) {
						leftclick = 0;
					}
					//it'll start a counter if it's not visible
					else {
						leftclick = leftclick + 1;
					}
					//when it reaches 12 it will click
					if (leftclick == 12) {
						//does left mouse click and resets click
						mouse_event(MOUSEEVENTF_LEFTDOWN, x, y, 0, 0);
						mouse_event(MOUSEEVENTF_LEFTUP, x, y, 0, 0);
						leftclick = 0;
					}
					if (righteye.size() > 0) {
						rightclick = 0;
					}
					//it'll start a counter if it's not visible
					else {
						rightclick = rightclick + 1;
					}
					//when it reaches 12 it will click
					if (rightclick == 12) {
						//does right mouse click and resets click
						mouse_event(MOUSEEVENTF_RIGHTDOWN, x, y, 0, 0);
						mouse_event(MOUSEEVENTF_RIGHTUP, x, y, 0, 0);
						rightclick = 0;
					}
				}
				//if the tracking is off it will do this
				if (b1 != 1) {
					//tracks the eye
					for (int i = 0; i < righteye.size(); i++)
					{
						cv::Point ept1(righteye[i].x + righteye[i].width, righteye[i].y + righteye[i].height);
						cv::Point ept2(righteye[i].x, righteye[i].y);

						rectangle(captureFrame, ept1, ept2, cvScalar(0, 255, 0, 0), 1, 8, 0);
					}
					//if eye is visible resets counter
					if (righteye.size() > 0) {
						turnon = 0;
					}
					//if not the counter goes up
					else if (nose.size() > 0) {
						turnon = turnon + 1;
					}
					//if it reaches the counter then it turns on mouse control
					if (turnon == 12) {
						GetCursorPos(&pt);
						x = pt.x;
						y = pt.y;
						SetCursorPos(x, y);
						b1 = 1;
						turnon = 0;
						//shows a notification, to notify user that it is turned on
						notifyIcon1->BalloonTipText = L"Tracking On";
						notifyIcon1->ShowBalloonTip(3000);
					}
				}
			}
			//if the nose is not tracked, meaning facing away starts counter
			if (nose.size() == 0) {
			turnoff = turnoff + 1;
			}
			
			else {
			turnoff = 0;
			}
			//when it reaches counter then it turns off mouse controll
			if (turnoff == 10) {
			b1 = 0;
			c = 0;
			//shows a notification, to notify user that it is turned off
			notifyIcon1->BalloonTipText = L"Tracking Off";
			notifyIcon1->ShowBalloonTip(3000);
			}
			DrawCVImage2(pictureBox1, captureFrame);
			waitKey(33);

		}
	}
	private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {
		//disables the button and the webcam is in use
		button1->Enabled = false;
		captureDevice.open(0);
		backgroundWorker1->RunWorkerAsync(10);
	}


	private: System::Void button2_Click(System::Object^  sender, System::EventArgs^  e) {
		if (b1 == 0) {
			GetCursorPos(&pt);
			x = pt.x;
			y = pt.y;
			SetCursorPos(x, y);
			b1 = 1;
			button2->Text = L"Tracking";
		}
		else if (b1 == 1) {
			b1 = 0;
			c = 0;
			button2->Text = L"Tracking";
		}
	}

	private: System::Void DrawCVImage2(System::Windows::Forms::Control^ control, cv::Mat& colorImage)
	{
		//draws the Image it converts the image so it can appear on the picturebox
		System::Drawing::Graphics^ graphics = control->CreateGraphics();
		System::IntPtr ptr(colorImage.ptr());
		System::Drawing::Bitmap^ b = gcnew System::Drawing::Bitmap(colorImage.cols, colorImage.rows, colorImage.step, System::Drawing::Imaging::PixelFormat::Format24bppRgb, ptr);
		System::Drawing::RectangleF rect(0, 0, control->Width, control->Height);
		graphics->DrawImage(b, rect);
	}

	};
}
