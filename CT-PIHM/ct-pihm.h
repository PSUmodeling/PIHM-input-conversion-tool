/**********************************************************************************
 * File        : pihm.h                                                           *
 * Function    : Declaration and Definition of global variables and data structure*
 * Developer of PIHM 2.0: Mukesh Kumar (muk139@psu.edu)                           *
 * Developer of PIHM 1.0: Yizhong Qu   (quyizhong@gmail.com)                      *
 * Version     : Nov, 2007 (2.0)                                                  *
 *--------------------------------------------------------------------------------*
 *                                                                                *
 *..............MODIFICATIONS/ADDITIONS in PIHM 2.0...............................* 
 * a) Definition of new variables for ELEMENT data model related to 		  *
 *	i. Subsurface: KsatH, KsatV, infKsatV,infD, RzD, macD, macKsatH, macKsatV *
 *	vAreaF, hAreaF								  * 
 *	ii. ET: LAImax, vegFrac, Albedo, Rs_ref, Rmin				  *
 *	iii. Snow: meltF							  *
 *	iv. Surface: dhBydx, dhBYdy, surfH					  *
 * b) Definition of new variables for RIVER data model				  *
 *	i. KsatH, KsatV, bedThick 						  *
 * c) Definition of new structures:						  *
 *	i. geology								  *
 *	ii. Land Cover								  *
 *	iii. Calibration							  *
 * d) Definition of New Control Parameters for each file			  *
 *--------------------------------------------------------------------------------*
 * For questions or comments, please contact                                      *
 *      --> Mukesh Kumar (muk139@psu.edu)                                	  *
 *      --> Prof. Chris Duffy (cxd11@psu.edu)                                     *
 * This code is free for research purpose only.                                   *
 * Please provide relevant references if you use this code in your research work  *
 *--------------------------------------------------------------------------------*
 **********************************************************************************/

/* Definition of Global Variable Types */

typedef struct element_type	/* Data model for a triangular element */ 
	{
  	int index; 		/* Element No. */ 
  	int node[3];        	/* anti-clock-wise */
  	int nabr[3];        	/* neighbor i shares edge i (0: on boundary) */
    
  	double edge[3];   	/* edge i is from node i to node i+1 */
  	double area;      	/* area of element */
  
  	double x;         	/* x of centroid */
  	double y;         	/* y of centroid */
  	double zmin;      	/* z_min of centroid */
  	double zmax;      	/* z_max of centroid */
  
  	double KsatH;		/* horizontal geologic saturated hydraulic conductivity */
  	double KsatV;		/* vertical geologic saturated hydraulic conductivity */
	double infKsatV;	/* vertical surface saturated hydraulic conductivity */
  	double Porosity;		
	double infD;		/* depth from ground surface accross which head is calculated during infiltration */
  	double Alpha;		/* Alpha from van-genuchten eqn which is given by satn = 1/pow(1+pow(abs(Alpha*psi),Beta),1-1/Beta) */
  	double Beta;
  	double RzD;		/* Root zone depth */
  	double macD;		/* macropore Depth */
  	double macKsatH;	/* macropore horizontal saturated hydraulic conductivity */
  	double macKsatV;	/* macropore vertical saturated hydraulic conductivity */
  	double vAreaF;	/* macropore area fraction on a vertical cross-section */
  	double hAreaF;	/* macropore area fraction on a horizontal cross-section */
  	int Macropore;       	/* 1: macropore; 0: regular soil */

  	double LAImax;    	/* maxm. LAI accross all seasons for a vegetation type */
  	double VegFrac;   	/* areal vegetation fraction in a triangular element */ 
  	double Albedo;    	/* albedo of a triangular element */ 
  	double Rs_ref;     	/* reference incoming solar flux for photosynthetically active canopy */
  	double Rmin;       	/* minimum canopy resistance */
  	double Rough;      	/* surface roughness of an element */
  
  	double windH;		/* wind measurement height */
  
    
  	int soil;           	/* soil type */
	int geol;		/* geology type */
  	int LC;             	/* Land Cover type  */
  	int IC;             	/* initial condition type */
  	int BC[3];             	/* boundary type. 0:natural bc (no flow); 1:Dirichlet BC; 2:Neumann BC */
  	int prep;           	/* precipitation (forcing) type */
  	int temp;           	/* temperature (forcing) type   */
  	int humidity;       	/* humidity type */
  	int WindVel;        	/* wind velocity type  */
  	int Rn;             	/* net radiation input */
  	int G;              	/* radiation into ground */
  	int pressure;       	/* pressure type */
  	int source;         	/* source (well) type */
 	int meltF;		/* meltFactor */ 
	/* for calculation of dh/ds */
  	double surfH[3];	/* Total head in neighboring cells */	
  	double surfX[3];	/* Center X location of neighboring cells */
  	double surfY[3];	/* Center Y location of neighboring cells */
  	double dhBYdx;	/* Head gradient in x dirn. */
  	double dhBYdy;	/* Head gradient in y dirn. */
	} element;

