#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/socket.h>  
#include <arpa/inet.h>
#include <netdb.h>  
#include <string.h>  
#include "cJSON.h"

#if 0
//http get 
#define HOST "www.baidu.com"  
#define PAGE "/"  
#define PORT 8080
#define USERAGENT "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/30.0.1599.114 Safari/537.36"  
#define ACCEPTLANGUAGE "zh-CN,zh;q=0.8,en;q=0.6,en-US;q=0.4,en-GB;q=0.2"  
#define ACCEPTENCODING "gzip,deflate,sdch"  
char *build_get_query(char *host,char *page){  
    char *query;  
    char *getpage=page;  
    char *tpl="GET %s HTTP/1.1\r\nHost:%s\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\nUser-Agent:%s\r\nAccept-Language:%s\r\n\r\n";//Accept-Encoding:%s\r\n  
    query=(char *)malloc(strlen(host)+strlen(getpage)+strlen(USERAGENT)+strlen(tpl)+strlen(ACCEPTLANGUAGE)-5);//+strlen(ACCEPTENCODING)  
    sprintf(query,tpl,getpage,host,USERAGENT,ACCEPTLANGUAGE);//ACCEPTENCODING  
    return query;  
}  
char *get_ip(char *host){  
    struct hostent *hent;  
    int iplen=15;  
    char *ip=(char *)malloc(iplen+1);  
    memset(ip,0,iplen+1);  
    if((hent=gethostbyname(host))==NULL){  
        perror("Can't get ip");  
        exit(1);  
    }  
    if(inet_ntop(AF_INET,(void *)hent->h_addr_list[0],ip,iplen)==NULL){  
        perror("Can't resolve host!\n");  
        exit(1);  
    }  
    return ip;  
}  
void usage(){  
    fprintf(stderr,"USAGE:htmlget host [page]\n\thost:the website hostname. ex:www.baidu.com\n\tpage:the page to retrieve. ex:index.html,default:/\n");  
}  
int create_tcp_socket(){  
    int sock;  
    if((sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))<0){  
        perror("Can't create TCP socket!\n");  
        exit(1);  
    }  
    return sock;  
} 
int htpp_get(int argc,char *argv[])
{
	struct sockaddr_in *remote;  
    int sock;  
    int tmpres;  
    char *ip;  
    char *get;  
    char buf[BUFSIZ+1];  
    char *host;  
    char *page;  
  
  
    if(argc==1){  
        usage();  
        exit(2);  
    }  
    host=argv[1];  
    if(argc>2){  
        page=argv[2];  
    }else{  
        page=PAGE;  
    }  
    fprintf(stdout,"page:%s,hostName:%s\n",page,host);  
    sock=create_tcp_socket();  
    ip=argv[1];//get_ip(host);  
    fprintf(stderr,"IP is %s\n",ip);  
    remote=(struct sockaddr_in *)malloc(sizeof(struct sockaddr_in*));  
    remote->sin_family=AF_INET;  
    tmpres=inet_pton(AF_INET,ip,(void *)(&(remote->sin_addr.s_addr)));  
    if(tmpres<0){  
        perror("Can't set remote->sin_addr.s_addr");  
        exit(1);  
    }else if(tmpres==0){  
        fprintf(stderr,"%s is not a valid IP address\n",ip);  
        exit(1);  
    }  
    remote->sin_port=htons(PORT);  
    if(connect(sock,(struct sockaddr *)remote,sizeof(struct sockaddr))<0){  
        perror("Could not connect!\n");  
        exit(1);  
    }  
    get =build_get_query(host,page);  
    fprintf(stdout,"<start>\n%s\n<end>\n",get);  
    int sent=0;  
    while(sent<strlen(get)){  
        tmpres=send(sock,get+sent,strlen(get)-sent,0);  
        if(tmpres==-1){  
            perror("Can't send query!");  
            exit(1);  
        }  
        sent+=tmpres;  
    }  
    memset(buf,0,sizeof(buf));  
    int htmlstart=0;  
    char *htmlcontent;  
    while((tmpres=recv(sock,buf,BUFSIZ,0))>0){  
        if(htmlstart==0){  
            htmlcontent=strstr(buf,"\r\n\r\n");  
            if(htmlcontent!=NULL){  
                htmlstart=1;  
                htmlcontent+=4;  
            }  
        }else{  
            htmlcontent=buf;  
        }  
        if(htmlstart){  
            fprintf(stdout,"%s",htmlcontent);  
        }  
        memset(buf,0,tmpres);  
       // fprintf(stdout,"\n\n\ntmpres Value:%d\n",tmpres);  
    }  
    fprintf(stdout,"\nreceive data over!\n");  
    if(tmpres<0){  
        perror("Error receiving data!\n");  
    }  
    free(get);  
    free(remote);  
    //free(ip);  
    close(sock);  
    return 0;  
}
#else
#define BUFFER_SIZE 1024  
#define HTTP_POST "POST /%s HTTP/1.1\r\nHOST: %s:%d\r\nAccept: */*\r\n"\  
    "Content-Type:application/x-www-form-urlencoded\r\nContent-Length: %d\r\n\r\n%s"  
