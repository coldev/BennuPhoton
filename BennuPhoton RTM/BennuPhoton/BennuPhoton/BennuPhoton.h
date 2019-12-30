#ifndef BennuPhotonLib 

#define BennuPhotonLib


#include <list>

using namespace std;



//#include "SDL.h"

#include "bgddl.h"
#include "dlvaracc.h"
//#include "xstrings.h"

typedef struct
{
	int number_player;	
	int data_length;

	char * data;	

} Paquete_Red;



typedef struct
{
	int code_event;

	int  data_int;
	

} Tipo_Evento;


#define PHOTON_CONNECTED	0
#define PHOTON_PLAYER_NUMBER			1
#define PHOTON_RELIABLE_PACKET			2





#endif // ! 
