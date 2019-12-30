#ifndef ColdevPhotonLib 

#define ColdevPhotonLib

//#include "LoadBalancing-cpp/inc/Client.h"

#include <list>
#include <vector>

using namespace std;



#include <ctime>

class CounterClock
{
public:

	

	CounterClock(clock_t increment_time)  
	{		 
		IncTime = increment_time;
		StartTime = CurrentTimeMs();
		AlarmTime = StartTime + IncTime;
	}

#define CLOCKS_PER_MS (CLOCKS_PER_SEC / 1000)

 

	clock_t CurrentTimeMs()
	{
		return clock() / CLOCKS_PER_MS;
	}


	bool IsAlarm()
	{
		if (CurrentTimeMs() > AlarmTime)
		{
			AlarmTime = CurrentTimeMs() + IncTime;
			return true;
		}
		return false;
	}


private:
	clock_t StartTime;
	clock_t AlarmTime;
	clock_t IncTime;
};




//================================  event codes ============================
#define cd_PHOTON_EVENT_PLAYER_CONNECTED 1
#define cd_PHOTON_EVENT_PLAYER_DISCONNECTED 2

#define cd_PHOTON_EVENT_CONNECTED 90
#define cd_PHOTON_EVENT_CONNECTING 100
#define cd_PHOTON_EVENT_JOINING 101
#define cd_PHOTON_EVENT_JOINED 102
#define cd_PHOTON_EVENT_DISCONNECTING 103
#define cd_PHOTON_EVENT_DISCONNECTED 104
#define cd_PHOTON_EVENT_PLAYER_KICKED 105


#define cd_PHOTON_EVENT_ERROR_CONNECTING -300
#define cd_PHOTON_EVENT_ERROR_JOINING -310
#define cd_PHOTON_EVENT_ERROR_LEAVING -320
#define cd_PHOTON_EVENT_ERROR_PASSWORD -330


#define cd_PHOTON_EVENT_REGION_LIST 400
#define cd_PHOTON_EVENT_ROOM_LIST_UPDATE 401
#define cd_PHOTON_EVENT_ROOM_PROPERTIES_CHANGE 402
#define cd_PHOTON_EVENT_PLAYER_PROPERTIES_CHANGE 403

//================================ end+  event codes ============================


//================================ region codes ============================
#define cd_PHOTON_REGION_DEFAULT 0
#define cd_PHOTON_REGION_SELECT 1
#define cd_PHOTON_REGION_BEST 2
//================================ end+  region codes ============================


//================================ PRIVATE EVENTS ============================

#define cd_EVENT_ROOM_PASSWORD 40
#define cd_EVENT_PLAYER_KICK   41


//================================ end+  PRIVATE EVENTS ============================
typedef struct
{
	int number_player;

	int data_length;
	void * data;

	int reliable;   //internals
	int *target_players;
	int  target_count;

} cd_type_Internal_net_pack;



typedef struct
{
	int number_player;	 

	int data_length;
	void * data;

} cd_type_net_pack;

 

typedef struct
{
	int  code_event;
	int  data_int;
	int  data_list;

} cd_type_event;


typedef struct
{
	char data[255];
	int  sendcode;
	int player_id;

} cd_type_private_event;



typedef struct
{
	int  max_players;
	int  count_players;

	char name[255];

} cd_type_room;



typedef struct
{
	int  playerid;
	int  isMasterClient;	 

	char name[255];

} cd_type_player;


typedef struct
{
	char name[30];

} cd_type_region;



#define PHOTON_CONNECTED			0

#define PHOTON_PLAYER_NUMBER		1
#define PHOTON_PLAYER_MAX			2
#define PHOTON_PLAYER_COUNT			3

#define PHOTON_DEBUG				4	
#define PHOTON_ISMASTERCLIENT		5
#define PHOTON_COMPRESSENABLE		6
#define PHOTON_ENCRYPTENABLE		7

#define PHOTON_MASTERCLIENTID 8
#define PHOTON_SERVERPING 9
#define PHOTON_SERVERTIME 10
 


#define CONNECT_MODE_JOINORCREATE 0
#define CONNECT_MODE_CREATE 1
#define CONNECT_MODE_JOIN 2




