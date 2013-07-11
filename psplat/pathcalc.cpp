/**
 * Pathloss Calculator. Takes a hardcoded path array and feeds it into 
 * the OpenCL ITWOM calculations. The goal here is to test the OpenCL 
 * ITWOM code. 
 *
 * @author Andrew J. Musselman 
 */

#include "clutil.h"
#include "splat.h"
#include<unistd.h>
#include<assert.h>

//Declration of point to point for serial ITWOM
void point_to_point(double elev[], double tht_m, double rht_m, double eps_dielect, double sgm_conductivity, double eno_ns_surfref, double frq_mhz, int radio_climate, int pol, double conf, double rel, double &dbloss, char *strmode, int &errnum);

const int ELEVDIST_DFLT=100; /* default distance between path elements, 100m */ 
const int ELEVSIZE_DFLT=2000; /* default number of path elevation samples */ 

void allPoints(size_t numElev, double dist, double *elev, double *signal,
                  double sourceAlt, double destAlt, double eps_dielect, 
                  double sgm_conductivity, double eno_ns_surfref, 
                  double frq_mhz, int radio_climate, int pol, double conf,
                  double rel){
   if(numElev > ARRAYSIZE){
      throw;
   }
   double itm_elev[ARRAYSIZE+2];
   double loss = 0;
   char strmode[10000];
   int errnum;

   for(int i = 0; i < numElev; i++){
      itm_elev[i+2] = elev[i]; 
   }


   for(int i = 1; i <= numElev; i++){
      itm_elev[0] = i; /* Number of points */ 
      itm_elev[1] = dist; /* Distance between points */
   
      point_to_point(itm_elev,sourceAlt, destAlt, eps_dielect, sgm_conductivity,
         eno_ns_surfref, frq_mhz, radio_climate, pol, conf, rel, loss,
         strmode,errnum);
      signal[i] = loss;
   }
}

void allPoints_runCl(size_t numElev, double dist, double *elev, double *signal,
                  double sourceAlt, double destAlt, double eps_dielect, 
                  double sgm_conductivity, double eno_ns_surfref, 
                  double frq_mhz, int radio_climate, int pol, double conf,
                  double rel,cl_kernel kernel,cl_context context,
                  cl_command_queue queue){
   int err;
   //Array to represent the elevation array
   cl_mem elevBuffer = clCreateBuffer(context, 
      CL_MEM_READ_ONLY |CL_MEM_COPY_HOST_PTR, 
      sizeof(double)*numElev,
      elev,
      &err);
   if(err < 0) {
      perror("Couldn't create a buffer");
      exit(1);   
   };

   //dbloss - this is the result buffer
   cl_mem dblossBuffer = clCreateBuffer(context, 
      CL_MEM_READ_WRITE, 
      sizeof(double)*numElev,
      NULL,
      &err);
   if(err < 0) {
      perror("Couldn't create a buffer");
      exit(1);   
   };

   /* Create Kernel arguments */
   err = clSetKernelArg(kernel, 0, sizeof(cl_int),(void*)&numElev);
   if(err < 0) {
      fprintf(stderr,"Couldn't create a kernel argument:elevSize Code:%d",err);
      exit(1);
   }

   err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &elevBuffer);
   if(err < 0) {
      fprintf(stderr,"Couldn't create a kernel argument:elev Code:%d",err);
      exit(1);
   }

   err = clSetKernelArg(kernel, 2, sizeof(cl_double), (void*)&dist);
   if(err < 0) {
      fprintf(stderr,"Couldn't create a kernel argument:pathDist Code:%d",err);
      exit(1);
   }

   err = clSetKernelArg(kernel, 3, sizeof(cl_double),(void*)&sourceAlt );
   if(err < 0) {
      fprintf(stderr,"Couldn't create a kernel argument:scourceAlt Code:%d",err);
      exit(1);
   }

   err = clSetKernelArg(kernel, 4, sizeof(cl_double),(void*)&destAlt );
   if(err < 0) {
      fprintf(stderr,"Couldn't create a kernel argument:destAlt Code:%d",err);
      exit(1);
   }

   err = clSetKernelArg(kernel, 5, sizeof(cl_double), (void*)&eps_dielect);
   if(err < 0) {
      fprintf(stderr,"Couldn't create a kernel argument:eps Code:%d",err);
      exit(1);
   }

   err = clSetKernelArg(kernel, 6, sizeof(cl_double), (void*)&sgm_conductivity);
   if(err < 0) {
      fprintf(stderr,"Couldn't create a kernel argument:sgm Code:%d",err);
      exit(1);
   }

   err = clSetKernelArg(kernel, 7, sizeof(cl_double), &eno_ns_surfref);
   if(err < 0) {
      fprintf(stderr,"Couldn't create a kernel argument:surfref Code:%d",err);
      exit(1);
   }

   err = clSetKernelArg(kernel, 8, sizeof(cl_double), (void*)&frq_mhz );
   if(err < 0) {
      fprintf(stderr,"Couldn't create a kernel argument:frq Code:%d",err);
      exit(1);
   }

   err = clSetKernelArg(kernel, 9, sizeof(cl_int), (void*)&radio_climate);
   if(err < 0) {
      fprintf(stderr,"Couldn't create a kernel argument:climate Code:%d",err);
      exit(1);
   }

   err = clSetKernelArg(kernel, 10, sizeof(cl_int), &pol);
   if(err < 0) {
      fprintf(stderr,"Couldn't create a kernel argument:pol Code:%d",err);
      exit(1);
   }

   err = clSetKernelArg(kernel, 11, sizeof(cl_double),(void*)&conf );
   if(err < 0) {
      fprintf(stderr,"Couldn't create a kernel argument:conf Code:%d",err);
      exit(1);
   }
 
   err = clSetKernelArg(kernel, 12, sizeof(cl_double), (void*)&rel);
   if(err < 0) {
      fprintf(stderr,"Couldn't create a kernel argument:relBuffer Code:%d",err);
      exit(1);
   }

   err = clSetKernelArg(kernel, 13, sizeof(cl_mem), &dblossBuffer );
   if(err < 0) {
      fprintf(stderr,"Couldn't create a kernel argument:dbloss Code:%d",err);
      exit(1);
   }

   /* Enqueue kernel */
   err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &numElev, 
         NULL, 0, NULL, NULL); 
   if(err < 0) {
      fprintf(stderr,"Couldn't enqueue the kernel, code:%d",err);
      exit(1);
   }

   err = clFinish(queue);
   if(err < 0){
      fprintf(stderr,"Kernel didn't finish, code: %d",err);
      exit(1);
   }

   /* Read the kernel's output */
   err = clEnqueueReadBuffer(
         queue,//Command Queue 
         dblossBuffer, //CL buffer
         CL_TRUE, //Blocking read
         0, //Offset
         sizeof(double)*numElev, //size to read
         signal, 
         0, 
         NULL, 
         NULL);
   if(err < 0) {
      fprintf(stderr,"Error code %d\n",err);
      perror("Couldn't read the buffer");
      exit(1);
   }

   //Cleanup Buffers
   clReleaseMemObject(elevBuffer);
   clReleaseMemObject(dblossBuffer);

}

