 
  
 
include "Online.prg";
 
 
 
 


//==================================  GLOBALS ==============================
global
     
   

 		



/*
  ==================================
   MAIN
  ==================================
*/						
private

	
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
     jugador.speedx= 0;
	 jugador.speedy= 0;
	 
	 jugador.y= y;
	 jugador.x= x;
	 
  
	if (key(_up)) y-= velocidad * deltaTime;  jugador.speedy= -velocidad ;     end
	if (key(_down)) y+= velocidad * deltaTime; jugador.speedy= velocidad ; end
	
	if (key(_left)) x-= velocidad * deltaTime; jugador.speedx= -velocidad ; end
	if (key(_right)) x+= velocidad * deltaTime; jugador.speedx= velocidad ; end
	
	
	 
    frame;       
  end;    

  let_me_alone();
  
    	 

End


 


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

