// BennuPhoton.cpp: define las funciones exportadas de la aplicación DLL.
//

#include "stdafx.h"



#include <windows.h>
#include <stdio.h>
#include <string.h>

#include "ColdevPhoton.h"



#include "BennuPhoton.h"

ColdevPhoton *global_ColdevPhoton = NULL;



 



 


//funciones internas de Bennu
//-----------------------------------------------------------------------

#if defined(__GNUC__)
#define M_DLLAPI    WINAPI *    //devcpp
#else
#define M_DLLAPI    __cdecl  *  //visual c++
#endif

typedef const char * (M_DLLAPI tstrget)(int code);
typedef void (M_DLLAPI tstrdiscard)(int code);
typedef int (M_DLLAPI tstrnew)(const char *s);
typedef int (M_DLLAPI tstrconcat)(int code1, char * str2);
typedef void (M_DLLAPI tstruse)(int code);







const char * string_get(int code)
{
	const char *cad = "";
	HINSTANCE hDLL = LoadLibrary("libbgdrtm.dll");

	if (hDLL)
	{
		tstrget pfunc = (tstrget)GetProcAddress(hDLL, "string_get");
		if (pfunc)
			cad = (*pfunc)(code);

		FreeLibrary(hDLL);
	}

	return cad;
}

void   string_discard(int code)
{
	HINSTANCE hDLL = LoadLibrary("libbgdrtm.dll");

	if (hDLL)
	{
		tstrdiscard pfunc = (tstrdiscard)GetProcAddress(hDLL, "string_discard");
		if (pfunc)
			(*pfunc)(code);

		FreeLibrary(hDLL);
	}

}

int   string_new(char *s)
{
	HINSTANCE hDLL = LoadLibrary("libbgdrtm.dll");
	int res = 0;

	if (hDLL)
	{
		tstrnew pfunc = (tstrnew)GetProcAddress(hDLL, "string_new");
		if (pfunc)
			res = (*pfunc)(s);

		FreeLibrary(hDLL);
	}

	return res;
}


int   string_concat(int code1, char * str2)
{
	HINSTANCE hDLL = LoadLibrary("libbgdrtm.dll");
	int res = 0;

	if (hDLL)
	{
		tstrconcat pfunc = (tstrconcat)GetProcAddress(hDLL, "string_concat");
		if (pfunc)
			res = (*pfunc)(code1, str2);

		FreeLibrary(hDLL);
	}

	return res;
}


void   string_use(int code)
{
	HINSTANCE hDLL = LoadLibrary("libbgdrtm.dll");

	if (hDLL)
	{
		tstruse pfunc = (tstruse)GetProcAddress(hDLL, "string_use");
		if (pfunc)
			(*pfunc)(code);

		FreeLibrary(hDLL);
	}

}








extern "C" DLLEXPORT char *  __bgdexport(bennuphoton, globals_def) =
"INT PHOTON_CONNECTED = 0; \n"
"INT PHOTON_PLAYER_ID = 0; \n"
"INT PHOTON_PLAYER_MAX = 4; \n"
"INT PHOTON_PLAYER_COUNT = 0; \n"

"INT PHOTON_DEBUG = 0; \n"
"INT PHOTON_IS_MASTER_CLIENT = 0; \n"
"INT PHOTON_COMPRESS_ENABLE = 0; \n"
"INT PHOTON_ENCRYPT_ENABLE = 0; \n"

"INT PHOTON_MASTER_CLIENT_ID = 0; \n"
"INT PHOTON_SERVER_PING = 0; \n"
"INT PHOTON_SERVER_TIME = 0; \n"


;



