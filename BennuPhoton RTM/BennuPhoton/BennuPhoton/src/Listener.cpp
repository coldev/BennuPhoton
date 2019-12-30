#include "Photon_lib.h"
#include "..\ColdevPhoton.h"



//================================================
//interface global vars 
extern ColdevPhoton *global_ColdevPhoton ;


//================================================



//marco to disable warning -Wunused-but-set-variable
#define USED_VAR(type, name) type name = 0; if(name){}

extern UIListenerPtr gUIListener;

Listener::Listener(CPhotonLib& lib, ExitGames::Common::Logger& logger) 
#ifdef _EG_MS_COMPILER
#	pragma warning(push)
#	pragma warning(disable:4355)
#endif
: cPhotonLib(lib)
, mLogger(logger)
#ifdef _EG_MS_COMPILER
#	pragma warning(pop)
#endif
{
}

Listener::~Listener()
{
}

void Listener::debugReturn(int /*debugLevel*/, const ExitGames::Common::JString& string)
{
	gUIListener->writeString(ExitGames::Common::JString(L"debugReturn - ") + string + L"\n");
}

void Listener::connectionErrorReturn(int errorCode)
{
	EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"code: %d", errorCode);
	gUIListener->writeString(ExitGames::Common::JString(L"received connection error ") + errorCode);
	cPhotonLib.setState(CPhotonLib::Disconnected);
	gUIListener->writeString(L"-------DISCONNECTED-------");
}

void Listener::clientErrorReturn(int errorCode)
{
	EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"code: %d", errorCode);
	gUIListener->writeString(ExitGames::Common::JString(L"received error ") + errorCode + L" from client");
}

void Listener::warningReturn(int warningCode)
{
	EGLOG(ExitGames::Common::DebugLevel::WARNINGS, L"code: %d", warningCode);
	gUIListener->writeString(ExitGames::Common::JString(L"received warning ") + warningCode + L" from client");
}

void Listener::serverErrorReturn(int errorCode)
{
	EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"code: %d", errorCode);
	gUIListener->writeString(ExitGames::Common::JString(L"received error ") + errorCode + " from server");
}

void Listener::joinRoomEventAction(int playerNr, const ExitGames::Common::JVector<int>& /*playernrs*/, const ExitGames::LoadBalancing::Player& player)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"%ls joined the game", player.getName().cstr());
	gUIListener->writeString(L"");
	gUIListener->writeString(ExitGames::Common::JString(L"player ") + playerNr + L" " + player.getName() + L" has joined the game");

	//========================================	
	global_ColdevPhoton->ex_SET_NET_EVENT(cd_PHOTON_EVENT_PLAYER_CONNECTED, playerNr); //evento red
	//========================================


	 
	if (cPhotonLib.getClient().getLocalPlayer().getIsMasterClient()  )
	{ 
	 cd_type_private_event evento;

	 if (strlen(global_ColdevPhoton->cd_password_lobby) > 0)
	 { 
	  evento.player_id = playerNr;
	  evento.sendcode = cd_EVENT_ROOM_PASSWORD;
	  strcpy_s(evento.data, 255, global_ColdevPhoton->cd_password_lobby);//send passw

	  global_ColdevPhoton->cd_PrivateEvents_List.push_back(evento);
	 }
	}
}

void Listener::leaveRoomEventAction(int playerNr, bool isInactive)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	gUIListener->writeString(L"");
	gUIListener->writeString(ExitGames::Common::JString(L"player ") + playerNr + L" has left the game");


	//========================================	
	global_ColdevPhoton->ex_SET_NET_EVENT(cd_PHOTON_EVENT_PLAYER_DISCONNECTED, playerNr); //evento red
   //========================================
}





