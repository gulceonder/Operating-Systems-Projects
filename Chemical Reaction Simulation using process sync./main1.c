#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
//function to create random int for the amount of sleep time with expo function
//source: https://stackoverflow.com/questions/34558230/generating-random-numbers-of-exponential-distribution
double ran_expo(double lambda){
    double u;
    u =  rand() / ((double) RAND_MAX);
    return -log(1- u) / lambda;
}

int C_number = 0;
int H_number = 0;
int N_number = 0;
int O_number= 0;

int mol_Created = 0;
pthread_t tid[2];
pthread_mutex_t lock;

//for atom list atoms are represented by numbers:  
//0=empty, 2=C, 3=H, 5=N, 7=0 and are called as atomNUM
int atomID; //initialize atom_id as 0
struct atom {
int atomID;
char atomTYPE; // C, H, O or N
int atomNUM;
};


struct tube {
int tubeID;
int tubeTS; // time stamp (ID of the atom spilled first)
int moleculeTYPE; // 1:H2O, 2:CO2, 3: NO2, 4: NH3
int atom_list[4];

};

struct Information {
int tubeID;
struct tube mytube ;
}; 

struct Information Info;

//Declare tube struct as global pointer to acces and change inside any function
struct tube *tube_1 ;
struct tube *tube_2 ;
struct tube *tube_3 ;


void update_info(struct tube *tube_no){
    Info.tubeID= tube_no->tubeID;
    Info.mytube.moleculeTYPE = tube_no->moleculeTYPE;
}

 
//returns the multiplication that is molecule number
//multiply each number in the atom_list array and compare it to the values for each molecule
// molecule values are Co2: 98, H20: 63, NO2:245, NH3:135 (obtained by the multiplication of each atom number)
int check(struct tube* tube_no){
    //Critical section use mutex lock
    pthread_mutex_lock(&lock);
    int molnum=1;
 for (int i = 0; i < 4; ++i) {
     if(tube_no->atom_list[i]){
       molnum= molnum* tube_no->atom_list[i];}
    }
    switch (molnum)
    {
    case 98:

        tube_no->moleculeTYPE= 98;
        pthread_mutex_unlock(&lock);
        return 0;
        
    case 63:

        tube_no->moleculeTYPE= 63;
        pthread_mutex_unlock(&lock);
        return 0;
        break;
    case 245:

        tube_no->moleculeTYPE= 245;
        pthread_mutex_unlock(&lock);
        return 0;
    case 135:

        tube_no->moleculeTYPE= 135;
        pthread_mutex_unlock(&lock);
        return 0;

    default:
    pthread_mutex_unlock(&lock);
    return molnum;
    
   
    }
    
}

//functions to insert atom to a tube
void *insert_tube_1(void *input){
    int i;
    //if empty  and write to tubeTS
   if(tube_1->atom_list[0]==0){
       tube_1->tubeTS = ((struct atom*)input)->atomID;
   } 

    //insert the atom number max 4 atoms can for a molecule
   for(i=0 ; i < 4; i++){
       if(tube_1->atom_list[i]==0){
           tube_1->atom_list[i]= ((struct atom*)input)->atomNUM;
            break;
       }
   }
//if molecule formed set atomlist to empty
        if(!check(tube_1)){
        for(int i = 0; i < 4; ++i){   
        tube_1->atom_list[i]=0;};
        update_info(tube_1);
        mol_Created=1;}

};


void *insert_tube_2(void *input){
    int i;
    //if empty  and write to tubeTS
   if(tube_2->atom_list[0]==0){
       tube_2->tubeTS = ((struct atom*)input)->atomID;
   } 

    //insert the atom number
   for(i=0 ; i < 4; i++){
       if(tube_2->atom_list[i]==0){
           tube_2->atom_list[i]= ((struct atom*)input)->atomNUM;
            break;
       }
   }

        if(!check(tube_2)){
        for(int i = 0; i < 4; ++i){   
        tube_2->atom_list[i]=0;};
        update_info(tube_2);
        mol_Created=1;}

};

void *insert_tube_3(void *input){
    int i;
    //if empty  and write to tubeTS
   if(tube_3->atom_list[0]==0){
       tube_3->tubeTS = ((struct atom*)input)->atomID;
   } 

    //insert the atom number
   for(i=0 ; i < 4; i++){
       if(tube_3->atom_list[i]==0){
           tube_3->atom_list[i]= ((struct atom*)input)->atomNUM;
            break;
       }
   }

        if(!check(tube_3)){
        for(int i = 0; i < 4; ++i){   
        tube_3->atom_list[i]=0;};
        update_info(tube_3);
        mol_Created=1;}

};