extern "C" DLLEXPORT DLVARFIXUP __bgdexport(bennuphoton, globals_fixup)[] =
{
	/* Nombre de variable global, puntero al dato, tamaño del elemento, cantidad de elementos */

	{ "PHOTON_CONNECTED"  , NULL, -1, -1 },
	{ "PHOTON_PLAYER_ID"  , NULL, -1, -1 },
	{ "PHOTON_PLAYER_MAX"  , NULL, -1, -1 },
	{ "PHOTON_PLAYER_COUNT"  , NULL, -1, -1 },

	{ "PHOTON_DEBUG"  , NULL, -1, -1 },
	{ "PHOTON_IS_MASTER_CLIENT"  , NULL, -1, -1 },
	{ "PHOTON_COMPRESS_ENABLE"  , NULL, -1, -1 },
	{ "PHOTON_ENCRYPT_ENABLE"  , NULL, -1, -1 },

	{ "PHOTON_MASTER_CLIENT_ID"  , NULL, -1, -1 },
	{ "PHOTON_SERVER_PING"  , NULL, -1, -1 },
	{ "PHOTON_SERVER_TIME"  , NULL, -1, -1 },

	 

	{ NULL              , NULL, -1, -1 }
};



void Cambiar_VAR_GLOBAL(int codigo, int valor)
{

	GLOINT32(bennuphoton, codigo) = valor;
}


int Obtener_VAR_GLOBAL(int codigo)
{

	return GLOINT32(bennuphoton, codigo) ;
}


 


//---------------------------------------------------
//FUNCIONES PPALES
//---------------------------------------------------
 





//===================================  CORE ====================================




static  int  f_photon_init(INSTANCE * my, int * params)
{
	const char *matchname = string_get(params[0]);
	const char *playername = string_get(params[1]);
	const char *appver = string_get(params[2]);
	const char *appid = string_get(params[3]);
 
	//create a global photon interface class
	global_ColdevPhoton = new ColdevPhoton((char*)matchname, (char*)playername, (char*)appver, (char*)appid);


	//liberar memoria
	string_discard(params[0]);
	string_discard(params[1]);
	string_discard(params[2]);
	string_discard(params[3]);

	global_ColdevPhoton->Str_Photon_Debug("Photon VARS INIT ok \n");

	return 1;
}


static  int  f_photon_terminate(INSTANCE * my, int * params)
{
	if (global_ColdevPhoton ) 	delete global_ColdevPhoton;

	global_ColdevPhoton = NULL;

	return 0;
}


//=====================================================  end + CORE ==============================



//=====================================================  PACKS ==============================

static  int  f_photon_get_pack(INSTANCE * my, int * params)
{ 
	return global_ColdevPhoton->ex_GET_NET_PACK();
}


static  int  f_photon_remove_pack(INSTANCE * my, int * params)
{	
	return global_ColdevPhoton->ex_REMOVE_NET_PACK()		;
}



static  int  f_photon_send_pack(INSTANCE * my, int * params)
{
	void *data =(void *) params[0];
	int reliable= params[1]   ;

	return global_ColdevPhoton->ex_SEND_NET_PACK(data, reliable)		;
}



static  int  f_photon_send_pack_group(INSTANCE * my, int * params)
{
	void *data = (void *)params[0];
	int reliable = params[1];
	int *targets = (int*)params[2]		;
	int targetCount = params[3];

	return global_ColdevPhoton->ex_SEND_NET_PACK_GROUP(data, reliable,targets, targetCount);
}

//=====================================================  end + PACKS ==============================



//=====================================================  EVENTS ==============================


static  int  f_photon_get_event(INSTANCE * my, int * params)
{
	return global_ColdevPhoton ->ex_GET_NET_EVENT();
}


static  int  f_photon_remove_event(INSTANCE * my, int * params)
{
	return global_ColdevPhoton->ex_REMOVE_NET_EVENT();
}


//=====================================================  end + EVENTS ==============================



//=====================================================  ROOMS ==============================

static  int  f_photon_joinorcreate(INSTANCE * my, int * params)
{
	global_ColdevPhoton->ex_Create_Thread(CONNECT_MODE_JOINORCREATE);

	return 0;
}

static  int  f_photon_create(INSTANCE * my, int * params)
{
	global_ColdevPhoton->ex_Create_Thread(CONNECT_MODE_CREATE);

	return 0;
}

