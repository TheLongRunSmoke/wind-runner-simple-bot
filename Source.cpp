#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdarg.h>
#include <windows.h>
#include <shellapi.h>
#include <stdlib.h>
#include <stdio.h>
#include <direct.h>
#include <atlbase.h>
#include <atlconv.h>
#include <iostream>
#include <algorithm>
#include <string>
#include <time.h>

using namespace cv;

IplImage* image = 0;
IplImage* numimage = 0;
IplImage* temp = 0;
string path = "";
char* EXPATH;
string TIME;

void Exec(string exeS, string dirS, string paramsS){
	string path = dirS + "\\" + exeS;
	CA2T e(path.c_str());
	LPCTSTR exe = e;
	CA2T p(paramsS.c_str());
	LPCTSTR params = p;
	CA2T d(dirS.c_str());
	LPCTSTR dir = d;
	SHELLEXECUTEINFO ShExecInfo = { 0 };
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = exe; 
	ShExecInfo.lpParameters = params;
	ShExecInfo.lpDirectory = dir;
	ShExecInfo.nShow = SW_SHOW;
	ShExecInfo.hInstApp = NULL;
	if (ShellExecuteEx(&ShExecInfo)){
		WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
	};
	return;
}

void inPrint(cv::Mat img, string str, Point p){
	cv::putText(img, str, p, FONT_HERSHEY_PLAIN, 1, CV_RGB(255, 0, 0), 2);
	return;
}

string getExPath(char* arg){
	string result = "";
	result = arg;
	size_t pos = result.find_last_of("/\\");
	result = result.substr(0, pos);
	pos = 0;
	string out = "";
	while (result.find_first_of("/\\") != std::string::npos){
		pos = result.find_first_of("/\\");
		out += result.substr(0, pos) + "//";
		result = result.substr(pos + 1, result.length());
	}
	return out += result;
}

string OCR(cv::Mat img){
	string result = "";
	double    minval, maxval;
	Point    minloc, maxloc, matchloc;
	Mat src, numImg, numdst, numdst2, dst, dst2, tmpd;
	string filenameNumS;
	std::map <int, char> res;
	cv::cvtColor(img, src, CV_BGR2GRAY);
	cv::threshold(src, src, 120, 255, CV_THRESH_BINARY);
	char sym[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '(', ')', '+', 'd', ',' };
	double weight[] = { 1050000, 400000, 600000, 660000, 750000, 1450000, 1010000, 1010000, 1500000, 600000, 300000, 300000, 600000, 80000, 60000 };
	for (int i = 0; i < sizeof(sym) / sizeof(sym[0]); i++){
		do{
			filenameNumS = getExPath(EXPATH) + "//roi//nums//" + sym[i] + ".png";
			const char *filenameNum = filenameNumS.c_str();
			numimage = cvLoadImage(filenameNum, 0);
			cv::threshold(Mat(numimage), Mat(numimage), 100, 255, CV_THRESH_BINARY);
			tmpd = 0;
			cv::matchTemplate(src, Mat(numimage), tmpd, CV_TM_SQDIFF);
			cv::minMaxLoc(tmpd, &minval, &maxval, &minloc, &maxloc, Mat());
			matchloc = minloc;
			printf("%c", sym[i]);
			printf(" ocr: ");
			printf("%f", minval);
			if (minval < weight[i]){
				cvRectangle(&(IplImage)src, matchloc, Point(matchloc.x + numimage->width - 1, matchloc.y + numimage->height - 1), CV_RGB(0, 0, 0), -1, 4, 0);
				res[matchloc.x] = sym[i];
				printf(" detect: ");
				printf("%c", sym[i]);
			}
			printf("\n");
		} while (minval < weight[i]);
		cvReleaseImage(&numimage);
	}

	for (std::map<int, char>::const_iterator it = res.begin(); it != res.end(); ++it)
	{
		if (it->second != 'd'){
			result += it->second;
			printf("%c", it->second);
		}
		else{
			result += ':';
			printf("%c", ':');
		}
	}
	printf("\n");
	
	return result;
}

string findBoots(cv::Mat img){
	string result = "";
	Mat tmp, roi;
	string filenametemp = getExPath(EXPATH) + "//roi//boots.png";
	const char *filenametempS = filenametemp.c_str();
	temp = cvLoadImage(filenametempS, 1);
	double    minval, maxval;
	Point    minloc, maxloc, matchloc;
	cv::matchTemplate(img, Mat(temp), tmp, CV_TM_SQDIFF);
	cv::minMaxLoc(tmp, &minval, &maxval, &minloc, &maxloc, Mat());
	matchloc = minloc;
	printf("---------------------------\n");
	printf("%f", minval);
	if (minval < 10000000){
		printf("  Boots\n");
		//копирую область интереса
		roi = Mat(img);
		Rect roiR = Rect(matchloc.x + 50, matchloc.y, temp->width - 1 - 50, temp->height - 1);
		result = OCR(roi(roiR));
		//выделяю область
		cvRectangle(&(IplImage)img, matchloc, Point(matchloc.x + temp->width - 1, matchloc.y + temp->height - 1), CV_RGB(255, 0, 0), 2, 4, 0);
		inPrint(img, "Boots", Point(matchloc.x + 5, matchloc.y + 12));
	}
	else{
		printf("\n");
	}
	return result;
}

