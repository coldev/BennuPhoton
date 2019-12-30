#include "ColdevPhoton.h"

#include <mutex>          // std::mutex



ColdevPhoton::ColdevPhoton( char *matchname,  char * playername,  char * appversion,  char * appid  )
{
	Str_Photon_Debug("===========  PHOTON INIT MODULE   ============== \n");
	Str_Photon_Debug("Colombian Developers (c) MIT License \n");


	Str_Photon_Debug("INIT PHOTON VARS   \n");

	 strcpy_s( cd_Photon_MatchName ,255, matchname);
	 strcpy_s(cd_Photon_player_name, 255, playername);
	 strcpy_s(cd_Photon_AppVer, 50, appversion);
	 strcpy_s(cd_Photon_AppID, 255, appid);
	   

	  strcpy_s(cd_server_address, 255, "");

	  cd_region_mode = cd_PHOTON_REGION_DEFAULT;     //ExitGames::LoadBalancing::RegionSelectionMode::BEST;

	  strcpy_s(cd_current_region,30, "eu");//region europa default

	  strcpy_s(cd_password_lobby, 255, "");

	  VariableMutex =(void *) new std::mutex;
	   
	  Str_Photon_Debug("PHOTON VARS .. OK \n");
}

ColdevPhoton ::~ColdevPhoton()
{
	ex_Terminate();
}


#include <stdarg.h>

 


//======================================     DEBUG   ===========================================

void ColdevPhoton::Str_Photon_Debug(const char *format, ...)
{
 
	if (!ex_Get_VAR_GLOBAL(PHOTON_DEBUG)) return;

	FILE * pFile;
	pFile = fopen("Photon_DebugLog.txt", "a");
	if (pFile != NULL) 
	{ 
     
	 va_list args;
	 fprintf(pFile, "Log: ");
	 va_start(args, format);
	 vfprintf(pFile, format, args);
	 va_end(args);
	 fprintf(pFile, "\n");

	 fclose(pFile);
	}
}







//========================================   PACKS  ====================================================
//extern "C"    void aes_encrypt_cbc(uint8_t *buffer, uint8_t *in, int length);
#include "Photon_lib.h"


int  ColdevPhoton::ex_SEND_NET_PACK(void *data, int reliable)
{
 
	cd_type_net_pack  Paquete = *(cd_type_net_pack *)data;
	cd_type_Internal_net_pack PaqueteEnvio;

	PaqueteEnvio.data = Paquete.data;
	PaqueteEnvio.data_length = Paquete.data_length;
	PaqueteEnvio.reliable = reliable;

	LockMutex();  //threads safe

	cd_Shipping_List.push_back(PaqueteEnvio);

	UnlockMutex();  //threads safe

	return 1;
///========================================================
	 
}


int ColdevPhoton::ex_SEND_NET_PACK_GROUP(void *data, int reliable, int *target, int total_targets)
{

 	cd_type_net_pack  Paquete = *(cd_type_net_pack *)data;
	cd_type_Internal_net_pack PaqueteEnvio;

	PaqueteEnvio.data = Paquete.data;
	PaqueteEnvio.data_length = Paquete.data_length;
	PaqueteEnvio.reliable = reliable;
	PaqueteEnvio.target_players = target;
	PaqueteEnvio.target_count = total_targets;

	LockMutex();  //threads safe

	cd_ShippingGroup_List.push_back(PaqueteEnvio);
	
	UnlockMutex();  //threads safe
	 
	return 1;
	///========================================================

	 
}




int  ColdevPhoton::ex_GET_NET_PACK()
{
	LockMutex();  //threads safe

	cd_type_net_pack * Paquete = NULL;
	std::list<cd_type_net_pack *>::iterator ptr;

	ptr = cd_Delivery_List.begin();

	if (ptr != cd_Delivery_List.end())
		Paquete = *ptr;

	UnlockMutex();  //threads safe

	return (int)Paquete;
}


int  ColdevPhoton::ex_REMOVE_NET_PACK()
{
	LockMutex();  //threads safe

	std::list<cd_type_net_pack *>::iterator ptr = cd_Delivery_List.begin();

	if (ptr != cd_Delivery_List.end())
	{
		cd_type_net_pack *paquete = *ptr;

		if (paquete->data)  free(paquete->data);//free pack mem
		if (paquete) delete paquete;

		cd_Delivery_List.erase(cd_Delivery_List.begin()); //free list
	}

	UnlockMutex();  //threads safe

	return 0;
}

////================================   end + PACKS  ===========================================




////================================   EVENTS  ===========================================

