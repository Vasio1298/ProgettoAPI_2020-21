#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <stdbool.h>

#define MAX_ROW 250
#define MAX_COMMAND 30
enum commands {CHANGE=1, DELETE=2, PRINT=3, UNDO=4, REDO=5, QUIT=6};

//DICHIARAZIONE STRUTTURE
typedef struct ArrayDinText ArrayDinText;
typedef struct ArrayDinComm ArrayDinComm;
typedef struct CommandNode CommandNode;

struct CommandNode {
  char type[3];
  int indices[4];
  char **rows;
};

struct ArrayDinComm {
  CommandNode *comms;
  int dim;
};

struct ArrayDinText {
  char **rows;
  int dim;
};

//FUNZIONI DI INIZIALIZZAZIONE/MODIFICA STRUTTURE
ArrayDinText init_ADT() {
  ArrayDinText temp;
  temp.rows = malloc(sizeof (char*));
  temp.dim = 0;
  return temp;
};

ArrayDinText changeDim_ADT(ArrayDinText x,int num,bool inc) {
  if (inc) {
    x.rows = realloc(x.rows,(x.dim + num)*sizeof(char *));
    x.dim = x.dim + num;
  }
  else {
   //x.rows = realloc(x.rows,(x.dim - num)*sizeof(char *));
   x.dim = x.dim - num;
 }
 return x;
};

ArrayDinComm init_ADC() {
  ArrayDinComm temp;
  temp.comms = malloc(sizeof (CommandNode));
  strcpy(temp.comms[0].type,"NN");
  temp.comms[0].indices[0] = -1;
  temp.comms[0].indices[1] = -1;
  temp.comms[0].indices[2] = -1;
  temp.comms[0].indices[3] = -1;
  temp.comms[0].rows = NULL;
  temp.dim = 0;

  return temp;
};

ArrayDinComm changeDim_ADC(ArrayDinComm x,int num) {
  x.comms = realloc(x.comms,(x.dim + num + 1)*sizeof(CommandNode));
  x.dim = x.dim + num;
  return x;
};

CommandNode init_node(int num) {
  CommandNode temp;
  if (num > 0) {
    temp.rows = malloc(num * sizeof(char*));
    for(int i=0;i<num;i++)
      temp.rows[i] = malloc(MAX_ROW * sizeof(char));
  }
  else temp.rows = NULL;
  return temp;
};

CommandNode cleaning_node(CommandNode x) {
  for(int j=0;j<x.indices[1];j++) {
   x.rows[j] = NULL;
   //free(x.rows[j]);
  }
  x.rows = NULL;
  //free(x.rows);
  return x;
}

//DICHIARAZIONE VARIABILI
ArrayDinText Curr,Old;
ArrayDinComm Commands;
int offset,current;
int ind1,ind2,ind3,num;
char *command;

//DICHIARAZIONE FUNZIONI
bool processing_input();
int processing_command(char *command);

//FUNZIONI COMANDI BASE
void change_command();
void delete_command();
void print_command();
void undo_command();
void redo_command();

//FUNZIONI EFFETTIVE DEI COMANDI
void C1_change(CommandNode x);
void C2_change(CommandNode x);
void C3_change(CommandNode x);
void D2_delete(CommandNode x);
void D3_delete(CommandNode x);
void C1_undo(CommandNode x);
void C2_undo(CommandNode x);
void C3_undo(CommandNode x);
void D2_undo(CommandNode x);
void D3_undo(CommandNode x);

void update_text(bool changeDelete);
void actual_undo(CommandNode x);
void actual_redo(CommandNode x);

int main() {

  Curr = init_ADT();
  Old = init_ADT();
  Commands = init_ADC();

  command = malloc(MAX_COMMAND*sizeof(char));
  current = 0;
  offset = 0;

  bool end = false;

  do {
    end = processing_input();
  } while(!end);

  return 0;
}

bool processing_input() {
    fgets(command,MAX_COMMAND,stdin);

    if (strcmp(command,".\n\0")==0)
        return false;

    int command_type = processing_command(command);

    switch (command_type) {
        case CHANGE:
            change_command();
            return false;
        case DELETE:
            delete_command();
            return false;
        case PRINT:
            print_command();
            return false;
        case UNDO:
            undo_command();
            return false;
        case REDO:
            redo_command();
            return false;
        default:
            return true;
    }
}

int processing_command(char* command) {

    char command_char = command[strlen(command)-2];

    switch(command_char) {
        case 'c':
            ind1 = atoi(strtok(command,","));
            ind2 = atoi(strtok(NULL,"\n"));
            return CHANGE;
        case 'd':
            ind1 = atoi(strtok(command,","));
            ind2 = atoi(strtok(NULL,"\n"));
            return DELETE;
        case 'p':
            ind1 = atoi(strtok(command,","));
            ind2 = atoi(strtok(NULL,"\n"));
            return PRINT;
        case 'u':
            num = atoi(command);
            return UNDO;
        case 'r':
            num = atoi(command);
            return REDO;
        default:
            return QUIT;
    }
}