class ColdevPhoton
{
public:
	ColdevPhoton(  char *matchname,   char * playername,   char * appversion,   char * appid);
	~ColdevPhoton();

	//global vars

	bool cd_Exit_Thread = false;

	char cd_Photon_MatchName[255];
	char cd_Photon_AppVer [50];
	char cd_Photon_player_name [255];
	char cd_Photon_AppID [255];

	//list (endless)
	list <cd_type_Internal_net_pack > cd_Shipping_List;
	list <cd_type_Internal_net_pack > cd_ShippingGroup_List;


	list <cd_type_net_pack *> cd_Delivery_List;
	list <cd_type_event> cd_Event_List;
	list <cd_type_private_event> cd_PrivateEvents_List;


	//vector (finite)
	std::vector<cd_type_room>     cd_Room_List;
	std::vector<cd_type_player>   cd_Player_List;
	std::vector<cd_type_region>   cd_Region_List;

	
	
	
	void * cd_load_balancing_client=NULL;//async queries (list rooms, players,etc)
	unsigned char cd_region_mode;
	
	cd_type_room cd_current_room;
	cd_type_player cd_current_player;
	char cd_current_region[30];
	 


	char cd_server_address[255];
	
	int cd_connect_mode;

	char cd_password_lobby[255];

	 

	int CanalesUDP;


	//threads
	void * VariableMutex = NULL;
	 
 

	//======================================

	//debug
	void Str_Photon_Debug(const char *format, ...);
	
	//packs
	int  ex_SEND_NET_PACK(void *data,int realiable);
	int  ex_SEND_NET_PACK_GROUP(void *data, int reliable, int *target, int total_targets);
	int  ex_GET_NET_PACK();
	int  ex_REMOVE_NET_PACK();

	//events
	void ex_SET_NET_EVENT(int codigo, int data, int lista = 0);
	int  ex_GET_NET_EVENT();
	int  ex_REMOVE_NET_EVENT();

	//vars
	void ex_Set_VAR_GLOBAL(int codigo, int valor);
	int  ex_Get_VAR_GLOBAL(int codigo);

	//threads
	void ex_Create_Thread(int connectmode);
	
	//unload
	void ex_Terminate();

	//rooms
	int  ex_LIST_ROOMS();
	int  ex_GET_ROOM(int roomid);
 	int  ex_GET_CURRENT_ROOM();
	void ex_SET_ROOM_PASSWORD(char *pass);
	int  ex_SET_ROOM_ISOPEN(bool isopen);
	int  ex_SET_ROOM_ISVISIBLE(bool isvisible);

	 int   ex_ROOM_SET_PROPS(char *llave, char* valor);
	const char *   ex_ROOM_GET_PROPS(char *llave);


	//players
 	int ex_PLAYER_SET_CUSTOM_PROPS(char *llave, char * valor); 
	const char *   ex_PLAYER_GET_CUSTOM_PROPS(int playerNro, char *llave);
 
	int  ex_PLAYER_LIST( );
	int   ex_PLAYER_GET(int playerid);
	int   ex_PLAYER_GET_BY_INDEX(int playerindex);
 	int  ex_GET_CURRENT_PLAYER();
	int  ex_Kick_PLAYER(int playernro);


	//regions
	int  ex_REGION_LIST( );
	char *  ex_REGION_GET(int regionid);
	void  ex_REGION_SET(int regionmode, char *regioncode);
	 

	//server
	int ex_SERVER_PING();
	char * ex_SERVER_GETADDRESS();
	void ex_SERVER_FETCHSERVERTIMESTAMP();
	int ex_SERVER_GETSERVERTIME();


	//threads
	void LockMutex();
	void UnlockMutex();
	void ex_FramePhotonCore();

	//hash maps
	int  ex_HASH_MAP_CREATE();
	int  ex_HASH_MAP_COUNT(int lista);
	const char *  ex_HASH_MAP_KEY_GET(int lista, int indice);
	int   ex_HASH_MAP_ITEM_SET(int lista, char *llave, char *valor);
};








//strings helpers
char * cd_MallocStringCopy(char *source);
void ext_wcstombs(char *dest, const wchar_t *source, size_t length);




#endif // ! 