bool findSome(cv::Mat img, int object){
	bool result = false;
	Mat tmp;
	double limit = 0;
	double minlimit = 0;
	switch (object){
		case 0: { //обработчик для кнопки Run
			string filenametemp = getExPath(EXPATH) + "//roi//run.png";
			const char *filenametempS = filenametemp.c_str();
			temp = cvLoadImage(filenametempS, 1);
			limit = 50000;
			break;
		}
		case 1:{ //обработчик для Next
			string filenametemp = getExPath(EXPATH) + "//roi//run.png";
			const char *filenametempS = filenametemp.c_str();
			temp = cvLoadImage(filenametempS, 1);
			limit = 150000000;
			minlimit = 100000;
			break;
		}
		case 11:{ //обработчик для Next в конце пробега
				   string filenametemp = getExPath(EXPATH) + "//roi//run.png";
				   const char *filenametempS = filenametemp.c_str();
				   temp = cvLoadImage(filenametempS, 1);
				   limit = 300000000;
				   break;
		}
		case 2:{ //обработчик для Назад
			string filenametemp = getExPath(EXPATH) + "//roi//back.png";
			const char *filenametempS = filenametemp.c_str();
			temp = cvLoadImage(filenametempS, 1);
			limit = 50000;
			break;
		}
		case 3:{ //обработчик для ботинок
				   string filenametemp = getExPath(EXPATH) + "//roi//boots.png";
				   const char *filenametempS = filenametemp.c_str();
				   temp = cvLoadImage(filenametempS, 1);
				   limit = 10000000;
				   break;
		}
		default:{
			break;
		}
	}
	cv::matchTemplate(img, Mat(temp), tmp, CV_TM_SQDIFF);
	double    minval, maxval;
	Point    minloc, maxloc, matchloc;
	cv::minMaxLoc(tmp, &minval, &maxval, &minloc, &maxloc, Mat());
	matchloc = minloc;
	printf("---------------------------\n");
	printf("%f", minval);
	if ((minval < limit) && (minval > minlimit)){
		result = true;
		switch (object){
		case 0:{
				   printf("  Run\n");
				   inPrint(img, "Run", Point(matchloc.x + 5, matchloc.y + 12));
				   cvRectangle(&(IplImage)img, matchloc, Point(matchloc.x + temp->width - 1, matchloc.y + temp->height - 1), CV_RGB(255, 0, 0), 2, 4, 0);
				   break;
		}
		case 1:{
				   printf("  Next\n");
				   inPrint(img, "Next", Point(matchloc.x + 5, matchloc.y + 12));
				   cvRectangle(&(IplImage)img, matchloc, Point(matchloc.x + temp->width - 1, matchloc.y + temp->height - 1), CV_RGB(255, 0, 0), 2, 4, 0);
				   break;
		}
		case 11:{
				   printf("  Next\n");
				   inPrint(img, "Next", Point(matchloc.x + 5, matchloc.y + 12));
				   cvRectangle(&(IplImage)img, matchloc, Point(matchloc.x + temp->width - 1, matchloc.y + temp->height - 1), CV_RGB(255, 0, 0), 2, 4, 0);
				   break;
		}
		case 2:{
				   printf("  Back\n");
				   inPrint(img, "Back", Point(matchloc.x + 5, matchloc.y + 12));
				   cvRectangle(&(IplImage)img, matchloc, Point(matchloc.x + temp->width - 1, matchloc.y + temp->height - 1), CV_RGB(255, 0, 0), 2, 4, 0);
				   break;
		}
		case 3:{
				   printf("  Boots\n");
				   break;
		}
		default:{
				   printf("\n");
				   break;
		}
		}
	}
	else{
		printf("\n");
	}
	return result;
}

Mat getScreen(){
	Mat res;
	//получаю скрин
	Exec("//screen.bat", getExPath(EXPATH) + "//adb", "");
	// имя картинки задаётся первым параметром
	string filenameS = getExPath(EXPATH) + "//adb//screen.png";
	const char *filename = filenameS.c_str();
	image = cvLoadImage(filename, 1);
	//поворачиваю скрин
	cv::transpose(Mat(image), res);
	cv::flip(res, res, 0);
	return res;
}

