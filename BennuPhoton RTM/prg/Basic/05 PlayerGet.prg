 
 include "Photon.inc"

 


//==================================  GLOBALS ==============================
global
  	
   string MatchName= "My_Player_Info"; 
   string PlayerName= ""; //blank = auto generate name
   string AppVersion =  "1.0";  
   string AppID = "7f105a0f-c232-4ddd-81ff-f9c6029a4a69";//=> replace your APPID

   
   
   string mensaje_evento;  
   string mensaje_jugador_conectado;  
	


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
   
   
  //==============    PROCESS ===========

  read_net_events();

 //==========================================  
  
  write(0,100,10,0,"  Photon. (Player Info Example) ");
  
  write(0,50,20,0,"FPS: ");  
  write_int(0,100,20,0, &fps);
  write_var(0,100,30,0, mensaje_evento);
  
  write(0,50,40,0,"PLAYER Nr: ");    
  write_int(0,150,40,0, &PHOTON_PLAYER_ID);

  write_var(0,20,50,0, mensaje_jugador_conectado);
    
   
  
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
  int codigo, evento,i; 
 Photon_Player pointer player;  
  
begin

   loop     
	  //get net events
	  while (   (  evento_red = PHOTON_EVENT_GET()  )     > 0)
	     codigo = evento_red.data_id;
		 evento = evento_red.code_event;
		 
	     if (evento == PHOTON_EVENT_PLAYER_CONNECTED )//connected
		      
		     say("Cliente conectado # "+itoa(codigo));
			 mensaje_evento ="event connected player";
			 			  
			 player = photon_player_get( codigo);              
			 if (player>0)
			   mensaje_jugador_conectado = "["+itoa(codigo)+"] Jugador conectado nombre "+player.name;
			 end
			 
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
			  
		 elseif (evento == PHOTON_EVENT_DISCONNECTING )
		     mensaje_evento ="event disconnecting";	    	    
			 say("evento desconectando.. ");
			 
		 elseif (evento == PHOTON_EVENT_DISCONNECTED )
		     mensaje_evento ="event disconnected";	    	    
			 say("evento desconectado.. ");
			 
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
	
  