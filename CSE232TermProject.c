#include <stdio.h> 
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define MAXCHAR 40
#define MAXNODE 30

bool edit (char *,int);
bool insert(int, char *);
bool delete(int);
bool print();
bool save();
bool menu();
bool commit();
char toUpperChar(char);
bool deleteEmpty();
void doMethod(int index);
bool insertDiffArrayDelete(int  , int );
bool insertDiffArrayInsert(int  , int  , char* );
void doMethod(int );

struct dfs 
{
	int code;		// 1-insertion 2-deletion 0-otherwise
	int	statno;	
	char statement[40];  // only for insertion
};
struct node
{
	int statno;
	char statement[MAXCHAR];
	struct node* next;
};
typedef struct node node;
typedef struct dfs dfs;
struct dfs diffs[20]; // max. 20 changes
int version=0;
node textbuffer[MAXNODE];      
node *head = &textbuffer[0];
char txt[50]; 
char dif[50];

int main()
{ 
	int i = 1;
	for (i = 1; i <= MAXNODE; i++) // assigns the appropriate numbers at startup to statno and next variables in the textbuffer array
	{
		textbuffer[i - 1].statno = 50;
		textbuffer[i - 1].next = &textbuffer[i];
		textbuffer[i -1].statement[0] = '*';
		textbuffer[i -1].statement[1] = '\n';
	}
	textbuffer[MAXNODE-1].next = NULL;  //sets the last element's .next to null
	i = 0;
	for(i ; i < 20 ; i++)
	{
		diffs[i].code = 0;
	}
	
	menu(); // menu is a seperate function for extended flexibilty
	
    return 0;
}