int main(int argc, char** argv)
{
	IplImage *src = 0, *boots = 0;
	Mat tmp, roi;

	EXPATH = argv[0];

	bool isWait = false;

	time_t rawtime;
	std::tm* timeinfo;
	char TIMEc[80];
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(TIMEc, 80, "%Y-%m-%d-%H-%M-%S", timeinfo);
	TIME = TIMEc;
	// инициализация таймера
	HANDLE hTimer = NULL;
	LARGE_INTEGER TimeDelay;
	hTimer = CreateWaitableTimer(NULL, TRUE, NULL);

	cvNamedWindow("screen", CV_WINDOW_AUTOSIZE);

	string filenameBootsS = getExPath(argv[0]) + "//roi//boots.png";
	const char *filenameBoots = filenameBootsS.c_str();
	boots = cvLoadImage(filenameBoots, 1);
	printf("Press any key, for start.\n");
	waitKey(0);
	while (1){
		//получаю скрин и загружаю его в память 
		src = cvCloneImage(&(IplImage)getScreen());
		//определяю экран
		if (findSome(src, 1) && findSome(src, 3)){ //есть кнопка далее и ботинки
			//найти количесво ботинок или время необходимое для востановления
			printf("Main screen.\n");
			string count = findBoots(src);
			if (count.length() > 3){
				string min = count.substr(1, 2);
				string sec = count.substr(4, 2);
				string number = count.substr(8, 3);
				if (std::stoi(number) == 0){ //если ботинки кончились, то надо подождать...
					int secdelay = std::stoi(min) * 60 + std::stoi(sec);
					printf("Need wait for %i seconds\n", secdelay);
					if (secdelay > 120){ // Таймер глючит при больших периодах ожидания
						TimeDelay.QuadPart = -1 * 120 * 1000 * 1000 * 10;
						printf("Wait for %i seconds\n", 120);
					}
					else{
						TimeDelay.QuadPart = -1 * secdelay * 1000 * 1000 * 10;
						printf("Wait for %i seconds\n", secdelay);
					}
					isWait = true;
					SetWaitableTimer(hTimer, &TimeDelay, 0, NULL, NULL, 0);
				}
				else{  //если ботинки есть, то надо бегать
					printf("Go to prerun screen.\n");
					Exec("//tapNext.bat", getExPath(argv[0]) + "//adb", "");
					Sleep(2500);
				}	
			}
			else{
				printf("Go to prerun screen.\n");
				Exec("//tapNext.bat", getExPath(argv[0]) + "//adb", "");
				Sleep(2500);
			}
		}
		else{
			if (findSome(src, 0) && findSome(src, 3)){ //Если есть кнопка Run и ботинки
				printf("Prerun screen.\n");
				string countb = findBoots(src);
				if (std::stoi(countb) > 0){ //Если можно бегать, то вперёд
					printf("let's run...");
					Exec("//tapNext.bat", getExPath(argv[0]) + "//adb", "");
					Sleep(2500);
					TimeDelay.QuadPart = -1 * 65 * 1000 * 1000 * 10; //Просто подожду 65 секунд.
					isWait = true;
					SetWaitableTimer(hTimer, &TimeDelay, 0, NULL, NULL, 0);
				}
				else{ //Если попыток нет, то надо вернутся и подождать
					printf("Back to main screen.\n");
					Exec("//tapBack.bat", getExPath(argv[0]) + "//adb", "");
				}
			}
			else{
				if (findSome(src, 11) && !findSome(src, 3)){ //Если кнопка Next не на черном фоне, и ботинки не отображаются
					printf("Run ended.\n");
					Exec("//tapNext.bat", getExPath(argv[0]) + "//adb", "");
					Sleep(2500);
				}
				else{ //Случилась фигня неясной этимологии, жду 5 секунд и пробую снова.
					printf("Something wrong...strange.");
					TimeDelay.QuadPart = -1 * 5 * 1000 * 1000 * 10;
					isWait = true;
					SetWaitableTimer(hTimer, &TimeDelay, 0, NULL, NULL, 0);
				}
			}
		}
		cvShowImage("screen", src);
		char key = cvWaitKey(1000);
		
		printf("\n");
		if (isWait){
			if (WaitForSingleObject(hTimer, INFINITE) != WAIT_OBJECT_0)
				printf("WaitForSingleObject failed (%d)\n", GetLastError());
			else printf("Wait over, now!\n");
			isWait = false;
		}

		if (key == 27)            //если нажат Esc завершаю программу
			break;
	}
	// освобождаю ресурсы
	cvReleaseImage(&src);
	cvReleaseImage(&boots);
	// удаляю окна
	cvDestroyAllWindows();
	return 0;
};