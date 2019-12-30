 
  
 

type Posicion2D   //paquete personalizado
    int tipo_paquete = 1;     
	int x;	
	int y; 	
end;
 


//==================================  GLOBALS ==============================
global
     
   Posicion2D jugador;  

 	float velocidad = 20.0;	
	float deltaTime;	



/*
  ==================================
   MAIN
  ==================================
*/						
private

	
Begin
 
  
  set_mode (320,240,32);                     
   
  file = fpg_load ("test.fpg") ;
 
  set_fps (30,0); //max.  frames        
  
  
 // write_var(0,100,70,0, deltaTime);
  
  
  
  graph = 101;
  x=y=100;
  size =25;
   
   compute_deltatime();
   
  while (!key(_esc) )
 
     
  
	if (key(_up)) y-= velocidad * deltaTime;  end
	if (key(_down)) y+= velocidad * deltaTime; end
	
	if (key(_left)) x-= velocidad * deltaTime; end
	if (key(_right)) x+= velocidad * deltaTime; end
	
	
	 
    frame;       
  end;    

  let_me_alone();
  
    	 

End


 


process compute_deltatime()
private 
   	    last_time=0;		 
begin
   
  loop
	
   deltaTime = (GET_TIMER() -  last_time)/100.0;
   last_time = GET_TIMER();
   
   frame;
  end
end

