#include "evgame.h"
//#include "time.h"
#include "ga.h"
#include "network.h"
//extern vector<vector<int> >  vLadjmx;
Evgame::Evgame(int nett,int gamet,int imitt,int synct,int step,int LearnAbilityRateType,int moreThanType,int neighbors)
{
	nettype=nett;
	gametype=gamet;
	imittype=imitt;
	synctype=synct;
	mem=0;
	LearnRateType=LearnAbilityRateType;
	moreThanFuncType=moreThanType;
	stepCount=step;
    
	char *nets,*games,*imits,*syncs,*rate,*moreType,*stepName,*neib; //��Ŵ����ļ���
	switch (nett)
	{
	case 10: nets="mix";break;
	case 11: nets="lat";break;
	case 12: nets="BAnet";break;
	case 13: nets="beeLat";break;
	default: exit(21);
	}

	switch (gamet)
	{
	case 20:games="2x2";break;
	case 21:games="pd";break;
	case 22:games="sd";break;
	case 23:games="sh";break;
	case 24:games="eg2x2";break;
	case 25:games="gen";break;

	default: exit(21);
	}
	
	switch (imitt)
	{
	case 30: imits="Imx";break;
	case 31: imits="Ird";break;
	case 32: imits="Idy";break;
	case 33: imits="Iga";break;
    case 34: imits="Isl";break;
    case 35: imits="Imsl";break; 
    case 36: imits="Isld";break;
    case 37: imits="Ipuc";break;
    case 38: imits="Ipdy";break;
	default: exit(21);
	}
	
	switch (synct)
	{
	case 40: syncs="Syn";break;
	case 41: syncs="Asyn";break;
	default: exit(21);
	}
	
	switch (LearnAbilityRateType)
	{
	case 0: rate="1.0p";break;
	case 1: rate="0.3p";break;
	case 2: rate="0.6p";break;
	case 3: rate="0.9p";break;
	default: exit(21);
	}
	
	switch (moreThanFuncType)
	{
	case -1: moreType="noFunc";break;
	case 0: moreType="worstOnly";break;
	case 1: moreType="moreThan1";break;
	case 2: moreType="moreThan2";break;
	case 3: moreType="moreThan3";break;
	case 4: moreType="bestOnly";break;
	default: exit(21);
	}
	
	switch (step)
	{
	case 0: stepName="NoSkip";break;
	case 1: stepName="Step1";break;
	case 2: stepName="Step2";break;
	case 3: stepName="Step3";break;
	case 4: stepName="Step4";break;
	case 5: stepName="Step5";break;
	case 6: stepName="Step6";break;
	case 7: stepName="Step7";break;
	case 8: stepName="Step8";break;
	case 9: stepName="Step9";break;
	case 10: stepName="Step10";break;
	default: exit(21);
	}
	
	switch (neighbors)
	{
	case 1: neib="Neib1";break;
	case 2: neib="Neib2";break;
	case 3: neib="Neib3";break;
	case 4: neib="Neib4";break;
	case 5: neib="Neib5";break;
	case 6: neib="Neib6";break;
	case 7: neib="Neib7";break;
	case 8: neib="Neib8";break;
	default: exit(21);
	}
	//time_t t;
	//time(&t);
	srand((unsigned)time(0));
	sprintf_s(fhead,60,"%s%s%s%s%s%s",nets,games,imits,rate,stepName,neib);//д�����ļ�
//	if(_access(fhead,0)!=-1)
//	{printf("directory exist %s", fhead);exit(23);}
	_mkdir(fhead);
    memset(mst,0,sizeof(mst));
	memset(tpf,0,sizeof(tpf));
	printf("construct egame ok\n");
    
	memset(smst,0,sizeof(smst));
	memset(stpf,0,sizeof(stpf));
	
	mcr.resize(ILOOP,vector<vector<vector<double> > >(egX,vector<vector<double> >(egY,vector<double>(JLOOP,0.0))));
	mfg.resize(ILOOP,vector<vector<vector<double> > >(egX,vector<vector<double> >(egY,vector<double>(4096,0.0))));
	smcr.resize(egX,vector<vector<double> >(egY,vector<double>(JLOOP,0.0)));
	smfg.resize(egX,vector<vector<double> >(egY,vector<double>(4096,0.0)));
	stdcr.resize(egX,vector<vector<double> >(egY,vector<double>(JLOOP,0.0)));

	switch (nett)
	{
	case 10: 
	case 11: evgm(neighbors);dataproc();break;
	case 12: evgm(neighbors);dataproc();break;
	case 13: evgm(neighbors);dataproc(); break;
	default: exit(21);
	}
	
}

