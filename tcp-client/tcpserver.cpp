// tcpclient.cpp: ���������� ����� ����� ��� ����������� ����������.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <Winsock2.h>

#define SERVICE_PORT 1500

int send_string(SOCKET s, const char * sString);

int main()
{
	SOCKET S;
	SOCKET NS;

	sockaddr_in serv_addr;

	WSADATA wsadata;

	char sName[128];
	bool bTerminate = false;

	// ������������� ���������� �������
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	// ������� ��������� ����� ������� ������
	gethostname(sName, sizeof(sName));
	printf("\nServer host: %s\n", sName);
	
	// �������� tcp-������ 
	if ((S = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		fprintf(stderr, "Can't create socket\n");
		exit(1);
	}

	// ��������� ��������� �������
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	
	// ��������� ������ �� ���� ��������� ������� �����������,
	// � ��������� �� localhost
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	// �������� �������� �� �������������� ������� ����
	serv_addr.sin_port = htons((u_short)SERVICE_PORT);

	// ��������� ����� � �������� ������� ���������� � ������
	if (bind(S, (sockaddr*)&serv_addr, sizeof(serv_addr)) == INVALID_SOCKET)
	{
		fprintf(stderr, "Can't bind\n");
		exit(1);
	}

	// ������� ������ � ����� ������������� ��������� �����
	// � ������������ ����������� �������� �������� �� ���������� 5
	if (listen(S, 5) == INVALID_SOCKET)
	{
		fprintf(stderr, "Can't listen\n");
		exit(1);
	}

	printf("Server listen on %s:%d\n", inet_ntoa(serv_addr.sin_addr), ntohs(serv_addr.sin_port));

	// �������� ���� ��������� ����������� ��������
	while (!bTerminate)
	{
		printf("Wait for connections.....\n");

		sockaddr_in clnt_addr;
		int addrlen = sizeof(clnt_addr);
		memset(&clnt_addr, 0, sizeof(clnt_addr));

		// ��������� ������ � ����� �������� ������� �� ����������
		// ����� ����������, �.�. ���������� ���������� ������
		// ��� ����������� ������� ��� ������
		NS = accept(S, (sockaddr*)&clnt_addr, &addrlen);
		if (NS == INVALID_SOCKET)
		{
			fprintf(stderr, "Can't accept connection\n");
			break;
		}

		// �������� ��������� ��������������� ������ NS
		// � ���������� � �������
		addrlen = sizeof(serv_addr);
		getsockname(NS, (sockaddr*)&serv_addr, &addrlen);

		// ������� inet_ntoa ���������� ��������� �� ���������� �����
		// ������� ������������ �� � ����� ������ printf �� ���������
		printf("Accepted connection on %s:%d ",
			inet_ntoa(serv_addr.sin_addr), ntohs(serv_addr.sin_port));
		printf("from client %s:%d\n",
			inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));
	}

	send_string(NS, "* * * Welcome to simple UPCASE TCP-server * * *\r\n");
	char sReceiveBuffer[1024] = { 0 };
	
	// �������� � ������������ ������ �� �������
	while (true)
	{
		int nReaded = recv(NS, sReceiveBuffer, sizeof(sReceiveBuffer) - 1, 0);
		
		// � ������ ������ (��������, ������������ �������) ������
		if (nReaded <= 0) break;
		
		// �������� ����� ����, ������� ����� ��������������
		// �������� ����������� 0 ��� ASCII ������
		sReceiveBuffer[nReaded] = 0;

		// ����������� ������� �������� �����
		for (char* pPtr = sReceiveBuffer; *pPtr != 0; pPtr++)
		{
			if (*pPtr == '\n' || *pPtr == '\r')
			{
				*pPtr = 0;
				break;
			}
		}

		// ���������� ������ ������
		if (sReceiveBuffer[0] == 0) continue;

		printf("Received data: %s\n", sReceiveBuffer);

		// ����������� ���������� ������� ��� ����������� ����� � ������� �������
		if (strcmp(sReceiveBuffer, "info") == 0)
		{
			send_string(NS, "Test TCP-server.\r\n");
		}
		else if (strcmp(sReceiveBuffer, "exit") == 0)
		{
			send_string(NS, "Bye...\r\n");
			printf("Client initialize disconnection.\r\n");
			break;
		}
		else if (strcmp(sReceiveBuffer, "shutdown") == 0)
		{
			send_string(NS, "Server go to shutdown.\r\n");
			Sleep(200);
			bTerminate = true;
			break;
		}
		else
		{
			// ����������� ������ � ������� �������
			char sSendBuffer[1024];
			_snprintf(sSendBuffer, sizeof(sSendBuffer), "Server reply: %s\r\n",
				_strupr(sReceiveBuffer));
			send_string(NS, sSendBuffer);
		}
	}

	// ��������� ��������� �����
	closesocket(S);

	// ����������� ������� ���������� �������
	WSACleanup();
	
	return 0;
}

int send_string(SOCKET s, const char * sString)
{
	return send(s, sString, strlen(sString), 0);
}