static  int  f_photon_join(INSTANCE * my, int * params)
{
	global_ColdevPhoton->ex_Create_Thread(CONNECT_MODE_JOIN);

	return 0;
}


static  int  f_photon_room_list(INSTANCE * my, int * params)
{
	return global_ColdevPhoton->ex_LIST_ROOMS();
}

static  int  f_photon_room_get(INSTANCE * my, int * params)
{
	return global_ColdevPhoton->ex_GET_ROOM(params[0]);
}


static  int  f_photon_room_getcurrent(INSTANCE * my, int * params)
{
	return global_ColdevPhoton->ex_GET_CURRENT_ROOM( );
}


static  int  f_photon_room_setpassword(INSTANCE * my, int * params)
{
	const char *clave = string_get(params[0]);

	global_ColdevPhoton->ex_SET_ROOM_PASSWORD((char*)clave);

	string_discard(params[0]);

	return 1;
}


static  int  f_photon_room_set_isopen(INSTANCE * my, int * params)
{
	return global_ColdevPhoton->ex_SET_ROOM_ISOPEN(params[0] != 0);
}


static  int  f_photon_room_set_isvisible(INSTANCE * my, int * params)
{
	return global_ColdevPhoton->ex_SET_ROOM_ISVISIBLE(params[0] != 0);
}


static  int  f_photon_room_set_properties(INSTANCE * my, int * params)
{
	const char *llave = string_get(params[0]);
	const char *valor = string_get(params[1]);

	int res = global_ColdevPhoton->ex_PLAYER_SET_CUSTOM_PROPS ((char*)llave, (char*)valor);

	string_discard(params[0]);
	string_discard(params[1]);

	return res;
}



static  int  f_photon_room_get_properties(INSTANCE * my, int * params)
{
	const char *llave = string_get(params[0]);

	const char *valor = global_ColdevPhoton->ex_ROOM_GET_PROPS ( (char*)llave);

	string_discard(params[0]);

	int res;

	if (valor)
		res = string_new((char*)valor);
	else
		res = string_new("");

	return res;
}
//=====================================================  end + ROOMS ==============================




//=====================================================  PLAYERS==============================

static  int  f_photon_player_list(INSTANCE * my, int * params)
{
	return global_ColdevPhoton->ex_PLAYER_LIST( );
}


static  int  f_photon_player_get(INSTANCE * my, int * params)
{
	return global_ColdevPhoton->ex_PLAYER_GET(params[0]);
}


static  int  f_photon_player_get_byindex(INSTANCE * my, int * params)
{
	return global_ColdevPhoton->ex_PLAYER_GET_BY_INDEX(params[0]);
}


static  int  f_photon_player_getcurrent(INSTANCE * my, int * params)
{
	return global_ColdevPhoton->ex_GET_CURRENT_PLAYER( );
}


static  int  f_photon_player_kick(INSTANCE * my, int * params)
{
	return global_ColdevPhoton->ex_Kick_PLAYER(params[0]);
}



static  int  f_photon_player_set_properties(INSTANCE * my, int * params)
{
	const char *llave = string_get(params[0]);
	const char *valor = string_get(params[1]);

	int res = global_ColdevPhoton->ex_PLAYER_SET_CUSTOM_PROPS((char*)llave, (char*)valor);

	string_discard(params[0]);
	string_discard(params[1]);

	return res;
}



static  int  f_photon_player_get_properties(INSTANCE * my, int * params)
{
	const char *llave = string_get(params[1]);	 	  

	const char *valor=  global_ColdevPhoton->ex_PLAYER_GET_CUSTOM_PROPS (params[0], (char*)llave);

	string_discard(params[1]);

	int res;
	
	if (valor)
	  res= string_new((char*)valor);
	else
	  res = string_new("");

	return res;
}
//=====================================================  end + PLAYERS ==============================



//=====================================================  REGIONS ==============================

static  int  f_photon_region_list(INSTANCE * my, int * params)
{
	return global_ColdevPhoton->ex_REGION_LIST( );
}