int Evgame::evgm(int neighbor)//���ƶ�ģʽ�µ��ݻ����Ĺ���
{

	#pragma omp parallel for //private(i,k,m,n,x,y,ni,id,dr,dg,CRatio,node1,node2)

	for(int i=0;i<ILOOP;i++)
	{
		int id=omp_get_thread_num();
		int k,m,n,x,y,ni;
    	double dr,dg,cr;
   		double CRatio;
   		int     node1;//=TotalVectorArray[i];
    	int     node2;//=TotalVectorArray[i+1];
		for (x=0;x<egX;x++)
		{
	   		for(y=0;y<egY;y++)
   	  		{	
				if(gametype==25) //�Ŵ��㷨���� 
				{	
					dg=3;
					dr=1;
				}
				else
				{
					dg=(x+0.0)/10.0;
					dr=(y+0.0)/10.0;
	//				if (x==0) {dg=0.6;dr=1.2;}
	//				if (x==1) {dg=1.0;dr=1.2;}
	//				if (x==2) {dg=1.0;dr=0.8;}
				}

        		NodeAdj banet(nettype,neighbor);//���繹��
				GAgame gagm(gametype,imittype,synctype,stepCount,banet.vLadjmx);
				//��ֵ���������ĸ��ʴ�С
				switch (LearnRateType)
				{
				case 0: gagm.LearnAbilityRate=1.0;break;
				case 1: gagm.LearnAbilityRate=0.3;break;
				case 2: gagm.LearnAbilityRate=0.6;break;
				case 3: gagm.LearnAbilityRate=0.9;break;
				default: exit(21);
				}
				
				gagm.moreThan=moreThanFuncType;
				
				netsize=banet.vLadjmx.size(); //�����С ,size = 1000
				//printf("run egame");
				//tsize=vLadjmx.size;ttmplen=gagm.tmplen;
				if(synctype==ASYNC)//asyn   ASYNC=41   �첽�ݻ�����
				{
					gagm.Hstinit();//��ʼ����ʷ ��dr=0.5 
					gagm.Fitinit();//��Ӧ�ȳ�ʼ�� 
					gagm.Game(dg,dr);//���岩��  ��ֹģʽ 
					
					for(ni=0;ni<(JLOOP*netsize);ni++)
					{
						gagm.Gameas(dg);//�첽���� 
						gagm.ruleupdate(dg,dr);
			
						if((ni%netsize)==0)//ÿִ����һ�����ĺ󣬾ͼ���һ�κ����ʲ����棻ÿ50����ӡһ������ 
						{
							if (ni%(50*netsize)==0)
							{
								printf("v %lf  b %lf  i %d    k %d    %lf id %d \n",dr,dg,i,ni/netsize,CRatio,id);		//						banet.AdjPrint();
							}
				//			gagm.Infoprint();
							CRatio=gagm.getCratioas();
							mcr[i][x][y][ni/netsize]=CRatio;//mpicrh[k]=CRatio; //vmcr.push_back(CRatio);//	
						}
					}
				}
				else   //SYN  ͬ���ݻ�����
				{				
					//printf("\nl134");
					gagm.Hstinit(0.5);
					printf("\nl135");
					for(k=0;k<JLOOP;k++) //game JLOOP times represent the history of cooperation.
					{
			//			gagm.Hstinit(cr);
						gagm.Fitinit();
						CRatio=gagm.Game(dg,dr); // game gloop times
						gagm.ruleupdate(dg,dr);
							
						if (k%100==0)
						{
							printf("v=%lf  b=%lf  i=%d    k=%d   CRatio=%lf id=%d \n",dr,dg,i,k,CRatio,id);
//							banet.AdjPrint();
						}
						mcr[i][x][y][k]=CRatio;//mpicrh[k]=CRatio; //vmcr.push_back(CRatio);//				
					}//END OF FOR(k=0)
		
				}
				
				/*
				if(x==0){
					printf("ȫ���������Ѳ������\n");
					int count = 0;
					for(int i=0;i<20;i++){
						for(int j=0;j<20;j++) {
							if(gagm.Strategy[i*20+j][0]==1) count++;
							printf("%d ",gagm.Strategy[i*20+j][0]);
						}
						printf("\n");
					}
					double tmpRatio=gagm.getCratioas();
					printf("CRatio is:%f \n",tmpRatio);
					printf("ratio of 1 is:%f \n",(double)count/400.0);
				}
				*/
	//			nets=netsize;////////////////////////////////////////////////////////////////
				int ttp,ti,tj;
				printf("\n");

				tpf[i][x][y]=gagm.getpayoff();// only use in mixnet!!!!!!!!!!!!!!!2012618
				if(mem!=0) //		if (mem==1)   //mem meaning have changed ,in moving plane game 20121108 
					for (m=0;m<(int)banet.vLadjmx.size();m++)
					{	//ttp��ʾ4bit�Ĳ�����ʮ���Ʊ�ʾ ����16�֣���2X2�� 
						ttp=gagm.Strategy[m][0]+gagm.Strategy[m][1]*2+gagm.Strategy[m][2]*4+gagm.Strategy[m][3]*8;
						mst[i][x][y][ttp]+=1;//mpist[ttp]+=1;   //��ʾ���ֲ���������1��ͳ�Ʋ��Դ����� 
						//printf("%d%d%d%d\t",gagm.Strategy[m][0],gagm.Strategy[m][1],gagm.Strategy[m][2],gagm.Strategy[m][3] );
					}
//				getchar();
				
				int amountcount=0;
				for(m=0;m<banet.vLadjmx.size();m++)//size=1000
					for(n=0;n<banet.vLadjmx[m].size();n++)//size=999
					{
						node1=m;
						node2=banet.vLadjmx[m][n];//m���ڽӵ� 
						amountcount++;
						ti=gagm.GameHistory[node1][node2]%4096;//2^16,2X2�Ĳ��ģ���6�β�����ʷ�����䳤��Ϊ6 
						tj=gagm.GameHistory[node2][node1]%4096;
						mfg[i][x][y][ti]=mfg[i][x][y][ti]++;
						mfg[i][x][y][tj]=mfg[i][x][y][tj]++;
			//			if (mfg[x][y][i][m]>4096||mtfg[x][y][i][m]<0) printf("mtfg error\n");
				//		fprintf(fgenefr,"%d %d ",gagm.GameHistory[node1][node2]%MEMLEN,gagm.GameHistory[node2][node1]%MEMLEN);
					}
				
				for(m=0;m<4096;m++)
					mfg[i][x][y][m]=mfg[i][x][y][m]*1.0/(amountcount*2);
				
			}//x end
		}//y end
	}//prarallare end;
	return 0;
}



