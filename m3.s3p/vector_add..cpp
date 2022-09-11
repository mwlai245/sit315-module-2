#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>

#define PRINT 1

int SZ = 8;
int *v1, *v2, *v3;

//Creates memory object buffer.
cl_mem bufV1;
//create for other vectors 
cl_mem bufV2;
cl_mem bufV3;

//Defining the devices that will be used
cl_device_id device_id;
//Creates an Open CL context, this allows for 
cl_context context;
//Create a program object
cl_program program;
//Creates Kernel objects for all kernal functions in the program, this connects the functions to the devices
cl_kernel kernel;
//Defines a Que that will be resoncible for scheduling tasks to be executed by devices 
cl_command_queue queue;
//Creates an id that can be used to assign waits to particular instruction
cl_event event = NULL;

int err;

//Create a device 
cl_device_id create_device();
//Custom function that will take our current file and Kernel file for run
void setup_openCL_device_context_queue_kernel(char *filename, char *kernelname);
// Builds the program on the speciefed device, takes into account the context
cl_program build_program(cl_context ctx, cl_device_id dev, const char *filename);
// Sets up kernal memory, for both read and wite shared memor and the Que for the messages sent by the host
void setup_kernel_memory();
// Send the kernal arguments to all devices so they can execute
void copy_kernel_args();
//Delete all memory that has been assigned to devices and any virtual memory allocated for buffers
void free_memory();

void init(int *&A, int size);
void print(int *A, int size);

int main(int argc, char **argv)
{
    //if there is more than 1 arg then it will update the size
    if (argc > 1)
        SZ = atoi(argv[1]);

    init(v1, SZ);
    init(v2, SZ);
    v3 = (int*)malloc(sizeof(int*) * SZ);
    

    //Defines the dimention that will be used, 1 is 1d Array,2 is in 2 dimention(2d array),3 is in3 dimentation(3D Array) 
    size_t global[1] = {(size_t)SZ};

    //initial vector
    print(v1, SZ);
    print(v2, SZ);

    setup_openCL_device_context_queue_kernel((char *)"./vector_add.cl", (char *)"addition_magnitude");

    setup_kernel_memory();
    copy_kernel_args();

    // Queues a command to execute kernel on a device
    // What are its arguments? Check the documenation to find more https://www.khronos.org/registry/OpenCL/sdk/2.2/docs/man/html/clEnqueueNDRangeKernel.html)
    clEnqueueNDRangeKernel(queue, kernel, 1, NULL, global, NULL, 0, NULL, &event);
    // Similar to barrier, used as an interrupt
    clWaitForEvents(1, &event);

    // Queues a command to read from the buffer, (what is the purpose of this function? What are its arguments?)
    clEnqueueReadBuffer(queue, bufV3, CL_TRUE, 0, SZ * sizeof(int), &v3[0], 0, NULL, NULL);

    //Prints result vector
    print(v3, SZ);

    //frees memory for device, kernel, queue, buffer,program and context as well as any others etc.
    //you will need to modify this to free your own buffers
    free_memory();
}

void init(int *&A, int size)
{
    A = (int *)malloc(sizeof(int) * size);

    for (long i = 0; i < size; i++)
    {
        A[i] = rand() % 100; // any number less than 100
    }
}

void print(int *A, int size)
{
    if (PRINT == 0)
    {
        return;
    }

    if (PRINT == 1 && size > 15)
    {
        for (long i = 0; i < 5; i++)
        {                        //rows
            printf("%d ", A[i]); // print the cell value
        }
        printf(" ..... ");
        for (long i = size - 5; i < size; i++)
        {                        //rows
            printf("%d ", A[i]); // print the cell value
        }
    }
    else
    {
        for (long i = 0; i < size; i++)
        {                        //rows
            printf("%d ", A[i]); // print the cell value
        }
    }
    printf("\n----------------------------\n");
}

void free_memory()
{
    //free the buffers
    clReleaseMemObject(bufV1);
    clReleaseMemObject(bufV2);
    clReleaseMemObject(bufV3);
    //free opencl objects
    clReleaseKernel(kernel);
    clReleaseCommandQueue(queue);
    clReleaseProgram(program);
    clReleaseContext(context);

    //may need to 
    free(v1);
    free(v2);
    free(v3);
}


void copy_kernel_args()
{
    // Sends the arguments to the kernal, note 0 ,1 are the position for the arguments in the kernel function
    // we also send the size required as well as a pointer to the position in data
    clSetKernelArg(kernel, 0, sizeof(int), (void *)&SZ);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&bufV1);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&bufV2);
    clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&bufV3);


    if (err < 0)
    {
        perror("Couldn't create a kernel argument");
        printf("error = %d", err);
        exit(1);
    }
}

