#include<Windows.h>
#include<CommCtrl.h>
#include"resource.h"

BOOL CALLBACK DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


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
		SendMessage(GetDlgItem(hWnd,IDC_SPIN_PREFIX),UDM_SETRANGE,0,MAKELPARAM(30,1));
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
			SendMessage(hIPaddress, IPM_GETADDRESS, 0, (LPARAM)&dwIPaddress);
			if (FIRST_IPADDRESS(dwIPaddress) < 128)SendMessage(hIPmask, IPM_SETADDRESS, 0, 0xFF000000);
			else if (FIRST_IPADDRESS(dwIPaddress) < 192)SendMessage(hIPmask, IPM_SETADDRESS, 0, 0xFFFF0000);
			else if (FIRST_IPADDRESS(dwIPaddress) < 224)SendMessage(hIPmask, IPM_SETADDRESS, 0, 0xFFFFFF00);
		break;
		case IDOK:
		break;
		case IDCANCEL:
			EndDialog(hWnd, 0);
		}
	}
	break;
	case WM_CLOSE:
		EndDialog(hWnd, 0);
	}
	
	return FALSE;

}