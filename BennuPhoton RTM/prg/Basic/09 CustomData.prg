 
  include "Photon.inc"

  include "input/input.prg"

 
//==================================  CONST ==============================

const  
  total_conectados = 255;
    
end 


//==================================  TYPE ==============================

 

type NetCustomPack //generic
    int packtype;  	
end


type Posicion2D //custom
    int packtype = 1;    
	int x;	
	int y; 	
end

type NetChat //custom
    int packtype = 2;    
	char  message[255];	
end





 

 


//==================================  GLOBALS ==============================
global
   INT tamano_posicion2D, tamano_netchat ;
   
 
	
   string MatchName= "My_CustomData_Match"; 
   string PlayerName= ""; //blank = auto generate name
   string AppVersion =  "1.0";  
   string AppID = "7f105a0f-c232-4ddd-81ff-f9c6029a4a69";//=> replace your APPID

   
   
   int Clientes_Conectados[total_conectados];
   Posicion2D  Clientes_Posicion[total_conectados];
   string mensaje_evento,es_servidor;   

   
   Posicion2D jugador; 
   NetChat  jugador_mensaje;

   
   struct ChatItems[8]; //chat visual list     
	string chatitem;	
   end
      

	  
/*
  ==================================
   MAIN
  ==================================
*/	
private 
    int i;		

    int opcion_region; 	

   struct mis_regiones[4]; //chat visual list     
	string name;	
   end
	
Begin

 set_mode (320,240,32); 
 
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


tamano_posicion2D = sizeof(Posicion2D);
tamano_netchat = sizeof(NetChat);

 

  for (i=0;i<total_conectados;i++)
     Clientes_Conectados[i]=false;    
  end
  
                     
   
  file = fpg_load ("test.fpg") ;
 
 set_fps (200,0); //max.  frames        
  
  //photon_debug = true;
  
  
  //PHOTON_INIT(string match_name,  string  player_name, 
  //            string app_version, string app_id)
  
  PHOTON_INITIALIZE(MatchName, PlayerName , AppVersion, AppID);

  photon_region_set(PHOTON_REGION_SELECT, mis_regiones[opcion_region].name  );  
   
  
  
  //connect to photon server
  PHOTON_ROOM_JOINORCREATE();
   
  
  write(0,100,10,0,"  Photon. (CustomData Example) ");
  
  write(0,50,20,0,"FPS: ");  
  write_int(0,100,20,0, &fps);
  write_var(0,100,30,0, mensaje_evento);
  
  write(0,50,40,0,"PLAYER Nr: ");    
  write_int(0,150,40,0, &PHOTON_PLAYER_ID);

  write_var(0,50,50,0, es_servidor);
  write(0,50,60,0,"Press (F5) to chat ");

  write(0,20,70,0,"MyPing: ");  
  write_int(0,100,70,0, &photon_server_ping);
  
  write(0,20,80,0,"region: "+  mis_regiones[opcion_region].name);  
   
  
  graph = 101;
  x=y=100;
  size =25;
  
  //===================    PROCESS ========================
  read_net_events(); 
  read_net_packs();
  //===================    PROCESS ========================
  
  
  
   
  Write(0,20,230,0,"chat:");
  Create_Chat_List();
  
  
  while (!key(_esc) )   
	if (key(_up)) y--;  end
	if (key(_down)) y++; end
	if (key(_left)) x--; end
	if (key(_right)) x++; end
	
	if (key(_f5) ) 
	  while (key(_f5)) frame; end
	  
	  if (not exists(type InputChat))//not multiple chat instances
	     InputChat();
      end 	  
	end
	
	if (PHOTON_CONNECTED)
	
     if (x != jugador.x OR y != jugador.y)	//cambio la posicion
      jugador.x = x;
      jugador.y = y;
	  	  
	  send_player_packet();	
	 end 
	 
	 if (photon_is_master_client)
	  es_servidor = "es_servidor";
	 else
	  es_servidor = ""	; 
	 end
	
	 
	end //if  connected
  
    frame;       
  end;    

  let_me_alone();
  
  PHOTON_FINALIZE();  	 

