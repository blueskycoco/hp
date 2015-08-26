#include <stdio.h>
#include <stdlib.h>

int main(int argc,void *argv[])
{
if(argc>=2 && atoi(argv[1])==1)
{
	system("insmod libertas.ko");
	system("insmod libertas_sdio.ko");
}
system("./wpa_supplicant -Dwext -ieth1 -c /etc/wpa_supplicant.conf &");
system("sleep 8");
system("udhcpc -i eth1 &");	
}
