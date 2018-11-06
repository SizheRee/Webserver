#include <iostream>
#include <Windows.h>
#include <cstdio>
 
using namespace std;
 
HANDLE hMutex = NULL; //互斥量
 
struct message{
    int data;
};

DWORD WINAPI thread(LPVOID lvParamter)
{
    DWORD curid;
    message &m=*(message *)lvParamter;

    printf("message.data:%d\n", m.data);

    curid = GetCurrentThreadId();
    for (int i = 0; i < 1000; i++)
    {
        WaitForSingleObject(hMutex, INFINITE); //互斥锁
        printf("Thread %lu is working! i:%d\n", curid, i);
        ReleaseMutex(hMutex); //释放互斥锁
    }
    ExitThread(233);
    return 50;
}
 
 
int main()
{

    hMutex = CreateMutex(NULL, FALSE, (char *)"Test"); //创建互斥量
    message* pm = new message;
    pm->data=1;
    long unsigned  threadid;
    HANDLE hThread = CreateThread(NULL, 0, thread, (LPVOID) pm, 0, &threadid);  //创建线程01

    DWORD exitCode = 0;

    while(1){
        Sleep(50);
        bool res = GetExitCodeThread(hThread, &exitCode);
        WaitForSingleObject(hMutex, INFINITE); //互斥锁
        printf("res:%d exitCode:%lu STILL_ACTIVE:%d\n", res, exitCode, STILL_ACTIVE);
        ReleaseMutex(hMutex); //释放互斥锁
        if(STILL_ACTIVE!=exitCode){
            printf("thread %lu exitcode:%lu\n", threadid, exitCode);
            break;
        }
    }
    CloseHandle(hThread); //关闭句柄

    system("pause");
    return 0;
}