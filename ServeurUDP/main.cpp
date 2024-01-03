#include <iostream>
#include <cstdio>
#include <stdio.h>
#include <ctime>
#include <cstring>

#ifdef _WIN32
#include <Windows.h>
#include <lmcons.h>
#elif __linux__
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
typedef int SOCKET;
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) 
#define LOGIN_NAME_MAX 50
#else
#error "Système d'exploitation non supporté"
#endif
#define PORT 6666

int main() {

#ifdef _WIN32
	WSADATA WSAData;
	if (WSAStartup(0x0101, &WSAData) != 0) {
		std::cout << "WSAStartup failed" << std::endl;
		return 1;
	}
#endif

	SOCKET sd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sd == INVALID_SOCKET) {
		std::cout << "socket() failed" << std::endl;
#ifdef _WIN32
		WSACleanup();
#endif
		return 1;
	}

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
#ifdef _WIN32
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	#else
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
#endif

	if (bind(sd, (sockaddr*)&addr, sizeof(addr)) != 0) {
		std::cout << "bind() failed pour le port" << std::endl;
		closesocket(sd);
#ifdef _WIN32
		WSACleanup();
#endif
		return 1;
	}

	sockaddr_in client;
	char buffer[2048];

	bool stop = false;

	while (!stop) {
#ifdef _WIN32
		int clientlen = sizeof(client);
		int count = recvfrom(sd, buffer, sizeof(buffer), 0, (sockaddr*)&client, &clientlen);
#else
		socklen_t clientlen = sizeof(client);
		int count = recvfrom(sd, buffer, sizeof(buffer), 0, (sockaddr*)&client, &clientlen);
#endif
		if (count < 0) {
			std::cout << "recvfrom() failed" << std::endl;
			closesocket(sd);
#ifdef _WIN32
			WSACleanup();
#endif
			return 1;
		}

		buffer[count] = 0;
		std::cout << "RECU: " << buffer << std::endl;
		std::string commande(buffer);

		if (commande.find("echo ") == 0)
		{
			std::string parametre = commande.substr(commande.find(' ') + 1);
			sendto(sd, parametre.c_str(), parametre.length(), 0, (const sockaddr*)&client, sizeof(client));
		}
		else if (commande.find("ping ") == 0) {
			std::string parametre = "pong";
			sendto(sd, parametre.c_str(), parametre.length(), 0, (const sockaddr*)&client, sizeof(client));
		}
		else if (commande.find("date ") == 0) {
			time_t now = time(0);
			tm ltm;
#ifdef _WIN32
			localtime_s(&ltm, &now);
#else
			localtime_r(&now, &ltm);
#endif
			char date[80];
			strftime(date, 80, "%a %b %d %H:%M:%S %Y", &ltm);
			std::string parametre = date;
			sendto(sd, parametre.c_str(), parametre.length(), 0, (const sockaddr*)&client, sizeof(client));
		}
		else if (commande.find("usager ") == 0) {

#ifdef _WIN32 
			char username[UNLEN + 1];
			DWORD username_len = UNLEN + 1;
			if (GetUserName(username, &username_len)) {
				std::string parametre = username;
				sendto(sd, parametre.c_str(), parametre.length(), 0, (const sockaddr*)&client, sizeof(client));
			}
#else
			char username[LOGIN_NAME_MAX + 1];
			if (getlogin_r(username, LOGIN_NAME_MAX) == 0) {
				std::string parametre = username;
				sendto(sd, parametre.c_str(), parametre.length(), 0, (const sockaddr*)&client, sizeof(client));
			}
#endif

		}
		else if (commande.find("exec ") == 0) {
			std::string parametre = commande.substr(commande.find(' ') + 1);
			char buff[1024];
			std::string result;

			FILE* pipe;

#ifdef _WIN32 
			pipe = _popen(parametre.c_str(), "r");
#else 
			pipe = popen(parametre.c_str(), "r");
#endif

			size_t bytesRead = 0;
			int linesRead = 0;
			bool outputFound = false;

			while (fgets(buffer, sizeof(buffer), pipe) != NULL && linesRead < 10 && bytesRead < 300) {
				bytesRead += strlen(buffer);
				result.append(buffer);
				linesRead++;
				outputFound = true;
			}

#ifdef _WIN32 
			_pclose(pipe);
#else 
			pclose(pipe);
#endif

			if (outputFound)
				sendto(sd, result.c_str(), result.length(), 0, (const sockaddr*)&client, sizeof(client));
			else
				sendto(sd, "OK", 2, 0, (const sockaddr*)&client, sizeof(client));
		}
		else if (commande.find("bye ") == 0) {
			stop = true;
			std::string parametre = "";
			sendto(sd, parametre.c_str(), parametre.length(), 0, (const sockaddr*)&client, sizeof(client));
		}
		else {
			sendto(sd, "ERREUR", 7, 0, (const sockaddr*)&client, sizeof(client));
		}

	}

	closesocket(sd);
#if defined(_WIN32)
	WSACleanup();
#endif
}