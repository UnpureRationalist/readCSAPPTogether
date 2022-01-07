/*
 * tiny.c - A simple, iterative HTTP/1.0 Web server that uses the 
 *     GET method to serve static and dynamic content.
 */
#include "csapp.h"

void doit(int fd);
void read_requesthdrs(rio_t *rp);
int parse_uri(char *uri, char *filename, char *cgiargs);
void serve_static(int fd, char *filename, int filesize);
void get_filetype(char *filename, char *filetype);
void serve_dynamic(int fd, char *filename, char *cgiargs);
void clienterror(int fd, char *cause, char *errnum, 
		 char *shortmsg, char *longmsg);


/* 主函数 */
int main(int argc, char **argv) 
{
    int listenfd, connfd, port, clientlen;
    struct sockaddr_in clientaddr;

    if (argc != 2) {
	fprintf(stderr, "usage: %s <port>\n", argv[0]);
	exit(1);
    }
    port = atoi(argv[1]);

    listenfd = Open_listenfd(port); // 设置 listenfd 为被动的监听套接字
    while (1) {
	clientlen = sizeof(clientaddr);
	connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen); // 接受连接请求，返回一个连接描述符 connfd
	doit(connfd);                                             // 执行事物
	Close(connfd);                                            // 关闭连接
    }
}


/* 处理 HTTP 请求 */
void doit(int fd) 
{
    int is_static;
    struct stat sbuf;
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char filename[MAXLINE], cgiargs[MAXLINE];
    rio_t rio;
  
    /* 读请求行 */
    Rio_readinitb(&rio, fd); // 初始化 rio
    Rio_readlineb(&rio, buf, MAXLINE); // 将缓存写入到 buf 中

    printf("%s\n", buf);
    sscanf(buf, "%s %s %s", method, uri, version); // 分别得到 method uri version 的值
    if (strcasecmp(method, "GET")) { // 请求类型不是 GET，返回错误信息
        clienterror(fd, method, "501", "Not Implemented",
                "Tiny does not implement this method");
        return;
    }                                                    
    read_requesthdrs(&rio); // 读请求报头，并忽略其他内容

    /* 解析请求 */
    is_static = parse_uri(uri, filename, cgiargs); // 判断请求内容是静态还是动态
    if (stat(filename, &sbuf) < 0) { // 将文件信息写到 sbuf 中，结果小于 0 表示未找到该文件
	clienterror(fd, filename, "404", "Not found",
		    "Tiny couldn't find this file");
	return;
    }                                                    

    if (is_static) { /* 请求静态文件 */          
	if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)) { 
	    clienterror(fd, filename, "403", "Forbidden",
			"Tiny couldn't read the file");
	    return;
	}
	serve_static(fd, filename, sbuf.st_size);        
    }
    else { /* 请求动态文件 */
	if (!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode)) { 
	    clienterror(fd, filename, "403", "Forbidden",
			"Tiny couldn't run the CGI program");
	    return;
	}
	serve_dynamic(fd, filename, cgiargs);            
    }
}



/* 读请求报头并打印 */
void read_requesthdrs(rio_t *rp) 
{
    char buf[MAXLINE];

    Rio_readlineb(rp, buf, MAXLINE);
    printf("%s\n", buf); // 这一行显示服务器的 ip 地址和端口号
    while(strcmp(buf, "\r\n")) {          
	Rio_readlineb(rp, buf, MAXLINE);
	printf("%s", buf);
    }
    return;
}


/* 静态内容的主目录是当前目录，而执行文件的主目录是 ./cgi-bin，本函数在 uri 中提取出请求的文件名 */
int parse_uri(char *uri, char *filename, char *cgiargs) 
{
    char *ptr;

    if (!strstr(uri, "cgi-bin")) { // 静态内容 
	strcpy(cgiargs, "");                            
	strcpy(filename, ".");                           
	strcat(filename, uri);                           
	if (uri[strlen(uri)-1] == '/')                  
	    strcat(filename, "home.html");               
	return 1;
    }
    else { // 动态内容                     
	ptr = index(uri, '?');                          
	if (ptr) {
	    strcpy(cgiargs, ptr+1);
	    *ptr = '\0';
	}
	else 
	    strcpy(cgiargs, "");                        
	strcpy(filename, ".");                           
	strcat(filename, uri);                          
	return 0;
    }
}


/* 处理静态请求，回复一个 HTTP 响应，包含一个本地文件内容 */
void serve_static(int fd, char *filename, int filesize) 
{
    int srcfd;
    char *srcp, filetype[MAXLINE], buf[MAXBUF];
 
    /* 将响应报头存放到 buf 中 */
    get_filetype(filename, filetype);       
    sprintf(buf, "HTTP/1.0 200 OK\r\n");    
    sprintf(buf, "%sServer: Tiny Web Server\r\n", buf);
    sprintf(buf, "%sContent-length: %d\r\n", buf, filesize);
    sprintf(buf, "%sContent-type: %s\r\n\r\n", buf, filetype);
    Rio_writen(fd, buf, strlen(buf));       

    /* 向客户端发送响应报头及响应体 */
    srcfd = Open(filename, O_RDONLY, 0);    
    srcp = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0); // 将请求文件映射到一个虚拟内存空间
    Close(srcfd);                           
    Rio_writen(fd, srcp, filesize);         
    Munmap(srcp, filesize); // 释放虚拟内存区域                
}


/* 判断静态请求的文件类型 */
void get_filetype(char *filename, char *filetype) 
{
    if (strstr(filename, ".html"))
	strcpy(filetype, "text/html");
    else if (strstr(filename, ".gif"))
	strcpy(filetype, "image/gif");
    else if (strstr(filename, ".jpg"))
	strcpy(filetype, "image/jpeg");
    else
	strcpy(filetype, "text/plain");
}  


/* 处理动态请求，在子进程中运行一个 CGI 程序 */
void serve_dynamic(int fd, char *filename, char *cgiargs) 
{
    char buf[MAXLINE], *emptylist[] = { NULL };

    /* 向客户端发送表明请求成功的响应行 */
    sprintf(buf, "HTTP/1.0 200 OK\r\n"); 
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Server: Tiny Web Server\r\n");
    Rio_writen(fd, buf, strlen(buf));
  
    if (Fork() == 0) { // 子进程，负责发送响应的剩余部分
	setenv("QUERY_STRING", cgiargs, 1); // 初始化子进程的环境变量
	Dup2(fd, STDOUT_FILENO); // 重定向标准输出到与客户端连接的套接字        
	Execve(filename, emptylist, environ); 
    }
    Wait(NULL); // 子进程终止时，回收相关资源
}


/* 检查一些错误并告知客户端 */
void clienterror(int fd, char *cause, char *errnum, 
		 char *shortmsg, char *longmsg) 
{
    char buf[MAXLINE], body[MAXBUF];

    /* Build the HTTP response body */
    sprintf(body, "<html><title>Tiny Error</title>");
    sprintf(body, "%s<body bgcolor=""ffffff"">\r\n", body);
    sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
    sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
    sprintf(body, "%s<hr><em>The Tiny Web server</em>\r\n", body);

    /* Print the HTTP response */
    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: text/html\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
    Rio_writen(fd, buf, strlen(buf));
    Rio_writen(fd, body, strlen(body));
}



