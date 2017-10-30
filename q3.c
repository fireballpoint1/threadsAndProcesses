#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
 
void selectionSort(int arr[], int n);
void swap(int *xp, int *yp);
void merge(int a[], int l1, int h1, int h2);
 
void mergeSort(int a[], int l, int h)
{
    int i, len=(h-l+1);
 
    // Using selection sort for small sized array
    if (len<=5)
    {
        selectionSort(a+l, len);
        return;
    }
 
    pid_t pid_left,pid_right;
    pid_left = fork();
    if (pid_left<0)
    {
        // Lchild proc not created
        perror("Left Child process not created\n");
        _exit(-1);
    }
    else if (pid_left==0)
    {
        mergeSort(a,l,l+len/2-1);
        _exit(0);
    }
    else
    {
        pid_right = fork();
        if (pid_right<0)
        {
            // Rchild proc not created
            perror("Right Child process not created\n");
            _exit(-1);
        }
        else if(pid_right==0)
        {
            mergeSort(a,l+len/2,h);
            _exit(0);
        }
    }
 
    int status;
 
    // Wait for child processes to finish
    waitpid(pid_left, &status, 0);
    waitpid(pid_right, &status, 0);
 
    // Merge the sorted subarrays
    merge(a, l, l+len/2-1, h);
}

void swap(int *xp, int *yp)
{
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}
 
void selectionSort(int array[], int n)
{
    int i, j, min_idx;
 
    // One by one move boundary of unsorted subarray
    for (i = 0; i < n-1; i++)
    {
        // Find the minimum element in unsorted array
        min_idx = i;
        for (j = i+1; j < n; j++)
          if (array[j] < array[min_idx])
            min_idx = j;
 
        // Swap the found minimum element with the first element
        swap(&array[min_idx], &array[i]);
    }
}
 
void merge(int a[], int l1, int h1, int h2)
{
    // We can directly copy  the sorted elements
    // in the final array, no need for a temporary
    // sorted array.
    int count=h2-l1+1;
    int sorted[count];
    int i=l1, k=h1+1, m=0;
    while (i<=h1 && k<=h2)
    {
        if (a[i]<a[k])
            sorted[m++]=a[i++];
        else if (a[k]<a[i])
            sorted[m++]=a[k++];
        else if (a[i]==a[k])
        {
            sorted[m++]=a[i++];
            sorted[m++]=a[k++];
        }
    }
 
    while (i<=h1)
        sorted[m++]=a[i++];
 
    while (k<=h2)
        sorted[m++]=a[k++];
 
    int arr_count = l1;
    for (i=0; i<count; i++,l1++)
        a[l1] = sorted[i];
}
 
// To check if array is actually sorted or not
void printingFunc(int arr[], int len)
{
 
    printf("\nSorted Array:\n");
    for(int i=0;i<len;i++)
    {
        printf("%d ",arr[i]);
    }
    printf("\n");

    
    return;
}
 
// To fill randome values in array for testing
// purpise
void scan_function(int a[], int len)
{
 
    printf("Enter the elements\n");
    for(int i=0;i<len;i++)
    {
        scanf("%d",&a[i]);
    }

}
 
// Driver code
int main()
{
    int shmid;
    key_t key = IPC_PRIVATE;
    int *array_shared;
 
    int length;
 
    printf("Enter No of elements of Array:");
    scanf("%d",&length); 
 
    // Calculate segment length
    size_t SHM_SIZE = sizeof(int)*length;
 
    // Create the segment.
    if ((shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666)) < 0)
    {
        perror("shmget");
        _exit(1);
    }
 
    // Now we attach the segment to our data space.
    if ((array_shared = shmat(shmid, NULL, 0)) == (int *) -1)
    {
        perror("shmat");
        _exit(1);
    }
 
    //srand(time(NULL));
    scan_function(array_shared, length);
 
    mergeSort(array_shared, 0, length-1);
 
    printingFunc(array_shared, length);
 
    /* Detach from the shared memory now that we are
       done using it. */
    if (shmdt(array_shared) == -1)
    {
        perror("shmdt");
        _exit(1);
    }

    /* Delete the shared memory segment. */
    if (shmctl(shmid, IPC_RMID, NULL) == -1)
    {
        perror("shmctl");
        _exit(1);
    }
    
 
    return 0;
}