//main thread
void  *generator(void *input){
    int i;

   int *args = (int *)input;
   int times;
   times= args[0]+args[1]+args[2]+args[3]; //number of times a new atom will be created randomly
    int error;

 for(i=0 ; i < times; i ++){   
  sleep(ran_expo(args[4])); // make it random

  int atom_no= (rand());
  atom_no = atom_no % 4;

  switch(atom_no){
    case 0:
    if(C_number<args[0]){//if Cnumber is achieved do not create and decrease i by one to create another atom instead.
     { struct atom *Carbon = (struct atom *)malloc(sizeof(struct atom));

      Carbon->atomID = ++atomID;
      Carbon->atomTYPE = 'C';
      Carbon->atomNUM = 2;
      C_number+=1;
      pthread_t tid;
      printf("C with ID:%d is created.\n",atomID);
      //check tube conditions from 1 to 3 and insert to available one if none that waste
      if(((check(tube_1)== 1)||(check(tube_1)== 7)||(check(tube_1)== 49))){
      error = pthread_create(&(tid), NULL,&insert_tube_1, (void *)Carbon);
      if (error != 0){
        printf("\nThread can't be created :[%s]",strerror(error));}}
        else if ((check(tube_2)== 1)||(check(tube_2)== 7)||(check(tube_2)== 49))
        {
            error = pthread_create(&(tid), NULL,&insert_tube_2, (void *)Carbon);
        if (error != 0){
        printf("\nThread can't be created :[%s]",strerror(error));}}
        else if ((check(tube_3)== 1)||(check(tube_3)== 7)||(check(tube_3)== 49))
        {
            error = pthread_create(&(tid), NULL,&insert_tube_3, (void *)Carbon);
        if (error != 0){
        printf("\nThread can't be created :[%s]",strerror(error));}}
        
        else {printf("C with ID:%d is wasted.\n",atomID);}
        
        pthread_join(tid, NULL);
       
      }}else --i;


      break;

    case 1:
    if(H_number< args[1]){
    { struct atom *Hydrogen = (struct atom *)malloc(sizeof(struct atom));

      Hydrogen->atomID = ++atomID;
      Hydrogen->atomTYPE = 'H';
      Hydrogen->atomNUM = 3;
      H_number+=1;
      pthread_t tid;
      printf("H with ID:%d is created.\n",atomID);
      if((check(tube_1)== 1) ||(check(tube_1)== 3)||(check(tube_1)== 5) ||(check(tube_1)== 7)||(check(tube_1)== 15) ||(check(tube_1)== 21) ||(check(tube_1)== 45) ){
      error = pthread_create(&(tid), NULL,&insert_tube_1, (void *)Hydrogen);
      if (error != 0){
        printf("\nThread can't be created :[%s]",strerror(error));}
         } else if ((check(tube_2)== 1) ||(check(tube_2)== 3)||(check(tube_2)== 5) ||(check(tube_2)== 7)||(check(tube_2)== 15) ||(check(tube_2)== 21) ||(check(tube_2)== 45))
        {
            error = pthread_create(&(tid), NULL,&insert_tube_2, (void *)Hydrogen);
        if (error != 0){
        printf("\nThread can't be created :[%s]",strerror(error));}} 
        else if ((check(tube_3)== 1) ||(check(tube_3)== 3)||(check(tube_3)== 5) ||(check(tube_3)== 7)||(check(tube_3)== 15) ||(check(tube_3)== 21) ||(check(tube_3)== 45))
        {
            error = pthread_create(&(tid), NULL,&insert_tube_3, (void *)Hydrogen);
        if (error != 0){
        printf("\nThread can't be created :[%s]",strerror(error));}}else {printf("H with ID:%d is wasted.\n",atomID);}
        pthread_join(tid, NULL);
}}else --i;

      

      break;

    case 2:
    if(O_number<args[2]){
    { struct atom *Oxygen = (struct atom *)malloc(sizeof(struct atom));

      Oxygen->atomID = ++atomID;
      Oxygen->atomTYPE = '0';
      Oxygen->atomNUM = 7;
      O_number+=1;
      pthread_t tid;
       printf("O with ID:%d is created.\n",atomID);
      if((check(tube_1)== 1||(check(tube_1)== 2)||(check(tube_1)== 3)||(check(tube_1)== 5)||(check(tube_1)== 7)||(check(tube_1)== 9)||(check(tube_1)== 14)||(check(tube_1)== 35))){

      error = pthread_create(&(tid), NULL,&insert_tube_1, (void *)Oxygen);

      if (error != 0){
        printf("\nThread can't be created :[%s]",strerror(error));}
        }else if ((check(tube_2)== 1||(check(tube_2)== 2)||(check(tube_2)== 3)||(check(tube_2)== 5)||(check(tube_2)== 7)||(check(tube_2)== 9)||(check(tube_2)== 14)||(check(tube_2)== 35)))
        {
            error = pthread_create(&(tid), NULL,&insert_tube_2, (void *)Oxygen);
        if (error != 0){
        printf("\nThread can't be created :[%s]",strerror(error));}
        }else if ((check(tube_3)== 1||(check(tube_3)== 2)||(check(tube_3)== 3)||(check(tube_3)== 5)||(check(tube_3)== 7)||(check(tube_3)== 9)||(check(tube_3)== 14)||(check(tube_3)== 35)))
        {
            error = pthread_create(&(tid), NULL,&insert_tube_3, (void *)Oxygen);
        if (error != 0){
        printf("\nThread can't be created :[%s]",strerror(error));}}else {printf("O with ID:%d is wasted.\n",atomID);}
        pthread_join(tid, NULL);
}} else --i;
      break;

    case 3:
    if(N_number< args[3]){
    { struct atom *Nitrogen = (struct atom *)malloc(sizeof(struct atom));

      Nitrogen->atomID = ++atomID;
      Nitrogen->atomTYPE = 'N';
      Nitrogen->atomNUM = 5;
      N_number+=1;
      pthread_t tid;
      printf("N with ID:%d is created.\n",atomID);
      if((check(tube_1)== 1||(check(tube_1)== 3)||(check(tube_1)== 9)||(check(tube_1)== 27)||(check(tube_1)== 7)||(check(tube_1)== 49))){

      error = pthread_create(&(tid), NULL,&insert_tube_1, (void *)Nitrogen);
      if (error != 0){
        printf("\nThread can't be created :[%s]",strerror(error));}}else if ((check(tube_2)== 1||(check(tube_2)== 3)||(check(tube_2)== 9)||(check(tube_2)== 27)||(check(tube_2)== 7)||(check(tube_2)== 49)))
        {
            error = pthread_create(&(tid), NULL,&insert_tube_2, (void *)Nitrogen);
        if (error != 0){
        printf("\nThread can't be created :[%s]",strerror(error));}}
        else if ((check(tube_3)== 1||(check(tube_3)== 3)||(check(tube_3)== 9)||(check(tube_3)== 27)||(check(tube_3)== 7)||(check(tube_3)== 49)))
        {
            error = pthread_create(&(tid), NULL,&insert_tube_3, (void *)Nitrogen);
        if (error != 0){
        printf("\nThread can't be created :[%s]",strerror(error));}}
        else {printf("N with ID:%d is wasted.\n",atomID);}
         pthread_join(tid, NULL);

      }}else --i;
      break;

    default:
      break;
}
     if(mol_Created){
         switch (Info.mytube.moleculeTYPE)
         {
         case 98:
             printf("CO2 created  in tube %d \n", Info.tubeID);

             break;
         case 63:
             printf("H2O created  in tube %d \n", Info.tubeID);

             break;
         case 245:
             printf("NO2 created  in tube %d \n", Info.tubeID);

             break;
         case 135:
             printf("NH3 created  in tube %d \n",Info.tubeID);

             break;
         
         default:
             break;
         }
        //printf("smth created %d in tube %d \n",Info.mytube.moleculeTYPE, Info.tubeID);


        mol_Created=0;
    }

    }
}



