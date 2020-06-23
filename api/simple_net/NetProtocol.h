#pragma once


/*
* data type define
*/
typedef signed char        int8;
typedef short              int16;
typedef int                int32;
typedef unsigned char      uint8;
typedef unsigned short     uint16;
typedef unsigned int       uint32;
#ifdef _WIN32
typedef __int64            int64;
typedef unsigned __int64   uint64;
#else
typedef long long          int64;
typedef unsigned long long uint64;
#endif

typedef unsigned char      byte;

#pragma pack (push, 1)

#define TCP_FLAG              "tcp_post"  //tcp flag
#define PROTOCOL_VERSION      "01"        //protocol version


enum MSG_TYPE{
	MSG_NORMAL = 1,
	MSG_HEARBEAT = 2,
	MSG_HANDSHAKE = 3,
};

enum FRAME_TYPE{
	FRAME_STRING = 1,
	FRAME_BINARY = 2
};

/*
* protocol header
*/
typedef struct _TCP_HEADER
{
	char     flag[10];         //message flag
	char     version[2];       //protocol version
	char     reserve[8];      //reserve
	uint8    msgtype;          //MSG_TYPE
	uint8    frametype;        //FRAME_TYPE
	uint32   msglen;           //message length
}TCP_HEADER, *PTCP_HEADER;

typedef struct _UDP_HEADER
{
	char     flag[10];         //message flag
	uint32   msglen;           //message length
	uint16   pkgcount;         //package count
	uint16   curpkgindex;      //current package count
	uint16   crc;
}UDP_HEADER, *PUPD_HEADER;

#pragma pack (pop)