#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
int main(void)
{
	printf("%d",O_RDWR|O_CLOEXEC);
}