void Listener::RevisarEventosPrivados(ExitGames::Common::Hashtable eventDataContent, nByte eventCode)
{

	if (cPhotonLib.getClient().getLocalPlayer().getIsMasterClient()) return; //solo clientes pueden leer eventos

	if (eventCode == cd_EVENT_ROOM_PASSWORD)
	{

		ExitGames::Common::JString miclave;

		if (eventDataContent.getValue(L"password"))
		{
			miclave = ExitGames::Common::ValueObject<ExitGames::Common::JString>(eventDataContent.getValue(L"password")).getDataCopy();

			char clavered[255];
			ext_wcstombs(clavered, miclave, miclave.length());

			if (strcmp(clavered, global_ColdevPhoton->cd_password_lobby) != 0)//clave de red no coincide
			{
				global_ColdevPhoton->ex_SET_NET_EVENT(cd_PHOTON_EVENT_ERROR_PASSWORD, cd_PHOTON_EVENT_ERROR_PASSWORD); //evento red --------------

				global_ColdevPhoton->cd_Exit_Thread = true;//desconectar
			}
		}

	}


	if (eventCode == cd_EVENT_PLAYER_KICK)
	{

		int player_id = 0;

		if (eventDataContent.getValue(L"kick"))
			player_id = (ExitGames::Common::ValueObject<int>(eventDataContent.getValue(L"kick"))).getDataCopy();

		if (cPhotonLib.getClient().getLocalPlayer().getNumber() == player_id)
		{
			global_ColdevPhoton->ex_SET_NET_EVENT(cd_PHOTON_EVENT_PLAYER_KICKED, cd_PHOTON_EVENT_PLAYER_KICKED); //evento red --------------

			global_ColdevPhoton->cd_Exit_Thread = true;//desconectar
		}
	}
}




 
#include "../zlib/ColdevCompress.h"


void Listener::customEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContentObj)
{
	  
	
	switch (eventCode)
	{
		 
	case 0:
	{
		// First we need to get a pointer to our payload that's contained in the outer Hashtable at key EV_RT_KEY_DATA
		ExitGames::Common::Hashtable eventDataContent = ExitGames::Common::ValueObject<ExitGames::Common::Hashtable>(eventContentObj).getDataCopy();

			

			if (eventDataContent.getValue(L"pak_"    ))
			{ 
				global_ColdevPhoton->LockMutex();  //threads safe			 
				
				nByte *paquete_bennu = (ExitGames::Common::ValueObject<nByte *>(eventDataContent.getValue(L"pak_"   ))).getDataCopy();

				int paquete_longitud = *(ExitGames::Common::ValueObject<nByte*>(eventDataContent.getValue(L"pak_"   ))).getSizes();
			  
			  //======================
			  cd_type_net_pack *paquete = new cd_type_net_pack;
			 


			  //=========== compress
			  if (global_ColdevPhoton->ex_Get_VAR_GLOBAL(PHOTON_COMPRESSENABLE))
			  {
				  ColdevCompressData *comprime = new ColdevCompressData;
				  int destbuffer = NULL, destlength;
				  paquete->data_length = 0;
				  
				   
				  if (comprime->cd_uncompressBytes((unsigned char*)paquete_bennu, paquete_longitud, &destbuffer, &destlength) == 0)
				  { 
				  //=========== compress ================
				   paquete->data_length = destlength;				   
	
				   paquete->data = (char*)malloc(destlength);	
				   memset(paquete->data, 0, destlength);	
				   memcpy(paquete->data,(void*) destbuffer, destlength);
	
				  //=================================			    
				   
				  }

				  if (comprime) delete comprime;//compress =========

			  }
			  else  
			  { 
				   
			  //===================  
			   paquete->data_length = paquete_longitud;
			   paquete->data = ( char*)malloc(paquete_longitud);
			   memset(paquete->data,0, paquete_longitud);
			   memcpy(paquete->data, paquete_bennu, paquete_longitud);
			    
			  //---------------------------
			  }


			  paquete->number_player = playerNr;			  
			  
			  if (paquete->data_length > 0)
				  global_ColdevPhoton->cd_Delivery_List.push_back(paquete);
			  else
				  delete paquete;  //no hay paquetes validos

			   //=====================
			  ExitGames::Common::MemoryManagement::deallocateArray(paquete_bennu);

			  global_ColdevPhoton->UnlockMutex();  //threads safe
			   
			}//while pak_

	}//mensajes ppal
	break;

	default:
		ExitGames::Common::Hashtable eventDataContent = ExitGames::Common::ValueObject<ExitGames::Common::Hashtable>(eventContentObj).getDataCopy();

		RevisarEventosPrivados(eventDataContent, eventCode);
		break;
	}//switch

	 
}




