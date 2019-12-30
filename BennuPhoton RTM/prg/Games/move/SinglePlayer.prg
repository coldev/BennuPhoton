Program move;
Begin

   set_mode (640,480,16);
   load_fpg("lucha.fpg");
   luchador();
End

//***********************************************

Process luchador()
Begin
 x=100;y=400;
 graph=1;
 Frame;
 flags=1;
write(0,50,50,0,"teclas:    left (izq.)  right (der.)    space (golpe)"); 


Loop


If (key(_right));flags=1;graph++; x+=4;
 If(graph>4) graph=1;
 If (x>580) x=80;
End End End
If (key(_left));flags=0;graph++;x-=4;
If(graph>3) graph=1;
If (x<80) x=580;
End End End
If (key(_space))graph=5;Frame(700);graph=1;
End  

If (key(_esc)) exit("",0);End

Frame;
End
End  

//************************************** 






