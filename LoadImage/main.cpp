#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include <iostream>
#include <vector>
#include <locale>
#include <fstream>
#include <sstream>
#include <winsock2.h>
#include <windows.h>
#include <mutex>
#include <ctime>
using namespace std;

#pragma comment(lib,"ws2_32.lib")

struct imName
{
	string url;
	int numb;
	imName(string url, int numb) {
		this->url = url;
		this->numb = numb;
	};
};

mutex m;
vector<imName> urls;
ofstream fout("log.txt");

void writeInfoFile(string info)
{
	m.lock();

	time_t rawtime;
	struct tm * timeinfo;
	char buffer[80];                                // строка, в которой будет храниться текущее время

	time(&rawtime);                               // текущая дата в секундах
	timeinfo = localtime(&rawtime);               // текущее локальное время, представленное в структуре

	strftime(buffer, 80, "%Y-%m-%d %X", timeinfo); // форматируем строку времени
	fout << buffer << " thread: " << this_thread::get_id() << " ";
	fout << info;
	
	m.unlock();
}

DWORD WINAPI downloadImage(LPVOID lpath) {
	imName &pathStruct = *((imName*)lpath);
	WSADATA wsaData;
	SOCKET Socket;
	SOCKADDR_IN SockAddr;
	struct hostent *host;
	char buffer[1000];
	char imageid[1000];
	int i = 0;
	int nDataLength;
	char recbytes[4096];
	string path = pathStruct.url;
	string start = "new thread for image ";
	writeInfoFile(start + path + "\n");

	string imagePath = "";
	string image = "";
	string::size_type pos = path.find("/");
	if (pos != string::npos)
	{
		imagePath = path.substr(pos, path.length() - pos);
		image = path.substr(pos+1, path.length() - pos);
		_itoa(pathStruct.numb, imageid, 10);
		image.insert(image.length() - 4, imageid);
	}
	string hostPath;
	hostPath = path.substr(0, pos);
	//cout << "host " << hostPath << endl;
	//cout << "path " << imagePath << endl;
	//cout << "image " << image << endl;

	string get_http = "GET " + imagePath + " HTTP/1.1\r\nHost: " + hostPath + "\r\nConnection: close\r\n\r\n";
	//cout << get_http;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		cout << "WSAStartup failed.\n";
		writeInfoFile("WSAStartup failed.\n");
		system("pause");
	}

	Socket = socket(AF_INET, SOCK_STREAM, 0); //IPPROTO_TCP
	host = gethostbyname(hostPath.c_str());

	SockAddr.sin_port = htons(80);
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);

	if (connect(Socket, (SOCKADDR*)(&SockAddr), sizeof(SockAddr)) != 0) {
		cout << "Could not connect" << endl;
		writeInfoFile("Could not connect\n");
		system("pause");
	}

	// send GET / HTTP
	send(Socket, get_http.c_str(), strlen(get_http.c_str()), 0);
	fstream temp;

	temp.open(image, ios::out | ios::binary);
	if ((nDataLength = recv(Socket, buffer, sizeof(buffer) - 1, 0)) > 0) {
		_itoa(nDataLength, recbytes, 10);
		string s = recbytes;
		writeInfoFile("received " + s + " bytes\n");
		int i = 0;
		char *cpos = strstr(buffer, "\r\n\r\n");
		temp.write(cpos + strlen("\r\n\r\n"), nDataLength - (cpos - buffer) - strlen("\r\n\r\n"));

	}

	while ((nDataLength = recv(Socket, buffer, 1, 0)) > 0) {
		_itoa(nDataLength, recbytes, 10);
		string s = recbytes;
		writeInfoFile("received "+ s + " bytes\n");
		temp.write(buffer, nDataLength);
	}

	closesocket(Socket);
	WSACleanup();
	Sleep(2000);
	writeInfoFile("Done.\n");
	cout << "Done";;
	return 0;
}

int main(void) {

	setlocale(LC_ALL, "");
	string smth = "localhost/Minions.jpg";
	string smth2 = "localhost/home.jpg";
	int i = 0;
	HANDLE myHandle = NULL;
	while (true)
	{
		DWORD myThreadID;
		string url;
		cout << endl << "Image url: ";  cin >> url;
		if (url == "z") break;
		imName imageInfo(url, i);
		urls.push_back(imageInfo);
		myHandle = CreateThread(0, 0, downloadImage, &urls[i], 0, &myThreadID);
		i++;
	}
	CloseHandle(myHandle);
	fout.close();
	system("pause");
	return 0;
}