typedef struct nodes_type	/* Data model for a node */
	{
  	int index; 		/* Node no. */
     
  	double x;          	/* x coordinate */
  	double y;          	/* y coordinate */
  	double zmin;       	/* z bed rock elevation */
  	double zmax;       	/* z surface elevation  */
  
	} nodes;

typedef struct element_IC_type	/* Initial state variable conditions on each element */
	{
  	int index;
  
  	double interception;	/* Interception storage (Note all these variables have dimension of L */
  	double snow;		/* Snow depth */
  	double surf;		/* Overland flow depth */
  	double unsat;		/* unsaturated zone depth */
  	double sat;		/* saturated zone depth */
  
	} element_IC;

typedef struct soils_type
	{
  	int index;           	/* index */
  
  	double KsatV;       	/* vertical saturated soil conductivity */
  	double ThetaS;      	/* soil porosity */
  	double ThetaR;      	/* soil moisture residual */
  	double Alpha;      	/* soil curve parameter 1 */
  	double Beta;       	/* soil curve parameter 2 */
  
  	double hAreaF;       	/* macroporous area fraction on horizontal section */
  	double macKsatV;      /* macroporous saturated vertical conductivity */
 
  	double infD;		/* depth from ground surface accross which head is calculated during infiltration */       
  
	} soils;

typedef struct geol_type
        {
        int index;              /* index */

        double KsatH;         /* horizontal saturated geology conductivity */
        double KsatV;         /* vertical saturated geology conductivity */
        double ThetaS;        /* geology porosity */
        double ThetaR;        /* residual porosity */
        double Alpha;         /* van genuchten parameter */
        double Beta;          /* van genuchten parameter */

        double vAreaF;        /* macroporous area fraction on vertical section */
        double macKsatH;      /* macroporous saturated horizontal conductivity */
        double macD;

        } geol;

typedef struct lc_type
	{
  	int index;           	/* index */

  	double LAImax;     	/* max LAI */
  	double VegFrac;    	/* Canopy Fracn */
  	double Albedo;     	/* Albedo */
  	double Rs_ref;     	
  	double Rmin;       	/* Minimum stomatal resistance */
  	double Rough;      	/* surface roughness factor  */
  	double RzD;	       	/* rootZone Depth*/
	} LC;

typedef struct river_segment_type
	{
  	int index;
  
  	double x;          	/* x of river segment */
  	double y;  
  	double zmin;       	/* bed elevation  */
  	double zmax;       	/* bank elevation */
  	double depth;      	/* max depth */
  	double Length;	/* Riv segment Length */
  	double Rough;		/* Manning's roughness coeff */
 	double KsatH;		/* Side conductivity */
	double KsatV;		/* Bed conductivity */
	double bedThick;
	double coeff;		/* Coefficient c in D = c*pow(B/2,interpOrd) where D is depth*/ 
  	int FromNode;		/* Upstream Node no. */
  	int ToNode;		/* Dnstream Node no. */
  	int down;            	/* down stream segment */
  	int LeftEle;		/* Left neighboring element */
  	int RightEle;		/* Right neighboring element */
  	int shape;           	/* shape type    */
  	int material;        	/* material type */
  	int IC;              	/* IC type */
  	int BC;              	/* BC type */
  	int reservoir;
  
	} river_segment;

typedef struct river_shape_type
	{
  	int index;           
  	double depth;      	/* depth */
  	int interpOrd;       	/* Interpolation order for river shape: Order =1 (rectangle), 2(triangle), 3(quadratic) and 4(cubic)*/
  	double coeff;	       	/* Coefficient c in D = c*pow(B/2,interpOrd) */
  
	} river_shape;

