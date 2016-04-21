#ifndef comm_macro_h__
#define comm_macro_h__

#define	SOCKET_XMLDATA_LEN	1024
#define NET_DATA_HEADER_LEN	4

#define MAX_A_FRAME_LENGTH						(800 * 1024)
#define MAX_A_FRAME_LENGTH_ON_MOBILE_DEV		(200 * 1024)
#define MICROSEC_TICK_40						(40 * 1000)

#define USE_BASE_PORT		30000
#define USE_MAX_PORT		65000

#define SERVER_RANDOM_PORT		\
	(USE_BASE_PORT+(int)((float)(USE_MAX_PORT-USE_BASE_PORT)*rand()/(RAND_MAX+2.6)))



#endif

