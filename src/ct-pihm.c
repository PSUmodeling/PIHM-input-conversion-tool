#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "ct-pihm.h"            /* Data Model and Variable Declarations     */

void            read_alloc(char *, Model_Data, Control_Data *); /* Variable definition */

/* Main Function */
int main(int argc, char *argv[])
{
    char            tmpLName[11], tmpFName[11]; /* rivFlux File names */
    Model_Data      mData;      /* Model Data                */
    Control_Data    cData;      /* Solver Control Data       */
    void           *cvode_mem;  /* Model Data Pointer        */
    int             flag;       /* flag to test return value */
    FILE           *Ofile[22];  /* Output file     */
    FILE           *status;     /* output the new initial file */
    char           *ofn[22];
    FILE           *iproj;      /* Project File */
    int             N;          /* Problem size              */
    int             i, j, k, ii;    /* loop index                */
    double          t;          /* simulation time           */
    double          NextPtr, StepSize;  /* stress period & step size */
    clock_t         start, end_r, end_s;    /* system clock at points    */
    double          cputime_r, cputime_s;   /* for duration in double  */
    char           *filename;
    time_t          rawtime, zerorawtime;
    struct tm      *timeinfo;
    struct tm       zerotime;

    mkdir("output", 0755);

    /* Project Input Name */
    if (argc != 2)
    {
        iproj = fopen("projectName.txt", "r");
        if (iproj == NULL)

        {
            printf("\t\nUsage ./pihm project_name");
            printf("\t\n         OR              ");
            printf("\t\nUsage ./pihm, and have a file in the current directory "
                "named projectName.txt with the project name in it");
            exit(0);
        }
        else
        {
            filename = (char *)malloc(15 * sizeof(char));
            fscanf(iproj, "%s", filename);
        }
    }
    else
    {
        /* get user specified file name in command line */
        filename = (char *)malloc(strlen(argv[1]) * sizeof(char));
        strcpy(filename, argv[1]);
    }
    /* allocate memory for model data structure */
    mData = (Model_Data) malloc(sizeof *mData);
    printf("\n ...  Input file conversion is starting ... \n");

    /* read in input files with "filename" as prefix */
    read_alloc(filename, mData, &cData);
    printf("\nStart writing new input files for MM-PIHM ...\n");

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
    zerotime.tm_isdst = 0;
    zerorawtime = mktime(&zerotime);

    /* write in *.riv with new format */
    printf("\n  1) writing %s.riv ... ", filename);
    ofn[0] = (char *)malloc((strlen(filename) + 10) * sizeof(char));
    strcpy(ofn[0], "output/");
    strcat(ofn[0], filename);
    Ofile[0] = fopen(strcat(ofn[0], ".riv"), "w");
    fprintf(Ofile[0], "%-8s%d\n", "NUMRIV", mData->NumRiv);
    fprintf(Ofile[0], "%-8s%-8s%-8s%-8s%-8s%-8s%-8s%-8s%-8s%-8s\n",
        "INDEX", "FROM", "TO", "DOWN", "LEFT", "RIGHT", "SHAPE", "MATRL", "BC",
        "RES");
    for (i = 0; i < mData->NumRiv; i++)
    {
        fprintf(Ofile[0], "%-8d",
            mData->Riv[i].index);
        fprintf(Ofile[0], "%-8d%-8d",
            mData->Riv[i].FromNode, mData->Riv[i].ToNode);
        fprintf(Ofile[0], "%-8d", mData->Riv[i].down);
        fprintf(Ofile[0], "%-8d%-8d",
            mData->Riv[i].LeftEle, mData->Riv[i].RightEle);
        fprintf(Ofile[0], "%-8d%-8d",
            mData->Riv[i].shape, mData->Riv[i].material);
        fprintf(Ofile[0], "%-8d",
            mData->Riv[i].BC);
        fprintf(Ofile[0], "%d\n",
            mData->Riv[i].reservoir);
    }

    fprintf(Ofile[0], "%-8s%d\n", "SHAPE", mData->NumRivShape);
    fprintf(Ofile[0], "%-8s%-8s%-8s%s\n", "INDEX", "DPTH", "OINT", "CWID");
    for (i = 0; i < mData->NumRivShape; i++)
    {
        fprintf(Ofile[0], "%-8d", mData->Riv_Shape[i].index);
        fprintf(Ofile[0], "%-8.2f", mData->Riv_Shape[i].depth);
        fprintf(Ofile[0], "%-8d%-.1f\n",
            mData->Riv_Shape[i].interpOrd, mData->Riv_Shape[i].coeff);
    }
    fprintf(Ofile[0], "%-12s%d\n", "MATERIAL", mData->NumRivMaterial);
    fprintf(Ofile[0], "%-8s%-8s%-8s%-12s%-12s%-12s\n",
        "INDEX", "ROUGH", "CWR", "KH", "KV", "BEDTHICK");
    for (i = 0; i < mData->NumRivMaterial; i++)
    {
        fprintf(Ofile[0], "%-8d", mData->Riv_Mat[i].index);
        /* 86400 from day to second */
        fprintf(Ofile[0], "%-8.2f", mData->Riv_Mat[i].Rough * 86400);
        fprintf(Ofile[0], "%-8.2f%-12.3e",
            mData->Riv_Mat[i].Cwr, mData->Riv_Mat[i].KsatH / 86400);
        fprintf(Ofile[0], "%-12.3e%-.2f\n",
            mData->Riv_Mat[i].KsatV / 86400, mData->Riv_Mat[i].bedThick);
    }
    fprintf(Ofile[0], "%-8s%d\n", "BC", mData->NumRivBC);
    for (i = 0; i < mData->NumRivBC; i++)
    {
        fprintf(Ofile[0], "%s\t%d\t%d\n", mData->TSD_Riv[i].name,
            mData->TSD_Riv[i].index, mData->TSD_Riv[i].length);
        for (j = 0; j < mData->TSD_Riv[i].length; j++)
        {
            fscanf(Ofile[0], "%9.4lf\t%9.4lf\n", mData->TSD_Riv[i].TS[j][0],
                mData->TSD_Riv[i].TS[j][1]);
        }
    }
    fprintf(Ofile[0], "%-8s%d\n", "RES", mData->NumRes);
    fclose(Ofile[0]);
    printf("done.\n");

    /* rewrite *.mesh */
    printf("\n  2) writing %s.mesh ...", filename);
    ofn[1] = (char *)malloc((strlen(filename) + 10) * sizeof(char));
    strcpy(ofn[1], "output/");
    strcat(ofn[1], filename);
    Ofile[1] = fopen(strcat(ofn[1], ".mesh"), "w");
    fprintf(Ofile[1], "NUMELE\t%d\n", mData->NumEle);
    fprintf(Ofile[1], "%-8s%-8s%-8s%-8s%-8s%-8s%-s\n",
        "INDEX", "NODE1", "NODE2", "NODE3", "NABR1", "NABR2", "NABR3");
    for (i = 0; i < mData->NumEle; i++)
    {
        fprintf(Ofile[1], "%-8d", mData->Ele[i].index);
        fprintf(Ofile[1], "%-8d%-8d%-8d", mData->Ele[i].node[0],
            mData->Ele[i].node[1], mData->Ele[i].node[2]);
        fprintf(Ofile[1], "%-8d%-8d%d\n", mData->Ele[i].nabr[0],
            mData->Ele[i].nabr[1], mData->Ele[i].nabr[2]);
    }
    fprintf(Ofile[1], "%-8s%d\n", "NUMNODE", mData->NumNode);
    fprintf(Ofile[1], "%-8s%-16s%-16s%-16s%s\n",
        "INDEX", "X", "Y", "ZMIN", "ZMAX");
    for (i = 0; i < mData->NumNode; i++)
    {
        fprintf(Ofile[1], "%-8d", mData->Node[i].index);
        fprintf(Ofile[1], "%-16.4lf%-16.4lf",
            mData->Node[i].x, mData->Node[i].y);
        fprintf(Ofile[1], "%-16.4lf%-.4lf\n",
            mData->Node[i].zmin, mData->Node[i].zmax);
    }
    fclose(Ofile[1]);
    printf("done.\n");

    /* rewrite *.att */
    printf("\n  3) writing %s.att ...", filename);
    ofn[2] = (char *)malloc((strlen(filename) + 10) * sizeof(char));
    strcpy(ofn[2], "output/");
    strcat(ofn[2], filename);
    Ofile[2] = fopen(strcat(ofn[2], ".att"), "w");
    fprintf(Ofile[2], "%-8s%-8s%-8s%-8s%-8s%-8s%-8s%-8s%-8s%s\n", "INDEX",
        "SOIL", "GEOL", "LC", "METEO", "LAI", "SS", "BC0", "BC1", "BC2");
    for (i = 0; i < mData->NumEle; i++)
    {
        fprintf(Ofile[2], "%-8d", mData->Ele[i].index);
        fprintf(Ofile[2], "%-8d%-8d%-8d",
            mData->Ele[i].soil, mData->Ele[i].geol, mData->Ele[i].LC);
        fprintf(Ofile[2], "%-8d%-8d%-8d",
            mData->Ele[i].prep, 0, mData->Ele[i].source);
        for (j = 0; j < 3; j++)
        {
            fprintf(Ofile[2], "%-8d", mData->Ele[i].BC[j]);
        }
        fprintf(Ofile[2], "\n");
    }
    fclose(Ofile[2]);
    printf("done.\n");

    /* rewrite *.soil  */
    printf("\n  4) writing %s.soil ...", filename);
    ofn[3] = (char *)malloc((strlen(filename) + 10) * sizeof(char));
    strcpy(ofn[3], "output/");
    strcat(ofn[3], filename);
    Ofile[3] = fopen(strcat(ofn[3], ".soil"), "w");
    fprintf(Ofile[3], "%-8s%d\n", "NUMSOIL", mData->NumSoil);
    fprintf(Ofile[3],
        "%-8s%-8s%-8s%-8s%-8s%-12s%-12s%-12s%-8s%-8s%-8s%-8s%-8s%-8s%-8s%s\n",
        "INDEX", "SILT", "CLAY", "OM", "BD", "KINF", "KSATV", "KSATH", "MAXSMC",
        "MINSMC", "ALPHA", "BETA", "MACHF", "MACVF", "DMAC", "QTZ");
    for (i = 0; i < mData->NumSoil; i++)
    {
        fprintf(Ofile[3], "%-8d%-8d%-8d%-8d%-8d",
            mData->Soil[i].index, -999, -999, -999, -999);
        fprintf(Ofile[3], "%-12.3E%-12.3E%-12.3E",
            mData->Soil[i].KsatV / 86400, mData->Soil[i].KsatV / 86400,
            mData->Geol[i].KsatH / 86400);
        fprintf(Ofile[3], "%-8.3lf%-8.3lf", mData->Soil[i].ThetaS,
            mData->Soil[i].ThetaR);
        fprintf(Ofile[3], "%-8.3lf%-8.3lf", mData->Soil[i].Alpha,
            mData->Soil[i].Beta);
        fprintf(Ofile[3], "%-8.2lf%-8.2lf%-8.2lf%.2lf\n",
            mData->Soil[i].hAreaF, mData->Geol[i].vAreaF, mData->Geol[i].macD, 0.25);
    }
    fprintf(Ofile[3], "%-12s%.2lf\n", "DINF", mData->Soil[1].infD);
    fprintf(Ofile[3], "%-12s%.1lf\n", "KMACV_RO",
        mData->Geol[1].macKsatH / mData->Geol[1].KsatH);
    fprintf(Ofile[3], "%-12s%.1lf\n", "KMACH_RO",
        mData->Soil[1].macKsatV / mData->Soil[1].KsatV);
    fclose(Ofile[3]);
    printf("done.\n");

    /* rewirte *.para  */
    printf("\n  7) writing %s.para ...", filename);
    ofn[6] = (char *)malloc((strlen(filename) + 10) * sizeof(char));
    strcpy(ofn[6], "output/");
    strcat(ofn[6], filename);
    Ofile[6] = fopen(strcat(ofn[6], ".para"), "w");
    fprintf(Ofile[6], "%-20s%-20d"
        "# simulation type: 1 = spinup simulation, 0 = normal simulation\n",
        "SIMULATION_MODE", 0);
    fprintf(Ofile[6], "%-20s%-20d"
        "# initialization type 0: relaxation, 1: use .ic file\n",
        "INIT_MODE", 0);
    fprintf(Ofile[6], "%-20s%-20d"
        "# write ASCII output? 0: no, 1: yes\n",
        "ASCII_OUTPUT", 1);
    fprintf(Ofile[6], "%-20s%-20d"
        "# write water balance? 0: no, 1: yes\n",
        "WATBAL_OUTPUT", 0);
    fprintf(Ofile[6], "%-20s%-20d"
        "# write initial condition? 0 = no, "
        "1 = outputs will be written to restart directory\n",
        "WRITE_IC", 1);
    fprintf(Ofile[6], "%-20s%-20d"
        "# unsaturation formulation: 1 = kinematic, 2 = diffusion\n",
        "UNSAT_MODE", mData->UnsatMode);
    fprintf(Ofile[6], "%-20s%-20d"
        "# surface overland flow formulation: 1 = kinematic, 2 = diffusion\n",
        "SURF_MODE", mData->SurfMode);
    fprintf(Ofile[6], "%-20s%-20d"
        "# river routing formulation: 1 = kinematic, 2 = diffusion\n",
        "RIV_MODE", mData->RivMode);
    rawtime = zerorawtime + cData.StartTime;
    timeinfo = localtime(&rawtime);
    fprintf(Ofile[6], "%-20s%4.4d-%2.2d-%2.2d %2.2d:%2.2d\n", "START",
        timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
        timeinfo->tm_hour, timeinfo->tm_min);
    rawtime = zerorawtime + cData.EndTime;
    timeinfo = localtime(&rawtime);
    fprintf(Ofile[6], "%-20s%4.4d-%2.2d-%2.2d %2.2d:%2.2d\n", "END",
        timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
        timeinfo->tm_hour, timeinfo->tm_min);
    fprintf(Ofile[6], "%-20s%-20d"
        "# maximum number of spinup years (if spinup simulation)\n",
        "MAX_SPINUP_YEAR", 50);
    fprintf(Ofile[6], "%-20s%-20d# model step size (s)\n",
        "MODEL_STEPSIZE", (int)cData.b * 60);
    fprintf(Ofile[6], "%-20s%-20d# land surface (ET) step size (s)\n",
        "LSM_STEP", (int)cData.ETStep * 60);

    fprintf(Ofile[6], "################################################################################\n");
    fprintf(Ofile[6], "# CVode parameters                                                             #\n");
    fprintf(Ofile[6], "################################################################################\n");
    fprintf(Ofile[6], "%-20s%-20.1E# absolute solver tolerance (m)\n",
        "ABSTOL", cData.abstol);
    fprintf(Ofile[6], "%-20s%-20.1E# relative solver tolerance (-)\n",
        "RELTOL", cData.reltol);
    fprintf(Ofile[6], "%-20s%-20.1E# initial solver step size (s)\n",
        "INIT_SOLVER_STEP", cData.InitStep);
    fprintf(Ofile[6], "%-20s%-20.1f# Number of non-convergence failures\n",
        "NUM_NONCOV_FAIL", 0.0);
    fprintf(Ofile[6], "%-20s%-20.1f# Maximum number of non-linear \n",
        "MAX_NONLIN_ITER", 3.0);
    fprintf(Ofile[6], "%-20s%-20.1f# Minimum number of non-linear iterations\n",
        "MIN_NONLIN_ITER", 1.0);
    fprintf(Ofile[6], "%-20s%-20.1f# CVode max step decrease factor\n",
        "DECR_FACTOR", 1.2);
    fprintf(Ofile[6], "%-20s%-20.1f# CVode max step increase factor\n",
        "INCR_FACTOR", 1.2);
    fprintf(Ofile[6], "%-20s%-20.1f# Minimum CVode max step (s)\n",
        "MIN_MAXSTEP", 1.0);
    rawtime = zerorawtime + cData.StartTime;
    timeinfo = localtime(&rawtime);
    fprintf(Ofile[6], "################################################################################\n");
    fprintf(Ofile[6], "# OUTPUT CONTROL                                                               #\n");
    fprintf(Ofile[6], "# Output intervals can be \"YEARLY\", \"MONTHLY\", \"DAILY\", \"HOURLY\", or any       #\n");
    fprintf(Ofile[6], "# positive integer to indicate intervals in seconds. Setting interval to 0     #\n");
    fprintf(Ofile[6], "# will turn off the output for the corresponding variable.                     #\n");
    fprintf(Ofile[6], "################################################################################\n");

    fprintf(Ofile[6], "%-20sHOURLY\n", "SURF");
    fprintf(Ofile[6], "%-20sHOURLY\n", "UNSAT");
    fprintf(Ofile[6], "%-20sHOURLY\n", "GW");
    fprintf(Ofile[6], "%-20sHOURLY\n", "RIVSTG");
    fprintf(Ofile[6], "%-20sHOURLY\n", "RIVGW");
    fprintf(Ofile[6], "%-20sHOURLY\n", "SNOW");
    fprintf(Ofile[6], "%-20sHOURLY\n", "CMC");
    fprintf(Ofile[6], "%-20sHOURLY\n", "INFIL");
    fprintf(Ofile[6], "%-20sHOURLY\n", "RECHARGE");
    fprintf(Ofile[6], "%-20sHOURLY\n", "EC");
    fprintf(Ofile[6], "%-20sHOURLY\n", "ETT");
    fprintf(Ofile[6], "%-20sHOURLY\n", "EDIR");
    fprintf(Ofile[6], "%-20sHOURLY\n", "RIVFLX0");
    fprintf(Ofile[6], "%-20sHOURLY\n", "RIVFLX1");
    fprintf(Ofile[6], "%-20sHOURLY\n", "RIVFLX2");
    fprintf(Ofile[6], "%-20sHOURLY\n", "RIVFLX3");
    fprintf(Ofile[6], "%-20sHOURLY\n", "RIVFLX4");
    fprintf(Ofile[6], "%-20sHOURLY\n", "RIVFLX5");
    fprintf(Ofile[6], "%-20sHOURLY\n", "RIVFLX6");
    fprintf(Ofile[6], "%-20sHOURLY\n", "RIVFLX7");
    fprintf(Ofile[6], "%-20sHOURLY\n", "RIVFLX8");
    fprintf(Ofile[6], "%-20sHOURLY\n", "RIVFLX9");
    fprintf(Ofile[6], "%-20sHOURLY\n", "RIVFLX10");
    fprintf(Ofile[6], "%-20sHOURLY\n", "SUBFLX");
    fprintf(Ofile[6], "%-20sHOURLY\n", "SURFFLX");
    fprintf(Ofile[6], "%-20sMONTHLY\n","IC");
    fclose(Ofile[6]);
    printf("done.\n");

    /* rewirte *.calib  */
    printf("\n  8) writing %s.calib ...", filename);
    ofn[7] = (char *)malloc((strlen(filename) + 10) * sizeof(char));
    strcpy(ofn[7], "output/");
    strcat(ofn[7], filename);
    Ofile[7] = fopen(strcat(ofn[7], ".calib"), "w");
    fprintf(Ofile[7],
        "%-20s%-8.1lf# horizontal saturated hydraulic conductivity\n",
        "KSATH", cData.Cal.KsatH);
    fprintf(Ofile[7],
        "%-20s%-8.2lf# vertital saturated hydraulic conductivity\n", "KSATV",
        cData.Cal.KsatV);
    fprintf(Ofile[7], "%-20s%-8.2lf# infiltration conductivity\n", "KINF",
        cData.Cal.infKsatV);
    fprintf(Ofile[7],
        "%-20s%-8.2lf# macropore horizontal saturated hydraulic conductivity\n",
        "KMACSATH", cData.Cal.macKsatH);
    fprintf(Ofile[7],
        "%-20s%-8.2lf# macropore vertical saturated hydraulic conductivity\n",
        "KMACSATV", cData.Cal.macKsatV);
    fprintf(Ofile[7], "%-20s%-8.2lf# depth of infiltration layer\n", "DINF",
        cData.Cal.infD);
    fprintf(Ofile[7], "%-20s%-8.2lf# rooting depth\n", "DROOT",
        cData.Cal.RzD);
    fprintf(Ofile[7], "%-20s%-8.2lf# macropore depth\n", "DMAC",
        cData.Cal.macD);
    fprintf(Ofile[7], "%-20s%-8.2lf# soil porosity\n", "POROSITY", cData.Cal.Porosity);
    fprintf(Ofile[7], "%-20s%-8.2lf# van Genuchten alpha\n", "ALPHA", cData.Cal.Alpha);
    fprintf(Ofile[7], "%-20s%-8.2lf# van Genuchten beta (n)\n", "BETA", cData.Cal.Beta);
    fprintf(Ofile[7], "%-20s%-8.2lf# macropore area fraction on a vertical cross-section\n", "MACVF", cData.Cal.vAreaF);
    fprintf(Ofile[7], "%-20s%-8.2lf# macropore area fraction on a horizontal cross-section\n", "MACHF", cData.Cal.hAreaF);
    fprintf(Ofile[7], "%-20s%-8.2lf# areal fractional coverage of green vegetation\n", "VEGFRAC", cData.Cal.VegFrac);
    fprintf(Ofile[7], "%-20s%-8.2lf# surface albedo\n", "ALBEDO", cData.Cal.Albedo);
    fprintf(Ofile[7], "%-20s%-8.2lf# surface roughness (Manning's n)\n", "ROUGH", cData.Cal.Rough);
    fprintf(Ofile[7], "%-20s%-8.2lf# canopy water evaporation\n", "EC", mData->pcCal.Et0);
    fprintf(Ofile[7], "%-20s%-8.2lf# total plant transpiration\n", "ETT", mData->pcCal.Et1);
    fprintf(Ofile[7], "%-20s%-8.2lf# direct soil evaporation\n", "EDIR", mData->pcCal.Et2);
    fprintf(Ofile[7], "%-20s%-8.2lf# river channel roughness\n", "ROUGH_RIV", cData.Cal.rivRough);
    fprintf(Ofile[7], "%-20s%-8.2lf# bank hydraulic conductivity\n", "KRIVH", cData.Cal.rivKsatH);
    fprintf(Ofile[7], "%-20s%-8.2lf# bed hydraulic conductivity\n", "KRIVV", cData.Cal.rivKsatV);
    fprintf(Ofile[7], "%-20s%-8.2lf# bed thickness\n", "BEDTHCK", cData.Cal.rivbedThick);
    fprintf(Ofile[7], "%-20s%-8.2lf# river channel depth\n", "RIV_DPTH", cData.Cal.rivDepth);
    fprintf(Ofile[7], "%-20s%-8.2lf# river channel width\n", "RIV_WDTH", cData.Cal.rivShapeCoeff);
    fprintf(Ofile[7], "SCENARIO\n");
    fprintf(Ofile[7], "%-20s%-8.2lf# precipitation multiplier\n", "PRCP", cData.Cal.Prep);
    fprintf(Ofile[7], "%-20s%-8.2lf# air temperature offset (K)\n", "SFCTMP", cData.Cal.Temp);
    fclose(Ofile[7]);
    printf("done.\n");
}
