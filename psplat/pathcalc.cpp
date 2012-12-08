/**
 * Pathloss Calculator. Takes a hardcoded path array and feeds it into 
 * the OpenCL ITWOM calculations. The goal here is to test the OpenCL 
 * ITWOM code. 
 *
 * @author Andrew J. Musselman 
 */

#include "clutil.h"
#include "splat.h"

const int ELEVSIZE 50;
const int ELEVDIST 10000; /*Path Distance in meters, 10km */ 
/* This means a point every 200 meters, which may or may not be valid 
 * under the ITWOM. That doesn't matter since I'm using canned data anyways */

int main(int argc, char* argv[]){
   int numElevElements = ELEVSIZE;
   double elevDistance = ELEVDIST;
   double elev[ELEVSIZE];
   double itm_elev[ELEVSIZE+2];/* Extra two for legnth and number of elements */
   double signal[ELEVSIZE]; /* Array to store the results */
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

   /* Create a very even slope */
   for(int i = 0; i < ELEVSIZE; i++){
      elev[i]=i;
   }

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

   //The size of the array
   cl_mem elevSizeBuffer = clCreateBuffer(context, 
      CL_MEM_READ_ONLY |CL_MEM_COPY_HOST_PTR, 
      sizeof(int),
      &numElevElements,
      &err);
   if(err < 0) {
      perror("Couldn't create a buffer");
      exit(1);   
   };

   //Array to represent the elevation array
   cl_mem elevBuffer = clCreateBuffer(context, 
      CL_MEM_READ_ONLY |CL_MEM_COPY_HOST_PTR, 
      sizeof(double*ELEVSIZE),
      &elev,
      &err);
   if(err < 0) {
      perror("Couldn't create a buffer");
      exit(1);   
   };

   //The overall distance of the path
   cl_mem pathDistBuffer = clCreateBuffer(context, 
      CL_MEM_READ_ONLY |CL_MEM_COPY_HOST_PTR, 
      sizeof(double),
      &elevDistance,
      &err);
   if(err < 0) {
      perror("Couldn't create a buffer");
      exit(1);   
   };

   //Source transmitter altitude
   cl_mem sourceAltBuffer = clCreateBuffer(context, 
      CL_MEM_READ_ONLY |CL_MEM_COPY_HOST_PTR, 
      sizeof(double),
      &source,
      &err);
   if(err < 0) {
      perror("Couldn't create a buffer");
      exit(1);   
   };

   //Detination receiver altitude
   cl_mem destAltBuffer = clCreateBuffer(context, 
      CL_MEM_READ_ONLY |CL_MEM_COPY_HOST_PTR, 
      sizeof(double),
      &destAlt,
      &err);
   if(err < 0) {
      perror("Couldn't create a buffer");
      exit(1);   
   };

   //EPS_dielect ... Some sort of diaeletric constant
   cl_mem epsBuffer = clCreateBuffer(context, 
      CL_MEM_READ_ONLY |CL_MEM_COPY_HOST_PTR, 
      sizeof(double),
      &eps_dielect,
      &err);
   if(err < 0) {
      perror("Couldn't create a buffer");
      exit(1);   
   };

   //Ground Conductvity 
   cl_mem sgmBuffer = clCreateBuffer(context, 
      CL_MEM_READ_ONLY |CL_MEM_COPY_HOST_PTR, 
      sizeof(double),
      &sgm_conductivity,
      &err);
   if(err < 0) {
      perror("Couldn't create a buffer");
      exit(1);   
   };

   //Surface Reflectivity 
   cl_mem surfrefBuffer = clCreateBuffer(context, 
      CL_MEM_READ_ONLY |CL_MEM_COPY_HOST_PTR, 
      sizeof(double),
      &eno_ns_surfref,
      &err);
   if(err < 0) {
      perror("Couldn't create a buffer");
      exit(1);   
   };

   //Frequency (Mhz) 
   cl_mem frqBuffer = clCreateBuffer(context, 
      CL_MEM_READ_ONLY |CL_MEM_COPY_HOST_PTR, 
      sizeof(double),
      &frq_mhz,
      &err);
   if(err < 0) {
      perror("Couldn't create a buffer");
      exit(1);   
   };

   //Frequency (Mhz) 
   cl_mem frqBuffer = clCreateBuffer(context, 
      CL_MEM_READ_ONLY |CL_MEM_COPY_HOST_PTR, 
      sizeof(double),
      &frq_mhz,
      &err);
   if(err < 0) {
      perror("Couldn't create a buffer");
      exit(1);   
   };

   //Radio Climate 
   cl_mem climateBuffer = clCreateBuffer(context, 
      CL_MEM_READ_ONLY |CL_MEM_COPY_HOST_PTR, 
      sizeof(int),
      &radio_climate,
      &err);
   if(err < 0) {
      perror("Couldn't create a buffer");
      exit(1);   
   };

   //Polarization 
   cl_mem polBuffer = clCreateBuffer(context, 
      CL_MEM_READ_ONLY |CL_MEM_COPY_HOST_PTR, 
      sizeof(int),
      &pol,
      &err);
   if(err < 0) {
      perror("Couldn't create a buffer");
      exit(1);   
   };

   //conf (?) 
   cl_mem confBuffer = clCreateBuffer(context, 
      CL_MEM_READ_ONLY |CL_MEM_COPY_HOST_PTR, 
      sizeof(double),
      &conf,
      &err);
   if(err < 0) {
      perror("Couldn't create a buffer");
      exit(1);   
   };

   //rel (?) 
   cl_mem relBuffer = clCreateBuffer(context, 
      CL_MEM_READ_ONLY |CL_MEM_COPY_HOST_PTR, 
      sizeof(double),
      &rel,
      &err);
   if(err < 0) {
      perror("Couldn't create a buffer");
      exit(1);   
   };
   /* Create a command queue */
   queue = clCreateCommandQueue(context, devices[0][0], 0, &err);
   if(err < 0) {
      perror("Couldn't create a command queue");
      exit(1);   
   };
   
   /* create kernel */
   kernel = clCreateKernel(program, "PlotLRPaths_cl", &err);
   if(err < 0) {
      perror("Couldn't create a kernel");
      exit(1);
   }

   /* Create Kernel arguments */
   err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &sourceBuffer);
   if(err < 0) {
      fprintf(stderr,"Couldn't create a kernel argument:Source Code:%d",err);
      exit(1);
   }

   /* Enqueue kernel */
   err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &siteArrayCount, 
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
         demBuffer, //CL buffer
         CL_TRUE, //Blocking read
         0, //Offset
         sizeof(struct dem)*MAXPAGES, //size to read
         dem, 
         0, 
         NULL, 
         NULL);
   if(err < 0) {
      fprintf(stderr,"Error code %d",err);
      perror("Couldn't read the buffer");
      exit(1);
   }
  
}