bool menu()
{
	int stat;
	char input , c;
	char chPtr[50];
	
	do
	{
		printf("Enter;\nE for Edit\nI for Insert\nD for Delete\nP for Print\nS for Save\nX to Quit\nC To commit>> ");
		scanf(" %c", &input);
		while((c = getchar()) != '\n');
		input = toUpperChar(input);	
		
		if(input == 'E') // at edit we get the statement no and statemen. we also empty buffer each time to avoid unneccessary bugs
		{
			int versionCheck = 0;
			printf("Enter file name:\n");
			scanf("%s", chPtr);
			while((c = getchar()) != '\n');
			printf("Enter version check (0 for latest)(1 for startup) :\n");
			scanf("%d",&versionCheck);
			while((c = getchar()) != '\n');
			edit(chPtr,versionCheck);
			

		}
		else if(input == 'I')
		{
			printf("Enter Statement Number\n");
			scanf(" %d", &stat);
			while((c = getchar()) != '\n');
			printf("Enter Statement\n");
			gets(chPtr);
			strcat(chPtr,"\n");
			insertDiffArrayInsert(1,stat,chPtr); // add to the diffs buffer
			insert(stat,chPtr); // and now insert the line to the textbuffer
		}
		else if(input == 'D')
		{
			printf("Enter Statement Number\n");
			scanf(" %d", &stat);
			while((c = getchar()) != '\n');
			insertDiffArrayDelete(2,stat); // same as insert, add the modification to the diffs buffer as well
			delete(stat);
		}
		else if(input == 'P')
		{
			print();
		}
		else if(input == 'S')
		{
			save();
		}
		else if(input == 'C')
		{
			commit();
		}
		else
		{
		 	printf("invalid input try again\n");
		}	
	} while (input != 'X');
	printf("Quitting\n");
}
bool edit(char *filename, int versionCheck)
{
	strcpy(txt, filename); // gets the file name and saves it to global variables with ".txt" and ".dif" extensions for future use
	strcat(txt, ".txt");
	strcpy(dif,filename);
	strcat(dif, ".dif");
	FILE *file, *fileCom;
    int count = 0;
    int nodeCount = 0;
    char str[MAXCHAR];
    struct node* temp = head;
    struct dfs* tempdfs = &diffs[0];
    int incnt = 0;


    file = fopen(txt, "r");
    if (!file) // see if we could open the file properly
    {
        printf("Can't open file\nCreating %s instead\n", filename);
        file = fopen((txt), "w+"); // else create it
        //exit( EXIT_FAILURE );
    }
    fileCom = fopen(dif, "r"); // create a pointer to the dif file
    if (!fileCom)
    {
        fileCom = fopen(dif, "w+");
    }
	while (fgets(str, MAXCHAR, file) != NULL){ // read the whole .txt file line by line
        sscanf(str,"%d",&temp->statno);
        if(temp->statno < 10)
        {
            for(count = 2;count < MAXCHAR;count++)
                temp->statement[count-2] = str[count];

        }
        else
        {	
            for(count = 3;count < MAXCHAR;count++)
                temp->statement[count-3] = str[count];
        }
        temp = temp->next;
    }
    
	while (fgets(str, 20, fileCom) != NULL) // get variables from .dif file in a formatted fashion with sscanf
	{

       sscanf(str, "%d %d", &diffs[incnt].code, &diffs[incnt].statno);
       for(count = 4; count < 43 ; count++)
        {
            diffs[incnt].statement[count-4] = str[count];
        }
        incnt++;
    }	
   if (versionCheck == 0) //  if user wants the latest version
	{
        int i = 1;
        int counter = 1;
        while(diffs[i].code != 0)
        {
            if(diffs[i].code == -1 && diffs[i+1].code == 0 )
            {
                counter++;
                i++;
                break;
            }
            else if(diffs[i].code == -1)
            {
                counter++;
                i = i+2;
            }
            else
            {
                doMethod(i);
                i++;
            }
        }
        diffs[i].code = counter;
        version = counter;
        return true;
    }
	else if(versionCheck != 1) //  if user wants a specific version
	{
		int i = 1;
		int counter = 1;
		while(diffs[i].code != 0)
		{
			if(diffs[i].code == -1)
			{
				counter++;
				if(counter == versionCheck)
				{
					printf("done");
					
					i = i+1;
					diffs[i].code = counter;
					i++;
					for(i ; i < 20 ; i++)
					{
						diffs[i].code = 0;
					}
					version = counter;
					return true;
				}
				i = i+2;
			}
			else
			{
				doMethod(i);
				i++;	
			}
		}
	}
	else if(versionCheck == 1) //  if user wants the startup version
	{
		diffs[0].code = 1;
	}
	version++;

			
	fclose(file); // close the file when we are done with it
	return true;
}
bool insert(int statno, char *stat)
{	
	struct node* prev = head;
	struct node* temp = head;
	struct node* tmpPtrPrev = head;
	int i = 0;
	int j = 0;
	int counter = 1;
	if (statno > 30) // if the line to be inserted is above 30 break since limit is 30
	{
		print("error");
		return false;
	}
	
	if (head->statement[0] == '*') // for the case of first insertion to the list
	{
		for (j = 0 ; j < 40 ; j++)
		{
			head->statement[j] = stat[j]; // set the heads statement to the input from function
		}
		head->statno = statno;
		return true;
	}
	else if (prev->statno >= statno) // when the insertion is added at the start of the list
	{
		if (prev->statno == statno)
		{
			for(j = 0 ; j < 40 ; j++)
			{
				prev->statement[j] = stat[j];	
			}
			prev->statno = statno;
			return true;
		}
		else
		{
			while (temp->next->statement[0] != '*')
			{
				temp = temp->next;
			}
			tmpPtrPrev = temp->next;
			temp->next = temp->next->next;
			tmpPtrPrev->next = prev;
			head = tmpPtrPrev;
			for (j = 0 ; j < 40 ; j++)
			{
				tmpPtrPrev->statement[j] = stat[j];	
			}
			tmpPtrPrev->statno = statno;
			return true;
			
		}
	}
	else // for the case of inserting to the middle and to the end of the list
	{
		while (prev->next->statno <= statno)
		{
			if(prev->next->statno == statno)
			{
				for(j = 0 ; j < 40 ; j++)
				{
					prev->next->statement[j] = stat[j];	
				}
				prev->next->statno = statno;
				return true;
			}
			else
			{
				prev = prev->next;
			}
		}
		while(temp->next->statement[0] != '*')
		{
			temp = temp->next;
		}
		for(j = 0 ; j < 40 ; j++)
		{
			temp->next->statement[j] = stat[j];	
		}
		temp->next->statno = statno;
		tmpPtrPrev = temp->next;
		temp->next = temp->next->next;
		tmpPtrPrev->next = prev->next;
		prev->next = tmpPtrPrev;
		
		return true;	
	}

}
bool delete(int statno)
{
	
	struct node *prev = head;
	struct node *tmp;
	struct node *tmp2 = head;
	int counter = 0;
	if(head->statno == 50) // 50 is our signature to see if the list is empty
	{
		printf("list is empty\n");
		return false;
	}
	else if(head->statno == statno) // if we delete from head
	{
		head = head->next;
		for(int i = 0; i< 29 ; i++)
		{
			tmp2 = tmp2->next;
		}
		tmp2 ->next = prev->next;
		prev ->next = NULL;
		prev->statno == 50;
		prev->statement[0] = '*';
		return true;
	}
	else // normally delete
	{
		while(prev->next->statno != statno)
		{

			prev = prev->next;
			counter++;
			if(counter == 29) //null pointer ex 
			{
				printf("not found\n");
				return false;
			}
		}	
	}
	
	while(prev->next->statno != statno)
	{

		prev = prev->next;
		counter++;
		if(counter == 29) //null pointer ex 
		{
			printf("not found");
			return false;
		}
	}
	tmp = prev->next;
	for(int i = 0; i< 29 ; i++) // we find the last element
	{
		tmp2 = tmp2->next;
	}
	tmp2 ->next = prev->next;
	prev->next = prev->next->next;
	tmp ->next = NULL;
	tmp->statno == 50;
	tmp->statement[0] = '*';
	
}
bool print()
{
    struct node* temp = head;
    int charCount=0;
	printf("printing\n");
	int i=0;
	while(temp->statement[0] != '*') // print the while textbuffer to the console in a linked list fashion
	{
		printf("%d ", temp->statno);
		printf("%s", temp->statement);
		temp = temp->next;
   	}
	return true;
}
bool save()
{
    FILE* filePtr = fopen(dif,"w"); // open the file from global variable we saved before
    fprintf(filePtr, "%d\n", diffs[0].code);
    int i = 1;
    for(i = 0;i<20;i++) // find the first 0 in the diffs buffer and set it to -1 for better formmatting and to avoid bugs
    {
    	if(diffs[i].code == 0)
    	{
    		diffs[i].code = -1;
    		break;
		}
	}
	
    for (i=0; diffs[i].code != 0; i++) // while saving we have 3 cases
    {
        if (diffs[i].code == 1 && i!=0) // format the line if the code is for insert
        {
            fprintf(filePtr,"%d %d %s",diffs[i].code, diffs[i].statno, diffs[i].statement);
        }
        else if (diffs[i].code == 2) // for the delete
        {
            fprintf(filePtr,"%d %d\n",diffs[i].code, diffs[i].statno);
        }
        else if (diffs[i].code == -1 && diffs[i+1].code == 0) // the case for control
        {
        	fprintf(filePtr, "%d\n",diffs[i].code); // 
        	version++;
        	diffs[i+1].code = version;
        	break;
        }
        else if(diffs[i].code == -1) // and if the line is -1 which means end of version print -1 and the version
        {
        	fprintf(filePtr, "%d\n",diffs[i].code);
            i=i+1;
            fprintf(filePtr,"%d\n",diffs[i].code);
		}
        
    }
   
    
    fclose(filePtr);
    printf("Saved\n");
    return true;
}
bool commit()
{
    FILE *filePtr = fopen(txt, "w"); // at commit we print the contents of buffertext to the .txt file
    node* iter = head;
    int counter = 0;
    for (counter ; counter < 29 ; counter++)  // gradually continue whole buffer
    {
        if (iter->statement[0] == '*') // skip if the line is a *
		continue; 
        
        fprintf(filePtr, "%d %s", iter->statno, iter->statement); // then using fprintf print it
        iter = iter->next;
    }
    fclose(filePtr);
    version = 0;
    filePtr = fopen(dif, "w"); // also open .dif file to empty it and write 0 in it as demonstrated in the pdf file
    fprintf(filePtr, "%d", version);
    fclose(filePtr);

    printf("Commit Done\n");
    for(counter = 0; counter < 20 ; counter++)
    {
    	diffs[counter].code = 0;
	}
	return true;
}
char toUpperChar(char ch) // for the input in menu to avoid any mistakes
{
	if (ch >= 'A' && ch <= 'Z')
		return ch;
	else
		return ch - 32;
}
void doMethod(int index) // for making the same changes in diffs buffer
{
	if(diffs[index].code == 1) // if the line is insert
	{
		insert(diffs[index].statno,diffs[index].statement);
	}
	else if(diffs[index].code == 2) // if the line is delete
	{

		delete(diffs[index].statno);
	}
}
bool insertDiffArrayInsert(int code , int statno , char* ptr) // inserting to the diffs buffer
{
	int i=0;
	for(i ; i<20 ; i++ )
	{
		if(diffs[i].code == 0)
		{
			diffs[i].code = code;
			diffs[i].statno = statno;
			strcpy(diffs[i].statement,ptr);
			return 1;
		}
	}
}
bool insertDiffArrayDelete(int code , int statno) // and deleting from the diffs buffers
{
	int i=0;
	for(i ; i<20 ; i++ )
	{
		if(diffs[i].code == 0)
		{
			diffs[i].code = code;
			diffs[i].statno = statno;
			return 1;
		}
	}
}


	

