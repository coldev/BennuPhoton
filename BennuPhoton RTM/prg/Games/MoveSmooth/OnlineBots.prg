  include "../Photon.inc"

 #define lerpf(a,b,t) ( a + ((b - a)* t ) ) 

const  
  total_conectados = 255;
  total_bots = 5;   

  UPDATE_FREC_SEND_PACK = 200; //update each 100 ms  
end 


//==================================  TYPE  ==============================

 

type GenericPack  //paquete generico 
    int tipo_paquete;     
end

type Posicion2D   //paquete personalizado
    int tipo_paquete = 1;     
    int x,y;
	
	int speedx, speedy;
end;


type Posicion2DBots   //paquete server      
    int x,y;	 
	
	int speedx, speedy;
end;
 
 
type Posicion2DServer   //paquete personalizado
    int tipo_paquete = 2;     
    int x,y;
	 
	
	int speedx, speedy;
	
	Posicion2DBots bots[total_bots];
end;

 


//==================================  GLOBALS ==============================
global
   INT tamano_posicion2D ;
   INT tamano_posicion2DServer ;
   
   int ret;
	
   string MatchName= "My_Basic_Match"; 
   string PlayerName= ""; //blank = auto generate name
   string AppVersion =  "1.0";  
   string AppID = "7f105a0f-c232-4ddd-81ff-f9c6029a4a69";//=> replace your APPID

   
   
   int Clientes_Conectados[total_conectados];
   Posicion2D  Clientes_Posicion[total_conectados];
   Posicion2DBots  Bots_Server [total_conectados];
   int contador_bots;
   
   string mensaje_evento,es_servidor;   

    
    Posicion2D jugador; 
	Posicion2DServer jugador_servidor;

   int Client_Targets[total_conectados];
   
   
   	float deltaTime;	
	float velocidad = 20.0;

end




/*
  ==================================
   MAIN
  ==================================
*/

Process CreateOnlineModule()						
private
	int i,opcion_region;	
	
	struct mis_regiones[4]; //chat visual list     
	string name;	
   end
	
Begin

mis_regiones[0].name = "us"; 
mis_regiones[1].name = "eu"; 
mis_regiones[2].name = "asia"; 

 write(0,10,20,0,"Region:(1)America (2)Europa (3)Asia");  

  loop
    if (key(_1)) opcion_region=0;break; end 
	if (key(_2)) opcion_region=1;break; end 
	if (key(_3)) opcion_region=2;break; end 
	  
  
    frame;
  end

  delete_text(0);//borrar text writes
  
 //=========================== 


tamano_posicion2D = sizeof(Posicion2D);
tamano_posicion2DServer =sizeof(Posicion2DServer);

  for (i=0;i<total_conectados;i++)
     Clientes_Conectados[i]=false;    
  end
 

  photon_compress_enable = true;  //enable compress packs (NET FAST)
  
 
  //photon_debug = true;   
  
  //PHOTON_INIT(string match_name,  string  player_name, 
  //            string app_version, string app_id)
  
  PHOTON_INITIALIZE(MatchName, PlayerName , AppVersion, AppID);

  //PHOTON_REGION_SET(PHOTON_REGION_BEST,"");
  photon_region_set(PHOTON_REGION_SELECT, mis_regiones[opcion_region].name  );  
  
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
  
  
  
 
  
  //===================    PROCESS ========================
  read_net_events(); 
  read_net_packs();
  
 
  
  //===================    PROCESS ========================
  
  
  
  while (!key(_esc) )   
 	 
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
 private 
      MAX_LAG_PIXEL_TOLERANCY;
 
begin
  
  if (Clientes_Conectados[codigo]  OR codigo == PHOTON_PLAYER_ID ) //ya creado
    return;
  end
  
  Clientes_Conectados[codigo]= true;
  
   //the same sprite data (see multiplayer.prg)
   graph = 101;
  size =25;
  
  x =  Clientes_Posicion[codigo].x;
  y =  Clientes_Posicion[codigo].y;
  
  MAX_LAG_PIXEL_TOLERANCY= velocidad/4;//velocidad es 20 , maximo error por lag es la mitad
  
  while ( Clientes_Conectados[codigo])
    
  
    if (abs(Clientes_Posicion[codigo].x - x) > MAX_LAG_PIXEL_TOLERANCY ) 
     x =  lerpf(x,Clientes_Posicion[codigo].x, deltaTime);
	end//end
    if (abs(Clientes_Posicion[codigo].y - y) > MAX_LAG_PIXEL_TOLERANCY )  	
	 y =  lerpf(y,Clientes_Posicion[codigo].y, deltaTime);
	end 
	 
     x += Clientes_Posicion[codigo].speedx *  deltaTime;      
	 y += Clientes_Posicion[codigo].speedy *  deltaTime;
	 
 	 
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
	 
	  if (generico.tipo_paquete == 1 )	//posicion2d  
     	say("recibido pos2d  ");
		
		
		//crea un cliente si no existe....
         memcopy(&Clientes_Posicion[paquete_recibido.player_id],
		paquete_recibido.data, paquete_recibido.data_length);
		
		
		//si llega un paquete quiere decir que el cliente existe
		create_net_clients( paquete_recibido.player_id);		
	  end
	  
	  if (generico.tipo_paquete == 2 AND NOT photon_is_master_client)	//posicion2d del server 
	  
    		//crea un cliente si no existe....		 	
         memcopy(&jugador_servidor,
		paquete_recibido.data, paquete_recibido.data_length);
		
		Clientes_Posicion[photon_master_client_id].x = jugador_servidor.x;
		Clientes_Posicion[photon_master_client_id].y = jugador_servidor.y;
		
		Clientes_Posicion[photon_master_client_id].speedx = jugador_servidor.speedx;
		Clientes_Posicion[photon_master_client_id].speedy = jugador_servidor.speedy;		
		
        create_net_clients( photon_master_client_id);	 	
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
 	 
       
	 photon_pack_sendgroup(& paquete, false, &Client_Targets,  cuenta_conectados ); //send clients	 
 	
	 
end	

 
