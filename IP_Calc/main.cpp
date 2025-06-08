#define _CRT_SECURE_NO_WARNINGS
#include<Windows.h>
#include<CommCtrl.h>
#include<iostream>
#include<Shlwapi.h>
#include"resource.h"

BOOL CALLBACK DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
VOID Info(HWND hWnd);


INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT CmdShow)
{
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG_MAIN), NULL, DlgProc, 0);
	return 0;
}

BOOL CALLBACK DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		SendMessage(GetDlgItem(hWnd, IDC_SPIN_PREFIX), UDM_SETRANGE, 0, MAKELPARAM(30, 1));
		SetFocus(GetDlgItem(hWnd, IDC_IPADDRESS));
		break;
	case WM_COMMAND:
	{
		HWND hIPaddress = GetDlgItem(hWnd, IDC_IPADDRESS);
		HWND hIPmask = GetDlgItem(hWnd, IDC_IPMASK);
		HWND hPrefix = GetDlgItem(hWnd, IDC_EDIT_PREFIX);
		DWORD dwIPaddress = 0;
		DWORD dwIPmask = 0;
		switch (LOWORD(wParam))
		{

		case IDC_IPADDRESS:
			if (HIWORD(wParam) == EN_CHANGE)
			{
				SendMessage(hIPaddress, IPM_GETADDRESS, 0, (LPARAM)&dwIPaddress);
				if (FIRST_IPADDRESS(dwIPaddress) < 128)SendMessage(hIPmask, IPM_SETADDRESS, 0, 0xFF000000);
				else if (FIRST_IPADDRESS(dwIPaddress) < 192)SendMessage(hIPmask, IPM_SETADDRESS, 0, 0xFFFF0000);
				else if (FIRST_IPADDRESS(dwIPaddress) < 224)SendMessage(hIPmask, IPM_SETADDRESS, 0, 0xFFFFFF00);
			}
			break;

		case IDC_EDIT_PREFIX:
		{
			if (HIWORD(wParam) == EN_CHANGE)
			{
				DWORD dwIPMask = UINT_MAX;
				CHAR chPrefix[3]{};
				SendMessage(hPrefix, WM_GETTEXT, 3, (LPARAM)chPrefix);
				BYTE bCount = atoi(chPrefix);
				dwIPMask = ~(dwIPMask >> bCount);
				SendMessage(hIPmask, IPM_SETADDRESS, 0, dwIPMask);
				Info(hWnd);
			}
		}
		break;
		case IDOK:
		{
			CONST INT SIZE = 256;
			CHAR bufer_netIP[SIZE];
			CHAR bufer_broadcastIP[SIZE];
			CHAR bufer_numberIP[SIZE];
			CHAR bufer_nodesIP[SIZE];
			CHAR bufer_info[SIZE];
			SendMessage(hIPmask, IPM_GETADDRESS, 0, (LPARAM)&dwIPmask);
			SendMessage(hIPaddress, IPM_GETADDRESS, 0, (LPARAM)&dwIPaddress);
			DWORD dwNetIP = dwIPaddress & dwIPmask;
			DWORD dwBroadcastIP = dwIPaddress | ~ dwIPmask;
			DWORD dwnumberIP = ~dwIPmask+1;
			DWORD dwnodesIP = dwnumberIP - 2;
			sprintf(bufer_netIP, "%i.%i.%i.%i", FIRST_IPADDRESS(dwNetIP), SECOND_IPADDRESS(dwNetIP), THIRD_IPADDRESS(dwNetIP), FOURTH_IPADDRESS(dwNetIP));
			sprintf(bufer_broadcastIP, "%i.%i.%i.%i", FIRST_IPADDRESS(dwBroadcastIP), SECOND_IPADDRESS(dwBroadcastIP), THIRD_IPADDRESS(dwBroadcastIP), FOURTH_IPADDRESS(dwBroadcastIP));
			sprintf(bufer_numberIP, "%i", dwnumberIP);
			sprintf(bufer_nodesIP, "%i", dwnodesIP);
			sprintf(bufer_info,
				"Адрес сети:\t\t\%s\nШироковещательный адрес:\t%s\nКоличество IP-адресов:\t%s\nКоличество узлов:\t\t%s",
				bufer_netIP, bufer_broadcastIP, bufer_numberIP, bufer_nodesIP);
			MessageBox(hWnd,bufer_info,"INFO", MB_ICONINFORMATION);
		}
		break;
		case IDCANCEL:
			EndDialog(hWnd, 0);
		}
	}
	break;
	case WM_NOTIFY:
	{
		if (wParam == IDC_IPMASK || wParam == IDC_IPADDRESS)
		{
			HWND hIPmask = GetDlgItem(hWnd, IDC_IPMASK);
			HWND hPrefix = GetDlgItem(hWnd, IDC_EDIT_PREFIX);
			DWORD dwIPmask = 0;
			SendMessage(hIPmask, IPM_GETADDRESS, 0, (LPARAM)&dwIPmask);
			DWORD dwCount = 0;
			while (dwIPmask & 0x80000000)
			{
				dwCount++;
				dwIPmask <<= 1;
			}
			CHAR chPrefix[3] = "";
			sprintf(chPrefix, "%i", dwCount);
			SendMessage(hPrefix, WM_SETTEXT, 0, (LPARAM)chPrefix);
		}
			Info(hWnd);
	}
	break;
	case WM_CLOSE:
		EndDialog(hWnd, 0);
	}

	return FALSE;

}