void Listener::connectReturn(int errorCode, const ExitGames::Common::JString& errorString, const ExitGames::Common::JString& cluster)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"connected to cluster " + cluster);
	if(errorCode)
	{
		EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
		cPhotonLib.setState(CPhotonLib::ErrorConnecting);
		return;
	}
	cPhotonLib.setState(CPhotonLib::Connected);
	gUIListener->writeString(L"-------CONNECTED-------");


}

void Listener::disconnectReturn(void)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	cPhotonLib.setState(CPhotonLib::Disconnected);
	gUIListener->writeString(L"\n-------DISCONNECTED-------\n");

}

void Listener::createRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& /*gameProperties*/, const ExitGames::Common::Hashtable& /*playerProperties*/, int errorCode, const ExitGames::Common::JString& errorString)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	if(errorCode)
	{
		EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
		gUIListener->writeString(L"opCreateRoom() failed: " + errorString);
		//cPhotonLib.setState(CPhotonLib::Connected);
		cPhotonLib.setState(CPhotonLib::ErrorJoining);
		return;
	}

	global_ColdevPhoton->ex_Set_VAR_GLOBAL (PHOTON_PLAYER_NUMBER, localPlayerNr);

	EGLOG(ExitGames::Common::DebugLevel::INFO, L"localPlayerNr: %d", localPlayerNr);
	gUIListener->writeString(L"... room " + cPhotonLib.getClient().getCurrentlyJoinedRoom().getName() + " has been created");
	gUIListener->writeString(L"regularly sending dummy events now");
	
	cPhotonLib.setState(CPhotonLib::Joined);
	gUIListener->writeString(L"\n----------JOINED-------\n");
}

void Listener::joinOrCreateRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& /*gameProperties*/, const ExitGames::Common::Hashtable& /*playerProperties*/, int errorCode, const ExitGames::Common::JString& errorString)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	if(errorCode)
	{
		EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
		gUIListener->writeString(L"opJoinOrCreateRoom() failed: " + errorString);
		//cPhotonLib.setState(CPhotonLib::Connected);
		cPhotonLib.setState(CPhotonLib::ErrorJoining);
		return;
	}

	global_ColdevPhoton->ex_Set_VAR_GLOBAL(PHOTON_PLAYER_NUMBER, localPlayerNr);


	EGLOG(ExitGames::Common::DebugLevel::INFO, L"localPlayerNr: %d", localPlayerNr);
	gUIListener->writeString(L"... room " + cPhotonLib.getClient().getCurrentlyJoinedRoom().getName() + " has been entered");
	gUIListener->writeString(L"regularly sending dummy events now");

	cPhotonLib.setState(CPhotonLib::Joined);
	gUIListener->writeString(L"\n----------JOINED-------\n");
}

void Listener::joinRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& /*gameProperties*/, const ExitGames::Common::Hashtable& /*playerProperties*/, int errorCode, const ExitGames::Common::JString& errorString)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	if(errorCode)
	{		
		EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
		gUIListener->writeString(L"opJoinRoom() failed: " + errorString);
		//cPhotonLib.setState(CPhotonLib::Connected);
		cPhotonLib.setState(CPhotonLib::ErrorJoining);
		return;
	}

	global_ColdevPhoton->ex_Set_VAR_GLOBAL(PHOTON_PLAYER_NUMBER, localPlayerNr);


	EGLOG(ExitGames::Common::DebugLevel::INFO, L"localPlayerNr: %d", localPlayerNr);
	gUIListener->writeString(L"... room " + cPhotonLib.getClient().getCurrentlyJoinedRoom().getName() + " has been successfully joined");
	gUIListener->writeString(L"regularly sending dummy events now");

	cPhotonLib.setState(CPhotonLib::Joined);
	gUIListener->writeString(L"\n----------JOINED-------\n");
}

