//size will be upto this amounts of threads
__kernel void addition_magnitude(const int size,
                      __global int* v1,__global int* v2,__global int* v3) {
    
    // Thread identifiers
    const int globalIndex = get_global_id(0);   
 
    //uncomment to see the index each PE works on
    //printf("Kernel process index :(%d)\n ", globalIndex);

    v3[globalIndex] = v1[globalIndex] + v2[globalIndex];
}