int Evgame::evgmmb()//�ƶ�ģʽ�µ��ݻ����Ĺ���
{
	#pragma omp parallel for// private(i,k,m,n,x,y,ni,id,dr,dg,CRatio,node1,node2)
	for(int i=0;i<ILOOP;i++)
	{
		int id=omp_get_thread_num();
		int j,k,m,n,x,y,ni;
		double dr,dg,dv;
		double CRatio;
		int     node1;
		int     node2;
		for (x=0;x<egX;x++)
		{
		   for(y=0;y<egY;y++)
   			{	
				dg=(x+1.0)/10.0;//dr=(y+1.0)/10.0; //dr is the velosity
				dv=(y+0.0)/10.0;
				dr=0;
		//		dv=0.5;
		//		dg=0.1;
				NodeAdj banet(nettype,dv);
				GAgame gagm(gametype,imittype,synctype,stepCount,banet.vLadjmx);
				netsize=banet.vLadjmx.size();
					printf("run egame");
						//tsize=vLadjmx.size;ttmplen=gagm.tmplen;
				if(synctype==ASYNC)//asyn
				{
					gagm.Hstinit();
					gagm.Fitinit();
					gagm.Game(dg,dr);
					for(ni=0;ni<(JLOOP*netsize);ni++)
					{
						gagm.Gameas(dg);
						gagm.ruleupdate(dg,dr);
					
						if((ni%netsize)==0)
						{
							if (ni%(50*netsize)==0)
							{
								printf("v %lf s %lf  b %lf  i %d    k %d    %lf id %d \n",dv,dr,dg,i,ni/netsize,CRatio,id);		//						banet.AdjPrint();
							}
						//	gagm.Infoprint();
							CRatio=gagm.getCratioas();
							mcr[i][x][y][ni/netsize]=CRatio;//mpicrh[k]=CRatio; //vmcr.push_back(CRatio);//	
						}
					}
				}
				else   //SYN
				{				
					for(k=0;k<JLOOP;k++) //game JLOOP times represent the history of cooperation.
					{
						gagm.Hstinit();
						gagm.Fitinit();

						for(j=0;j<gagm.GLOOP;j++)
						{
							banet.Mplmove();
							CRatio=gagm.Gamemob(dg,dr); 				
						
						}
						gagm.Pfpmob();

						gagm.ruleupdate(dg,dr);
						if (k%50==0)
						{
							printf("v %lf s %lf b %lf  i %d    k %d    %lf id %d \n",dv,dr,dg,i,k,CRatio,id);
		//						banet.AdjPrint();
						}
						mcr[i][x][y][k]=CRatio;//mpicrh[k]=CRatio; //vmcr.push_back(CRatio);//				
					}//END OF FOR(k=0)
				}
			//	nets=netsize;////////////////////////////////////////////////////////////////
				int ttp,ti,tj;
				printf("\n");
				tpf[i][x][y]=gagm.getpayoff();// only use in mixnet!!!!!!!!!!!!!!!2012618
				if(mem!=0) //		if (mem==1)   //mem meaning have changed ,in moving plane game 20121108 
					for (m=0;m<(int)banet.vLadjmx.size();m++)
					{	
						ttp=gagm.Strategy[m][0]+gagm.Strategy[m][1]*2+gagm.Strategy[m][2]*4+gagm.Strategy[m][3]*8;
						mst[i][x][y][ttp]+=1;//mpist[ttp]+=1;   //
						printf("%d%d%d%d\t",gagm.Strategy[m][0],gagm.Strategy[m][1],gagm.Strategy[m][2],gagm.Strategy[m][3] );
					}
		//		getchar();
				int amountcount=0;
				for(m=0;m<banet.vLadjmx.size();m++)
					for(n=0;n<banet.vLadjmx[m].size();n++)
					{
						node1=m;
						node2=banet.vLadjmx[m][n];
						amountcount++;
						ti=gagm.GameHistory[node1][node2]%64;//need rewrite 4096     201309
						tj=gagm.GameHistory[node2][node1]%64;
						mfg[i][x][y][ti]=mfg[i][x][y][ti]++;
						mfg[i][x][y][tj]=mfg[i][x][y][tj]++;
				//		if (mfg[x][y][i][m]>63||mtfg[x][y][i][m]<0) printf("mtfg error\n");
					//	fprintf(fgenefr,"%d %d ",gagm.GameHistory[node1][node2]%MEMLEN,gagm.GameHistory[node2][node1]%MEMLEN);
					}
						for(m=0;m<64;m++) mfg[i][x][y][m]=mfg[i][x][y][m]*1.0/(amountcount*2);
						
				}//y end
		   }//x end
	}//prarallare end;
	return 0;
}