void allPoints_cl(size_t numElev, double dist, double *elev, double *signal,
                  double sourceAlt, double destAlt, double eps_dielect, 
                  double sgm_conductivity, double eno_ns_surfref, 
                  double frq_mhz, int radio_climate, int pol, double conf,
                  double rel){
   int err;

   /* OpenCL structures */
   cl_device_id device;
   cl_context context;
   cl_program program;
   cl_kernel kernel;
   cl_command_queue queue;
   cl_int i, j;

   cl_uint numPlatforms;
   cl_uint numDevices[MAX_DEVICE];
   cl_device_id devices[MAX_PLATFORM][MAX_DEVICE];
   cl_platform_id platforms;
   if(getDevices(devices,MAX_PLATFORM,MAX_DEVICE,&numPlatforms,numDevices) > 0){
      fprintf(stderr,"Couldn't get devices");    
      exit(1);
   }

   cl_mem destBuffer;
   cl_mem demBuffer;
    
   //Hardcoded to get first device...
   context = clCreateContext(NULL, 1, &devices[0][0], NULL, NULL, &err);
   //Call the OpenCL Kernel:
   

   program = build_program(context, device,"/home/amusselm/projects/srproject/Parallel-LRP/psplat/itm_support.cl" );
   /* Create a command queue */
   queue = clCreateCommandQueue(context, devices[0][0], 0, &err);
   if(err < 0) {
      perror("Couldn't create a command queue");
      exit(1);   
   };
   
   /* create kernel */
   kernel = clCreateKernel(program, "point_to_point_cl", &err);
   if(err < 0) {
      perror("Couldn't create a kernel");
      exit(1);
   }
   allPoints_runCl(numElev,dist,elev,signal,sourceAlt,destAlt,
                eps_dielect,sgm_conductivity,eno_ns_surfref,frq_mhz,radio_climate,
                pol,conf,rel,kernel,context,queue);

   //Program cleanup
   clReleaseKernel(kernel);
   clReleaseCommandQueue(queue);
   clReleaseProgram(program);
   clReleaseContext(context);
}

