 
  
 
include "OnlineBots.prg";
 
 
 
 


//==================================  GLOBALS ==============================
global
     
   

 		



/*
  ==================================
   MAIN
  ==================================
*/						
private
   i;
	
Begin
 
  
  set_mode (320,240,32);                     
   
  file = fpg_load ("test.fpg") ;
 
  set_fps (60,0); //max.  frames        
  
 
  
  
  graph = 101;
  x=y=100;
  size =25;
   
   compute_deltatime();
   
   
   
   CreateOnlineModule();//network
   

   
   
  while (!key(_esc) )
  
    if (NOT photon_is_master_client)	//cliente?
     jugador.speedx= 0;
	 jugador.speedy= 0;
	 
	 jugador.y= y;
	 jugador.x= x;
	else      //servidor?
	 
     jugador_servidor.speedx= 0;
	 jugador_servidor.speedy= 0;
	 
	 jugador_servidor.y= y;
	 jugador_servidor.x= x;  	
    end

	
  
	if (key(_up))
   	   y-= velocidad * deltaTime;  
	   jugador.speedy= -velocidad ; 
	   jugador_servidor.speedy= -velocidad ;     
	end
	if (key(_down)) 
		y+= velocidad * deltaTime; 
		jugador.speedy= velocidad ; 
		jugador_servidor.speedy= velocidad ; 
	end
	
	if (key(_left))
		x-= velocidad * deltaTime; 
		jugador.speedx= -velocidad ;
		jugador_servidor.speedx= -velocidad ;  
	end
	if (key(_right)) 
		x+= velocidad * deltaTime; 
		jugador.speedx= velocidad ; 
		jugador_servidor.speedx= velocidad ;  
	end
	
	
	
	if (PHOTON_CONNECTED AND
	   not exists(type CreateBots) AND photon_is_master_client)//es servidor?
	
	  for (i=0;i<total_bots;i++)
        CreateBots(rand(1,320),rand(1,240));
      end
	  	  
      CREATE_REPLICATE_NET_OBJECT(&jugador_servidor, tamano_posicion2DServer, UPDATE_FREC_SEND_PACK);
	end
	
	
	if (PHOTON_CONNECTED AND
	NOT photon_is_master_client AND (NOT exists(type CREATE_REPLICATE_NET_OBJECT)))//es cliente?
	
	  CREATE_REPLICATE_NET_OBJECT(&jugador, tamano_posicion2D, UPDATE_FREC_SEND_PACK);	
	  
	  for (i=0;i<total_bots;i++)
        CreateBots_NET(  i);
      end
	  
	end
	 
    frame;       
  end;    

  //let_me_alone();
  
    	 

End


 
 
process CreateBots(x,y)
private 
   tiempo, speedx,speedy;
   idbot;
   velocidadbot;
   
begin
  graph = 100;  
  size =25;  
  
  
  idbot = contador_bots;
  contador_bots++;
      
  	   velocidadbot= 10;//rand(10,20);  //desplazamientos pequenos se nota menos  el lag
	   
	   speedx = velocidadbot ;
	   speedy = velocidadbot ;
	   	   
  loop
   
	
	x+= speedx * deltaTime;
	y+= speedy * deltaTime;
	
	if (x > 320) speedx= -speedx; end
	if (x < 0) speedx= speedx * -1; end

	if (y > 240) speedy= -speedy; end
	if (y < 0) speedy= speedy * -1; end
	
	
	jugador_servidor.Bots[idbot].x = x;
	jugador_servidor.Bots[idbot].y = y;
	jugador_servidor.Bots[idbot].speedx = speedx;
	jugador_servidor.Bots[idbot].speedy = speedy;
	
	
    frame;
  end

end 




process CreateBots_NET( codigo)
private 
   tiempo, speedx,speedy;
   MAX_LAG_PIXEL_TOLERANCY;
   Posicion2DBots Bot;
   
begin
  graph = 100;  
  size =25;  
  
   
  
  loop    
    MAX_LAG_PIXEL_TOLERANCY= jugador_servidor.Bots[codigo].speedx / 2;//  maximo error permitido por lag es la mitad
  
    if ( abs(jugador_servidor.Bots[codigo].x - x) > MAX_LAG_PIXEL_TOLERANCY ) 
     x =  lerpf(x,jugador_servidor.Bots[codigo].x, deltaTime);
	end//end
    if (abs(jugador_servidor.Bots[codigo].y - y) > MAX_LAG_PIXEL_TOLERANCY )  	
	 y =  lerpf(y,jugador_servidor.Bots[codigo].y, deltaTime);
	end 
	 
     x += jugador_servidor.Bots[codigo].speedx *  deltaTime;      
	 y += jugador_servidor.Bots[codigo].speedy *  deltaTime;
	 
	
    frame;
  end

end  






process compute_deltatime()
private 
   	    last_time=0;
		diferencia;		
begin
   
  loop
	
   deltaTime = (GET_TIMER() -  last_time)/100.0;
   last_time = GET_TIMER();
   
   frame;
  end
end

