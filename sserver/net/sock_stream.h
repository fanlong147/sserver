#ifndef __SSERVER_NET_SOCK_STREAM_H__
#define __SSERVER_NET_SOCK_STREAM_H__

#include <string>
using std::string;

//��֧��IPV4������
class SockStream {
public:
	SockStream();
	~SockStream();

	SockStream(const SockStream& rhs) = delete;
	SockStream& operator=(const SockStream& rhs) = delete;

	int CreateSocket();	//����Socket

	void SetSockFd(int sock_fd = -1);
	int GetSockFd() const { return sock_fd_; }

	int SetTimeout(long second, long u_second);

	int Connect(string& remote_ip, int remote_port);

	int SetNoBlock(bool flag = true);		//true�����ã� false:ȡ������
	int SetNoDelay(bool flag = true);		//true�����ã� false:ȡ������
	int SetKeepAlive(bool flag = true);		//true�����ã� false:ȡ������ 
//	int SetSockOpt();

	int Send(const char * data_buf, int num_bytes, int flag = 0);
	int Recv(char * data_buf, int num_bytes, int flag = 0);

	int Close();

private:
	int sock_fd_;				//socket�ļ�������

	int sock_family_;				//��ַ��
	int sock_type_;					//�׽�������
	int sock_protocal_;				//Э������
};

#endif
