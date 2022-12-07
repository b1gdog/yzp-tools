#include <Windows.h>

int main()
{
	SetCursorPos(0, 0);   //将鼠标移动到0,0坐标
	mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);	//鼠标左键单击一次
	/*
		将上面两个代码写入到死循环中
		将鼠标坐标改为右上角、右下角、左下角，并且一直左键，可以做到控制鼠标和键盘的效果
	*/
	return 0;
}