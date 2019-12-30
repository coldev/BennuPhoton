/*
  AUTHOR: PRG
   
   
  input text  

*/

global
   string CadenaGlobalChat;
end


function escribe(
string * nmbre, 
int * uno ,
int * contador, 
int* limite_contador,
int*ascii2 ,
int *simbolo
)
begin
if (substr(nmbre[0],-1,1)=="|")
			nmbre[0]=substr(nmbre[0],0,len(nmbre[0])-1);
		end

		simbolo[0]++;

		        if (ascii==13)   return;
                end
				
                if (ascii<>ascii2[0])   uno[0]=false;
                end

               if (ASCII<>0)
                      ascii2[0]=ascii;
			if (uno[0]==false)
				if (not key(_BACKSPACE))
						nmbre[0]+= chr(ASCII);
				else
						nmbre[0]=substr(nmbre[0],0,len(nmbre[0])-1);
				end
				uno[0]=true;
				contador[0]=0;
			else
                        contador[0]++;
			end
		else
			uno[0]=false;
			limite_contador[0]=5;

		end
		if (contador[0]>limite_contador[0])
                uno[0]=false;
                if (limite_contador[0]>1)
                limite_contador[0]=1;
                end
		end



			if (simbolo[0]>12)
				nmbre[0]+="|";
				simbolo[0]=0;
				frame;
			end
end;



process ChatInput( x,y, int longitud ) 
private	 
//string nmbre="";
	simbolo, uno, contador, limite_contador;
	ascii2;
	chatidtexto ;

begin
   CadenaGlobalChat = "";
   
 chatidtexto = write_string(0,x,y,0,&CadenaGlobalChat);
  
 repeat
    if (strlen(CadenaGlobalChat) < longitud OR  key(_BACKSPACE))
	 escribe(&CadenaGlobalChat, &uno,&contador,&limite_contador,&ascii2, &simbolo);	
	end
	
	while (scan_code  ) frame; end//pausa al teclear
	
	frame;
 until (key(_esc) OR key(_enter))  
  
 
 delete_text(chatidtexto);
end;

 
	  