int Evgame::dataproc()//���ݴ��̴���
{
	int ii,jj,kk,xx,yy;
	printf("\nmst\n");
	for( ii=0;ii<ILOOP;ii++)
	{
	//	acrh[ii-1]=mcr;
		for(int xx=0;xx<egX;xx++)
			for(int yy=0;yy<egY;yy++)
			{
				for(jj=0;jj<JLOOP;jj++)
				{
					smcr[xx][yy][jj]+=mcr[ii][xx][yy][jj];
				//	acrh[ii-1][xx][yy][jj]=mcr[xx][yy][jj];
				}
				
				for(jj=0;jj<16;jj++) 
				{
					smst[xx][yy][jj]+=mst[ii][xx][yy][jj];
//					printf("%lf\t",mst[ii][xx][yy][jj]);////////////////////////////////
				}
				stpf[xx][yy]+=tpf[ii][xx][yy];
				for(jj=0;jj<4096;jj++) 
					smfg[xx][yy][jj]+=mfg[ii][xx][yy][jj];		
			}
	}

	printf("\nsmstILOOP %d,netsize %d \n",ILOOP,netsize);
	for(int xx=0;xx<egX;xx++)
		for(int yy=0;yy<egY;yy++)
		{
			for(jj=0;jj<JLOOP;jj++)
			{
				smcr[xx][yy][jj]=smcr[xx][yy][jj]*1.0/ILOOP;
			}
			for(jj=0;jj<16;jj++) 
			{
//				printf("%lf\t",smst[xx][yy][jj]);/////////////////////////////////////////
				smst[xx][yy][jj]=smst[xx][yy][jj]*1.0/(ILOOP*netsize);//////////////////////////////////////////////
//				printf("n%lf\t",smst[xx][yy][jj]);/////////////////////////////////////////
			}
			for(jj=0;jj<4096;jj++) 
			{
//				printf("fg%lf\t",smfg[xx][yy][jj]);/////////////////////////////////////////
				smfg[xx][yy][jj]=smfg[xx][yy][jj]*1.0/(ILOOP);
//				printf("nfg%lf\t",smfg[xx][yy][jj]);/////////////////////////////////////////
			}
			stpf[xx][yy]=stpf[xx][yy]*1.0/ILOOP;
		}
		writefile();

	return 0;
}