void change_command() {

  update_text(true);

  CommandNode new = init_node(ind2-ind1+1);

  for(int i=0;i<ind2-ind1+1;i++) {
    fgets(new.rows[i],MAX_ROW,stdin);
    //new.rows[i] = realloc(new.rows[i],strlen(new.rows[i])+1);
  }
  new.indices[1] = ind2-ind1+1;
  new.indices[3] = Old.dim;

  if (ind1>Curr.dim) {
    strcpy(new.type,"C1");
    new.indices[0] = Curr.dim;
    new.indices[2] = 0;
    C1_change(new);
  }
  else {
    new.indices[0] = ind1-1;
    if (ind2<=Curr.dim) {
      strcpy(new.type,"C2");
      new.indices[2] = new.indices[1];
      C2_change(new);
    }
    else {
      strcpy(new.type,"C3");
      new.indices[2] = Curr.dim-ind1+1;
      C3_change(new);
    }
  }
  Commands = changeDim_ADC(Commands,1);
  Commands.comms[Commands.dim] = new;
  current = Commands.dim;
}

void C1_change(CommandNode x) {
  int x1 = x.indices[0];
  int x2 = x.indices[1];
  Curr = changeDim_ADT(Curr,x2,true);     //STEP A
  for (int i=0;i<x2;i++)                  //STEP B
    Curr.rows[x1+i] = x.rows[i];          //STEP B
}

void C2_change(CommandNode x) {
  int x1 = x.indices[0];
  int x3 = x.indices[2];
  int x4 = x.indices[3];
  Old = changeDim_ADT(Old,x3,true);            //STEP A
  for (int i=0;i<x3;i++) {                     //STEP B
    Old.rows[x4+i] = Curr.rows[x1+i];          //STEP B
    Curr.rows[x1+i] = x.rows[i];               //STEP B
  }
}

void C3_change(CommandNode x) {
  int x1 = x.indices[0];
  int x2 = x.indices[1];
  int x3 = x.indices[2];
  int x4 = x.indices[3];
  Curr = changeDim_ADT(Curr,x2-x3,true);      //STEP A
  Old = changeDim_ADT(Old,x3,true);           //STEP A
  for (int i=0;i<x2;i++) {                    //STEP B
    if (i < x3)                               //STEP B
      Old.rows[x4+i] = Curr.rows[x1+i];       //STEP B
    Curr.rows[x1+i] = x.rows[i];              //STEP B
  }
}

void delete_command() {

  update_text(true);

  CommandNode new = init_node(0);
  new.indices[1] = 0;
  new.indices[3] = Old.dim;

  if (ind1>Curr.dim || ind2==0) {
    strcpy(new.type,"D1");
    new.indices[0] = 0;
    new.indices[2] = 0;
  }
  else {
    if (ind1==0)
      ind1=1;
    new.indices[0] = ind1-1;
    if (ind2>=Curr.dim) {
      strcpy(new.type,"D2");
      new.indices[2] = Curr.dim-ind1+1;
      D2_delete(new);
    }
    else {
      strcpy(new.type,"D3");
      new.indices[2] = ind2-ind1+1;
      D3_delete(new);
    }
  }
  Commands = changeDim_ADC(Commands,1);
  Commands.comms[Commands.dim] = new;
  current = Commands.dim;
}

void D2_delete(CommandNode x) {
  int x1 = x.indices[0];
  int x3 = x.indices[2];
  int x4 = x.indices[3];
  Old = changeDim_ADT(Old,x3,true);                  //STEP A

  for (int i=0;i<x3;i++) {                           //STEP B
    Old.rows[x4+i] = Curr.rows[x1+i];                //STEP B
    Curr.rows[x1+i] = NULL;                          //STEP B
  }
  Curr = changeDim_ADT(Curr,x3,false);               //STEP C
}

void D3_delete(CommandNode x) {
  int x1 = x.indices[0];
  int x3 = x.indices[2];
  int x4 = x.indices[3];
  Old = changeDim_ADT(Old,x3,true);                  //STEP A

  for (int i=0;i<x3;i++) {                           //STEP B
    Old.rows[x4+i] = Curr.rows[x1+i];                //STEP B
    Curr.rows[x1+i] = NULL;                          //STEP B
  }
  for (int i=0;i<Curr.dim-x3-x1;i++) {               //STEP C
    Curr.rows[x1+i] = Curr.rows[x1+x3+i];            //STEP C
    Curr.rows[x1+x3+i] = NULL;                       //STEP C
  }
  Curr = changeDim_ADT(Curr,x3,false);               //STEP D
}

void print_command() {
  update_text(false);

  for(int i=ind1;i<=ind2;i++) {
    if(i==0 || i>Curr.dim)
      fputs(".\n",stdout);
    else fputs(Curr.rows[i-1],stdout);
  }
}

void undo_command() {
  if (num >= current + offset)                //CASO 1
    offset = -current;
  else offset = offset - num;                 //CASO 2
}

void redo_command() {
  if (num >= Commands.dim - current - offset)  //CASO 3
    offset = Commands.dim - current;
  else offset = offset + num;                  //CASO 4
}