#define HTTP_GET "GET /%s HTTP/1.1\r\nHOST: %s:%d\r\nAccept: */*\r\n\r\n"  
#define MY_HTTP_DEFAULT_PORT 8080  
  
static int http_tcpclient_create(const char *host, int port){  
    struct hostent *he;  
    struct sockaddr_in server_addr;   
    int socket_fd;  
  
    if((he = gethostbyname(host))==NULL){  
        return -1;  
    }  
  
   server_addr.sin_family = AF_INET;  
   server_addr.sin_port = htons(port);  
   server_addr.sin_addr = *((struct in_addr *)he->h_addr);  
  
    if((socket_fd = socket(AF_INET,SOCK_STREAM,0))==-1){  
        return -1;  
    }  
  
    if(connect(socket_fd, (struct sockaddr *)&server_addr,sizeof(struct sockaddr)) == -1){  
        return -1;  
    }  
  
    return socket_fd;  
}  
  
static void http_tcpclient_close(int socket){  
    close(socket);  
}  
  
static int http_parse_url(const char *url,char *host,char *file,int *port)  
{  
    char *ptr1,*ptr2;  
    int len = 0;  
    if(!url || !host || !file || !port){  
        return -1;  
    }  
  
    ptr1 = (char *)url;  
  
    if(!strncmp(ptr1,"http://",strlen("http://"))){  
        ptr1 += strlen("http://");  
    }else{  
        return -1;  
    }  
  
    ptr2 = strchr(ptr1,'/');  
    if(ptr2){  
        len = strlen(ptr1) - strlen(ptr2);  
        memcpy(host,ptr1,len);  
        host[len] = '\0';  
        if(*(ptr2 + 1)){  
            memcpy(file,ptr2 + 1,strlen(ptr2) - 1 );  
            file[strlen(ptr2) - 1] = '\0';  
        }  
    }else{  
        memcpy(host,ptr1,strlen(ptr1));  
        host[strlen(ptr1)] = '\0';  
    }  
    //get host and ip  
    ptr1 = strchr(host,':');  
    if(ptr1){  
        *ptr1++ = '\0';  
        *port = atoi(ptr1);  
    }else{  
        *port = MY_HTTP_DEFAULT_PORT;  
    }  
  
    return 0;  
}  
  
  
static int http_tcpclient_recv(int socket,char *lpbuff){  
    int recvnum = 0;  
  
    recvnum = recv(socket, lpbuff,BUFFER_SIZE*4,0);  
  
    return recvnum;  
}  
  
static int http_tcpclient_send(int socket,char *buff,int size){  
    int sent=0,tmpres=0;  
  
    while(sent < size){  
        tmpres = send(socket,buff+sent,size-sent,0);  
        if(tmpres == -1){  
            return -1;  
        }  
        sent += tmpres;  
    }  
    return sent;  
}  
  
static char *http_parse_result(const char*lpbuf)  
{  
    char *ptmp = NULL;   
    char *response = NULL;  
    ptmp = (char*)strstr(lpbuf,"HTTP/1.1");  
    if(!ptmp){  
        printf("http/1.1 not faind\n");  
        return NULL;  
    }  
    if(atoi(ptmp + 9)!=200){  
        printf("result:\n%s\n",lpbuf);  
        return NULL;  
    }  
  
    ptmp = (char*)strstr(lpbuf,"\r\n\r\n");  
    if(!ptmp){  
        printf("ptmp is NULL\n");  
        return NULL;  
    }  
    response = (char *)malloc(strlen(ptmp)+1);  
    if(!response){  
        printf("malloc failed \n");  
        return NULL;  
    }  
    strcpy(response,ptmp+4);  
    return response;  
}  
  
