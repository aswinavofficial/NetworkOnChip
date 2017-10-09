
#include<iostream>
#include<math.h>
#include<ctime>
#include<sys/time.h>
#include<pthread.h>
#include<stdlib.h>
#include<malloc.h>
#include<string.h>
#include<semaphore.h>
#include<unistd.h>
#define bit bool
#define LEFT 0
#define RIGHT 1
#define UP 2
#define DOWN 3
#define PARENT 2
#define LCHILD 3
#define RCHILD 4
#define NONE 0
#define SEND 1
#define RECEIVE 2
#define EMPTY 0
#define WAITINPUT 1
#define LOADED 2
#define WAITREAD 3
#define READY 1
#define INPROGRESS 2
#define TIMEOUT 10000
using namespace std;
bool loop,simactive;
int threadstatus;
int type;
int row,col;

struct packet {
    int source_address_row, source_address_col;
    int dest_address_row, dest_address_col;
    char data[30];
    int start_timer;
    int end_timer;
    int nhops;
};

class Channel
{
	public:
	sem_t init,transfer;
	int status;
	bool REQ,ACK,DATA,CLK,CHOKE;
	bool Busy_Bit, Receive_Bit, Transfer_Bit;
	struct packet Channel_Data;
	Channel()
	{
		sem_init(&init,0,1);
		sem_init(&transfer,0,1);
		status=EMPTY;
		REQ=false;
		ACK=false;
		DATA=false;
		CLK=false;
		CHOKE=false;
		Busy_Bit=false;
		Receive_Bit=false;
		Transfer_Bit=false;
	}
};
struct Interface
{
	Channel *Connected_Channel;
	bool Interface_Active;
	int Channel_Operation;
	struct packet Receive_Register;
	bool Packet_In_Send_Register;
	struct packet Send_Register;
	bool Packet_In_Receive_Register;
	
};
class MeshNode {
	public:	
	Interface interfaces[4];
	int x,y;
	int Routing_Algorithm(struct packet);
	void Control_Logic();
	void Acquire_Data_Packet(int,int);
};


