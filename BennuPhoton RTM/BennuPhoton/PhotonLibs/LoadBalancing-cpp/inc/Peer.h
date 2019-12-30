/* Exit Games Photon LoadBalancing - C++ Client Lib
 * Copyright (C) 2004-2017 by Exit Games GmbH. All rights reserved.
 * http://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#pragma once

#include "Photon-cpp/inc/PhotonPeer.h"
#include "Photon-cpp/inc/Enums/EventCode.h"
#include "Photon-cpp/inc/Enums/EventCache.h"
#include "Photon-cpp/inc/Enums/EventKey.h"
#include "Photon-cpp/inc/Enums/OperationCode.h"
#include "Photon-cpp/inc/Enums/ParameterCode.h"
#include "Photon-cpp/inc/Enums/ReceiverGroup.h"
#include "LoadBalancing-cpp/inc/Enums/ErrorCode.h"
#include "LoadBalancing-cpp/inc/Enums/PeerStates.h"
#include "LoadBalancing-cpp/inc/Enums/LobbyType.h"
#include "LoadBalancing-cpp/inc/Enums/MatchmakingMode.h"
#include "LoadBalancing-cpp/inc/AuthenticationValues.h"
#include "LoadBalancing-cpp/inc/LobbyStatsRequest.h"
#include "LoadBalancing-cpp/inc/LobbyStatsResponse.h"
#include "LoadBalancing-cpp/inc/RaiseEventOptions.h"
#include "LoadBalancing-cpp/inc/RoomOptions.h"

namespace ExitGames
{
	namespace LoadBalancing
	{
		class Peer : public Photon::PhotonPeer
		{
		public:
			Peer(Photon::PhotonListener& listener, nByte connectionProtocol=Photon::ConnectionProtocol::DEFAULT);
			virtual ~Peer(void);

			virtual bool opJoinLobby(const Common::JString& lobbyName=Common::JString(), nByte lobbyType=LobbyType::DEFAULT);
			virtual bool opLeaveLobby(void);
			virtual bool opCreateRoom(const Common::JString& gameID, const RoomOptions& options=RoomOptions(), const Common::Hashtable& customLocalPlayerProperties=Common::Hashtable(), const Common::JVector<Common::JString>& expectedUsers=Common::JVector<Common::JString>());
			virtual bool opJoinRoom(const Common::JString& gameID, const RoomOptions& options=RoomOptions(), const Common::Hashtable& customLocalPlayerProperties=Common::Hashtable(), bool createIfNotExists=false, int playerNumber=0, int cacheSliceIndex=0, const Common::JVector<Common::JString>& expectedUsers=Common::JVector<Common::JString>());
			virtual bool opJoinRandomRoom(const Common::Hashtable& customRoomProperties=Common::Hashtable(), nByte maxPlayers=0, nByte matchmakingMode=MatchmakingMode::FILL_ROOM, const Common::JString& lobbyName=Common::JString(), nByte lobbyType=LobbyType::DEFAULT, const Common::JString& sqlLobbyFilter=Common::JString(), const Common::JVector<Common::JString>& expectedUsers=Common::JVector<Common::JString>());
			virtual bool opLeaveRoom(bool willComeBack=false, bool sendAuthCookie=false);
			template<typename Ftype> bool opRaiseEvent(bool reliable, const Ftype& parameters, nByte eventCode, RaiseEventOptions options=RaiseEventOptions());
			template<typename Ftype> bool opRaiseEvent(bool reliable, const Ftype pParameterArray, typename Common::Helpers::ArrayLengthType<Ftype>::type arrSize, nByte eventCode, RaiseEventOptions options=RaiseEventOptions());
			template<typename Ftype> bool opRaiseEvent(bool reliable, const Ftype pParameterArray, const short* pArrSizes, nByte eventCode, RaiseEventOptions options=RaiseEventOptions());
			virtual bool opAuthenticate(const Common::JString& appID, const Common::JString& appVersion, bool encrypted, const AuthenticationValues& authenticationValues=AuthenticationValues(), bool lobbyStats=false, const Common::JString& regionCode=Common::JString());
			virtual bool opAuthenticateOnce(const Common::JString& appID, const Common::JString& appVersion, nByte connectionProtocol, nByte encryptionMode, const AuthenticationValues& authenticationValues=AuthenticationValues(), bool lobbyStats=false, const Common::JString& regionCode=Common::JString());
			virtual bool opFindFriends(const Common::JString* friendsToFind, short numFriendsToFind);
			virtual bool opLobbyStats(const Common::JVector<LoadBalancing::LobbyStatsRequest>& lobbiesToQuery=Common::JVector<LoadBalancing::LobbyStatsRequest>());
			virtual bool opChangeGroups(const Common::JVector<nByte>* pGroupsToRemove, const Common::JVector<nByte>* pGroupsToAdd);
			virtual bool opWebRpc(const Common::JString& uriPath);
			template<typename Ftype> bool opWebRpc(const Common::JString& uriPath, const Ftype& parameters, bool sendAuthCookie=false);
			template<typename Ftype> bool opWebRpc(const Common::JString& uriPath, const Ftype pParameterArray, typename Common::Helpers::ArrayLengthType<Ftype>::type arrSize, bool sendAuthCookie=false);
			template<typename Ftype> bool opWebRpc(const Common::JString& uriPath, const Ftype pParameterArray, const short* pArrSizes, bool sendAuthCookie=false);
			virtual bool opGetRegions(bool encrypted, const Common::JString& appID);
			virtual bool opSetPropertiesOfPlayer(int playerNr, const Common::Hashtable& properties, const Common::Hashtable& expectedProperties=Common::Hashtable(), WebFlags webFlags=WebFlags());
			virtual bool opSetPropertiesOfRoom(const Common::Hashtable& properties, const Common::Hashtable& expectedProperties=Common::Hashtable(), WebFlags webFlags=WebFlags());
		private:
			Photon::OperationRequestParameters opCreateRoomImplementation(const Common::JString& gameID, const RoomOptions& options, const Common::Hashtable& customLocalPlayerProperties, const Common::JVector<Common::JString>& expectedUsers);
			Photon::OperationRequestParameters opJoinRoomImplementation(const Common::JString& gameID, const RoomOptions& options, const Common::Hashtable& customLocalPlayerProperties, bool createIfNotExists, int playerNumber, int cacheSliceIndex, const Common::JVector<Common::JString>& expectedUsers);
			Photon::OperationRequestParameters enterRoomImplementation(const RoomOptions* pOptions, const Common::Hashtable& customLocalPlayerProperties, const Common::JVector<Common::JString>& expectedUsers);
#ifndef EG_NO_CPP11 // workaround for a weird compiler bug in Clang when building against libstdc++ in C++98 mode
			virtual
#endif
				bool opRaiseEvent(bool reliable, const Common::Object& parameters, nByte eventCode, RaiseEventOptions options);
			virtual bool opWebRpc(const Common::JString& uriPath, const Common::Object& parameters, bool sendAuthCookie);

			typedef PhotonPeer super;

			friend class Client;
		};



		template<typename Ftype>
		bool Peer::opRaiseEvent(bool reliable, const Ftype& parameters, nByte eventCode, RaiseEventOptions options)
		{
			COMPILE_TIME_ASSERT2_TRUE_MSG(!Common::Helpers::ConfirmAllowed<Ftype>::dimensions, ERROR_THIS_OVERLOAD_IS_ONLY_FOR_SINGLE_VALUES);
			return opRaiseEvent(reliable, Common::Helpers::ValueToObject::get(parameters), eventCode, options);
		}

		template<typename Ftype>
		bool Peer::opRaiseEvent(bool reliable, const Ftype pParameterArray, typename Common::Helpers::ArrayLengthType<Ftype>::type arrSize, nByte eventCode, RaiseEventOptions options)
		{
			COMPILE_TIME_ASSERT2_TRUE_MSG(Common::Helpers::ConfirmAllowed<Ftype>::dimensions==1, ERROR_THIS_OVERLOAD_IS_ONLY_FOR_1D_ARRAYS);
			return opRaiseEvent(reliable, Common::Helpers::ValueToObject::get(pParameterArray, arrSize), eventCode, options);
		}

		template<typename Ftype>
		bool Peer::opRaiseEvent(bool reliable, const Ftype pParameterArray, const short* pArrSizes, nByte eventCode, RaiseEventOptions options)
		{
			COMPILE_TIME_ASSERT2_TRUE_MSG((Common::Helpers::ConfirmAllowed<Ftype>::dimensions>1), ERROR_THIS_OVERLOAD_IS_ONLY_FOR_MULTIDIMENSIONAL_ARRAYS);
			return opRaiseEvent(reliable, Common::Helpers::ValueToObject::get(pParameterArray, pArrSizes), eventCode, options);
		}

		template<typename Ftype>
		bool Peer::opWebRpc(const Common::JString& uriPath, const Ftype& parameters, bool sendAuthCookie)
		{
			COMPILE_TIME_ASSERT2_TRUE_MSG(!Common::Helpers::ConfirmAllowed<Ftype>::dimensions, ERROR_THIS_OVERLOAD_IS_ONLY_FOR_SINGLE_VALUES);
			return opWebRpc(uriPath, Common::Helpers::ValueToObject::get(parameters), sendAuthCookie);
		}

		template<typename Ftype>
		bool Peer::opWebRpc(const Common::JString& uriPath, const Ftype pParameterArray, typename Common::Helpers::ArrayLengthType<Ftype>::type arrSize, bool sendAuthCookie)
		{
			COMPILE_TIME_ASSERT2_TRUE_MSG(Common::Helpers::ConfirmAllowed<Ftype>::dimensions==1, ERROR_THIS_OVERLOAD_IS_ONLY_FOR_1D_ARRAYS);
			return opWebRpc(uriPath, Common::Helpers::ValueToObject::get(pParameterArray, arrSize), sendAuthCookie);
		}

		template<typename Ftype>
		bool Peer::opWebRpc(const Common::JString& uriPath, const Ftype pParameterArray, const short* pArrSizes, bool sendAuthCookie)
		{
			COMPILE_TIME_ASSERT2_TRUE_MSG((Common::Helpers::ConfirmAllowed<Ftype>::dimensions>1), ERROR_THIS_OVERLOAD_IS_ONLY_FOR_MULTIDIMENSIONAL_ARRAYS);
			return opWebRpc(uriPath, Common::Helpers::ValueToObject::get(pParameterArray, pArrSizes), sendAuthCookie);
		}
	}
}