char * http_post(const char *url,const char *post_str){  
  
    char post[BUFFER_SIZE] = {'\0'};  
    int socket_fd = -1;  
    char lpbuf[BUFFER_SIZE*4] = {'\0'};  
    char *ptmp;  
    char host_addr[BUFFER_SIZE] = {'\0'};  
    char file[BUFFER_SIZE] = {'\0'};  
    int port = 0;  
    int len=0;  
    char *response = NULL;  
  
    if(!url || !post_str){  
        printf("      failed!\n");  
        return NULL;  
    }  
  
    if(http_parse_url(url,host_addr,file,&port)){  
        printf("http_parse_url failed!\n");  
        return NULL;  
    }  
    //printf("host_addr : %s\tfile:%s\t,%d\n",host_addr,file,port);  
  
    socket_fd = http_tcpclient_create(host_addr,port);  
    if(socket_fd < 0){  
        printf("http_tcpclient_create failed\n");  
        return NULL;  
    }  
       
    sprintf(lpbuf,HTTP_POST,file,host_addr,port,strlen(post_str),post_str);  
  
    if(http_tcpclient_send(socket_fd,lpbuf,strlen(lpbuf)) < 0){  
        printf("http_tcpclient_send failed..\n");  
        return NULL;  
    }  
    printf("POST Sent:\n%s\n",lpbuf);  
  
    /*it's time to recv from server*/  
    if(http_tcpclient_recv(socket_fd,lpbuf) <= 0){  
        printf("http_tcpclient_recv failed\n");  
        return NULL;  
    }  
  
    http_tcpclient_close(socket_fd);  
  
    return http_parse_result(lpbuf);  
}  
  
char * http_get(const char *url)  
{  
  
    char post[BUFFER_SIZE] = {'\0'};  
    int socket_fd = -1;  
    char lpbuf[BUFFER_SIZE*4] = {'\0'};  
    char *ptmp;  
    char host_addr[BUFFER_SIZE] = {'\0'};  
    char file[BUFFER_SIZE] = {'\0'};  
    int port = 0;  
    int len=0;  
  
    if(!url){  
        printf("      failed!\n");  
        return NULL;  
    }  
  
    if(http_parse_url(url,host_addr,file,&port)){  
        printf("http_parse_url failed!\n");  
        return NULL;  
    }  
    //printf("host_addr : %s\tfile:%s\t,%d\n",host_addr,file,port);  
  
    socket_fd =  http_tcpclient_create(host_addr,port);  
    if(socket_fd < 0){  
        printf("http_tcpclient_create failed\n");  
        return NULL;  
    }  
  
    sprintf(lpbuf,HTTP_GET,file,host_addr,port);  
  
    if(http_tcpclient_send(socket_fd,lpbuf,strlen(lpbuf)) < 0){  
        printf("http_tcpclient_send failed..\n");  
        return NULL;  
    }  
  printf("GET Sent:\n%s\n",lpbuf);  
  
    if(http_tcpclient_recv(socket_fd,lpbuf) <= 0){  
        printf("http_tcpclient_recv failed\n");  
        return NULL;  
    }  
    http_tcpclient_close(socket_fd);  
  
    return http_parse_result(lpbuf);  
}  
#endif
void doit(char *text,const char *item_str)
{
	char *out;cJSON *json,*item_json;
	
	json=cJSON_Parse(text);
	if (!json) {printf("Error before: [%s]\n",cJSON_GetErrorPtr());}
	else
	{
		//out=cJSON_Print(json);
		item_json = cJSON_GetObjectItem(json, item_str);
		if (item_json)
		{
		    int nLen = strlen(item_json->valuestring);
		    printf("%s ,%d %s\n",item_str,nLen,item_json->valuestring);
		}
		else
			printf("get %s failed\n",item_str);
		cJSON_Delete(json);
		//printf("%s\n",out);
		//free(out);
	}
}

//get cmd http://101.200.236.69:8080/lamp/lamp/commond/wait?lampCode=aaaa
//ack http://101.200.236.69:8080/lamp/lamp/commond/response?commondId=f1d51484-8daf-47a3-a490-f56461d3ce23&isSuccess=true
int main(int argc,char *argv[])
{
	printf("Hello World\n");
	char *rcv=http_get("http://101.200.236.69:8080/lamp/lamp/commond/wait?lampCode=aaaa");
	printf("%s\n",rcv);
	doit(rcv,"code");
	free(rcv);
	rcv=http_get("http://101.200.236.69:8080/lamp/lamp/commond/response?commondId=f1d51484-8daf-47a3-a490-f56461d3ce23&isSuccess=true");
	printf("%s\n",rcv);
	doit(rcv,"errorMsg");
	free(rcv);
	//rcv=http_post("http://101.200.236.69:8080/lamp/device/register","macAddress=xxxx");
	//printf("%s\n",rcv);
	//free(rcv);
	return 0;
	//return htpp_get(argc,argv);
}
