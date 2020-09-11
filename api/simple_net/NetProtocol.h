#pragma once

#include <cstdint>
typedef unsigned char      byte;

#pragma pack (push, 1)

#define TCP_FLAG              "tcp_post"  //tcp flag

//version comment
// 01 : first edition
// 02 : Handlshake carry tcp param, param can extended, max len is MAX_TCP_PARAM-1
#define PROTOCOL_VERSION      "02"        //protocol version

//max tcp param len, now used sizeof(TCP_PARAM)
#define MAX_TCP_PARAM         1025


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
* protocol udp header
*/
typedef struct _TCP_HEADER
{
	char       flag[10];         //message flag
	char       version[2];       //protocol version
	char       reserve[8];       //reserve
	uint8_t    msgtype;          //MSG_TYPE
	uint8_t    frametype;        //FRAME_TYPE
	uint32_t   msglen;           //message length
}TCP_HEADER, *PTCP_HEADER;

//server send to client param
typedef struct _TCP_PARAM{
	uint32_t recv_buff_size;
	uint32_t heatbeat_time; //millsecond
	uint32_t keepalive_time;//millsecond
	uint32_t keepalive_count;
}TCP_PARAM, *PTCP_PARAM;

typedef struct _UDP_HEADER
{
	char       flag[10];         //message flag
	uint32_t   msglen;           //message length
	uint16_t   pkgcount;         //package count
	uint16_t   curpkgindex;      //current package count
	uint16_t   crc;
}UDP_HEADER, *PUPD_HEADER;

#pragma pack (pop)