void MeshNode::Control_Logic()
{
	threadstatus=READY;
	int currinterface=0;
	while(simactive){
	while(!loop);
	while(loop)
	{
		if(interfaces[currinterface].Connected_Channel!=NULL){
		//cout<<"\nChecking interface "<<currinterface<<" on node "<<x<<","<<y<<endl;
		if(interfaces[currinterface].Interface_Active)
		{
			switch(interfaces[currinterface].Channel_Operation)
			{
				case SEND:
						cout<<"\nNode "<<x<<","<<y<<" initiating send on interface "<<currinterface<<"\n";
						while(interfaces[currinterface].Connected_Channel->status!=EMPTY);
						if(!interfaces[currinterface].Connected_Channel->CHOKE&&!interfaces[currinterface].Connected_Channel->Busy_Bit)
						{
							cout<<"\nNode "<<x<<","<<y<<" no choke and busy\n";
							interfaces[currinterface].Connected_Channel->Busy_Bit=true;
							interfaces[currinterface].Connected_Channel->REQ=true;
							interfaces[currinterface].Connected_Channel->Transfer_Bit=true;
							int timer=0;
							interfaces[currinterface].Connected_Channel->status=WAITINPUT;
							while(timer<TIMEOUT&&!interfaces[currinterface].Connected_Channel->ACK)
							{
								for(int del=0;del<1000000;del++);
								timer++;
							}
							if(timer<TIMEOUT)
							{
								cout<<"\nNode "<<x<<","<<y<<" ACK within timeout\n";
								interfaces[currinterface].Connected_Channel->REQ=false;
								interfaces[currinterface].Connected_Channel->Channel_Data=interfaces[currinterface].Send_Register;
								strcpy((interfaces[currinterface].Connected_Channel->Channel_Data).data,interfaces[currinterface].Send_Register.data);
								for(int i=0;i<sizeof(struct packet);i++);
								interfaces[currinterface].Connected_Channel->status=LOADED;
								interfaces[currinterface].Connected_Channel->Busy_Bit=false;
								interfaces[currinterface].Connected_Channel->ACK=false;
								interfaces[currinterface].Connected_Channel->Transfer_Bit=false;
								interfaces[currinterface].Interface_Active=false;
								interfaces[currinterface].Packet_In_Send_Register=false;
								interfaces[currinterface].Channel_Operation=NONE;
							}
							else
							{
                                                                cout<<"Node "<<x<<","<<y<<" timeout or ACK\n";
								interfaces[currinterface].Connected_Channel->Busy_Bit=false;
								interfaces[currinterface].Connected_Channel->REQ=false;
								interfaces[currinterface].Connected_Channel->Transfer_Bit=false;
							}
						}
						else
						{
                                               cout<<"\nNode "<<x<<","<<y<<" removed choke\n";
							interfaces[currinterface].Connected_Channel->CHOKE=false;
						}
						break;
				                case RECEIVE:
						while(interfaces[currinterface].Connected_Channel->status!=WAITINPUT);
						cout<<"\nNode "<<x<<","<<y<<" initiating receive\n";
						if(interfaces[currinterface].Packet_In_Receive_Register)
						{
                                                 cout<<"\nNode "<<x<<","<<y<<" receive already full\n";
							interfaces[currinterface].Connected_Channel->REQ=false;
						}
						else
						{
							interfaces[currinterface].Connected_Channel->ACK=true;
							interfaces[currinterface].Connected_Channel->Receive_Bit=true;
							while(interfaces[currinterface].Connected_Channel->status!=LOADED);
							for(int i=0;i<sizeof(struct packet);i++);
							interfaces[currinterface].Receive_Register=interfaces[currinterface].Connected_Channel->Channel_Data;
							strcpy(interfaces[currinterface].Receive_Register.data,(interfaces[currinterface].Connected_Channel->Channel_Data).data);
							interfaces[currinterface].Packet_In_Receive_Register=true;
							interfaces[currinterface].Connected_Channel->status=EMPTY;
							interfaces[currinterface].Receive_Register.nhops++;
                                                       cout<<"\nNode "<<x<<","<<y<<" received packet "<<interfaces[currinterface].Receive_Register.data<<"\n";
							if(interfaces[currinterface].Packet_In_Send_Register)
								interfaces[currinterface].Connected_Channel->CHOKE=true;
							interfaces[currinterface].Connected_Channel->ACK=false;
							interfaces[currinterface].Connected_Channel->Receive_Bit=false;
							interfaces[currinterface].Connected_Channel->Busy_Bit=false;
							interfaces[currinterface].Connected_Channel->REQ=false;
							if(interfaces[currinterface].Receive_Register.dest_address_row==x&&interfaces[currinterface].Receive_Register.dest_address_col==y)
							{
                                                            cout<<"\nNode "<<x<<","<<y<<" got packet at dest\n";
								interfaces[currinterface].Receive_Register.end_timer=clock();
								int diff=interfaces[currinterface].Receive_Register.end_timer-interfaces[currinterface].Receive_Register.start_timer;
								cout<<"\nTime taken: "<<diff/(double)CLOCKS_PER_SEC<<" s\n";
				                                cout<<"\nHops: "<<interfaces[currinterface].Receive_Register.nhops<<"\n";
                            
cout<<"\nMesh "<<interfaces[currinterface].Receive_Register.source_address_row<<" "<<interfaces[currinterface].Receive_Register.source_address_col<<" "<<interfaces[currinterface].Receive_Register.dest_address_row<<" "<<interfaces[currinterface].Receive_Register.dest_address_col<<" "<<diff/(double)CLOCKS_PER_SEC<<" "<<interfaces[currinterface].Receive_Register.nhops<<"\n";
								interfaces[currinterface].Interface_Active=false;
								interfaces[currinterface].Packet_In_Receive_Register=false;
								loop=false;
							}
							else
							{
								interfaces[currinterface].Interface_Active=false;
								int reqdInterface=Routing_Algorithm(interfaces[currinterface].Receive_Register);
								cout<<"\nForwarding from "<<x<<","<<y<<" through interface "<<reqdInterface<<"\n";
								if(!interfaces[reqdInterface].Packet_In_Send_Register)
								{
									interfaces[reqdInterface].Send_Register=interfaces[currinterface].Receive_Register;
									strcpy(interfaces[reqdInterface].Send_Register.data,interfaces[currinterface].Receive_Register.data);
									interfaces[currinterface].Packet_In_Receive_Register=false;
									interfaces[reqdInterface].Packet_In_Send_Register=true;
									interfaces[reqdInterface].Interface_Active=true;
									interfaces[reqdInterface].Channel_Operation=SEND;
								}
							}
							interfaces[currinterface].Channel_Operation=NONE;
						}
						break;
			}
		}
		else if(!interfaces[currinterface].Interface_Active&&interfaces[currinterface].Connected_Channel->REQ)
		{
			interfaces[currinterface].Interface_Active=true;
			interfaces[currinterface].Channel_Operation=RECEIVE;
		}}
		currinterface=(currinterface+1)%4;
		//while(!loop);
	}
}
}


int MeshNode::Routing_Algorithm(struct packet p)
{
        int dy=p.dest_address_col;
        int dx=p.dest_address_row;
        if(dy<y)
        {
		return LEFT;
        }
        else if(dy>y)
        {
		return RIGHT;
        }
        else if(dx<x)
        {
		return UP;
        }
        else
        {
		return DOWN;
        }

}

void MeshNode::Acquire_Data_Packet(int a,int b)
{
	struct packet p;
        p.source_address_row=x;
        p.source_address_col=y;
	cout<<"\nReading data at Node "<<x<<","<<y<<"\n";
        cout<<"\nDestination "<<a<<","<<b<<"\n";
        p.dest_address_row=a;
        p.dest_address_col=b;
        cout<<"\nEnter the data to be transmitted\n";
        cin>>p.data;
	clock_t start;
	start=clock();
        p.start_timer=start;
        p.end_timer=0;
	p.nhops=0;
	int reqdInterface=Routing_Algorithm(p);
	interfaces[reqdInterface].Interface_Active=true;
	interfaces[reqdInterface].Channel_Operation=SEND;
	interfaces[reqdInterface].Send_Register=p;
	interfaces[reqdInterface].Packet_In_Send_Register=true;
	loop=true;
}


struct args
{
	int a,b;
};
Channel *list;
int channel_count;
MeshNode **MNode;


extern "C" void *call_func(void *f)
{
   
	struct args *curr=(struct args *)f;
	//cout<<"\nStarting thread for "<<curr->a<<","<<curr->b<<"\n";
	MNode[curr->a-1][curr->b-1].Control_Logic();
	return 0;
   
}

