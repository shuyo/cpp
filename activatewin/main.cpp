#define UNICODE
//#include <stdio.h>
#include <Windows.h>

int WINAPI WinMain(HINSTANCE hInstance,
		HINSTANCE hPrevInstance,
		LPSTR lpCmdLine,
		int nCmdShow) {

	int    argc;
	LPTSTR *argv;
	argv = CommandLineToArgvW(GetCommandLine(), &argc);

  BOOL result = 0;

  if (argc > 1) {
    HWND wnd = FindWindow(argv[1], NULL);
    HWND app = GetWindow(wnd,GW_OWNER);
    if (IsIconic(app)) ShowWindow(app, SW_SHOWNORMAL);
    result = SetForegroundWindow(app);
  }
  if (result == 0 && argc > 2) {
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    ZeroMemory(&si,sizeof(si));
    si.cb=sizeof(si);
    CreateProcess(NULL, argv[2], NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
    CloseHandle(pi.hThread);
    //WaitForSingleObject(pi.hProcess, INFINITE);
    //CloseHandle(pi.hProcess);
  }

	LocalFree(argv);
  return 0;

}
