  include "Photon.inc"

  

 
//==================================  TYPE  ==============================

 

type GenericPack  //paquete generico 
    int tipo_paquete;     
end

type Posicion2D   //paquete personalizado
    int tipo_paquete = 1;     
	int x;	
	int y; 
	char nombre[30];

	
    struct botsAllieds[50]	
	   int x,y,z,graph;
	end
end


 

 


//==================================  GLOBALS ==============================
global
   INT tamano_posicion2D ;
   int ret;
	
    
   
   
  Posicion2D jugador;  

  Posicion2D jugadorDescomprimido;
  

/*
  ==================================
   MAIN
  ==================================
*/						
private
	
  id_comp_1 , id_comp_2;
  
  int datosComp, longdest;
  int datosdesComp, londdescomp;
   
  
Begin

set_mode (320,240,32); 

jugador.nombre = "Minombre12345";  //test data  
jugador.x = 101;
jugador.y = 203;

  

tamano_posicion2D = sizeof(Posicion2D);

 id_comp_1= PHOTON_COMPRESS_INITIALIZE();
 id_comp_2= PHOTON_COMPRESS_INITIALIZE();

     write(0,100,30,0,"(Compress Example) ");
 
     write(0,20,40,0," Original Size: "+ tamano_posicion2D );

      //compress ==========================
	 if	(PHOTON_COMPRESS(id_comp_1, &jugador, tamano_posicion2D, &datosComp, &longdest ) >= 0) 	//try compress packed
		 write(0,20,50,0," Compress Size: "+ longdest ); 
     else
	    write(0,20,50,0," compress error " ); 
	 end 	 
	//===================================
 
 
 
 	  //=========  uncompress ============================
	  londdescomp = tamano_posicion2D; //original size		
	  if (PHOTON_UNCOMPRESS(id_comp_2, datosComp, longdest, &datosdesComp, &londdescomp  ) >= 0)//try uncompress packed
         write(0,20,60,0," Uncompress Size: "+ londdescomp ); 
		   
		 mem_copy(&jugadorDescomprimido, datosdesComp, londdescomp );  
		 
		 write(0,20,70,0," PlayerName: "+  jugadorDescomprimido.nombre + 
		 " X "+itoa(jugadorDescomprimido.x)+  " Y "+ itoa(jugadorDescomprimido.y)
		 ); 
		 
       else
	    write(0,20,60,0," uncompress error " ); 		   
	  end

	  //=================================================
  
  
      PHOTON_COMPRESS_FINALIZE(id_comp_1);//free mem
	  PHOTON_COMPRESS_FINALIZE(id_comp_2);
	  
    
  
  while (!key(_esc) )   
	 
    frame;       
  end;    

   
     	 

End






 




 
 
