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

SYSTEMTIME st;
ofstream fout("loooo.txt");
mutex m;
vector<imName> urls;
vector<HANDLE> handles;

DWORD WINAPI downloadImage(LPVOID lpath) {
	imName &pathStruct = *((imName*)lpath);
	auto myid = this_thread::get_id();
	stringstream ss;
	ss << myid;
	string mystring = ss.str();
	string j = "dg";
	GetLocalTime(&st);
	fout << st.wDay << "." << st.wMonth << "." << st.wYear << " " << st.wHour << ":" << st.wMinute << ":" << st.wSecond << " ";
	fout << "new thread " << this_thread::get_id() << "for image "<< pathStruct.url <<endl;
	//cout << "new thread " << this_thread::get_id() <<endl;
	WSADATA wsaData;
	SOCKET Socket;
	SOCKADDR_IN SockAddr;
	struct hostent *host;
	char buffer[1000];
	char imageid[1000];
	int i = 0;
	int nDataLength;
	//cout << "jhfgj";
	//string &path = *((string*)lpath);
	string path = pathStruct.url;

	string imagePath = "";
	string image = "";
	cout << path << endl;
	string::size_type pos = path.find("/");
	if (pos != string::npos)
	{
		imagePath = path.substr(pos, path.length() - pos);
		image = path.substr(pos+1, path.length() - pos);
		_itoa(pathStruct.numb, imageid, 10);
		image.insert(image.length() - 4, imageid);
		cout << "!!!" << image << "!!!" << endl;
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
		system("pause");
		//return 1;
	}

	Socket = socket(AF_INET, SOCK_STREAM, 0); //IPPROTO_TCP
	host = gethostbyname(hostPath.c_str());

	SockAddr.sin_port = htons(80);
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);

	if (connect(Socket, (SOCKADDR*)(&SockAddr), sizeof(SockAddr)) != 0) {
		cout << "Could not connect";
		system("pause");
	}

	// send GET / HTTP
	send(Socket, get_http.c_str(), strlen(get_http.c_str()), 0);
	fstream temp;

	temp.open(image, ios::out | ios::binary);
	if ((nDataLength = recv(Socket, buffer, sizeof(buffer) - 1, 0)) > 0) {
		int i = 0;
		char *cpos = strstr(buffer, "\r\n\r\n");
		int rec = nDataLength - (cpos - buffer) - strlen("\r\n\r\n");
		temp.write(cpos + strlen("\r\n\r\n"), rec);
		GetLocalTime(&st);
		fout << st.wDay << "." << st.wMonth << "." << st.wYear << " " << st.wHour << ":" << st.wMinute << ":" << st.wSecond << " ";
		fout << "thread " << this_thread::get_id()<< "received " <<rec<< "bytes" << endl;
	}

	while ((nDataLength = recv(Socket, buffer, 1, 0)) > 0) {
		temp.write(buffer, nDataLength);
		GetLocalTime(&st);
		fout << st.wDay << "." << st.wMonth << "." << st.wYear << " " << st.wHour << ":" << st.wMinute << ":" << st.wSecond << " ";
		fout << "thread " << this_thread::get_id() << "received " << nDataLength << "bytes" << endl;
	}
	GetLocalTime(&st);
	fout << st.wDay << "." << st.wMonth << "." << st.wYear << " " << st.wHour << ":" << st.wMinute << ":" << st.wSecond << " ";
	fout << "thread " << this_thread::get_id() << "download finished " << nDataLength << "bytes" << endl;
	closesocket(Socket);
	WSACleanup();
	Sleep(2000);
	cout << "Done";;
	return 0;
}

int main(void) {

		int i = 0;
	//while (true)
	//{
		DWORD myThreadID;
		HANDLE myHandle;
		string url;
		cout << endl << "Image url: ";  cin >> url;
		//if (url == "z") break;
		imName imageInfo(url, i);
		urls.push_back(imageInfo);
		myHandle = CreateThread(0, 0, downloadImage, &urls[i], 0, &myThreadID);
		i++;
	//}
	//fclose(pFile);
		//fout.close();
	system("pause");
	return 0;
}