static  int  f_photon_region_get(INSTANCE * my, int * params)
{
	char *region =  global_ColdevPhoton->ex_REGION_GET(params[0]);

	int res = string_new(region);

	return res;
}


static  int  f_photon_region_set(INSTANCE * my, int * params)
{
	const char *regiontoken = string_get(params[1]);

	global_ColdevPhoton->ex_REGION_SET(params[0], (char*)regiontoken);

	string_discard(params[0]);

	return 1;
}

//=====================================================  end + REGIONS ==============================



//=====================================================  SERVER ==============================
static  int  f_photon_server_address(INSTANCE * my, int * params)
{
	char *direccion = global_ColdevPhoton->ex_SERVER_GETADDRESS( );

	int res = string_new(direccion);

	return res;	 
}

//=====================================================  end + SERVER ==============================



#include "zlib/ColdevCompress.h"


 
//=====================================================  COMPRESS ==============================

static  int  f_photon_compress_init(INSTANCE * my, int * params)
{
	ColdevCompressData *comprime = new ColdevCompressData;

	return (int)comprime;
}


static  int  f_photon_compress_pack(INSTANCE * my, int * params)
{
	ColdevCompressData *comprime = (ColdevCompressData *)params[0];
	const unsigned char *pData = (const unsigned char*)params[1];
	unsigned long nDataSize = params[2];
	int *destbuffer = (int*)params[3];
	int *destlength = (int*)params[4];

	return comprime->cd_compressBytes(pData, nDataSize, destbuffer, destlength);
 
}

static  int  f_photon_uncompress_pack(INSTANCE * my, int * params)
{
	ColdevCompressData *comprime = (ColdevCompressData *)params[0];
	unsigned char * pCompressedData = (unsigned char *)params[1];
	unsigned long nCompressedDataSize = params[2];

	int *destbuffer = (int*)params[3];
	int *destlength = (int*)params[4];


	return comprime->cd_uncompressBytes(pCompressedData, nCompressedDataSize, destbuffer, destlength);
 
}

static  int  f_photon_compress_free(INSTANCE * my, int * params)
{
	ColdevCompressData *pointerCmp = (ColdevCompressData *)params[0];

	if (pointerCmp)  delete pointerCmp;

	return 0;
}
 

//=====================================================  end + COMPRESS ==============================


/* ----------------------------------------------------------------- */
/* Declaracion de funciones                                          */

#define FENIX_export(a,b,c,d)  {a,b,c,(void*)d}


