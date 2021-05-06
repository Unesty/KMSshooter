#include <sys/syscall.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int cfds[1000];
addresses[1000];
//int
void *ubuf;
char optvals[1];
int lc=3;
int clc;
int main(int argc, char** argv)
{

	optvals[0]=1;
	while(1){
		syscall(SYS_socket,2,1,0);
		syscall(SYS_setsockopt,3,0,1,optvals[0],1);
		bind(3,,27);
		//syscall(SYS_listen,3);
		//syscall(SYS_accept,);
		//syscall(SYS_recvfrom,);
		/*
		for(clc=4;clc>=lc;clc--){
			syscall(SYS_sendmsg,clc,pos,3+7,0,address[clc],16);
		}
		*/
	}
}
