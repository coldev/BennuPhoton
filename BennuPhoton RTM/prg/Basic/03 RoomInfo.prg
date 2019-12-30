 
 include "Photon.inc"


 


//==================================  GLOBALS ==============================
global
  	
   string MatchName= "My_RoomInfo_Gemix"; 
   string PlayerName= ""; //blank = auto generate name
   string AppVersion =  "1.0";  
   string AppID = "7f105a0f-c232-4ddd-81ff-f9c6029a4a69";//=> replace your APPID

   
   
   string mensaje_evento;   
 

	
	


/*
  ==================================
   MAIN
  ==================================
*/						
	
Begin

  
  set_mode (320,240,32);                     
   
 
  set_fps (200,0); //max.  frames        
  
  

  //PHOTON_DEBUG=true; //slow
 
  
  //PHOTON_INIT(string match_name,  string  player_name, 
  //            string app_version, string app_id)
  
  PHOTON_INITIALIZE(MatchName, PlayerName , AppVersion, AppID);

  //connect to photon server
  PHOTON_ROOM_JOINORCREATE();
   
   
  //==============  GEMIX PROCESS ===========

  read_net_events();

 //==========================================  
  
  write(0,100,10,0,"Gemix Photon. (Room Info Example) ");
  
  write(0,50,20,0,"FPS: ");  
  write_int(0,100,20,0, &fps);
  write_var(0,100,30,0, mensaje_evento);
  
  write(0,50,40,0,"PLAYER Nr: ");    
  write_int(0,150,40,0, &PHOTON_PLAYER_ID);
    
   
  
  while (!key(_esc) )   	
    frame;       
  end;    

  let_me_alone();
  
  PHOTON_FINALIZE();  	 

End

 
 
 


/*
  ==================================
   READ NET EVENTS
  ==================================
*/

process read_net_events()
private
  Photon_Event  pointer evento_red;  
  int codigo, evento;  
  
begin

   loop     
	  //get net events
	  while (   (  evento_red = PHOTON_EVENT_GET()  )     > 0)
	     codigo = evento_red.data_id;
		 evento = evento_red.code_event;
		 
	     if (evento == PHOTON_EVENT_PLAYER_CONNECTED )//connected
		     mensaje_evento ="event connected player"; 
		     say("Cliente conectado # "+itoa(codigo));
				 
		 elseif (evento == PHOTON_EVENT_PLAYER_DISCONNECTED )//disconnect
		     mensaje_evento ="event disconnected player";
		     say("Cliente desconectado # "+codigo);
	
		 elseif (evento == PHOTON_EVENT_CONNECTING )
		     say("evento conectando.. "); 
		     mensaje_evento ="event connecting";  
			 
		 elseif (evento == PHOTON_EVENT_CONNECTED )
		     say("evento conectado.. "); 
		     mensaje_evento ="event connected";  
             			 
			 
		 elseif (evento == PHOTON_EVENT_JOINING )
		     say("evento uniendo a partida.. ");
		     
		 elseif (evento == PHOTON_EVENT_JOINED )
		     mensaje_evento ="event joined";
			 say("evento  unido a partida.. ");
			 
			 create_room_info_event();//only available in join lobby event
			 
		 elseif (evento == PHOTON_EVENT_DISCONNECTING )
		     mensaje_evento ="event disconnecting";	    	    
			 say("evento desconectando.. ");
			 
		 elseif (evento == PHOTON_EVENT_DISCONNECTED )
		     mensaje_evento ="event disconnected";	    	    
			 say("evento desconectado.. ");
		 elseif (evento == PHOTON_EVENT_ROOM_LIST_UPDATE )
		     mensaje_evento ="event room list";	    	    
			 say("event room list.. ");
				 
			 
			 
		 elseif (evento == PHOTON_EVENT_ERROR_CONNECTING )
		     mensaje_evento ="error connecting";		     	    
		 elseif (evento == PHOTON_EVENT_ERROR_JOINING )
		     mensaje_evento ="error joining";		     	    
		 elseif (evento == PHOTON_EVENT_ERROR_LEAVING )
		     mensaje_evento ="error leaving";		     	    
		 end
	  
	     PHOTON_EVENT_REMOVE();//free list		  
	  end//end while
	  
	  frame;
  end
	  
end
	


	
	
process create_room_info_event()
private
  int    totalrooms;  
  Photon_Room pointer room;
   
  
begin

 
  room= photon_room_getcurrent( ); //get current room
  
  if (room>0)   
     write(0,50,110 ,0,
	 room.name+ "  (" +
	 itoa(room.num_players)+  "/" +
	 itoa(room.max_players)+  ") ");   
  end

  write(0,50,120 ,0, "Player MasterID:");
  write_int(0,150,120 ,0, &photon_master_client_id);
  
   
  loop    
     frame;
  end 
  
end	
