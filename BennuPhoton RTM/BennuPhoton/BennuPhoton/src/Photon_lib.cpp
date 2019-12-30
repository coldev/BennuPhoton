#include "Photon_lib.h"
#include "..\ColdevPhoton.h"


static const ExitGames::Common::JString appId = L"7f105a0f-c232-4ddd-81ff-f9c6029a4a69"; // set your app id here
static const ExitGames::Common::JString appVersion = L"1.0";

static const ExitGames::Common::JString PLAYER_NAME = L"user";


//============================================
//interface global vars 
extern ColdevPhoton *global_ColdevPhoton ;





//============================================



#if defined _EG_WINDOWS_PLATFORM
#	define SLEEP(ms) Sleep(ms)
#else
#	if defined _EG_LINUX_PLATFORM || defined _EG_MARMALADE_PLATFORM || defined _EG_ANDROID_PLATFORM || defined _EG_BLACKBERRY_PLATFORM
#		include "unistd.h"
#	endif
#	define SLEEP(ms) usleep(ms*1000)
#endif

class Listener;
extern UIListenerPtr gUIListener;

CPhotonLib::CPhotonLib(void)
#ifdef _EG_MS_COMPILER
#	pragma warning(push)
#	pragma warning(disable:4355)
#endif
	: mListener(*this, mLogger)
	//, mClient(mListener, appId, appVersion, ExitGames::Photon::ConnectionProtocol::UDP, true)
	, mClient(mListener, global_ColdevPhoton->cd_Photon_AppID, global_ColdevPhoton->cd_Photon_AppVer, ExitGames::Photon::ConnectionProtocol::UDP, true,  global_ColdevPhoton ->cd_region_mode )//, ExitGames::LoadBalancing::RegionSelectionMode::BEST)

#ifdef _EG_MS_COMPILER
#	pragma warning(pop)
#endif
{



	mClient.setDebugOutputLevel(DEBUG_RELEASE(ExitGames::Common::DebugLevel::INFO, ExitGames::Common::DebugLevel::WARNINGS)); // that instance of LoadBalancingClient and its implementation details
	mLogger.setListener(mListener);
	mLogger.setDebugOutputLevel(DEBUG_RELEASE(ExitGames::Common::DebugLevel::INFO, ExitGames::Common::DebugLevel::WARNINGS)); // this class
	ExitGames::Common::Base::setListener(&mListener);
	ExitGames::Common::Base::setDebugOutputLevel(DEBUG_RELEASE(ExitGames::Common::DebugLevel::INFO, ExitGames::Common::DebugLevel::WARNINGS)); // all classes that inherit from Base

	

	gUIListener->writeString(L"Initialize ExitGames library\n");
	m_currentState = PhotonPeerCreated;
	SampleCustomTypeFactory customTypeFactory;
	SampleCustomType::constructClass(customTypeFactory);
}

CPhotonLib::~CPhotonLib(void)
{
	SampleCustomType::deconstructClass();
}

ExitGames::LoadBalancing::Client& CPhotonLib::getClient()
{
	return mClient;
}

int CPhotonLib::createConnection()
{
	if (strlen(global_ColdevPhoton ->cd_Photon_player_name ) <= 0 )//no existe nombre player
	 mClient.connect(ExitGames::LoadBalancing::AuthenticationValues().setUserID(ExitGames::Common::JString()+GETTIMEMS()), PLAYER_NAME+GETTIMEMS());
	else
	 mClient.connect(ExitGames::LoadBalancing::AuthenticationValues().setUserID(ExitGames::Common::JString() + GETTIMEMS()), global_ColdevPhoton ->cd_Photon_player_name );

	m_currentState = Connecting;
	return 0;
}

int CPhotonLib::closeConnection()
{
	mClient.disconnect();
	m_currentState = Disconnecting;
	return 0;
}

short CPhotonLib::join(const char* gameId)
{
	
	ExitGames::LoadBalancing::RoomOptions options;
	options.setMaxPlayers(global_ColdevPhoton->ex_Get_VAR_GLOBAL(PHOTON_PLAYER_MAX) );
/*
	 ExitGames::Common::JVector<ExitGames::Common::JString> *propsListedInLobby= new ExitGames::Common::JVector<ExitGames::Common::JString>;
	propsListedInLobby-> addElement(L"PublicIp");
	options.setPropsListedInLobby(*propsListedInLobby);
	
	std::string cad;
	 ExitGames::Common::Hashtable *props=  new ExitGames::Common::Hashtable();
	 props-> put(L"PublicIp", cad);
	options.setCustomRoomProperties(*props);
	*/
 	
	if (global_ColdevPhoton ->cd_connect_mode == CONNECT_MODE_CREATE)
		return mClient.opCreateRoom(gameId, options);

	if (global_ColdevPhoton->cd_connect_mode == CONNECT_MODE_JOIN)
		return mClient.opJoinRoom(gameId );

	return mClient.opJoinOrCreateRoom(gameId, options);
}

 