VOID Info(HWND hWnd)
{
	CONST INT SIZE = 1024;
	CHAR bufer_netIP[SIZE];
	CHAR bufer_broadcastIP[SIZE];
	CHAR bufer_numberIP[SIZE];
	CHAR bufer_nodesIP[SIZE];
	CHAR bufer_info[SIZE];
	HWND hIPaddress = GetDlgItem(hWnd, IDC_IPADDRESS);
	HWND hIPmask = GetDlgItem(hWnd, IDC_IPMASK);
	HWND hInfo = GetDlgItem(hWnd, IDC_STATIC_INFO);
	DWORD dwIPaddress = 0;
	DWORD dwIPmask = 0;
	SendMessage(hIPmask, IPM_GETADDRESS, 0, (LPARAM)&dwIPmask);
	SendMessage(hIPaddress, IPM_GETADDRESS, 0, (LPARAM)&dwIPaddress);
	DWORD dwNetIP = dwIPaddress & dwIPmask;
	DWORD dwBroadcastIP = dwIPaddress | ~dwIPmask;
	DWORD dwnumberIP = ~dwIPmask + 1;
	DWORD dwnodesIP = dwnumberIP - 2;
	sprintf(bufer_netIP, "%i.%i.%i.%i", FIRST_IPADDRESS(dwNetIP), SECOND_IPADDRESS(dwNetIP), THIRD_IPADDRESS(dwNetIP), FOURTH_IPADDRESS(dwNetIP));
	sprintf(bufer_broadcastIP, "%i.%i.%i.%i", FIRST_IPADDRESS(dwBroadcastIP), SECOND_IPADDRESS(dwBroadcastIP), THIRD_IPADDRESS(dwBroadcastIP), FOURTH_IPADDRESS(dwBroadcastIP));
	sprintf(bufer_numberIP, "%i", dwnumberIP);
	sprintf(bufer_nodesIP, "%i", dwnodesIP);
	sprintf(bufer_info,
		"Адрес сети:\t\t\t%s\nШироковещательный адрес:\t%s\nКоличество IP-адресов:\t%s\nКоличество узлов:\t\t%s",
		bufer_netIP, bufer_broadcastIP, bufer_numberIP, bufer_nodesIP);
	SendMessage(hInfo, WM_SETTEXT, 0, (LPARAM)bufer_info);
}