void  ColdevPhoton::ex_SET_NET_EVENT(int codigo, int data, int lista)
{

	cd_type_event evento;
	evento.code_event = codigo;
	evento.data_int = data;
	evento.data_list = lista;

	LockMutex();  //threads safe

	cd_Event_List.push_back(evento);

	UnlockMutex();  //threads safe
}


int  ColdevPhoton::ex_GET_NET_EVENT()
{
	LockMutex();  //threads safe

	cd_type_event  *Evento = NULL;
	std::list<cd_type_event>::iterator ptr;

	ptr = cd_Event_List.begin();

	if (ptr != cd_Event_List.end())
		Evento = &(*ptr);

	UnlockMutex();  //threads safe

	return (int)Evento;
}



int  ColdevPhoton::ex_REMOVE_NET_EVENT()
{
	LockMutex();  //threads safe

	std::list<cd_type_event>::iterator ptr = cd_Event_List.begin();

	if (ptr != cd_Event_List.end())
	{
		cd_type_event  *Evento = NULL;
		Evento = &(*ptr);

		if (Evento->code_event == cd_PHOTON_EVENT_PLAYER_PROPERTIES_CHANGE)
			delete ((const  ExitGames::Common::Hashtable *)Evento->data_list );//borra listas llaves creadas 
             

		cd_Event_List.erase(cd_Event_List.begin()); //free list  //borra evento
	}

	UnlockMutex();  //threads safe

	return 0;
}


////================================  end + EVENTS  ===========================================




////================================   VARS  ===========================================


//===========================================
extern void Cambiar_VAR_GLOBAL(int codigo, int valor);
extern int Obtener_VAR_GLOBAL(int codigo);
//===========================================



void ColdevPhoton::ex_Set_VAR_GLOBAL(int codigo, int valor)
{ 
	Cambiar_VAR_GLOBAL(  codigo,   valor); 
}


int ColdevPhoton::ex_Get_VAR_GLOBAL(int codigo)
{
 	return Obtener_VAR_GLOBAL(  codigo); 
}


////================================   end + VARS  ===========================================








// =======================================   end+ THREADS =============================================



#include "TypeSupportApplication.h"  //photon includes
#include "StdIO_UIListener.h"


#include <iostream>
#include <string>


//======== tread vars ====

extern UIListenerPtr gUIListener;
CPhotonLib *photonLibMod = NULL;

void TerminarPhotonCore()
{
	if (photonLibMod) { delete photonLibMod; photonLibMod = NULL; }

}



void InicializarPhotonCore()
{

	gUIListener.reset(new StdIO_UIListener());

	TerminarPhotonCore();

	photonLibMod = new CPhotonLib;

}





void ColdevPhoton::ex_FramePhotonCore()
{
	if (!photonLibMod) return;

	photonLibMod->run();

}






void ColdevPhoton::ex_Create_Thread(int connectmode)
{

	cd_connect_mode = connectmode;

	Str_Photon_Debug("Creating Photon thread...\n");

	InicializarPhotonCore();


	Str_Photon_Debug("Photon thread created OK \n");
}



void ColdevPhoton::ex_Terminate()
{

	TerminarPhotonCore();

	Str_Photon_Debug("Thread deleted Ok \n");



	cd_Shipping_List.clear();
	cd_ShippingGroup_List.clear();
	cd_Delivery_List.clear();
	cd_PrivateEvents_List.clear();
	cd_Event_List.clear();

	cd_Room_List.clear();
	cd_Player_List.clear();
	cd_Region_List.clear();

	Str_Photon_Debug("===========  PHOTON RELEASE MODULE   ============== \n");
}




void ColdevPhoton::LockMutex()
{
//	std::mutex *mimutex =(std::mutex *) VariableMutex;

//	mimutex->lock();
}



void ColdevPhoton::UnlockMutex()
{
//	std::mutex *mimutex = (std::mutex *) VariableMutex;

//	mimutex->unlock();
}

// =======================================   end+ THREADS =============================================










#include "LoadBalancing-cpp/inc/Client.h"


// =======================================     ROOMS =============================================