void setup_kernel_memory()
{

   // The second parameter of the clCreateBuffer is cl_mem_flags flags
   //Creates buffer for read write, so that different nodes can read and write.
   //We send the defined context that was create as well as the flag which can be change for example to CL_MEM_READ_ONLY 
   //if we wanted a read only buffer, we can also send pointer to host data and a error code 
    bufV1 = clCreateBuffer(context, CL_MEM_READ_WRITE, SZ * sizeof(int), NULL, NULL);
    bufV2 = clCreateBuffer(context, CL_MEM_READ_WRITE, SZ * sizeof(int), NULL, NULL);
    bufV3 = clCreateBuffer(context, CL_MEM_READ_WRITE, SZ * sizeof(int), NULL, NULL);

    // Copy matrices to the GPU, by sheduling 
    clEnqueueWriteBuffer(queue, bufV1, CL_TRUE, 0, SZ * sizeof(int), &v1[0], 0, NULL, NULL);
    clEnqueueWriteBuffer(queue, bufV2, CL_TRUE, 0, SZ * sizeof(int), &v2[0], 0, NULL, NULL);
    clEnqueueWriteBuffer(queue, bufV3, CL_TRUE, 0, SZ * sizeof(int), &v3[0], 0, NULL, NULL);

}

void setup_openCL_device_context_queue_kernel(char *filename, char *kernelname)
{
    device_id = create_device();
    cl_int err;

    //Creates context environemnt, for the particular device, as passed by the Device ID
    context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &err);
    if (err < 0)
    {
        perror("Couldn't create a context");
        exit(1);
    }

    program = build_program(context, device_id, filename);

    //Creates a command que for the device
    queue = clCreateCommandQueueWithProperties(context, device_id, 0, &err);
    if (err < 0)
    {
        perror("Couldn't create a command queue");
        exit(1);
    };


    kernel = clCreateKernel(program, kernelname, &err);
    if (err < 0)
    {
        perror("Couldn't create a kernel");
        printf("error =%d", err);
        exit(1);
    };
}

cl_program build_program(cl_context ctx, cl_device_id dev, const char *filename)
{

    cl_program program;
    FILE *program_handle;
    char *program_buffer, *program_log;
    size_t program_size, log_size;

    /* Read program file and place content into buffer */
    program_handle = fopen(filename, "r");
    if (program_handle == NULL)
    {
        perror("Couldn't find the program file");
        exit(1);
    }
    fseek(program_handle, 0, SEEK_END);
    program_size = ftell(program_handle);
    rewind(program_handle);
    program_buffer = (char *)malloc(program_size + 1);
    program_buffer[program_size] = '\0';
    fread(program_buffer, sizeof(char), program_size, program_handle);
    fclose(program_handle);

    //Creates a program object a context and loads this with the required source data, we provide it wil the context created, 
    //the count and pointer to the program buffer and size 
    program = clCreateProgramWithSource(ctx, 1,
                                        (const char **)&program_buffer, &program_size, &err);
    if (err < 0)
    {
        perror("Couldn't create the program");
        exit(1);
    }
    free(program_buffer);

    /* Build program 

   The fourth parameter accepts options that configure the compilation. 
   These are similar to the flags used by gcc. For example, you can 
   define a macro with the option -DMACRO=VALUE and turn off optimization 
   with -cl-opt-disable.
   */
    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (err < 0)
    {

        /* Find size of log and print to std output */
        clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG,
                              0, NULL, &log_size);
        program_log = (char *)malloc(log_size + 1);
        program_log[log_size] = '\0';
        clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG,
                              log_size + 1, program_log, NULL);
        printf("%s\n", program_log);
        free(program_log);
        exit(1);
    }

    return program;
}

cl_device_id create_device() {

   cl_platform_id platform;
   cl_device_id dev;
   int err;

   /* Identify a platform */
   err = clGetPlatformIDs(1, &platform, NULL);
   if(err < 0) {
      perror("Couldn't identify a platform");
      exit(1);
   } 

   // Access a device
   // GPU
   err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &dev, NULL);
   if(err == CL_DEVICE_NOT_FOUND) {
      // CPU
      printf("GPU not found\n");
      err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &dev, NULL);
   }
   if(err < 0) {
      perror("Couldn't access any devices");
      exit(1);   
   }

   return dev;
}