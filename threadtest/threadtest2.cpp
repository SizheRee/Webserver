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

    #define POOL_SIZE 10 // 线程池大小

    hMutex = CreateMutex(NULL, FALSE, (char *)"Test"); //创建互斥量
    message* pm = new message;
    pm->data=1;
    long unsigned  threadid;
    HANDLE hThread = CreateThread(NULL, 0, thread, (LPVOID) pm, 0, &threadid);  //创建线程01


    WaitForSingleObject(hThread, INFINITE); // 等待，直到线程被激发
    printf("thread:%lu is closed\n", threadid);
    CloseHandle(hThread); //关闭句柄

    return 0;
}