int ColdevPhoton::ex_LIST_ROOMS()
{
	
	if (!cd_load_balancing_client)
	{
		Str_Photon_Debug("List Room Client is Null \n");
		return 0;
	}
	

 ExitGames::LoadBalancing::Client *cliente=(ExitGames::LoadBalancing::Client *) cd_load_balancing_client;
	
 
 const  ExitGames::Common::JVector<ExitGames::LoadBalancing::Room*> salas = cliente->getRoomList();

 unsigned int NroSalas= salas.getSize();

  
 Str_Photon_Debug("Room list : %d   \n", NroSalas);

 cd_Room_List.clear();//vacia lista

 unsigned int i;
 for (i = 0; i < NroSalas; i++)
 {
	 ExitGames::LoadBalancing::Room *sala = salas[i];
	 
	 cd_type_room item_sala;
	 item_sala.count_players = sala->getPlayerCount();
	 item_sala.max_players = sala->getMaxPlayers();
	 ext_wcstombs(item_sala.name, sala->getName(), sala->getName().length());

	 cd_Room_List.push_back(item_sala);

	 //===================== debug
	 
	 Str_Photon_Debug("Room name:  %s Players (%d , %d)  \n", item_sala.name, item_sala.count_players, item_sala.max_players);
	 //==========

 }

 return NroSalas;

}


int  ColdevPhoton::ex_GET_ROOM(int roomid)
{
	if (roomid >= cd_Room_List.size() ) return 0;


	cd_type_room *item_sala =   &cd_Room_List[roomid];	

	return (int)item_sala;
}


int  ColdevPhoton::ex_GET_CURRENT_ROOM( )
{
	if (!cd_load_balancing_client)
	{
		Str_Photon_Debug("List Room Client is Null \n");
		return 0;
	}


	ExitGames::LoadBalancing::Client *cliente = (ExitGames::LoadBalancing::Client *) cd_load_balancing_client;
	
	ExitGames::LoadBalancing::MutableRoom sala =  cliente->getCurrentlyJoinedRoom();

	cd_current_room.count_players = sala.getPlayerCount();
	cd_current_room.max_players = sala.getMaxPlayers();
	ext_wcstombs(cd_current_room.name, sala.getName(), sala.getName().length());

	
	return (int)&cd_current_room;
}


void ColdevPhoton::ex_SET_ROOM_PASSWORD(char *pass)
{
	strcpy_s(cd_password_lobby,255, pass);

}


int ColdevPhoton::ex_SET_ROOM_ISOPEN(bool isopen)
{
	if (!cd_load_balancing_client)
	{
		Str_Photon_Debug("List Room Client is Null \n");
		return 0;
	}	 

	ExitGames::LoadBalancing::Client *cliente = (ExitGames::LoadBalancing::Client *) cd_load_balancing_client;

	if (!cliente->getIsInRoom()) return 0;

	if (!cliente->getLocalPlayer().getIsMasterClient()) return 0;//no es servidor?

	cliente->getCurrentlyJoinedRoom().setIsOpen(isopen);

	return 1;
}



int ColdevPhoton::ex_SET_ROOM_ISVISIBLE(bool isvisible)
{
	if (!cd_load_balancing_client)
	{
		Str_Photon_Debug("List Room Client is Null \n");
		return 0;
	}

	ExitGames::LoadBalancing::Client *cliente = (ExitGames::LoadBalancing::Client *) cd_load_balancing_client;

	if (!cliente->getIsInRoom()) return 0;

	if (!cliente->getLocalPlayer().getIsMasterClient()) return 0;//no es servidor?

	cliente->getCurrentlyJoinedRoom().setIsVisible(isvisible);

	return 1;
}



int  ColdevPhoton::ex_ROOM_SET_PROPS(char *llave, char* valor)
{
	if (!cd_load_balancing_client)
	{
		Str_Photon_Debug("List Room Client is Null \n");
		return 0;
	}

	ExitGames::LoadBalancing::Client *cliente = (ExitGames::LoadBalancing::Client *) cd_load_balancing_client;

	if (!cliente->getIsInRoom()) return 0;

	if (!cliente->getLocalPlayer().getIsMasterClient()) return 0;//no es servidor?

	cliente->getCurrentlyJoinedRoom().addCustomProperty(llave, valor);

	return 1;
}


const char *  ColdevPhoton::ex_ROOM_GET_PROPS(char *llave )
{
	if (!cd_load_balancing_client)
	{
		Str_Photon_Debug("List Room Client is Null \n");
		return 0;
	}

	ExitGames::LoadBalancing::Client *cliente = (ExitGames::LoadBalancing::Client *) cd_load_balancing_client;

	if (!cliente->getIsInRoom()) return 0;

	if (!cliente->getLocalPlayer().getIsMasterClient()) return 0;//no es servidor?

	const char *res= cliente->getCurrentlyJoinedRoom().getCustomProperties().getValue(llave)->toString().UTF8Representation().cstr();

	return res;
}
// =======================================   end+  ROOMS =============================================







// =======================================   PLAYERS  =============================================

 