int Evgame::writefile()//����
{
   	crhp();
//  if (mem==1)fchmp();
   	fchmp();
	histp();
   	pfp();
	
	return 0;
}

int Evgame::crhp()//����ˮƽ�仯��ʷ��Ϣ����
{
	char filename[80];
	
	int x,y,i,k;

    sprintf_s(filename,80,".//%s//%s",fhead,"crh.txt");
	ofstream fccrh(filename);
	if (!fccrh)
		exit(331);

	for(x=0;x<egX;x++)
		for(y=0;y<egY;y++)
			for(k=0;k<JLOOP;k++)
				fccrh<<smcr[x][y][k]<<" ";
    fccrh.close();
/* 
	sprintf_s(filename,80,".//%s//%s",fhead,"crh00.txt");
	ofstream fccrh00(filename);
	if (!fccrh00)
		exit(331);

   for(i=0;i<ILOOP;i++)
	for(x=0;x<egX;x++)
		for(y=0;y<egY;y++)
			for(k=0;k<JLOOP;k++)
				fccrh00<<mcr[i][x][y][k]<<" ";
    fccrh00.close();
*/
    sprintf_s(filename,80,".//%s//%s",fhead,"stdcrh.txt");
	ofstream fdcrh(filename);
	if (!fdcrh)
		exit(331);

	for(x=0;x<egX;x++)
		for(y=0;y<egY;y++)
			for(k=0;k<JLOOP;k++)
				fdcrh<<stdcr[x][y][k]<<" ";
    fdcrh.close();
	return 0;
}
 
int Evgame::fchmp()//�м���״̬�µ����ղ��Ĳ���״̬����
{
    char filename[80];
	int x,y,i;
	sprintf_s(filename,80,".//%s//%s",fhead,"genest.txt"); 
	ofstream fcgst(filename);
	if (!fcgst)
		exit(331);
	for(x=0;x<egX;x++)
		for(y=0;y<egY;y++)
			for(i=0;i<16;i++)
				fcgst<<smst[x][y][i]<<" ";				
	fcgst.close();
	return 0;
}



int Evgame::histp()//����r��st��p ����״̬ͳ�ƣ����ղ��ĵ���ʷ״̬����
{
    char filename[80];
	int x,y,i;

	sprintf_s(filename,80,".//%s//%s",fhead,"genefr.txt");			
	ofstream fcgfr(filename);
	if (!fcgfr)
		exit(331);
	 

	for(x=0;x<egX;x++)
		for(y=0;y<egY;y++)
			for(i=0;i<4096;i++)
			{	
				if(smfg[x][y][i]<0)
				exit(24);//cout<<"hty error";
			//	mfg[x][y][i]=(float)mfg[x][y][i]/(ttmplen*ILOOP);
				fcgfr<<smfg[x][y][i]<<" ";
			}	
    fcgfr.close();

	double hrstp[egX][egY][4];
	memset(hrstp,0,sizeof(hrstp));
    sprintf_s(filename,80,".//%s//%s",fhead,"rstp.txt");
	ofstream fcrstp(filename);
	if (!fcrstp)
		exit(331);
 
	for(x=0;x<egX;x++)
		for(y=0;y<egY;y++)
			for(i=0;i<4096;i++)
				hrstp[x][y][i%4]+=smfg[x][y][i];///////

	for(x=0;x<egX;x++)
		for(y=0;y<egY;y++)
			for(i=0;i<4;i++)
				fcrstp<<hrstp[x][y][i]<<" ";
    fcrstp.close();
	return 0;
}

int Evgame::pfp() //��������ˮƽ����
{   char filename[80];
	int x,y,i;
	double pf[egX][egY];
	
	memset(pf,0,sizeof(pf));
    sprintf_s(filename,80,".//%s//%s",fhead,"pf.txt");
	ofstream fcpf(filename);
	if (!fcpf)
		exit(331);
	for(x=0;x<egX;x++)
		for(y=0;y<egY;y++)
			fcpf<<stpf[x][y]<<" ";
		
	fcpf.close();
    return 0;
}