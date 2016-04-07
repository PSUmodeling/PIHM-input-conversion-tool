/*******************************************************************************
 * File        : ct-pihm.c                                                     *
 * Version     : Apr, 2015                                                     *
 * Developer of ct-pihm:        Dacheng Xiao (dzx102@psu.edu)                  * 
 * Developer of PIHM-MF:        Yuning Shi (yshi@psu.edu)                      *
 * Developer of PIHM2.0:        Mukesh Kumar (muk139@psu.edu)                  *
 * Developer of PIHM1.0:        Yizhong Qu   (quyizhong@gmail.com)             *
 *-----------------------------------------------------------------------------*
 * ct-pihm is a tool to convert input files from PIHM2.0 to PIHM-MF.    * 
 *******************************************************************************/



#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "ct-pihm.h"            /* Data Model and Variable Declarations     */
#define UNIT_C 1440	     /* Unit Conversions */	

void read_alloc(char *, Model_Data, Control_Data *);	/* Variable definition */

/* Main Function */
int main(int argc, char *argv[])
	{  
	char tmpLName[11],tmpFName[11];	/* rivFlux File names */
  	Model_Data mData;               /* Model Data                */
  	Control_Data cData;             /* Solver Control Data       */
  	void *cvode_mem;                /* Model Data Pointer        */
  	int flag;                       /* flag to test return value */
  	FILE *Ofile[22];           	/* Output file     */
	FILE *status;			/* output the new initial file*/
	char *ofn[22];
	FILE *iproj;			/* Project File */
  	int N;                          /* Problem size              */
  	int i,j,k,ii;                      /* loop index                */
  	double t;    			/* simulation time           */
  	double NextPtr, StepSize;     /* stress period & step size */
  	clock_t start, end_r, end_s;    /* system clock at points    */
  	double cputime_r, cputime_s;  /* for duration in double  */
	char *filename;
	time_t rawtime,zerorawtime;
	struct tm * timeinfo;
	struct tm zerotime;
	/* Project Input Name */
	if(argc!=2)
		{
		iproj=fopen("projectName.txt","r");
		if(iproj==NULL)
			{
			printf("\t\nUsage ./pihm project_name");
			printf("\t\n         OR              ");
			printf("\t\nUsage ./pihm, and have a file in the current directory named projectName.txt with the project name in it");
			exit(0);
			}
		else
			{
			filename = (char *)malloc(15*sizeof(char));
			fscanf(iproj,"%s",filename);
			}
		}
	else
		{
  		/* get user specified file name in command line */
    		filename = (char *)malloc(strlen(argv[1])*sizeof(char));
		strcpy(filename,argv[1]);
		}

  	/* allocate memory for model data structure */
  	mData = (Model_Data)malloc(sizeof *mData);
  
  	printf("\n ...  Input file conversion is starting ... \n");
 
 	/* read in 9 input files with "filename" as prefix */
  	read_alloc(filename, mData, &cData);
	
	printf("\nStart writing new input files for PIHM-MF ...\n");
	
	/* time conversion */
	printf("\nPlease enter Start time in original PIHM time series:\n");
	printf("\t- YEAR(YYYY):\t");	
	scanf("%d", &(zerotime.tm_year));
	printf("\t- MONTH(MM):\t");
	scanf("%d", &(zerotime.tm_mon));
	printf("\t- DAY(DD):\t");
	scanf("%d", &(zerotime.tm_mday));
	printf("\t- HOUR(hh):\t");
	scanf("%d", &(zerotime.tm_hour));
	printf("\t- MINUTE(mm):\t");
	scanf("%d", &(zerotime.tm_min));
	printf("\t- SECOND(ss):\t");
	scanf("%d", &(zerotime.tm_sec));
	
	zerotime.tm_year = zerotime.tm_year - 1900;
	zerotime.tm_mon = zerotime.tm_mon - 1;	
	zerotime.tm_isdst=0;
	zerorawtime = mktime(&zerotime);	
	
	/* write in *.riv with new format*/
	printf("\n  1) writing %s.riv ... ", filename);
	ofn[0] = (char *)malloc((strlen(filename)+10)*sizeof(char));
        strcpy(ofn[0], "output/");
	strcat(ofn[0],filename);
        Ofile[0]=fopen(strcat(ofn[0], ".riv"),"w");
	fprintf(Ofile[0],"%s\t%d\n","NUMRIV",mData->NumRiv);
	fprintf(Ofile[0],"%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n","INDEX","FROM","TO","DOWN","LEFT","RIGHT","SHAPE","MATRL","BC","RES");
	for (i=0; i<mData->NumRiv; i++)
		{
		fprintf(Ofile[0],"%d\t", mData->Riv[i].index);
		fprintf(Ofile[0],"%d\t%d\t", mData->Riv[i].FromNode, mData->Riv[i].ToNode);
		fprintf(Ofile[0],"%d\t", mData->Riv[i].down);
		fprintf(Ofile[0],"%d\t%d\t", mData->Riv[i].LeftEle, mData->Riv[i].RightEle);
		fprintf(Ofile[0],"%d\t%d\t", mData->Riv[i].shape, mData->Riv[i].material);
		fprintf(Ofile[0],"%d\t", mData->Riv[i].BC);
		fprintf(Ofile[0],"%d\n", mData->Riv[i].reservoir);
		}
	fprintf(Ofile[0],"%s\t%d\n","SHAPE",mData->NumRivShape);
	fprintf(Ofile[0],"%s\t%s\t%s\t%s\n","INDEX","DPTH","OINT","CWID");

	for (i=0; i<mData->NumRivShape; i++)
               	{
                fprintf(Ofile[0], "%d\t", mData->Riv_Shape[i].index);
               	fprintf(Ofile[0], "%.2f\t", mData->Riv_Shape[i].depth);
               	fprintf(Ofile[0], "%d\t%.1f\n", mData->Riv_Shape[i].interpOrd,mData->Riv_Shape[i].coeff);
               	}
	fprintf(Ofile[0],"MATERIAL\t%d\n",mData->NumRivMaterial);
        fprintf(Ofile[0],"%s\t%s\t%s\t%-9s\t%-9s\t%s\n","INDEX","ROUGH","CWR","KH","KV","BEDTHICK");
        for (i=0; i<mData->NumRivMaterial; i++)
                {
                fprintf(Ofile[0], "%d\t", mData->Riv_Mat[i].index);
                fprintf(Ofile[0], "%.2f\t", mData->Riv_Mat[i].Rough*86400);/* 86400 from day to second */
                fprintf(Ofile[0], "%.2f\t%9.3e\t", mData->Riv_Mat[i].Cwr,mData->Riv_Mat[i].KsatH/86400);
		fprintf(Ofile[0], "%9.3e\t%.2f\n", mData->Riv_Mat[i].KsatV/86400,mData->Riv_Mat[i].bedThick);             
		}
	fprintf(Ofile[0],"BC\t%d\n",mData->NumRivBC);
	for(i=0; i<mData->NumRivBC; i++)
                {
                fprintf(Ofile[0], "%s\t%d\t%d\n", mData->TSD_Riv[i].name, mData->TSD_Riv[i].index, mData->TSD_Riv[i].length);

                for(j=0; j<mData->TSD_Riv[i].length; j++)
                        {
                        fscanf(Ofile[0], "%9.4lf\t%9.4lf\n", mData->TSD_Riv[i].TS[j][0], mData->TSD_Riv[i].TS[j][1]);
                        }
                }
	fprintf(Ofile[0],"%s\t%d\n","RES",mData->NumRes);	
	fclose(Ofile[0]);
	printf("done.\n");
	
	/* rewrite *.mesh */
	printf("\n  2) writing %s.mesh ...",filename);
        ofn[1] = (char *)malloc((strlen(filename)+10)*sizeof(char));
        strcpy(ofn[1],"output/");
	strcat(ofn[1], filename);
        Ofile[1]=fopen(strcat(ofn[1], ".mesh"),"w");
	fprintf(Ofile[1],"NUMELE\t%d\n",mData->NumEle);
	fprintf(Ofile[1],"%s\t%s\t%s\t%s\t%s\t%s\t%s\n","INDEX","NODE1","NODE2","NODE3","NABR1","NABR2","NABR3");
	for (i=0; i<mData->NumEle; i++)
                {
                fprintf(Ofile[1],"%d\t", mData->Ele[i].index);
                fprintf(Ofile[1],"%d\t%d\t%d\t", mData->Ele[i].node[0], mData->Ele[i].node[1], mData->Ele[i].node[2]);
                fprintf(Ofile[1],"%d\t%d\t%d\n", mData->Ele[i].nabr[0], mData->Ele[i].nabr[1], mData->Ele[i].nabr[2]);
                }	
	fprintf(Ofile[1],"NUMNODE\t%d\n",mData->NumNode);
	fprintf(Ofile[1],"%-s\t%-12s\t%-12s\t%-8s\t%-8s\n","INDEX","X","Y","ZMIN","ZMAX");
	for (i=0; i<mData->NumNode; i++)
                {
                fprintf(Ofile[1],"%d\t", mData->Node[i].index);
                fprintf(Ofile[1],"%-12.4lf\t%-12.4lf\t", mData->Node[i].x, mData->Node[i].y);
                fprintf(Ofile[1],"%-8.4lf\t%-8.4lf\n", mData->Node[i].zmin, mData->Node[i].zmax);
                }
	fclose(Ofile[1]);
	printf("done.\n");	

	/* rewrite *.att */
	printf("\n  3) writing %s.att ...",filename);
        ofn[2] = (char *)malloc((strlen(filename)+10)*sizeof(char));
        strcpy(ofn[2], "output/");
	strcat(ofn[2], filename);
        Ofile[2]=fopen(strcat(ofn[2], ".att"),"w");
        fprintf(Ofile[2],"%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n","INDEX","SOIL","GEOL","LC","METEO","LAI","SS","BC0","BC1","BC2");
	for (i=0; i<mData->NumEle; i++)
		{
		fprintf(Ofile[2],"%d\t",mData->Ele[i].index);
		fprintf(Ofile[2],"%d\t%d\t%d\t",mData->Ele[i].soil,mData->Ele[i].geol,mData->Ele[i].LC);
		fprintf(Ofile[2],"%d\t0\t%d\t",mData->Ele[i].prep,mData->Ele[i].source);
		for (j=0;j<3;j++)
			{
			fprintf(Ofile[2],"%d\t",mData->Ele[i].BC[j]);
			}
		fprintf(Ofile[2],"\n");
		}

	fclose(Ofile[2]);
	printf("done.\n");
	
	/* rewrite *.soil  */
	printf("\n  4) writing %s.soil ...",filename);
        ofn[3] = (char *)malloc((strlen(filename)+10)*sizeof(char));
        strcpy(ofn[3], "output/");
        strcat(ofn[3], filename);
        Ofile[3]=fopen(strcat(ofn[3], ".soil"),"w");
	fprintf(Ofile[3],"%s\t%d\n","NUMSOIL",mData->NumSoil);
	fprintf(Ofile[3],"%-7s%-7s%-7s%-7s%-7s%-11s%-11s%-11s%s\t%s\t%s\t%s\t%s\t%s\t%s\n","INDEX","SILT","CLAY","OM","BD","KINF","KSATV","KSATH","MAXSMC","MINSMC","ALPHA","BETA","MACHF","MACVF","QTZ");
	for (i=0; i<mData->NumSoil; i++)
                {
		fprintf(Ofile[3],"%-7d%-7d%-7d%-7d%-7d",mData->Soil[i].index,-999,-999,-999,-999);
		fprintf(Ofile[3],"%-11.3E%-11.3E%-11.3E",mData->Soil[i].KsatV/86400,mData->Geol[i].KsatV/86400,mData->Geol[i].KsatH/86400);
		fprintf(Ofile[3],"%-.3lf\t%-.3lf\t",mData->Soil[i].ThetaS,mData->Soil[i].ThetaR); 
		fprintf(Ofile[3],"%-.3lf\t%-.3lf\t",mData->Soil[i].Alpha,mData->Soil[i].Beta); 
		fprintf(Ofile[3],"%-.2lf\t%-.2lf\t-999\n",mData->Soil[i].hAreaF,mData->Geol[i].vAreaF); // QTZ -999
		}	
	fprintf(Ofile[3],"DINF\t%-7.1lf\n",mData->Soil[1].infD);
	fprintf(Ofile[3],"KMACV_RO\t%-.1lf\n",mData->Geol[1].macKsatH/mData->Geol[1].KsatH);	
	fprintf(Ofile[3],"KMACH_RO\t%-.1lf\n",mData->Soil[1].macKsatV/mData->Soil[1].KsatV);
	fclose(Ofile[3]);
	printf("done.\n");
	
	/* rewrite *.geol  
	printf("\n  5) writing %s.geol ...",filename);
        ofn[4] = (char *)malloc((strlen(filename)+10)*sizeof(char));
        strcpy(ofn[4], "output/");
        strcat(ofn[4], filename);
        Ofile[4]=fopen(strcat(ofn[4], ".geol"),"w");
        fprintf(Ofile[4],"%d\t%-9s\t%-9s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-9s\t%-7s\n",mData->NumGeol,"SATHK","SATDK","MAXSMC","MINSMC","ALPHA","BETA","MACVF","SATMACKH","DMAC");
        for (i=0; i<mData->NumGeol; i++)
                {
                fprintf(Ofile[3],"%d\t",mData->Geol[i].index); 
		fprintf(Ofile[3],"%-9.3e\t%-9.3e\t",mData->Geol[i].KsatH/86400,mData->Geol[i].KsatV/86400);
		fprintf(Ofile[3],"%-7.3lf\t%-7.3lf\t",mData->Geol[i].ThetaS,mData->Geol[i].ThetaR);
		fprintf(Ofile[3],"%-7.3lf\t%-7.3lf\t",mData->Geol[i].Alpha,mData->Geol[i].Beta);
		fprintf(Ofile[3],"%-7.2lf\t%-9.3e\t%-7.0lf\n",mData->Geol[i].vAreaF,mData->Geol[i].macKsatH/86400,mData->Geol[i].macD);                
		}
        fclose(Ofile[4]);
        printf("done.\n");*/

	/* rewrite *.ibc  */
        printf("\n  6) writing %s.ibc ...",filename);
        ofn[5] = (char *)malloc((strlen(filename)+10)*sizeof(char));
        strcpy(ofn[5], "output/");
        strcat(ofn[5], filename);
        Ofile[5]=fopen(strcat(ofn[5], ".ibc"),"w");
	fprintf(Ofile[5],"%d\t%d\n",mData->Num1BC,mData->Num2BC);
	
	fclose(Ofile[5]);
	printf("done.\n");

	/* rewirte *.para  */
	printf("\n  7) writing %s.para ...",filename);
	ofn[6] = (char *)malloc((strlen(filename)+10)*sizeof(char));
	strcpy(ofn[6], "output/");
	strcat(ofn[6], filename);
	Ofile[6]=fopen(strcat(ofn[6], ".para"),"w");
	fprintf(Ofile[6],"%-20s\t%d\t# Initialization type 0: Relaxation, 1: use .att file, 3: use .init file\n","INIT_MODE",cData.init_type);
	fprintf(Ofile[6],"%-20s\t1\t# Write ASCII output? 0: no, 1: yes\n","ASCII_OUTPUT");
	fprintf(Ofile[6],"%-20s\t0\t# Spin-up mode, 0: Standard model run, 1: Outputs will be writtedn to .init files\n","WRITE_IC");
	fprintf(Ofile[6],"%-20s\t%d\n","UNSAT_MODE",mData->UnsatMode);
	fprintf(Ofile[6],"%-20s\t%d\n","SURF_MODE",mData->SurfMode);
	fprintf(Ofile[6],"%-20s\t%d\n","RIV_MODE",mData->RivMode);
	fprintf(Ofile[6],"%-20s\t%d\n","SOLVER",cData.Solver);
	fprintf(Ofile[6],"%-20s\t%.0E\n","ABSTOL",cData.abstol);
	fprintf(Ofile[6],"%-20s\t%.0E\n","RELTOL",cData.reltol);
	fprintf(Ofile[6],"%-20s\t%.0E\n","INIT_SOLVER_STEP",cData.InitStep);
	fprintf(Ofile[6],"%-20s\t%.0lf\t# Maximum solver step (DANGER ZONE)\n","MAX_SOLVER_STEP",cData.MaxStep*60);
	fprintf(Ofile[6],"%-20s\t%.0lf\t# LSM (or ET) step size (unit: s)\n","LSM_STEP",cData.ETStep*60);
	
	rawtime = zerorawtime + cData.StartTime;
	timeinfo = localtime(&rawtime);	
	fprintf(Ofile[6],"%-20s\t%d-%02d-%02d %02d:%02d\n","START",timeinfo->tm_year+1900, timeinfo->tm_mon+1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min);

	rawtime = zerorawtime + cData.EndTime;
        timeinfo = localtime(&rawtime);
        fprintf(Ofile[6],"%-20s\t%d-%02d-%02d %02d:%02d\n","END",timeinfo->tm_year+1900, timeinfo->tm_mon+1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min);

	fprintf(Ofile[6],"%-20s\t%.0lf\n","OUTPUT_TYPE",cData.outtype);
	fprintf(Ofile[6],"%-20s\t%.0lf\t# PIHM step size (unit: s)\n","MODEL_STEPSIZE",cData.b*60);  
	
	fprintf(Ofile[6],"###############################################################################\n");
	fprintf(Ofile[6],"# OUTPUT CONTROL:                                                             #\n");
	fprintf(Ofile[6],"#  All options are listed below.                                              #\n");
	fprintf(Ofile[6],"#  Use '#' to comment out the variables you do not want to output, or set     #\n");
	fprintf(Ofile[6],"#  the output interval (in minutes) to 0.                                     #\n");
	fprintf(Ofile[6],"###############################################################################\n");
	
	fprintf(Ofile[6],"%-20s\t3600\n","GW");
	fprintf(Ofile[6],"%-20s\t3600\n","SURF");
	fprintf(Ofile[6],"%-20s\t3600\n","SNOW");
        fprintf(Ofile[6],"%-20s\t3600\n","RIVSTG"); 
	fprintf(Ofile[6],"%-20s\t3600\n","RECHARGE");
        fprintf(Ofile[6],"%-20s\t3600\n","CMC");
	fprintf(Ofile[6],"%-20s\t3600\n","UNSAT");
        fprintf(Ofile[6],"%-20s\t3600\n","EC");
	fprintf(Ofile[6],"%-20s\t3600\n","ETT");
        fprintf(Ofile[6],"%-20s\t3600\n","EDIR");
	fprintf(Ofile[6],"%-20s\t3600\n","RIVFLX0");
	fprintf(Ofile[6],"%-20s\t3600\n","RIVFLX1");
	fprintf(Ofile[6],"%-20s\t3600\n","RIVFLX2");
	fprintf(Ofile[6],"%-20s\t3600\n","RIVFLX3");
	fprintf(Ofile[6],"%-20s\t3600\n","RIVFLX4");
	fprintf(Ofile[6],"%-20s\t3600\n","RIVFLX5");
	fprintf(Ofile[6],"%-20s\t3600\n","RIVFLX6");
	fprintf(Ofile[6],"%-20s\t3600\n","RIVFLX7");
	fprintf(Ofile[6],"%-20s\t3600\n","RIVFLX8");
	fprintf(Ofile[6],"%-20s\t3600\n","RIVFLX9");
	fclose(Ofile[6]);
	printf("done.\n");
	
	/* rewirte *.calib  */
	printf("\n  8) writing %s.calib ...",filename);
	ofn[7] = (char *)malloc((strlen(filename)+10)*sizeof(char));
	strcpy(ofn[7], "output/");
	strcat(ofn[7], filename);
	Ofile[7]=fopen(strcat(ofn[7], ".calib"),"w");
        fprintf(Ofile[7],"%-20s\t%.1lf\t# Horizontal saturated hydraulic conductivity\n","KSATH",cData.Cal.KsatH);
	fprintf(Ofile[7],"%-20s\t%.1lf\t# Vertital saturated hydraulic conductivity\n","KSATV",cData.Cal.KsatV);
	fprintf(Ofile[7],"%-20s\t%.1lf\t# Infiltration conductivity\n","KINF",cData.Cal.infKsatV);
	fprintf(Ofile[7],"%-20s\t%.1lf\t# Horizontal saturated macropore hydraulic conductivity\n","KMACSATH",cData.Cal.macKsatH);
	fprintf(Ofile[7],"%-20s\t%.1lf\t# Vertital saturated macropore hydraulic conductivity\n","KMACSATV",cData.Cal.macKsatV);
	fprintf(Ofile[7],"%-20s\t%.1lf\t# Infiltration depth\n","DINF",cData.Cal.infD);
	fprintf(Ofile[7],"%-20s\t%.1lf\t# Rooting depth\n","DROOT",cData.Cal.RzD);
	fprintf(Ofile[7],"%-20s\t%.1lf\t# Macropore depth\n","DMAC",cData.Cal.macD);
	fprintf(Ofile[7],"%-20s\t%.1lf\n","POROSITY",cData.Cal.Porosity);
	fprintf(Ofile[7],"%-20s\t%.1lf\n","ALPHA",cData.Cal.Alpha);
	fprintf(Ofile[7],"%-20s\t%.1lf\n","BETA",cData.Cal.Beta);
	fprintf(Ofile[7],"%-20s\t%.1lf\n","MACVF",cData.Cal.vAreaF);
	fprintf(Ofile[7],"%-20s\t%.1lf\n","MACHF",cData.Cal.hAreaF);
	fprintf(Ofile[7],"%-20s\t%.1lf\n","VEGFRAC",cData.Cal.VegFrac);
	fprintf(Ofile[7],"%-20s\t%.1lf\n","ALBEDO",cData.Cal.Albedo);
	fprintf(Ofile[7],"%-20s\t%.1lf\n","ROUGH",cData.Cal.Rough);
	fprintf(Ofile[7],"%-20s\t%.1lf\n","PRCP",cData.Cal.Prep);
	fprintf(Ofile[7],"%-20s\t%.1lf\n","SFCTMP",cData.Cal.Temp);
        fprintf(Ofile[7],"%-20s\t%.1lf\n","EC",mData->pcCal.Et0);
        fprintf(Ofile[7],"%-20s\t%.1lf\n","ETT",mData->pcCal.Et1);
        fprintf(Ofile[7],"%-20s\t%.1lf\n","EDIR",mData->pcCal.Et2);
        fprintf(Ofile[7],"%-20s\t%.1lf\n","ROUGH_RIV",cData.Cal.rivRough);
        fprintf(Ofile[7],"%-20s\t%.1lf\n","KRIVH",cData.Cal.rivKsatH);
	fprintf(Ofile[7],"%-20s\t%.1lf\n","KRIVV",cData.Cal.rivKsatV);
	fprintf(Ofile[7],"%-20s\t%.1lf\n","BEDTHCK",cData.Cal.rivbedThick);
	fprintf(Ofile[7],"%-20s\t%.1lf\n","RIV_DPTH",cData.Cal.rivDepth);
	fprintf(Ofile[7],"%-20s\t%.1lf\n","RIV_WDTH",cData.Cal.rivShapeCoeff);
	
	fprintf(Ofile[7],"###############################################################################\n");
	fprintf(Ofile[7],"# LSM calibration:                                                            #\n");
	fprintf(Ofile[7],"#  Use '#' to comment out the variables if you are not using the LSM module   #\n");
	fprintf(Ofile[7],"###############################################################################\n");
	
	fprintf(Ofile[7],"%-20s\t1.0\n","#DRIP");
	fprintf(Ofile[7],"%-20s\t1.0\n","#CMCMAX");
	fprintf(Ofile[7],"%-20s\t1.0\n","#RS");
        fprintf(Ofile[7],"%-20s\t1.0\n","#CZIL"); 
	fprintf(Ofile[7],"%-20s\t1.0\n","#FXEXP");
        fprintf(Ofile[7],"%-20s\t1.0\n","#CFACTR");
	fprintf(Ofile[7],"%-20s\t1.0\n","#RGL");
        fprintf(Ofile[7],"%-20s\t1.0\n","#HS");
	fprintf(Ofile[7],"%-20s\t1.0\n","#REFSMC");
        fprintf(Ofile[7],"%-20s\t1.0\n","#WLTSMC");
	fclose(Ofile[7]);
	printf("done.\n");
	}