void Listener::joinRandomRoomReturn(int localPlayerNr, const ExitGames::Common::Hashtable& /*gameProperties*/, const ExitGames::Common::Hashtable& /*playerProperties*/, int errorCode, const ExitGames::Common::JString& errorString)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	if(errorCode)
	{
		EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
		gUIListener->writeString(L"opJoinRandomRoom() failed: " + errorString);
		//cPhotonLib.setState(CPhotonLib::Connected);
		cPhotonLib.setState(CPhotonLib::ErrorJoining);
		return;
	}

	global_ColdevPhoton->ex_Set_VAR_GLOBAL(PHOTON_PLAYER_NUMBER, localPlayerNr);


	EGLOG(ExitGames::Common::DebugLevel::INFO, L"localPlayerNr: %d", localPlayerNr);	
	gUIListener->writeString(L"... room " + cPhotonLib.getClient().getCurrentlyJoinedRoom().getName() + " has been successfully joined");
	gUIListener->writeString(L"regularly sending dummy events now");

	cPhotonLib.setState(CPhotonLib::Joined);
	gUIListener->writeString(L"\n----------JOINED-------\n");
}

void Listener::leaveRoomReturn(int errorCode, const ExitGames::Common::JString& errorString)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	if(errorCode)
	{
		EGLOG(ExitGames::Common::DebugLevel::ERRORS, L"%ls", errorString.cstr());
		gUIListener->writeString(L"opLeaveRoom() failed: " + errorString);
		//cPhotonLib.setState(CPhotonLib::Disconnecting);
		cPhotonLib.setState(CPhotonLib::ErrorLeaving);
		return;
	}
	cPhotonLib.setState(CPhotonLib::Left);
	gUIListener->writeString(L"\n----------LEFT-------\n");
}

void Listener::joinLobbyReturn(void)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	gUIListener->writeString(L"joined lobby");
}

void Listener::leaveLobbyReturn(void)
{
	EGLOG(ExitGames::Common::DebugLevel::INFO, L"");
	gUIListener->writeString(L"left lobby");
}




//update events ==================

  void Listener::onRoomListUpdate(void)
{
	  global_ColdevPhoton->ex_SET_NET_EVENT(cd_PHOTON_EVENT_ROOM_LIST_UPDATE, cd_PHOTON_EVENT_ROOM_LIST_UPDATE); //evento red --------------

}


  void Listener::onRoomPropertiesChange(const ExitGames::Common::Hashtable& /*changes*/)
{
	  global_ColdevPhoton->ex_SET_NET_EVENT(cd_PHOTON_EVENT_ROOM_PROPERTIES_CHANGE, cd_PHOTON_EVENT_ROOM_PROPERTIES_CHANGE); //evento red --------------

}


  void Listener::onPlayerPropertiesChange(int playerNr, const ExitGames::Common::Hashtable& propiedades/*changes*/)
{
	  const  ExitGames::Common::Hashtable *listapropiedades=   new ExitGames::Common::Hashtable(propiedades);

	  global_ColdevPhoton->ex_SET_NET_EVENT(cd_PHOTON_EVENT_PLAYER_PROPERTIES_CHANGE, playerNr, (int)listapropiedades); //evento red --------------

}


// receive available server regions during connect workflow (if you have specified in the constructor, that you want to select a region)
  void Listener::onAvailableRegions(const ExitGames::Common::JVector<ExitGames::Common::JString>& availableRegions, const ExitGames::Common::JVector<ExitGames::Common::JString>& /*availableRegionServers*/)
{

	int i, totalregiones=   availableRegions.getSize();

	global_ColdevPhoton->cd_Region_List.clear();

	for (i = 0; i < totalregiones; i++)
	{
		cd_type_region item_region;

		ext_wcstombs(item_region.name, availableRegions[i], availableRegions[i].length());
		
		global_ColdevPhoton->cd_Region_List.push_back(item_region);

		//debug
		global_ColdevPhoton->Str_Photon_Debug("Region %d %s ",i, item_region.name);
		//===========
	}

	int res= cPhotonLib.getClient().selectRegion (global_ColdevPhoton ->cd_current_region);

	

	//debug
	global_ColdevPhoton->Str_Photon_Debug("Region select %s result %d  ",  
		global_ColdevPhoton->cd_current_region, res );
    //==========

	global_ColdevPhoton->ex_SET_NET_EVENT(cd_PHOTON_EVENT_REGION_LIST, cd_PHOTON_EVENT_REGION_LIST); //evento red --------------
}





