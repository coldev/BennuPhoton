program test_mode7;

GLOBAL
    int graficos1;
    int color;
BEGIN
    //scale_mode=scale_nofilter;
    set_mode(320,240,8);
    set_fps(50,0);
    
    graficos1=load_fpg("graf2.fpg");
    //graficos1=load_fpg("circuit0.fpg");
    //say(itoa(graficos1));
    
    start_mode7(0,graficos1,1,-1,0,64);
    start_mode7(1,graficos1,2,0,0,64);
    //m7.color=rgb(0,128,255);
    m7[0].color=15;
    m7[0].camera=id;
    
    m7[0].height=9; //A cierta altura del fondo
    m7[0].distance=20; //23; //A cierta distancia del fondo original
    m7[0].focus=256; //128;
    
    m7[1].color=15;
    m7[1].camera=id;
    
    m7[1].height=-6; //A cierta altura del fondo
    m7[1].distance=20; //23; //A cierta distancia del fondo original
    m7[1].focus=256; //128;
    m7[1].z=m7[0].z-1;
    ctype=c_m7;
    //cflags=1;
    
    resolution=100;
    coche(graficos1);
    //file=graficos1;
    //height=0;
    //cnumber=c_1;
    //graph=3;
    //write_int(0,0,0,0,&color);
    
    coche2(graficos1);
    
    while (!key(_esc))
        color=map_get_pixel(graficos1,2,x/resolution,y/resolution);
        if (key(_up)) x+=100; end
        if (key(_down)) x-=100; end
        if (key(_left)) y-=100; end
        if (key(_right)) y+=100; end
        if (key(_c_minus) && m7[0].height>9)
            m7[0].height--;
            m7[1].height--;
        end
        if (key(_c_plus))
            m7[0].height++;
            m7[1].height++;
        end
        if (key(_w)) advance(100); end
        if (key(_s)) advance(-100); end
        if (key(_a)) angle+=2000; end
        if (key(_d)) angle-=2000; end
        
        if (key(_f1)) m7[0].focus--; end
        if (key(_f2)) m7[0].focus++; end
        if (key(_z)) m7[0].distance--; end
        if (key(_x)) m7[0].distance++; end
        if (key(_c)) m7[1].height--; end
        if (key(_v)) m7[1].height++; end
        frame;
    end
    stop_mode7(0);
    stop_mode7(1);
    let_me_alone();
END

process coche (file)
begin
    //x=160;
    //y=201;
    //size=200;
    priority=father.priority-1;
    resolution=100;
    graph=3;
    
    ctype=c_m7;
    cnumber=c_0;
    
    loop
        x=father.x;
        y=father.y;
        frame;
    end
    /*signal(id,s_freeze);
    frame;*/
end

process coche2 (file)
begin
    x=100;
    y=100;
    graph=3;
    ctype=c_m7;
    cnumber=c_0;
    //z=m7[0].z-2;
    //height=-170;
    
    //resolution=100;
    loop
        /*while (x<320)
            x++;
            if (key(_o)) height--; end
            if (key(_p)) height++; end
            frame;
        end
        while (x>0)
            x--;
            if (key(_o)) height--; end
            if (key(_p)) height++; end
            frame;
        end*/
        if (key(_o)) height--; end
        if (key(_p)) height++; end
        /*x=father.x;
        y=father.y;
        xadvance(father.angle, 1000);
        height=m7[0].height-9;*/
        frame;
    end
end