End


 




/*
  ==================================
   CREATE GRAPH CLIENTS
  ==================================
*/						

						
process create_net_clients( codigo) //crear graficos de clientes
   
begin
  
  if (Clientes_Conectados[codigo] OR codigo == PHOTON_PLAYER_ID ) //ya creado
    return;
  end
  
  Clientes_Conectados[codigo]= true;
  
  graph = 100;
  x=y=200;
  size =25;
  
  while ( Clientes_Conectados[codigo])
     
     x= Clientes_Posicion[codigo].x;      
	 y= Clientes_Posicion[codigo].y;
	 
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
  NetCustomPack pointer genericpack;
  NetChat  paqueteChat;
       
begin

  loop
  
//================  READ PACKETS ==================  	 
     while (  (paquete_recibido = PHOTON_PACK_GET()  )    > 0)     
      
	  genericpack= paquete_recibido.data;//get custom data in a generic struct
	 
	  if (genericpack.packtype == 1)//posicion2d msg
	    		
		//crea un cliente si no existe....
         memcopy(&Clientes_Posicion[paquete_recibido.player_id],
		paquete_recibido.data, paquete_recibido.data_length);
		
		
		//si llega un paquete quiere decir que el cliente existe
		create_net_clients( paquete_recibido.player_id);		
	  end

	  if (genericpack.packtype == 2)//chat msg
		//crea un cliente si no existe....
        memcopy(&paqueteChat,
		paquete_recibido.data, paquete_recibido.data_length);		
		
		//actualiza el listado de chat
        Message_Chat_List("["+itoa(paquete_recibido.player_id)+"] "+ paqueteChat.message);	       
      end
	  
	  
	  PHOTON_PACK_REMOVE();// remove first packet	    
	end//while 
//================  END + READ PACKETS ==================     

	 
     frame;
  end
  
end						





/*
  ==================================
   INPUT CHAT 
  ==================================
*/

process InputChat()
private 
   idchat;
   
begin 
   idchat= ChatInput(70,230, 20) ; 
   
   while (exists(idchat)) frame; end   //espera el chat
   
   jugador_mensaje.message = CadenaGlobalChat;
     
   if (jugador_mensaje.message != "")
     send_chat_packet();
   end
   
end




/*
  ==================================
   CREATE CHAT LIST
  ==================================
*/

process Create_Chat_List()
PRIVATE 
   i;
begin

 for (i=0;i<4;i++)   //create visual slots to chat
  write_var(0,120,150+(i*10),0,ChatItems[i].chatitem);
 end

 loop
  frame;
 end

end





/*
  ==================================
   MESSAGE CHAT LIST 
  ==================================
*/


function Message_Chat_List(string mensaje)
private 
   i;
begin
  
  for (i=5;i>0;i--)
    ChatItems[i].chatitem = ChatItems[i-1].chatitem;    //MOVE CHAT LIST UP-DOWN
  end 

   ChatItems[0].chatitem = mensaje;  
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
   SEND PLAYER PACKS
  ==================================
*/						
	
function send_player_packet()
private 
  Photon_Packed paquete; 
begin        
	  paquete.data = &jugador;
	  paquete.data_length = tamano_posicion2D; 

	  //send un reliable packs  
      PHOTON_PACK_SEND( & paquete ,false);//envia paquete	  
end	




/*
  ==================================
   SEND CHAT PACKS
  ==================================
*/						
	
function send_chat_packet()
private 
  Photon_Packed paquete; 
begin        
	 
	  paquete.data = &jugador_mensaje;
	  paquete.data_length = tamano_netchat; 
	  
	  
	  //send reliable packs  
      PHOTON_PACK_SEND( & paquete ,true);//envia paquete 
end	