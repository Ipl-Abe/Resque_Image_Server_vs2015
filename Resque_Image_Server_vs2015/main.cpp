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

std::string robo1 = "robot1"; //���{�b�g�P�摜�\���E�B���h�E��
std::string robo2 = "robot2"; //���{�b�g�Q�摜�\���E�B���h�E��
std::string robo3 = "robot3"; //���{�b�g�R�摜�\���E�B���h�E��


/*	�ڑ����IP�A�h���X��|�[�g�ԍ��̏���ێ����邽�߂̍\����
struct sockaddr_in {
u_char sin_family;       �A�h���X�t�@�~��
u_short sin_port;        �|�[�g�ԍ�
struct in_addr sin_addr; IP�A�h���X
};
*/


int main() {

	int port = 9877; //�|�[�g�ԍ��i�C�Ӂj
	int recvsocket;  //�\�P�b�g�f�B�X�N���v�^�߂�l�p�ϐ�

	struct sockaddr_in recvSockAddr; //��M�p
	struct sockaddr_in clientSockAddr; //���M�p

	int clientaddrlen = sizeof(clientSockAddr);

	//�e��p�����[�^
	int status;
	int numrcv;


	WSADATA data; //Windows Socket�d�l�Ɋւ�������i�[���邽�߂̍\����
	SOCKET sock;  //socet�f�B�X�N���v�^
	WSAStartup(MAKEWORD(2, 0), &data); //winsock�̏�����
	memset(&recvSockAddr, 0, sizeof(recvSockAddr));

	recvSockAddr.sin_family = AF_INET; //�A�h���X�t�@�~���i�C���^�[�l�b�g�ʐM�j
	recvSockAddr.sin_port = htons(port);//�|�[�g�ԍ�
	recvSockAddr.sin_addr.S_un.S_addr = INADDR_ANY; //��M�A�h���X(���ׂĂ�IP�A�h���X��҂��󂯂�)

													//�\�P�b�g�쐬 : �G���[�̏ꍇ�AINVALID�QSOCKET��Ԃ��B

													//recvsocket = socket(AF_INET,SOCK_DGRAM,0);
	sock = socket(AF_INET, SOCK_DGRAM, 0); //socket�f�B�X�N���v�^�̐���
	if (sock == INVALID_SOCKET) {   //�G���[����
		std::cout << "socket : " << WSAGetLastError() << std::endl;
		return 2;
	}

	//bind :�@�\�P�b�g���A�h���X�A�|�[�g�Ɍ��ѕt����@����̏ꍇ0���Ԃ�B
	status = bind(sock, (LPSOCKADDR)&recvSockAddr, (int)sizeof(recvSockAddr));
	if (status != 0) {  //�G���[����
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
	static const int receivesize = 65500; //��M�p�P�b�g�̍ő�e��
	static char buff[receivesize]; //��Mbuff�̏�����

	std::vector<uchar> ibuff;


	std::cout << "Image Server Setup" << std::endl;

	while (cv::waitKey(10)) {
		//�N���C�A���g����̃f�[�^�̎�M
		numrcv = recvfrom(sock, buff, receivesize, 0, (LPSOCKADDR)&clientSockAddr, &clientaddrlen);
		if (numrcv == -1) {  //�G���[����
			std::cout << "error : " << WSAGetLastError() << std::endl;
			status = closesocket(sock);
			break;
		}

		for (int i = 0; i < sizeof(buff); i++) {
			ibuff.push_back((uchar)buff[i]);
		}

		image = imdecode(cv::Mat(ibuff), CV_LOAD_IMAGE_COLOR); //��Mbuff���f�R�[�f�B���O
		image.copyTo(copyImg);
		//IP��1���@�̂��̂ł���ꍇ
		if (clientSockAddr.sin_addr.S_un.S_addr == inet_addr("192.168.2.10"))
		{
			cv::resize(copyImg, copyImg, cv::Size(), 2, 2);
			cv::imshow(robo1, copyImg);
		}
		//IP��2���@�̂��̂ł���ꍇ
		if (clientSockAddr.sin_addr.S_un.S_addr == inet_addr("192.168.2.22")) {
			cv::resize(copyImg, copyImg, cv::Size(), 2, 2);
			cv::imshow(robo2, copyImg);
		}
		//IP��3���@�̂��̂ł���ꍇ
		//if (clientSockAddr.sin_addr.S_un.S_addr = inet_addr("192.168.2.30")){

		//cv::imshow(robo3,image);
		//}

		ibuff.clear();
	}

	//�\�P�b�g�̃N���[�Y
	closesocket(sock);
	WSACleanup();

	return 1;
}