extern "C" DLLEXPORT DLSYSFUNCS  __bgdexport(bennuphoton, functions_exports)[] = {

	//core	    
	FENIX_export("PHOTON_INITIALIZE", "SSSS", TYPE_DWORD, f_photon_init),
	FENIX_export("PHOTON_FINALIZE", "", TYPE_DWORD, f_photon_terminate),

	//packs
	FENIX_export("PHOTON_PACK_GET", "", TYPE_DWORD, f_photon_get_pack),
	FENIX_export("PHOTON_PACK_REMOVE", "", TYPE_DWORD, f_photon_remove_pack),
	FENIX_export("PHOTON_PACK_SEND", "PI", TYPE_DWORD, f_photon_send_pack),
	FENIX_export("PHOTON_PACK_SENDGROUP", "PIPI", TYPE_DWORD, f_photon_send_pack_group),

	//events
	FENIX_export("PHOTON_EVENT_GET", "", TYPE_DWORD, f_photon_get_event),
	FENIX_export("PHOTON_EVENT_REMOVE", "", TYPE_DWORD, f_photon_remove_event),


	//rooms
	FENIX_export("PHOTON_ROOM_JOINORCREATE", "", TYPE_DWORD, f_photon_joinorcreate),
	FENIX_export("PHOTON_ROOM_CREATE", "", TYPE_DWORD, f_photon_create),
	FENIX_export("PHOTON_ROOM_JOIN", "", TYPE_DWORD, f_photon_join),

	FENIX_export("PHOTON_ROOM_LIST", "", TYPE_DWORD, f_photon_room_list),
	FENIX_export("PHOTON_ROOM_GET", "I", TYPE_DWORD, f_photon_room_get),
	FENIX_export("PHOTON_ROOM_GETCURRENT", "", TYPE_DWORD, f_photon_room_getcurrent),
	FENIX_export("PHOTON_ROOM_SET_PASSWORD", "S", TYPE_DWORD, f_photon_room_setpassword),
	FENIX_export("PHOTON_ROOM_SET_OPEN", "I", TYPE_DWORD, f_photon_room_set_isopen),
	FENIX_export("PHOTON_ROOM_SET_VISIBLE", "I", TYPE_DWORD, f_photon_room_set_isvisible),

	FENIX_export("PHOTON_ROOM_SET_PROPS", "SS", TYPE_DWORD, f_photon_room_set_properties),
	FENIX_export("PHOTON_ROOM_GET_PROPS", "S", TYPE_STRING, f_photon_room_get_properties),

 


	//players
	FENIX_export("PHOTON_PLAYER_LIST", "", TYPE_DWORD, f_photon_player_list),
	FENIX_export("PHOTON_PLAYER_GET", "I", TYPE_DWORD, f_photon_player_get),
	FENIX_export("PHOTON_PLAYER_GET_BY_INDEX", "I", TYPE_DWORD, f_photon_player_get_byindex),
	FENIX_export("PHOTON_PLAYER_GETCURRENT", "", TYPE_DWORD, f_photon_player_getcurrent),
	FENIX_export("PHOTON_PLAYER_KICK", "I", TYPE_DWORD, f_photon_player_kick),

	FENIX_export("PHOTON_PLAYER_SET_PROPS", "SS", TYPE_DWORD, f_photon_player_set_properties),
	FENIX_export("PHOTON_PLAYER_GET_PROPS", "IS", TYPE_STRING, f_photon_player_get_properties),

	 

	//regions
	FENIX_export("PHOTON_REGION_LIST", "", TYPE_DWORD, f_photon_region_list),
	FENIX_export("PHOTON_REGION_GET", "I", TYPE_STRING, f_photon_region_get),
	FENIX_export("PHOTON_REGION_SET", "IS", TYPE_DWORD, f_photon_region_set),

	//server
	FENIX_export("PHOTON_SERVER_ADDRESS", "", TYPE_STRING, f_photon_server_address),

	//compress
	FENIX_export("PHOTON_COMPRESS_INITIALIZE", "", TYPE_DWORD, f_photon_compress_init),
	FENIX_export("PHOTON_COMPRESS", "IPIPP", TYPE_DWORD, f_photon_compress_pack),
	FENIX_export("PHOTON_UNCOMPRESS", "IPIPP", TYPE_DWORD, f_photon_uncompress_pack),
	FENIX_export("PHOTON_COMPRESS_FINALIZE", "I", TYPE_DWORD, f_photon_compress_free),

	

	{ 0            , 0     , 0         , 0 }
};




/* ----------------------------------------------------------------- */

/* Bigest priority first execute
Lowest priority last execute */

extern "C" DLLEXPORT char * __bgdexport(bennuphoton, modules_dependency)[] = {
	"libsdlhandler",
	"mod_string",
	NULL
};

 
 

extern "C" DLLEXPORT void __bgdexport(bennuphoton, module_initialize)()
{

	printf("Initialize Photon - Bennu interface. \n");
}


extern "C" DLLEXPORT void __bgdexport(bennuphoton, module_finalize)()
{
	printf("Unloading.. Photon - Bennu interface. \n");


	 

	printf("Unload Photon Bennu Module \n");
}


extern "C" DLLEXPORT void __bgdexport(bennuphoton, process_exec_hook)()
{
	if (global_ColdevPhoton) 
	  global_ColdevPhoton->ex_FramePhotonCore();

	//printf("exec hook photon. \n");
}

 

