#include<iostream>
#include<fstream>
#include<math.h>
#include<ctime>
#include<sys/time.h>
#include<pthread.h>
#include<stdlib.h>
#include<malloc.h>
#include<string.h>
#include<semaphore.h>
#include<unistd.h>
#include "mesh.h"
using namespace std;






int main()
{
    bool Left_Reqd,Left_Avail;
    bool Right_Reqd,Right_Avail;
    bool Up_Reqd,Up_Avail;
    bool Down_Reqd,Down_Avail;
    int x,y;
	file.open("result.txt",ios::app);
    file<<"\nMESH TOPOLOGY SIMULATION\n\n\n";
    cout<<"\nMESH TOPOLOGY SIMULATION\n\n\n";

    cout<<"\nEnter Row Size : ";
    cin>>row;
    cout<<"\nEnter Column Size : ";
    cin>>col;
    file<<"Row Size : "<<row;
	file<<"	Column Size : "<<col;
    file.close();
	
    pthread_t threads[row][col];
    threadstatus=READY;
 
    struct args *argtab;
    argtab=(struct args *)malloc(sizeof(struct args));
	simactive=true;
       
                MNode=new MeshNode*[row];
                for(int i=0;i<row;i++)
                {
                    MNode[i]=new MeshNode[col];
                    for(int j=0;j<col;j++)
                    {
                        MNode[i][j].x=i;
                        MNode[i][j].y=j;
                        for(int k=0;k<4;k++)
                        {
                            MNode[i][j].interfaces[k].Connected_Channel=NULL;
                            MNode[i][j].interfaces[k].Interface_Active=false;
                            MNode[i][j].interfaces[k].Channel_Operation=NONE;
                        }
                    }
                }
                list=new Channel[row*col];
                channel_count=0;
                cout<<"Creating connections\n";
                for(int i=0;i<row;i++)
                {
                    for(int j=0;j<col;j++)
                    {
                        Left_Reqd=true;
                        Right_Reqd=true;
                        Up_Reqd=true;
                        Down_Reqd=true;
                        if(j==0)
                        {
                            Left_Reqd=false;
                            Left_Avail=false;
                        }
                        if(i==0)
                        {
                            Up_Reqd=false;
                            Up_Avail=false;
                        }
                        if(j==col-1)
                        {
                            Right_Reqd=false;
                            Right_Avail=false;
                        }
                        if(i==row-1)
                        {
                            Down_Reqd=false;
                            Down_Avail=false;
                        }
                        if(Left_Reqd)
                        {
                            MNode[i][j].interfaces[LEFT].Connected_Channel=MNode[i][j-1].interfaces[RIGHT].Connected_Channel;
                        }
                        if(Right_Reqd)
                        {
                            list[channel_count].REQ=false;
                            list[channel_count].status=EMPTY;
                            list[channel_count].ACK=false;
                            list[channel_count].DATA=false;
                            list[channel_count].CLK=false;
                            list[channel_count].CHOKE=false;
                            list[channel_count].Busy_Bit=false;
                            list[channel_count].Receive_Bit=false;
                            list[channel_count].Transfer_Bit=false;
                            MNode[i][j].interfaces[RIGHT].Connected_Channel=&list[channel_count];
                            channel_count++;
                        }
                        if(Up_Reqd)
                        {
                            MNode[i][j].interfaces[UP].Connected_Channel=MNode[i-1][j].interfaces[DOWN].Connected_Channel;
                        }
                        if(Down_Reqd)
                        {
                            list[channel_count].REQ=false;
                            list[channel_count].status=EMPTY;
                            list[channel_count].ACK=false;
                            list[channel_count].DATA=false;
                            list[channel_count].CLK=false;
                            list[channel_count].CHOKE=false;
                            list[channel_count].Busy_Bit=false;
                            list[channel_count].Receive_Bit=false;
                            list[channel_count].Transfer_Bit=false;
                            MNode[i][j].interfaces[DOWN].Connected_Channel=&list[channel_count];
                            channel_count++;
                        }
                    }
                }
                for(int i=0;i<row;i++)
                    for(int j=0;j<col;j++)
                    {
                        while(threadstatus!=READY);
                        threadstatus=INPROGRESS;
                        argtab->a=i+1;
                        argtab->b=j+1;
                       
                        pthread_create(&threads[i][j],0,call_func,(void *)argtab);
                        for(int k=0;k<4;k++)
                        {
                            MNode[i][j].interfaces[k].Interface_Active=false;
                            MNode[i][j].interfaces[k].Channel_Operation=NONE;
                            if(MNode[i][j].interfaces[k].Connected_Channel!=NULL)
                            {
                                MNode[i][j].interfaces[k].Connected_Channel->REQ=false;
                                MNode[i][j].interfaces[k].Connected_Channel->status=EMPTY;
                            }
                        }
                    }

              int sr,sc,dr,dc;
              int choice;
              char  data[30];

              cout<<"\n1. One Node to All ";
			  cout<<"\n2. All Source to one Destination Node ";
 			  cout<<"\n3. One Source and One Destination ";
			  cout<<"\n4. All source nodes to all Destination nodes "; 
			  cout<<"\n   Enter Your Choice(1-4)";
			  cin>>choice;

              switch(choice)
             {
				case 1:
              file.open("result.txt",ios::app);
              
			  cout<<"\nEnter Source Node Details : ";
              cout<<"\nx : ";
              cin>>sr;
              cout<<"\ny : ";
              cin>>sc;
			 cout<<"\nEnter the data to be transmitted\n";
             cin>>data;
            file<<"\n\n\nOne Node to All \n\n";
            file<<"Source Node "<<sr<<":"<<sc<<"\n";
            file<<"Data : "<<data;
            file.close();

			 for(int i=0;i<row;i++)
				{
			for(int j=0;j<col;j++)
				{
            if(i!=sr|| j!=sc)
              {
	     MNode[sr][sc].Acquire_Data_Packet(i,j,data);
         while(loop);
                }

				}	
				}

				break;

 				case 2:
			  cout<<"\nEnter Destination Details : ";
              cout<<"\nx : ";
              cin>>dr;
              cout<<"\ny : ";
              cin>>dc;
			  cout<<"\nEnter the data to be transmitted\n";
              cin>>data;
			file.open("result.txt",ios::app);
			file<<"\n\n\nAll Source to one Destination Node \n\n";
            file<<"Destination Node "<<dr<<":"<<dc<<"\n";
            file<<"Data : "<<data;
            file.close();

               for(int i=0;i<row;i++)
				{
			for(int j=0;j<col;j++)
				{
            if(i!=dr|| j!=dc)
              {
	     MNode[i][j].Acquire_Data_Packet(dr,dc,data);
         while(loop);
                }

				}	
				}
              break;


			case 3:
             cout<<"\nEnter Source Node Details : ";
              cout<<"\nx : ";
              cin>>sr;
              cout<<"\ny : ";
              cin>>sc;
              cout<<"\nEnter Destination Details : ";
              cout<<"\nx : ";
              cin>>dr;
              cout<<"\ny : ";
              cin>>dc;

			file.open("result.txt",ios::app);
			file<<"\n\n\nOne Source to one Destination  \n\n";
            file<<"Destination Node "<<dr<<":"<<dc<<"\n";
            file<<"Data : "<<data;
            file.close();

         MNode[sr][sc].Acquire_Data_Packet(dr,dc,data);
         while(loop);

		break;

       case 4 :

           for(sr=0;sr<row;sr++)
           for(sc=0;sc<col;sc++)
		   for(dr=0;dr<row;dr++)
		   for(dc=0;dc<col;dc++)
            if(sr!=dr || sc!=dc ) {
             MNode[sr][sc].Acquire_Data_Packet(dr,dc,data);
            while(loop);

            }
          


       break;









             }

            /*cout<<"\nEnter Source Node Details : ";
              cout<<"\nx : ";
              cin>>sr;
              cout<<"\ny : ";
              cin>>sc;
              cout<<"\nEnter Destination Details : ";
              cout<<"\nx : ";
              cin>>dr;
              cout<<"\ny : ";
              cin>>dc;

         MNode[sr][sc].Acquire_Data_Packet(dr,dc);
         while(loop); */
		delete MNode;
		delete list;
      simactive=false;




    return 0;   
}

