//custom properties
int  ColdevPhoton::ex_PLAYER_SET_CUSTOM_PROPS(char *llave, char * valor)
{
	if (!cd_load_balancing_client)
	{
		Str_Photon_Debug("VAR Client is Null \n");
		return 0;
	}


	ExitGames::LoadBalancing::Client *cliente = (ExitGames::LoadBalancing::Client *) cd_load_balancing_client;


	cliente->getLocalPlayer().addCustomProperty(llave, valor);
	return 1;
}

 

//get custom properties
const char *  ColdevPhoton::ex_PLAYER_GET_CUSTOM_PROPS(int playerNro, char *llave)
{
	if (!cd_load_balancing_client)
	{
		Str_Photon_Debug("VAR Client is Null \n");
		return 0;
	}


	ExitGames::LoadBalancing::Client *cliente = (ExitGames::LoadBalancing::Client *) cd_load_balancing_client;


	const  ExitGames::Common::JVector<ExitGames::LoadBalancing::Player*> jugadores = cliente->getCurrentlyJoinedRoom().getPlayers();

	int i, totaljugadores = cliente->getCurrentlyJoinedRoom().getPlayerCount();

 //==============
	 

	const char * res = 0;

	for (i = 0; i < totaljugadores; i++)
	{
		cd_type_player jugador;

		if (jugadores[i]->getNumber() == playerNro)
		{
			if (jugadores[i]->getCustomProperties().contains(llave))
				res =  jugadores[i]->getCustomProperties().getValue(llave)->toString().UTF8Representation().cstr() ;

			break;
		}
	}

	return res;
}

 



int  ColdevPhoton::ex_PLAYER_LIST( )
{
	if (!cd_load_balancing_client)
	{
		Str_Photon_Debug("VAR Client is Null \n");
		return 0;
	}


	ExitGames::LoadBalancing::Client *cliente = (ExitGames::LoadBalancing::Client *) cd_load_balancing_client;

	const  ExitGames::Common::JVector<ExitGames::LoadBalancing::Player*> jugadores=  cliente->getCurrentlyJoinedRoom().getPlayers();

	int i, totaljugadores = cliente->getCurrentlyJoinedRoom().getPlayerCount();
	
	//debug	 
	//Str_Photon_Debug("Player list : %d   \n", totaljugadores);
	//==========

	cd_Player_List.clear();


	for (i = 0; i < totaljugadores; i++)
	{
		cd_type_player jugador  ;

		jugador.playerid =  jugadores[i]->getNumber() ;
		jugador.isMasterClient = (int)jugadores[i]->getIsMasterClient();

		ExitGames::Common::JString nombre = jugadores[i]->getName();
		ext_wcstombs(jugador.name, nombre, nombre.length());

		cd_Player_List.push_back(jugador);

		//===================== debug		 
	//	Str_Photon_Debug("Jugador:  %s Ismaster %d  ID  %d  \n", jugador.name, jugador.isMasterClient, jugador.playerid);
		//==========

	}

	return totaljugadores;
}


int   ColdevPhoton::ex_PLAYER_GET(int playerid)
{
	int i;
	cd_type_player *jugador		;

		
	for(i=0;i<ex_PLAYER_LIST();i++)
	{ 
	  jugador = &cd_Player_List[i];

	  if (jugador->playerid == playerid) return (int)jugador;
	}

	return 0;
}


int   ColdevPhoton::ex_PLAYER_GET_BY_INDEX(int playerindex)
{
	if (playerindex >= cd_Player_List.size()) return 0;


	cd_type_player *jugador = &cd_Player_List[playerindex];

	return (int)jugador;
}



int  ColdevPhoton::ex_GET_CURRENT_PLAYER()
{
	if (!cd_load_balancing_client)
	{
		Str_Photon_Debug("VAR Client is Null \n");
		return 0;
	}


	ExitGames::LoadBalancing::Client *cliente = (ExitGames::LoadBalancing::Client *) cd_load_balancing_client;

	ExitGames::LoadBalancing::MutablePlayer jugador = cliente->getLocalPlayer();

	cd_current_player.isMasterClient = (int)jugador.getIsMasterClient();
	cd_current_player.playerid = jugador.getNumber();
	ext_wcstombs(cd_current_player.name, jugador.getName(), jugador.getName().length());


	return (int)&cd_current_player;
}



