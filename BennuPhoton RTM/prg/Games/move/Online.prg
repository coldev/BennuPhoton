  include "../Photon.inc"

  

const  
  total_conectados = 255;  

  UPDATE_FREC_SEND_PACK = 100; //update each 100 ms  
end 


//==================================  TYPE  ==============================

 

type GenericPack  //paquete generico 
    int tipo_paquete;     
end

type Posicion2D   //paquete personalizado
    int tipo_paquete = 1;     
	int x;	
	int y; 
	int graph;
	int flags;
end


 

 


//==================================  GLOBALS ==============================
global
   INT tamano_posicion2D ;
   int ret;
	
   string MatchName= "My_Basic_Match"; 
   string PlayerName= ""; //blank = auto generate name
   string AppVersion =  "1.0";  
   string AppID = "7f105a0f-c232-4ddd-81ff-f9c6029a4a69";//=> replace your APPID

   
   
   int Clientes_Conectados[total_conectados];
   Posicion2D  Clientes_Posicion[total_conectados];
   string mensaje_evento,es_servidor;   

    
   Posicion2D jugador;  

   int Client_Targets[total_conectados];
end




/*
  ==================================
   MAIN
  ==================================
*/

Process CreateOnlineModule()						
private
	int i;	
Begin

tamano_posicion2D = sizeof(Posicion2D);

  for (i=0;i<total_conectados;i++)
     Clientes_Conectados[i]=false;    
  end
  
  //photon_debug = true;   
  
  //PHOTON_INIT(string match_name,  string  player_name, 
  //            string app_version, string app_id)
  
  PHOTON_INITIALIZE(MatchName, PlayerName , AppVersion, AppID);

  //PHOTON_REGION_SET(PHOTON_REGION_BEST,"");
  
  //connect to photon server
  PHOTON_ROOM_JOINORCREATE();
   
  
  write(0,100,10,0,"  Photon. (Basic Example) ");
  
  write(0,50,20,0,"FPS: ");  
  write_int(0,100,20,0, &fps);
  write_var(0,100,30,0, mensaje_evento);
  
  write(0,50,40,0,"PLAYER Nr: ");    
  write_int(0,150,40,0, &PHOTON_PLAYER_ID);

  write_var(0,50,50,0, es_servidor);

  write(0,20,60,0, "Players: ");  
  write_int(0,100,60,0, &photon_player_count);
  
  write(0,150,60,0, "Players Max: ");  
  write_int(0,250,60,0, &photon_player_max);
  
  write(0,20,70,0,"MyPing: ");  
  write_int(0,100,70,0, &photon_server_ping);
  
  
  
  graph = 101;
  x=y=100;
  size =25;
  
  //===================    PROCESS ========================
  read_net_events(); 
  read_net_packs();
  
  CREATE_REPLICATE_NET_OBJECT(&jugador, tamano_posicion2D, UPDATE_FREC_SEND_PACK);	
  //===================    PROCESS ========================
  
  
  
  while (!key(_esc) )   
	if (key(_up)) y--;  end
	if (key(_down)) y++; end
	if (key(_left)) x--; end
	if (key(_right)) x++; end	
 	
      jugador.x = x;
      jugador.y = y;	  
 	 
	 if (photon_is_master_client)
	  es_servidor = "es_servidor";
	 else
	  es_servidor = ""	; 
	 end 
	 
  
    frame;       
  end;    

  let_me_alone();
  
  PHOTON_FINALIZE();  	 

End







/*
  ==================================
   CREATE_REPLICATE_NET_OBJECT 
  ==================================
*/	
process CREATE_REPLICATE_NET_OBJECT(pointer paqueteenviar, int tamanopaquete,    int NetupdateFrecuencyMS)
private 
   int counter_clock;
Begin

   LOOP
    if (   (counter_clock < GET_TIMER()) AND (PHOTON_CONNECTED)  )
     send_packet_others(paqueteenviar, tamanopaquete);	
   
     counter_clock = GET_TIMER() + NetupdateFrecuencyMS ;  //send pack each N ms
    end
	
    frame;
   end
END





 



/*
  ==================================
   CREATE GRAPH CLIENTS
  ==================================
*/						

						
process create_net_clients( codigo) //crear graficos de clientes
   
begin
  
  if (Clientes_Conectados[codigo]  OR codigo == PHOTON_PLAYER_ID ) //ya creado
    return;
  end
  
  Clientes_Conectados[codigo]= true;
  
  x=100;y=400; //the same sprite data (see multiplayer.prg)
 graph=1;
 Frame;
 flags=1;
  
  while ( Clientes_Conectados[codigo])
     
     x= Clientes_Posicion[codigo].x;      
	 y= Clientes_Posicion[codigo].y;
	 graph= Clientes_Posicion[codigo].graph;
	 flags= Clientes_Posicion[codigo].flags;
	 
 	 
     frame;
  end
  
end		



/*
  ==================================
   READ PACKAGES
  ==================================
*/

process read_net_packs()
private 
  Photon_Packed pointer paquete_recibido;
  GenericPack pointer generico;  

  
begin

  loop
  
//================  READ PACKETS ==================  	 
     while (  (paquete_recibido = PHOTON_PACK_GET()  )    > 0)     
      generico =  paquete_recibido.data;
	 
	  if (generico.tipo_paquete == 1)	//posicion2d  
	  
        //say("recibido paquete L: "+ itoa(paquete_recibido.data_length)
	   // +" jugador: "+ itoa(paquete_recibido.number_player) );
		
		//crea un cliente si no existe....
         memcopy(&Clientes_Posicion[paquete_recibido.player_id],
		paquete_recibido.data, paquete_recibido.data_length);
		
		
		//si llega un paquete quiere decir que el cliente existe
		create_net_clients( paquete_recibido.player_id);		
	  end
	   
	  PHOTON_PACK_REMOVE();// remove first packet	    
	end//while 
//================  END + READ PACKETS ==================     

	 
     frame;
  end
  
end						





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
			 //crea un nuevo cliente
		     create_net_clients( codigo);	
			 
		 elseif (evento == PHOTON_EVENT_PLAYER_DISCONNECTED )//disconnect
		     mensaje_evento ="event disconnected player";
		     say("Cliente desconectado # "+codigo);
		     Clientes_Conectados[codigo]= false;
			 
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
	


	
 




/*
  ==================================
   SEND PACKET OTHERS
  ==================================
*/						
	
function send_packet_others(pointer paqueteenviar, int tamanopaquete) 
private
  int   i=0,cuenta_conectados=0, totalplayers;  
  Photon_Player pointer player;
  Photon_Packed paquete;
begin
  
  //====================   ARMA PAQUETE PARA ENVIAR A OTROS CLIENTES =========================
  
     //======================== //prepara paquetes
	  
      paquete.data =  paqueteenviar;	 
	  paquete.data_length = tamanopaquete;  //indica el tamano del paquete
	  //========================	  
 
	 totalplayers= photon_player_list( ); //get current room	 
  
     for(i=0; i<totalplayers; i++) //get players connected
       player = photon_player_get_by_index(i);
   
       if ((player > 0 ) AND (player.player_id <> PHOTON_PLAYER_ID)  )  
	   
         Client_Targets[cuenta_conectados] =  player.player_id ;		 
		 cuenta_conectados++;		 
		 
       end
     end//for	
 	 
       
	 photon_pack_sendgroup(& paquete, true, &Client_Targets,  cuenta_conectados ); //send clients	 
 	
	 
end	




