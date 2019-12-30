program coso;

include "Online.prg";


global
lock;
pipa;
cabeza;
xb;yb;
xg;yg;





begin
load_fpg("coso.fpg");
//full_screen=1;
set_mode(640,480,16);
//put_screen(0,14);
fulano(320,240);

 

CreateOnlineModule()		;//network  ============
end






process fulano(x,y)
begin
    graph=5;priority=1;
    lock=mira();
    cabeza=gerolo();
    pipa=brazo();
    repeat
	//============ network ============
	  jugador.x = x;
      jugador.y = y;		 
	  jugador.cabeza_x = cabeza.x;
	  jugador.cabeza_y = cabeza.y;
	  jugador.cabeza_graph = cabeza.graph;
	  
	  jugador.brazo_x = pipa.x;
	  jugador.brazo_y = pipa.y;
	  jugador.brazo_graph = pipa.graph;
	  jugador.brazo_angle = pipa.angle;
	  
	 
	//============ network =============  
 	
	
        if(key(_left))x-=3;end
        if(key(_right))x+=3;end
        if(key(_up))y-=3;end
        if(key(_down))y+=3;end
        get_real_point(1,offset xb,offset yb);
        get_real_point(2,offset xg,offset yg);
        frame;
    until(key(_esc))
    exit();
end





process brazo()
private buf;
metra;
begin
    z=-1;
loop
        angle=get_angle(lock);
        x=xb;y=yb;
        if(angle<0)angle+=360000;end
        if(angle<60000 and angle>=0 or angle>=315000 and angle<360000)
            graph=1;
            cabeza.graph=8;
        end
        if(angle>=60000 and angle<120000)
            graph=2;
            cabeza.graph=9;
        end
        if(angle>=120000 and angle<225000)
            graph=3;
            cabeza.graph=10;
        end
        if(angle>=225000 and angle<315000)
            graph=4;
            cabeza.graph=11;
        end
        if(key(_1)) if(metra)metra=0;else metra=1;end repeat frame; until (!key(_1)) end
        if(mouse.left and !collision(type mira))
            if(!buf or metra)
            bang(x,y);buf=1;
            end
        else buf=0;
        end
        if(mouse.right and !collision(type mira))
            llama(x,y);llama(x,y);llama(x,y);end
			
		
		
        frame;
end
end


process brazo_network(codigo )
private buf;
metra;
begin
    z=-1;
 while ( Clientes_Conectados[codigo])
  x= Clientes_Posicion[codigo].brazo_x;
y= Clientes_Posicion[codigo].brazo_y;
graph= Clientes_Posicion[codigo].brazo_graph;     
angle= Clientes_Posicion[codigo].brazo_angle;     
	
		
        frame;
end
end

process mira()
begin
    graph=6;
    loop
        x=mouse.x;y=mouse.y;
        frame;
    end
end




process gerolo()

begin
    graph=8;//flags=1;
    loop
    x=xg;y=yg;	
    frame;
    end
end


process gerolo_network(codigo)//network cabeza
begin    
    while ( Clientes_Conectados[codigo])
x= Clientes_Posicion[codigo].cabeza_x;
y= Clientes_Posicion[codigo].cabeza_y;
graph= Clientes_Posicion[codigo].cabeza_graph;     
    frame;
    end
end






process bang(x,y)
private
dx;dy;
an;
p=1;
Bullet2D bala ;

begin
    graph=12;
    angle=get_angle(lock);
    an=angle;
    dx=lock.x;dy=lock.y;
    advance(13);
	
	//============  network ================
	bala.x = x; bala.y = y;
	bala.angle =bala.angle; 
	bala.mira_x = lock.x;
	bala.mira_y = lock.y;	
	send_packet_others(&bala, tamano_bullet2d);
	//=========================================
	
    repeat
        x+=get_distx(an,15);y+=get_disty(an,15);
        if(p)
            if(!(fget_dist(x,y,dx,dy)<8))
                an=fget_angle(x,y,dx,dy);
            else p=0; end
        end
        frame;
    until(out_region(id,0));
end





process bang_network()//network bang
private
dx;dy;
an;
p=1;

begin 
 

    graph=12;	
	x= Balas_Posicion[contador_balas].x;
	y= Balas_Posicion[contador_balas].y;
    angle=Balas_Posicion[contador_balas].angle;
    an=angle;
    dx=Balas_Posicion[contador_balas].mira_x;
	dy=Balas_Posicion[contador_balas].mira_y;

    advance(13);	
	
    repeat
        x+=get_distx(an,15);y+=get_disty(an,15);
        if(p)
            if(!(fget_dist(x,y,dx,dy)<8))
                an=fget_angle(x,y,dx,dy);
            else p=0; end
        end
        frame;
    until(out_region(id,0));
end






process llama(x,y)
private an; byte i;
flamethrower2D llama;
float vy;
begin
graph=15;
size=rand(5,20);
an=get_angle(lock)+rand(-10000,10000);
angle=rand(0,360)*1000; //esta linea causa el problema
xadvance(get_angle(lock),15);
flags=16;

	//================ NETWORK =========================
llama.x = x; llama.y = y; 
llama.angle = get_angle(lock);
	send_packet_others(&llama, tamano_llama2d);
	//=========================================


repeat
xadvance(an,6);
y-=vy;
vy+=0.3;
z++;
angle+=5000;
i+=5;
frame;
size+=7;
until(i>110)
end






process llama_network() //network fire
private an; byte i;
float vy;
begin
x = Llamas_Posicion[contador_llamas].x;
y = Llamas_Posicion[contador_llamas].y;

graph=15;
size=rand(5,20);
an=Llamas_Posicion[contador_llamas].angle+rand(-10000,10000);
angle=rand(0,360)*1000; //esta linea causa el problema
xadvance(Llamas_Posicion[contador_llamas].angle,15);
flags=16;
repeat
xadvance(an,6);
y-=vy;
vy+=0.3;
z++;
angle+=5000;
i+=5;
frame;
size+=7;
until(i>110)
end