void generateTerrain(double* elev, int numElevElements, int profile, double profileAlt){
   double startAlt;
   double endAlt;
   if(profileAlt < 0){
      startAlt = abs(profileAlt); 
      endAlt = 0;
   }
   else {
      startAlt = 0;
      endAlt = profileAlt;
   }

   /* Create a very even slope */
   if(profile == 0){
      double altStep = (endAlt-startAlt)/numElevElements;
      for(int i = 0; i < numElevElements; i++){
         elev[i]=startAlt+altStep*i;
      }
   }
   /* Create a sawtooth */
   else {
      int numSections = profile+1;
      int remainder = numElevElements%numSections;
      int sectionElements = numElevElements/numSections;
      double altStep = (endAlt-startAlt)/sectionElements;
      for(int i = 0; i < numSections; i++){
         for(int j = 0; j < sectionElements; j++){
            elev[(i*sectionElements)+j] = startAlt+altStep*j; 
         }
      }
      for(int k = remainder; k > 0; k--){
         elev[(numElevElements-1)-k] = endAlt;
      }
   }
}

void printElev(double* elev, int numElevElements, int profile, double profileAlt, double elevDistance){
   printf("Profile: %d Profile Altitude: %lf\n",profile,profileAlt);
   for(int i = 0; i < numElevElements; i++){
      printf("Point %d at %lf meters: %lf\n",i,elevDistance*i,elev[i]);
   }
}

void printUsage(){
   fprintf(stdout,"Usage: pathtest [-v mode] [-d distance] [-t txheight] [-r rxheight] [-n numpoints] [-f frequency] [-p profile] [-a terrainalt] [-m mode] [-h]\n");
}

void printResultsBoth(double* signal, double* signal_serial, int count, double elevDistance){
   printf("Results:\n");
   for(int i=1; i<count; i++){
      double difference = signal[i]-signal_serial[i];
      double max = signal[i]>signal_serial[i] ? signal[i] : signal_serial[i];
      double percent = difference/max;
      printf("distance: %lf,signal[%d]: %lf, signal_serial[%d]:%lf, difference:%lf, %lf\n",
            i*elevDistance,i,signal[i],i,signal_serial[i],difference,percent);
   }
}

void printResultsSingle(double* signal, int count, double elevDistance){
   printf("Results:\n");
   for(int i=1; i<count; i++){
      printf("distance: %lf,signal[%d]: %lf\n",i*elevDistance,i,signal[i]);
   }
}

int resultsPass(double* signal, double* signal_serial, int count){
   int differenceFound = 0;
   for(int i=1; i<count; i++){
      double difference = signal[i]-signal_serial[i];
      if (abs(difference) > .0000000000000002){
         differenceFound = 1; 
      }
   }
   return !differenceFound;
}

void printHelp(){
   printf("Pathtest - Tests the ITWOM model using generated data\n");
   printf("Options:\n");
   printf("-v ###:verbose, prints results and information\n");
   printf("   1 = Parameters and Pass/Fail\n");
   printf("   2 = All of the above plus complete terrain profile and results\n");
   printf("-d ### :distance, sets the distance between points in meters \n");
   printf("-t ### :transmitter height, sets the transmitter height in meters \n");
   printf("-r ### :receiver height, sets the receiver height in meters \n");
   printf("-n ### :number of points, sets the number of points in the test path \n");
   printf("-f ### :sets the frequency \n");
   printf("-p ### :terrain profile where:\n");
   printf("   0 = flat/constant slope up\n");
   printf("   1 = one peak\n");
   printf("   2 = two peaks\n");
   printf("   3 = three peaks\n");
   printf("-a ### :Peak terrain altitude (meters)\n");
   printf("-m ### :Execution Mode\n");
   printf("   0 = Run both parallel and serial and compare results\n");
   printf("   1 = Run both parallel and serial and do not compare results\n");
   printf("   2 = Run parallel only\n");
   printf("   3 = Run serial only\n");
}

void printAttrs(size_t numElevElements, 
   double elevDistance,
   double sourceAlt, /* Source Altitude (meters) */
   double destAlt, /* Source Altitude (meters) */
   double eps_dielect,
	double sgm_conductivity,
	double eno_ns_surfref,
	double frq_mhz,
	int radio_climate,
	int pol,
	double conf,
	double rel)
{
   printf("Path Attributes:\n");
   printf("numElevElements: %ld\n",numElevElements);
   printf("elevDistance: %lf\n",elevDistance);
   printf("sourceAlt: %lf\n",sourceAlt); /* Source Altitude (meters) */
   printf("destAlt: %lf\n",destAlt); /* Source Altitude (meters) */
   printf("eps_dielect: %lf\n",eps_dielect);
   printf("sgm_conductivity: %lf\n",sgm_conductivity);
   printf("eno_ns_surfref: %lf\n",eno_ns_surfref);
   printf("frq_mhz: %lf\n",frq_mhz);
   printf("radio_climate: %d\n",radio_climate);
   printf("pol: %d\n",pol);
   printf("conf: %lf\n",conf);
   printf("rel: %lf\n",rel);

}

