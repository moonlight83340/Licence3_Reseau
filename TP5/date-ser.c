#include "bor-util.h"
#include "bor-timer.h"

int dialoguer_avec_un_client(int soc){
	struct sockaddr_un adr_client;
	char buf1[2048],buf2[2048];
	printf("Attente message client ...\n");
	int k=bor_recvfrom_un_str(soc,buf1,sizeof(buf1),&adr_client);
	printf("Reçu %d octets de %s : \"%s\"\n",k,adr_client.sun_path,buf1 );
	//on fabrique la réponse
	time_t tps;
	time(&tps);
	sprintf(buf2,"DATE : %s",ctime(&tps));
	printf("Envoi de la réponse ...\n");
	k=bor_sendto_un_str(soc,buf2,&adr_client);
	if(k<0){
		return -1;
	}
	printf("Envoyé %d octets à %s : \"%s\"\n",k,adr_client.sun_path,buf2 );
	return 1;		//pour tester : commande netcat
}

int boucle_prime=1;

void capter_SIGITN(int sig){
	printf("signal %d capté\n", sig);
	boucle_prime=0;
}


int main (int argc,char * argv[]){
	if(argc-1 !=1){
		fprintf(stderr, "usage : %s socket_serveur \n",argv[0] );
		exit(1);
	}
	char * nom_serveur = argv[1];
	bor_signal(SIGINT,capter_SIGITN,0);
	struct sockaddr_un adr_serveur;
	int soc = bor_create_socket_un(SOCK_DGRAM,nom_serveur,&adr_serveur);
	if(soc <0){
		exit(1);
	}

	while(boucle_prime){
		int k=dialoguer_avec_un_client(soc);
		if(k<0){
			break;
		}
	}
	printf("Fermeture socket locale\n");
	close(soc);
	unlink(adr_serveur.sun_path);
	exit(0);
}