short CPhotonLib::leave(void)
{
	return mClient.opLeaveRoom();
}

int CPhotonLib::run()
{
	bool bRun = true;
	//char gameId[] =   //"demo_photon_game";

	global_ColdevPhoton->cd_load_balancing_client = & mClient;//usarlo fuera del thread

	//mClient.setCRCEnabled(true);
	//mClient.setQuickResendAttempts(4);


	//while(bRun  )
	{


		//if(gUIListener->anyKeyPressed()) 			bRun = false;


		if (global_ColdevPhoton->cd_Exit_Thread)//salir hilo
		{
			m_currentState = Left;
			if (! getClient().getIsInRoom() ) 
				bRun = false;//acelerar salida si no esta conectado
			 
		}

		if (getClient().getIsInRoom() )
		{ 
			 

		 global_ColdevPhoton->ex_Set_VAR_GLOBAL(PHOTON_PLAYER_COUNT, getClient().getCurrentlyJoinedRoom().getPlayerCount() );
		 global_ColdevPhoton->ex_Set_VAR_GLOBAL(PHOTON_PLAYER_MAX, getClient().getCurrentlyJoinedRoom().getMaxPlayers() );
		 global_ColdevPhoton->ex_Set_VAR_GLOBAL(PHOTON_ISMASTERCLIENT, (int)getClient().getLocalPlayer().getIsMasterClient() );

		 global_ColdevPhoton->ex_Set_VAR_GLOBAL(PHOTON_MASTERCLIENTID, getClient().getCurrentlyJoinedRoom().getMasterClientID());
		 global_ColdevPhoton->ex_Set_VAR_GLOBAL(PHOTON_SERVERPING, getClient().getRoundTripTime () );		  
		 global_ColdevPhoton->ex_Set_VAR_GLOBAL(PHOTON_SERVERTIME, getClient().getServerTime());

 
		}
	 
		  mClient.service(); // Call this regularly (e.g. once per gameloop) to transmit generated commands and to check for server responses and new events.
//		  mClient.service(); // Call this regularly (e.g. once per gameloop) to transmit generated commands and to check for server responses and new events.
//		  mClient.service(); // Call this regularly (e.g. once per gameloop) to transmit generated commands and to check for server responses and new events.

		   

		switch(m_currentState)
		{
		case PhotonPeerCreated:
			gUIListener->writeString(L"\n-------CONNECTING-------\n");
			createConnection();						

			break;
		case Connecting:
			// Waiting for callback function
			global_ColdevPhoton ->ex_SET_NET_EVENT(cd_PHOTON_EVENT_CONNECTING, cd_PHOTON_EVENT_CONNECTING); //evento red --------------

			break;
		case ErrorConnecting:
			// Stop run cycle
			gUIListener->writeString(L"\nErrorConnecting\n");
			bRun = false;

			global_ColdevPhoton->ex_SET_NET_EVENT(cd_PHOTON_EVENT_ERROR_CONNECTING, cd_PHOTON_EVENT_ERROR_CONNECTING); //evento red --------------

			break;
		case Connected:

			gUIListener->writeString(L"\n-------JOINING-------\n");
			m_currentState = Joining;

			if (join(global_ColdevPhoton->cd_Photon_MatchName ) == -1)  // (join(gameId) == -1)
				m_currentState = ErrorJoining;
						
			global_ColdevPhoton->ex_SET_NET_EVENT(cd_PHOTON_EVENT_CONNECTED, cd_PHOTON_EVENT_CONNECTED); //evento red --------------

			break;
		case Joining:
			// Waiting for callback function

			global_ColdevPhoton->ex_SET_NET_EVENT(cd_PHOTON_EVENT_JOINING, cd_PHOTON_EVENT_JOINING); //evento red --------------

			break;
		case ErrorJoining:
			// Stop run cycle
			bRun = false;
			gUIListener->writeString(L"ErrorJoining\n");

			global_ColdevPhoton->ex_SET_NET_EVENT(cd_PHOTON_EVENT_ERROR_JOINING, cd_PHOTON_EVENT_ERROR_JOINING); //evento red --------------

			break;
		case Joined :
			global_ColdevPhoton->ex_Set_VAR_GLOBAL(PHOTON_CONNECTED, 1);

			m_currentState = Receiving;
			gUIListener->writeString(L"\n-------SENDING/RECEIVING DATA-------\n");

			global_ColdevPhoton->ex_SET_NET_EVENT(cd_PHOTON_EVENT_JOINED, cd_PHOTON_EVENT_JOINED); //evento red --------------

			break;
		case Receiving:
			sendData();
			break;
		case Leaving:
			// Waiting for callback function
			break;
		case ErrorLeaving:
			// Stop run cycle
			bRun = false;

			global_ColdevPhoton->ex_SET_NET_EVENT(cd_PHOTON_EVENT_ERROR_LEAVING, cd_PHOTON_EVENT_ERROR_LEAVING); //evento red --------------

			break;
		case Left:
			m_currentState = Disconnecting;
			gUIListener->writeString(L"\n-------DISCONNECTING-------\n");
			closeConnection();			

			break;
		case Disconnecting:
			// Waiting for callback function

			global_ColdevPhoton->ex_SET_NET_EVENT(cd_PHOTON_EVENT_DISCONNECTING, cd_PHOTON_EVENT_DISCONNECTING); //evento red --------------

			break;
		case Disconnected:			
			// Stop run cycle
			bRun = false;			

			global_ColdevPhoton->ex_SET_NET_EVENT(cd_PHOTON_EVENT_DISCONNECTED, cd_PHOTON_EVENT_DISCONNECTED); //evento red --------------

			break;
		default:
			break;
		}
	//	SLEEP(1);
	}
	if(m_currentState >= Connected && m_currentState < Disconnecting)
	{
	//	m_currentState = Disconnecting;
		//closeConnection();
	}

//	global_ColdevPhoton->ex_Set_VAR_GLOBAL (PHOTON_CONNECTED, 0);


	return 0;
}



