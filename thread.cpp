#include <iostream>
#include <Windows.h>
 
using namespace std;
 
HANDLE hMutex = NULL; //互斥量
 
DWORD WINAPI thread01(LPVOID lvParamter)
{
    for (int i = 0; i < 10; i++)
    {
        WaitForSingleObject(hMutex, INFINITE); //互斥锁
        cout << "Thread 01 is working!" << endl;
        ReleaseMutex(hMutex); //释放互斥锁
    }
    return 0;
}
 
DWORD WINAPI thread02(LPVOID lvParamter)
{
    for (int i = 0; i < 10; i++)
    {
        WaitForSingleObject(hMutex, INFINITE); //互斥锁
        cout << "Thread 02 is working!" << endl;
        ReleaseMutex(hMutex); //释放互斥锁
    }
    return 0;
}
 
 
int main()
{
    hMutex = CreateMutex(NULL, FALSE, (char *)"Test"); //创建互斥量
    HANDLE hThread = CreateThread(NULL, 0, thread01, NULL, 0, NULL);  //创建线程01
    hThread = CreateThread(NULL, 0, thread02, NULL, 0, NULL);     //创建线程01
    
    CloseHandle(hThread); //关闭句柄
    system("pause");
    return 0;
}