void update_text(bool changeDelete) {
  int oldCurr = current;                          //STEP A
  current = current + offset;                     //STEP B
  if (offset != 0) {                              //STEP C
    if (current==0) {
      Old = changeDim_ADT(Old,Old.dim,false);     //STEP C
      Curr = changeDim_ADT(Curr,Curr.dim,false);  //STEP C
     } 	
    else if (oldCurr > current)                   //STEP C
      for (int i=0;i<oldCurr-current;i++)         //STEP C
        actual_undo(Commands.comms[oldCurr-i]);   //STEP C
    else                                          //STEP C
      for (int i=0;i<current-oldCurr;i++)         //STEP C
        actual_redo(Commands.comms[oldCurr+1+i]); //STEP C
  }
  offset = 0;                                     //STEP D
  int size = Commands.dim;                        //STEP D
  if (changeDelete && size>current)               //STEP E
    if (current == 0) {                           //STEP E
      for (int i=size;i>=1;i--)		    //STEP E	
       if (Commands.comms[i].type[0]=='C')	    //STEP E	
        Commands.comms[i] = cleaning_node(Commands.comms[i]); //STEP E
      Commands = changeDim_ADC(Commands,-size);   //STEP E
    }
    else {                                        //STEP E
      for (int k=size;k>current;k--)		    //STEP E	
        if (Commands.comms[k].type[0]=='C')       //STEP E
         Commands.comms[k] = cleaning_node(Commands.comms[k]); //STEP E
      Commands = changeDim_ADC(Commands,-size+current); //STEP E
    }
}

void actual_undo(CommandNode x) {
  if (strcmp(x.type,"C1")==0)
      C1_undo(x);
  else if (strcmp(x.type,"C2")==0)
      C2_undo(x);
  else if (strcmp(x.type,"C3")==0)
      C3_undo(x);
  else if (strcmp(x.type,"D2")==0)
      D2_undo(x);
  else if (strcmp(x.type,"D3")==0)
      D3_undo(x);
}

void actual_redo(CommandNode x) {
  if (strcmp(x.type,"C1")==0)
      C1_change(x);
  else if (strcmp(x.type,"C2")==0)
      C2_change(x);
  else if (strcmp(x.type,"C3")==0)
      C3_change(x);
  else if (strcmp(x.type,"D2")==0)
      D2_delete(x);
  else if (strcmp(x.type,"D3")==0)
      D3_delete(x);
}

void C1_undo(CommandNode x) {
  int x1 = x.indices[0];
  int x2 = x.indices[1];

  for (int i=0;i<x2;i++)                                  //STEP A
    Curr.rows[x1+i] = NULL;                               //STEP A
  Curr = changeDim_ADT(Curr,x2,false);                    //STEP B
}

void C2_undo(CommandNode x) {
  int x1 = x.indices[0];
  int x2 = x.indices[1];
  int x4 = x.indices[3];

  for (int i=0;i<x2;i++) {                //STEP A
    Curr.rows[x1+i] = Old.rows[x4+i];     //STEP A
    Old.rows[x4+i] = NULL;                //STEP A
  }
  Old = changeDim_ADT(Old,x2,false);      //STEP B
}

void C3_undo(CommandNode x) {
  int x1 = x.indices[0];
  int x2 = x.indices[1];
  int x3 = x.indices[2];
  int x4 = x.indices[3];

  for (int i=0;i<x2;i++) {                                 //STEP A
    if (i<x3) {                                            //STEP A
      Curr.rows[x1+i] = Old.rows[x4+i];                    //STEP A
      Old.rows[x4+i] = NULL;                               //STEP A
    }                                                      //STEP A
    else Curr.rows[x1+i] = NULL;                           //STEP A
  }
  Old = changeDim_ADT(Old,x3,false);                       //STEP B
  Curr = changeDim_ADT(Curr,x2-x3,false);                  //STEP B
}

void D2_undo(CommandNode x) {
  int x3 = x.indices[2];
  int x4 = x.indices[3];
  Curr = changeDim_ADT(Curr,x3,true);                      //STEP A

  for (int i=0;i<x3;i++) {                                 //STEP B
    Curr.rows[Curr.dim-1-i] = Old.rows[x4+x3-1-i];         //STEP B
    Old.rows[x4+x3-1-i] = NULL;                            //STEP B
  }

  Old = changeDim_ADT(Old,x3,false);                       //STEP C
}

void D3_undo(CommandNode x) {
  int x1 = x.indices[0];
  int x3 = x.indices[2];
  int x4 = x.indices[3];
  Curr = changeDim_ADT(Curr,x3,true);                     //STEP A
  int size = Curr.dim;                                    //STEP A

  for (int i=0;i<size-x1-x3;i++)                          //STEP B
    Curr.rows[size-1-i] = Curr.rows[size-x3-1-i];         //STEP B

  for (int i=0;i<x3;i++) {                                //STEP C
    Curr.rows[x1+i] = Old.rows[x4+i];                     //STEP C
    Old.rows[x4+i] = NULL;                                //STEP C
  }

  Old = changeDim_ADT(Old,x3,false);                      //STEP D
}
