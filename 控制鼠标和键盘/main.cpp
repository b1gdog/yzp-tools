#include <Windows.h>

int main()
{
	SetCursorPos(0, 0);   //������ƶ���0,0����
	mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);	//����������һ��
	/*
		��������������д�뵽��ѭ����
		����������Ϊ���Ͻǡ����½ǡ����½ǣ�����һֱ��������������������ͼ��̵�Ч��
	*/
	return 0;
}