typedef struct river_material_type
	{
  	int index;
  	double Rough;
  	double Cwr;		/* Weir Discharge Coefficient */
 	double KsatH;		/* Conductivity of river banks */
	double KsatV;		/* Conductivity of river bed */
	double bedThick; 	/* thickeness of conductive river bed */
	} river_material;

typedef struct river_IC_type
	{
  	int index;
  	double value;		/* initial flow depth */
  
	} river_IC;

typedef struct TSD_type
	{
  	char name[5];
  	int index;
  	int length;	/* length of time series */
  	int iCounter;	/* interpolation counter */
  	double **TS;	/* 2D time series data */
  
	} TSD;

typedef struct global_calib
	{
        double KsatH;		/* For explanation of each calibration variable, look for corresponding variables above */
        double KsatV;
        double infKsatV;
        double macKsatH;
        double macKsatV;
        double infD;
        double RzD;
        double macD;
        double Porosity;
        double Alpha;
        double Beta;
        double vAreaF;
        double hAreaF;
	double Temp;
	double Prep;
        double VegFrac;
        double Albedo;
        double Rough;

        double rivRough;
        double rivKsatH;
        double rivKsatV;
        double rivbedThick;
	double rivDepth;
	double rivShapeCoeff;
	} globalCal; 

typedef struct process_control
	{
	double Et0;
	double Et1;
	double Et2;
	} processCal;

typedef struct model_data_structure 	/* Model_data definition */
	{
  	int UnsatMode;               	/* Unsat Mode */
  	int SurfMode;                	/* Surface Overland Flow Mode */
  	int RivMode;                 	/* River Routing Mode */
    
  	int NumEle;                  	/* Number of Elements */
  	int NumNode;                 	/* Number of Nodes    */
  	int NumRiv;                  	/* Number of Rivere Segments */
  
  	int NumPrep;                 	/* Number of Precipatation time series types  */
  	int NumTemp;                 	/* Number of Temperature time series types      */
  	int NumHumidity;             	/* Number of Humidity time series types         */
  	int NumWindVel;              	/* Number of Wind Velocity time series types    */
  	int NumRn;                   	/* Number of Net Radiation time series types    */
  	int NumG;                    	/* Number of Ground Heat time series types      */
  	int NumP;                    	/* Number of Pressure time series types         */
  	int NumSource;               	/* Number of Source time series types           */
  
  	int NumSoil;                 	/* Number of Soils           */
  	int NumGeol;                 	/* Number of Geologies           */
  	int NumRes;                  	/* Number of Reservoir       */
  	int NumLC;                   	/* Number of Land Cover Index Data */
  
  	int NumMeltF;		       	/* Number of Melt Factor Time series */
  
  	int Num1BC;                  	/* Number of Dirichlet BC    */
  	int Num2BC;                  	/* Number of Numann BC       */
  	int NumEleIC;                	/* Number of Element Initial Condtion */
  
  	int NumRivShape;             	/* Number of River Shape     */
  	int NumRivMaterial;          	/* Number of River Bank/Bed Material */
  	int NumRivIC;                	/* Number of River Initial Condition  */
  	int NumRivBC;                	/* Number of River Boundary Condition */
  
  	element *Ele;                	/* Store Element Information  */
  	nodes *Node;                 	/* Store Node Information     */
  	element_IC *Ele_IC;          	/* Store Element Initial Condtion */
  	soils *Soil;                 	/* Store Soil Information     */
  	geol *Geol;                 	/* Store Geology Information     */
  	LC *LandC;		       	/* Store Land Cover Information */
  
  	river_segment *Riv;          	/* Store River Segment Information */
  	river_shape *Riv_Shape;      	/* Store River Shape Information   */
  	river_material *Riv_Mat;     	/* Store River Bank Material Information */
  	river_IC *Riv_IC;            	/* Store River Initial Condition   */

  	TSD *TSD_Inc;                	/* Infiltration Capacity Time Series Data */
  	TSD *TSD_LAI;                	/* Leaves Area Index Time Series Data     */
//  	TSD *TSD_DH;		     	/* Zero plane Displacement Height */  
  	TSD *TSD_RL;		     	/* Roughness Length */  
  	double *ISFactor;          	/* ISFactor is used to calculate ISMax from LAI */
	double *windH;		/* Height at which wind velocity is measured */
  	TSD  *TSD_MeltF;	       	/* Monthly Varying Melt Factor for Temperature Index model */  

  	TSD *TSD_EleBC;              	/* Element Boundary Condition Time Series Data  */
  	TSD *TSD_Riv;                	/* River Related Time Series Data  */
  	TSD *TSD_Prep;               	/* RainFall Time Series Data       */
  	TSD *TSD_Temp;               	/* Temperature Time Series Data    */
  	TSD *TSD_Humidity;           	/* Humidity Time Series Data       */
  	TSD *TSD_WindVel;            	/* Wind Velocity Time Series Data  */
  	TSD *TSD_Rn;                 	/* Net Radiation Time Series Data  */
  	TSD *TSD_G;                  	/* Radiation into Ground Time Series Data */
  	TSD *TSD_Pressure;           	/* Vapor Pressure Time Series data       */
  	TSD *TSD_Source;             	/* Source (well) Time Series data  */

  	double **FluxSurf;     	/* Overland Flux   */
  	double **FluxSub;      	/* Subsurface Flux */
  	double **FluxRiv;      	/* River Segement Flux */

  	double *ElePrep;		/* Precep. on each element */
  	double *EleETloss;		
  	double *EleNetPrep;		/* Net precep. on each elment */
  	double *EleViR;		/* Variable infiltration rate */
  	double *Recharge;		/* Recharge rate to GW */
  	double *EleSnow;		/* Snow depth on each element */
  	double *EleSnowGrnd;		/* Snow depth on ground element */
  	double *EleSnowCanopy;	/* Snow depth on canopy element */
  	double *EleIS;		/* Interception storage */
  	double *EleISmax;		/* Maximum interception storage (liquid precep) */
	double *EleISsnowmax;		/* Maximum interception storage (snow) */
  	double *EleTF;		/* Through Fall */
  	double **EleET;		/* Evapo-transpiration (from canopy, ground, subsurface, transpiration) */
  	double Q; 
 	double *DummyY; 
	double *PrintVar[22];
	processCal pcCal;
	} *Model_Data;