CPhotonLib::States CPhotonLib::getState() const
{
	return m_currentState;
}



void CPhotonLib::setState(CPhotonLib::States new_state)
{
	m_currentState = new_state;
}



#include "../zlib/ColdevCompress.h"
 
void CPhotonLib::sendData(void)
{
#define ARMA_PAQUETE()  nByte *pVal2 = (nByte *)Paquete.data; \
						ev.put(L"pak_", pVal2, Paquete.data_length); 

#define CANALES_UDP()/*	global_ColdevPhoton->CanalesUDP++;  \
						if (global_ColdevPhoton->CanalesUDP > 19) global_ColdevPhoton->CanalesUDP = 0; \
						options.setChannelID(global_ColdevPhoton->CanalesUDP);
*/
#define ARMA_COMPRIMIDO() 	ColdevCompressData *comprime=NULL; \
	if (global_ColdevPhoton->ex_Get_VAR_GLOBAL(PHOTON_COMPRESSENABLE)) \
	{ \
		comprime = new ColdevCompressData; \
		int destbuffer = NULL, destlength; \
        if (comprime->cd_compressBytes((unsigned char*)Paquete.data, Paquete.data_length, &destbuffer, &destlength) == 0) \
		{ \
			Paquete.data = (void *)destbuffer; \
			Paquete.data_length = destlength; \
		} \
        else  \
	     Paquete.data_length = 0; \
	}


#define LIBERA_COMPRIMIDO() if (comprime ) delete comprime;
 

	global_ColdevPhoton->LockMutex();  //threads safe


	//===================================

	ExitGames::Common::Hashtable ev;
	auto options = ExitGames::LoadBalancing::RaiseEventOptions();

	//===================================


	//=================================== ENVIO NORMAL

	std::list<cd_type_Internal_net_pack >::iterator ptrEnvio;

	ptrEnvio = global_ColdevPhoton->cd_Shipping_List.begin();

	/*while*/if (ptrEnvio != global_ColdevPhoton->cd_Shipping_List.end())
	{
		ev.removeAllElements();//envio inmediato

		cd_type_Internal_net_pack Paquete = *ptrEnvio;
		
		ARMA_COMPRIMIDO();
		ARMA_PAQUETE();	 
		CANALES_UDP();

		if (Paquete.data_length > 0)
		mClient.opRaiseEvent(Paquete.reliable != 0, ev, 0, options);		 

		LIBERA_COMPRIMIDO();

		ptrEnvio = global_ColdevPhoton->cd_Shipping_List.erase(ptrEnvio); //free mem
	}

    //==============================================================


	//=================================== ENVIO GRUPAL
		 
	ptrEnvio = global_ColdevPhoton->cd_ShippingGroup_List.begin();

	/*while*/if (ptrEnvio != global_ColdevPhoton->cd_ShippingGroup_List.end())
	{
		ev.removeAllElements();//envio inmediato

		cd_type_Internal_net_pack Paquete = *ptrEnvio;

		ARMA_COMPRIMIDO();

		ARMA_PAQUETE();
		CANALES_UDP();

		options.setNumTargetPlayers(Paquete.target_count);
		options.setTargetPlayers(Paquete.target_players);
 
		if (Paquete.data_length > 0)
		mClient.opRaiseEvent(Paquete.reliable != 0, ev, 0, options);
		
		LIBERA_COMPRIMIDO();

		ptrEnvio = global_ColdevPhoton->cd_ShippingGroup_List.erase(ptrEnvio); //free mem
	}

	//==============================================================



	//==============================  EVENTOS PRIVADOS =========================
	
	 

	std::list<cd_type_private_event >::iterator ptrEvent;

	ptrEvent = global_ColdevPhoton->cd_PrivateEvents_List.begin();

	while (ptrEvent != global_ColdevPhoton->cd_PrivateEvents_List.end())
	{		

		ev.removeAllElements();//envio inmediato

		cd_type_private_event Paquete2 = *ptrEvent;
		

		if (Paquete2.sendcode == cd_EVENT_ROOM_PASSWORD)
		{ 
		   ExitGames::Common::JString cadena(Paquete2.data);
		   ev.put(L"password", cadena);			 
		}
		else if (Paquete2.sendcode == cd_EVENT_PLAYER_KICK)
		{
			int player_id = Paquete2.player_id;
			ev.put(L"kick", player_id);
		}		 

		//--------------------		
		int para_jugador = Paquete2.player_id;
		options.setNumTargetPlayers(1);
		options.setTargetPlayers(&para_jugador);//eventos destinados solo a un target		

		 
		mClient.opRaiseEvent(true, ev, Paquete2.sendcode, options);//enviaaa
		//--------------------	 		

		ptrEvent = global_ColdevPhoton->cd_PrivateEvents_List.erase(ptrEvent); //free mem
	}
	
	

	//==========================================================================
	

	global_ColdevPhoton->UnlockMutex();  //threads safe
}






