


import "../bennuphoton"  //Photon networking
    
	
type photon_packed

 	int  player_id;	

	int data_length;
	 pointer data;
end ;

 

type photon_event

	int  code_event;
	int  data_id;

end ;


 

type photon_room

	int  max_players;
	int  num_players;

	char name[255];

end;



type photon_player

	int  player_id;
	int  is_Master_Client;	 

	char name[255];

end ;




//================================  event codes ============================
#define PHOTON_EVENT_PLAYER_CONNECTED 1
#define PHOTON_EVENT_PLAYER_DISCONNECTED 2

#define PHOTON_EVENT_CONNECTED 90
#define PHOTON_EVENT_CONNECTING 100
#define PHOTON_EVENT_JOINING 101
#define PHOTON_EVENT_JOINED 102
#define PHOTON_EVENT_DISCONNECTING 103
#define PHOTON_EVENT_DISCONNECTED 104
#define PHOTON_EVENT_PLAYER_KICKED 105


#define PHOTON_EVENT_ERROR_CONNECTING -300
#define PHOTON_EVENT_ERROR_JOINING -310
#define PHOTON_EVENT_ERROR_LEAVING -320
#define PHOTON_EVENT_ERROR_PASSWORD -330


 
#define PHOTON_EVENT_ROOM_PROPERTIES_CHANGE 402
#define PHOTON_EVENT_PLAYER_PROPERTIES_CHANGE 403

//================================ end+  event codes ============================


//================================ region codes ============================
#define PHOTON_REGION_DEFAULT 0
#define PHOTON_REGION_SELECT 1
#define PHOTON_REGION_BEST 2
//================================ end+  region codes ============================



 