int main(int argc, char* argv[]){
   size_t numElevElements = ELEVSIZE_DFLT;
   double elevDistance = ELEVDIST_DFLT;
   double sourceAlt=10; /* Source Altitude (meters) */
   double destAlt=10; /* Source Altitude (meters) */
   double eps_dielect=15.0;
	double sgm_conductivity=0.005;
	double eno_ns_surfref=301.0;
	double frq_mhz=300.0;
	int radio_climate=5;
	int pol=0;
	double conf=0.50;
	double rel=0.50;

   int verbose = 0;
   int profile = 0; /* default to constant */
   int profileAlt = 0; /* default to flat */

   int mode = 0;
   int runSerial = 1;
   int runCl = 1;
   int compareResults = 1;

   int opt;

   while ((opt = getopt(argc,argv,"d:t:r:f:n:p:a:v:m:h")) != -1){
      switch(opt) {
         case 'v':
            verbose = atoi(optarg);
            break;
         case 'd':
            elevDistance = atof(optarg);
            break;
         case 't':
            sourceAlt = atof(optarg);
            break;
         case 'r':
            destAlt = atof(optarg);
            break;
         case 'n':
            numElevElements = atoi(optarg);
            assert(numElevElements <= ARRAYSIZE);
            break;
         case 'm':
            mode = atoi(optarg);
            if(mode > 3 || mode < 0){
               fprintf(stderr,"Invalid mode\n");
               printUsage(); 
               exit(EXIT_FAILURE);
            }
            break;
         case 'f': 
            frq_mhz = atof(optarg);
            break;
         case 'p':
            profile = atoi(optarg);
            if(profile > 3 || profile < 0){
               fprintf(stderr,"Invalid Terain profile\n");
               printUsage();
               exit(EXIT_FAILURE);
            }
            break;
         case 'a':
            profileAlt = atof(optarg);
            break;
         case 'h':
            printUsage();
            printHelp();
            exit(EXIT_SUCCESS);
            break;
         default:
            fprintf(stderr,"Invalid Command Line Option");
            printUsage();
            exit(EXIT_FAILURE);
            break;
      }

   }

   switch (mode) {
      case 3:
         runSerial = 1;
         runCl = 0;
         compareResults = 0;
         break;
      case 2:
         runSerial = 0;
         runCl = 1;
         compareResults = 0;
         break;
      case 1:
         runSerial = 1;
         runCl = 1;
         compareResults = 0;
         break;
      case 0: 
      default:
         runSerial = 1;
         runCl = 1;
         compareResults = 1;
         break;
   }


   double *elev = new double [numElevElements];
   generateTerrain(elev,numElevElements,profile,profileAlt);

   if(verbose){
      printAttrs(numElevElements, elevDistance,sourceAlt, destAlt,eps_dielect,
         sgm_conductivity,
         eno_ns_surfref,frq_mhz,radio_climate,pol,conf,rel);
      if(verbose > 1){
         printElev(elev,numElevElements,profile,profileAlt,elevDistance);
      }
   }


   double *signal = new double [numElevElements]; /* Array to store the results */
   double *signal_serial = new double[numElevElements]; /* Array to store the results (from the non Cl)*/

   for(int i = 0; i < numElevElements; i++){
      signal[i] = 0.0;
      signal_serial[i] = 0.0;
   }

   if(runCl){
      //do the point_to_point in paraell
      allPoints_cl(numElevElements,elevDistance,elev,signal,sourceAlt,destAlt,
                   eps_dielect,sgm_conductivity,eno_ns_surfref,frq_mhz,radio_climate,
                   pol,conf,rel);
   }

   if(runSerial){ 
      //do the point_to_point in serial
      allPoints(numElevElements,elevDistance,elev,signal_serial,sourceAlt,destAlt,
                   eps_dielect,sgm_conductivity,eno_ns_surfref,frq_mhz,
                   radio_climate,
                   pol,conf,rel);
   }

   if(compareResults){
      if(resultsPass(signal,signal_serial,numElevElements)){
         printf("Results Pass!\n");
      }
      else{
         printf("Results Fail!\n");
      }
   } 

   if(verbose>1){
      if(runSerial && runCl){
         printResultsBoth(signal,signal_serial,numElevElements,elevDistance);
      } 
      else if (runSerial){
         printResultsSingle(signal_serial,numElevElements,elevDistance);
      }
      else if (runCl){
         printResultsSingle(signal,numElevElements,elevDistance);
      }
      else {
         //This shouldn't happen
         assert(false);
      }
   }


   delete [] elev;
   delete [] signal;
   delete [] signal_serial;
}