int main(int argc,char *argv[])
{   //initialize tubes
    tube_1 = (struct tube *)malloc(sizeof(struct tube));
    tube_1->tubeID = 1;
    tube_1->tubeTS = 0;
    tube_1->moleculeTYPE = 0;
    tube_1->atom_list[0]= 0;
    tube_1->atom_list[1]= 0;
    tube_1->atom_list[2]= 0;
    tube_1->atom_list[3]= 0;

    tube_2 = (struct tube *)malloc(sizeof(struct tube));
    tube_2->tubeID = 2;
    tube_2->tubeTS = 0;
    tube_2->moleculeTYPE = 0;
    tube_2->atom_list[0]= 0;
    tube_2->atom_list[1]= 0;
    tube_2->atom_list[2]= 0;
    tube_2->atom_list[3]= 0;

    tube_3 = (struct tube *)malloc(sizeof(struct tube));
    tube_3->tubeID = 3;
    tube_3->tubeTS = 0;
    tube_3->moleculeTYPE = 0;
    tube_3->atom_list[0]= 0;
    tube_3->atom_list[1]= 0;
    tube_3->atom_list[2]= 0;
    tube_3->atom_list[3]= 0;
    int i = 0;


    int num;
    int opt;
    int args[5];
    //read input options and convert from char to
        while((opt = getopt(argc, argv, ":c:h:o:n:g:")) != -1) 
    { 
        switch(opt) 
        { 
 
            case 'c':
                 
                num = atoi( optarg );
                // printf("carbon: %d \n", num );
                args[0]=num;
                break;
            case 'h':
                 
                num = atoi( optarg );
                // printf("hydrogen: %d \n", num );
                args[1]=num;

                break;
            case 'o':
                 
                num = atoi( optarg );
                // printf("oxygen: %d \n", num );
                args[2]=num;
                break;
            case 'n':
                 
                num = atoi( optarg );
                // printf("nitrogen: %d \n", num );
                args[3]=num;
                break;
            case 'g':
                 
                num = atoi( optarg );
                // printf("rate: %d \n", num );
                args[4]=num;
                break;   } 
                
    } 
      
    // optind is for the extra arguments
    // which are not parsed
    for(; optind < argc; optind++){     
        printf("extra arguments: %s\n", argv[optind]); 
    }


    int error;


    if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }
    //main thread
    while (i < 1) {
        error = pthread_create(&(tid[i]),
                               NULL,
                               &generator,(void *)args);
        if (error != 0)
            printf("\nThread can't be created :[%s]",
                   strerror(error));
        i++;
    }
    //wait for main thread to finish
    pthread_join(tid[0], NULL);
    
    pthread_mutex_destroy(&lock);

    return 0;
}