ExitGames::Common::CustomTypeFactory<1>* SampleCustomTypeFactory::copyFactory(void) const
{
	return new SampleCustomTypeFactory(*this);
}

void SampleCustomTypeFactory::destroyFactory(void)
{
	delete this;
}

ExitGames::Common::CustomType<1>* SampleCustomTypeFactory::create(short amount) const
{
	return new SampleCustomType[amount];
}

ExitGames::Common::CustomType<1>* SampleCustomTypeFactory::copy(const ExitGames::Common::CustomType<1>* pToCopy, short amount) const
{
	SampleCustomType* pRetVal = static_cast<SampleCustomType*>(create(amount));
	for(short i=0; i<amount; i++)
		pRetVal[i] = static_cast<const SampleCustomType*>(pToCopy)[i];
	return pRetVal;
}

void SampleCustomTypeFactory::destroy(const ExitGames::Common::CustomType<1>* pToDestroy) const
{
	delete[] (SampleCustomType*)pToDestroy;
}

unsigned int SampleCustomTypeFactory::sizeOf(void) const
{
	return sizeof(SampleCustomType);
}



SampleCustomType::SampleCustomType(void)
{
}

SampleCustomType::SampleCustomType(nByte foo, nByte bar)
{
	mFoo = foo;
	mBar = bar;
}

SampleCustomType::SampleCustomType(const SampleCustomType& toCopy)
	: super()
{
	mFoo = toCopy.mFoo;
	mBar = toCopy.mBar;
}

SampleCustomType::~SampleCustomType(void)
{
}

SampleCustomType& SampleCustomType::operator=(const SampleCustomType& toCopy)
{
	mFoo = toCopy.mFoo;
	mBar = toCopy.mBar;
	return *this;
}

void SampleCustomType::cleanup(void)
{
}

bool SampleCustomType::compare(const ExitGames::Common::CustomTypeBase& other) const
{
	return mFoo == ((SampleCustomType&)other).mFoo && mBar == ((SampleCustomType&)other).mBar;
}

void SampleCustomType::duplicate(ExitGames::Common::CustomTypeBase* const pRetVal) const
{
	*reinterpret_cast<SampleCustomType*>(pRetVal) = *this;
}

void SampleCustomType::deserialize(const nByte* const pData, short length)
{
	if(length!=2)
		return; // something went wrong
	mFoo = pData[0];
	mBar = pData[1];
}

short SampleCustomType::serialize(nByte* const pRetVal) const
{
	if(pRetVal)
	{
		pRetVal[0] = this->mFoo;
		pRetVal[1] = this->mBar;
	}
	return 2;
}

ExitGames::Common::JString& SampleCustomType::toString(ExitGames::Common::JString& retStr, bool withTypes) const
{
	return retStr = ExitGames::Common::JString(L"<") + (withTypes?ExitGames::Common::JString(L"(")+EG_STR_UCHAR+L")":L"") + mFoo + L", " + (withTypes?ExitGames::Common::JString(L"(")+EG_STR_UCHAR+L")":L"") + mBar + L">";
}




