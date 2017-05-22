#include <vector>
#include <fstream>

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string.h>

#pragma comment(lib, "wsock32.lib")
#pragma comment(lib, "ws2_32.lib")


#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

std::string robo1 = "robot1"; //ロボット１画像表示ウィンドウ名
std::string robo2 = "robot2"; //ロボット２画像表示ウィンドウ名
std::string robo3 = "robot3"; //ロボット３画像表示ウィンドウ名


/*	接続先のIPアドレスやポート番号の情報を保持するための構造体
struct sockaddr_in {
u_char sin_family;       アドレスファミリ
u_short sin_port;        ポート番号
struct in_addr sin_addr; IPアドレス
};
*/


int main() {

	int port = 9877; //ポート番号（任意）
	int recvsocket;  //ソケットディスクリプタ戻り値用変数

	struct sockaddr_in recvSockAddr; //受信用
	struct sockaddr_in clientSockAddr; //送信用

	int clientaddrlen = sizeof(clientSockAddr);

	//各種パラメータ
	int status;
	int numrcv;


	WSADATA data; //Windows Socket仕様に関する情報を格納するための構造体
	SOCKET sock;  //socetディスクリプタ
	WSAStartup(MAKEWORD(2, 0), &data); //winsockの初期化
	memset(&recvSockAddr, 0, sizeof(recvSockAddr));

	recvSockAddr.sin_family = AF_INET; //アドレスファミリ（インターネット通信）
	recvSockAddr.sin_port = htons(port);//ポート番号
	recvSockAddr.sin_addr.S_un.S_addr = INADDR_ANY; //受信アドレス(すべてのIPアドレスを待ち受ける)

													//ソケット作成 : エラーの場合、INVALID＿SOCKETを返す。

													//recvsocket = socket(AF_INET,SOCK_DGRAM,0);
	sock = socket(AF_INET, SOCK_DGRAM, 0); //socketディスクリプタの生成
	if (sock == INVALID_SOCKET) {   //エラー処理
		std::cout << "socket : " << WSAGetLastError() << std::endl;
		return 2;
	}

	//bind :　ソケットをアドレス、ポートに結び付ける　正常の場合0が返る。
	status = bind(sock, (LPSOCKADDR)&recvSockAddr, (int)sizeof(recvSockAddr));
	if (status != 0) {  //エラー処理
		std::cout << "bind : " << WSAGetLastError() << std::endl;
		return 3;
	}


	cv::Mat image = cv::Mat(320, 240, CV_8UC3);
	cv::Mat copyImg;
	cv::namedWindow(robo1,1);
	cv::namedWindow(robo2, 1);
	cv::namedWindow(robo3, 1);
	cv::moveWindow(robo1,1000,0);
	cv::moveWindow(robo2, 100, 500);
	cv::moveWindow(robo3, 1000, 500);
	static const int receivesize = 65500; //受信パケットの最大容量
	static char buff[receivesize]; //受信buffの初期化

	std::vector<uchar> ibuff;


	std::cout << "Image Server Setup" << std::endl;

	while (cv::waitKey(10)) {
		//クライアントからのデータの受信
		numrcv = recvfrom(sock, buff, receivesize, 0, (LPSOCKADDR)&clientSockAddr, &clientaddrlen);
		if (numrcv == -1) {  //エラー処理
			std::cout << "error : " << WSAGetLastError() << std::endl;
			status = closesocket(sock);
			break;
		}

		for (int i = 0; i < sizeof(buff); i++) {
			ibuff.push_back((uchar)buff[i]);
		}

		image = imdecode(cv::Mat(ibuff), CV_LOAD_IMAGE_COLOR); //受信buffをデコーディング
		image.copyTo(copyImg);
		//IPが1号機のものである場合
		if (clientSockAddr.sin_addr.S_un.S_addr == inet_addr("192.168.2.10"))
		{
			cv::resize(copyImg, copyImg, cv::Size(), 2, 2);
			cv::imshow(robo1, copyImg);
		}
		//IPが2号機のものである場合
		if (clientSockAddr.sin_addr.S_un.S_addr == inet_addr("192.168.2.22")) {
			cv::resize(copyImg, copyImg, cv::Size(), 2, 2);
			cv::imshow(robo2, copyImg);
		}
		//IPが3号機のものである場合
		//if (clientSockAddr.sin_addr.S_un.S_addr = inet_addr("192.168.2.30")){

		//cv::imshow(robo3,image);
		//}

		ibuff.clear();
	}

	//ソケットのクローズ
	closesocket(sock);
	WSACleanup();

	return 1;
}