typedef struct control_data_structure
	{
  	int Verbose;
  	int Debug;
  
  	int Solver;	/* Solver type */
  	int NumSteps;	/* Number of external time steps (when results can be printed) for the whole simulation */	
  
  	int gwD;	/* File boolean, Choose 1 if your want to print ground water */
	int surfD;	/* File boolean, Choose 1 if your want to print overland flow */
	int snowD;      /* File boolean, Choose 1 if your want to print snow Depth */
	int rivStg;     /* File boolean, Choose 1 if your want to print river Stage */
	int Rech;       /* File boolean, Choose 1 if your want to print recharge to ground water */
	int IsD;        /* File boolean, Choose 1 if your want to print interception depth */
	int usD;        /* File boolean, Choose 1 if your want to print unsaturated depth */
	int et[3];      /* File boolean, Choose 1 if your want to print individual evapo-transpiration components */
	int rivFlx[10];	/* File boolean, Choose 1 if your want to print river/river bed fluxes */
  
  	int gwDInt;	/* Time interval to output average val of variables */
	int surfDInt;
	int snowDInt;
	int rivStgInt;
	int RechInt;
	int IsDInt;
	int usDInt;
	int etInt;
	int rivFlxInt;
  
  	int init_type;		/* initialization mode */
  
  	double abstol;	/* absolute tolerance */
  	double reltol;	/* relative tolerance */
  	double InitStep;	/* initial step size */
  	double MaxStep;	/* Maximum step size */
  	double ETStep;	/* Step for et from interception */
  
  	int GSType, MaxK;	/* Maximum Krylov order */
  	double delt;
  
  	double StartTime;	/* Start time of simulation */
  	double EndTime;	/* End time of simulation */
  
  
  	int outtype;		
  	double a;		/* External time stepping controls */
  	double b;
  
  	double *Tout;  
 	
	globalCal Cal;		/* Convert this to pointer for localized calibration */ 
	} Control_Data;