int  ColdevPhoton::ex_Kick_PLAYER(int playernro)
{
	if (!cd_load_balancing_client)
	{
		Str_Photon_Debug("VAR Client is Null \n");
		return 0;
	}

	ExitGames::LoadBalancing::Client *cliente = (ExitGames::LoadBalancing::Client *) cd_load_balancing_client;

	if (!cliente->getLocalPlayer().getIsMasterClient()) return 0;//no es servidor?


	cd_type_private_event evento;

	evento.player_id = playernro;
	evento.sendcode = cd_EVENT_PLAYER_KICK;

	cd_PrivateEvents_List.push_back(evento);
	return 1;
}
// =======================================   end + PLAYERS  =============================================








// =======================================   REGIONS =============================================
 
int ColdevPhoton::ex_REGION_LIST(  )
{
	return cd_Region_List.size();
}




char * ColdevPhoton::ex_REGION_GET(int regionid)
{	
	if (regionid >= cd_Region_List.size()) return 0;

	return cd_Region_List[regionid].name;
}




void ColdevPhoton::ex_REGION_SET(int regionmode  , char *regioncode )
{
	cd_region_mode = regionmode;     //ExitGames::LoadBalancing::RegionSelectionMode::BEST;

	strcpy_s(cd_current_region, 30, regioncode);//region europa default
}
 

// =======================================   end+  REGIONS =============================================






// =======================================   SERVER =============================================
int ColdevPhoton::ex_SERVER_PING()
{
	if (!cd_load_balancing_client)
	{
		Str_Photon_Debug("VAR Client is Null \n");
		return 0;
	}


	ExitGames::LoadBalancing::Client *cliente = (ExitGames::LoadBalancing::Client *) cd_load_balancing_client;

	return cliente->getRoundTripTime();

}


char * ColdevPhoton::ex_SERVER_GETADDRESS()
{
	
	if (!cd_load_balancing_client)
	{
		Str_Photon_Debug("VAR Client is Null \n");
		return cd_server_address;
	}


	ExitGames::LoadBalancing::Client *cliente = (ExitGames::LoadBalancing::Client *) cd_load_balancing_client;

    ExitGames::Common::JString direccion =  cliente->getMasterserverAddress();

	ext_wcstombs(cd_server_address, direccion, direccion.length());


	return cd_server_address;
}




void ColdevPhoton::ex_SERVER_FETCHSERVERTIMESTAMP()
{
	if (!cd_load_balancing_client)
	{
		Str_Photon_Debug("VAR Client is Null \n");
		return ;
	}


	ExitGames::LoadBalancing::Client *cliente = (ExitGames::LoadBalancing::Client *) cd_load_balancing_client;

	cliente->fetchServerTimestamp();

}


int ColdevPhoton::ex_SERVER_GETSERVERTIME()
{
	if (!cd_load_balancing_client)
	{
		Str_Photon_Debug("VAR Client is Null \n");
		return 0;
	}


	ExitGames::LoadBalancing::Client *cliente = (ExitGames::LoadBalancing::Client *) cd_load_balancing_client;

	return cliente->getServerTime();

}
// =======================================   end+  SERVER =============================================






// =======================================   HASH MAPS =============================================


int ColdevPhoton::ex_HASH_MAP_CREATE()
{
	const  ExitGames::Common::Hashtable *listahash = new ExitGames::Common::Hashtable();

	return (int)listahash;

}

int ColdevPhoton::ex_HASH_MAP_COUNT(int lista)
{
	const  ExitGames::Common::Hashtable *listahash = (const  ExitGames::Common::Hashtable *)lista;

	return listahash->getSize()  ;

}


const char * ColdevPhoton::ex_HASH_MAP_KEY_GET(int lista, int indice)
{
	const  ExitGames::Common::Hashtable *listahash = (const  ExitGames::Common::Hashtable *)lista;
    
	return listahash->getKeys().getElementAt(indice).toString().UTF8Representation().cstr();

}


int  ColdevPhoton::ex_HASH_MAP_ITEM_SET(int lista, char *llave, char *valor)
{
	//const  ExitGames::Common::Hashtable *listahash = (const  ExitGames::Common::Hashtable *)lista;

	//return listahash->put(ExitGames::Common::JString( valor) );
	return 0;
}

// =======================================   end+  HASH MAPS =============================================







//======================================== STRINGS FUNCS =============================================

char * cd_MallocStringCopy( char *source)
{
 char *dest;

 int tamano1 = strlen(source) + 10;

 dest = (char *)malloc(tamano1);

 memset(dest, 0, tamano1); //init strings

 strcpy_s(dest, tamano1, source);

 return dest;

}

 

void ext_wcstombs(char *dest, const wchar_t *source, size_t length)
{
	wcstombs(dest, source, length);

	dest[length] = '\0';
}

//======================================= end + STRINGS ============================================== 
