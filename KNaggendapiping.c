#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//global variables
int programCounter=0;
unsigned int currentInstruction;
unsigned char memory[1000];
int flag=1;
int R[15];
int R15[100];
int i;
int OP1,OP2;
int topOfStack = -1; 
int result; 
int address;
int sizeOfFile=0;
int currentMemoryLocation=0; 

//function prototypes
void fetch();
void decode();
void execute();
void store();
void populatingOPRegisters();        

//checks if the stack is empty
int checkIfEmpty() 
{
	if(topOfStack == -1)
		return 1;
	else
		return 0;
}

//checks if the stack is full
int checkIfFull() 
{
   if(topOfStack == 100)
      return 1;
   else
      return 0;
}

//returns the value at the top of the stack
int returnStack() 
{
	//returns the top of the stack
   return R15[topOfStack];
}

//returns the top of the stack and pops it from stack
int pop() 
{
   int information;
   if(!checkIfEmpty())
   {
      information = R15[topOfStack];

      topOfStack = topOfStack - 1; //updates the top of the stack  
      return information;
   } 
   else
      printf("Error: Stack is Empty!\n");
}

//pushes information onto the stack
int push(int information) 
{

	if(!checkIfFull())
	{
		topOfStack =topOfStack + 1;
		R15[topOfStack]=information; //adds information to stack
	}
	else
		printf("Error: Stack is Full!!\n");
}

void checkInstruction(int currentInstruction, char* CPUoperation)
{
	//ADD
	if(currentInstruction==1)
	{
		if(CPUoperation=="decode")
		{
			printf("add instruction: ");
			
			//printing the bytes of the instruction
			printf("bytes: %x%x\n", memory[programCounter],memory[programCounter+1]);
			populatingOPRegisters();
		}
		else if(CPUoperation=="execute")
		{		
			result=OP1+OP2; //adding the two registers 
		}
		else if(CPUoperation=="store")
		{
			R[memory[programCounter+1]&15]=result; //adding the result to the specified register
			programCounter+=2;
		}
	}
	
	//ADDIMMEDIATE
	else if(currentInstruction==9)
	{
		if(CPUoperation=="decode")
		{
			printf("addimmediate instruction: ");
			printf("bytes: %x%x\n", memory[programCounter],memory[programCounter+1]);
			
			//setting OP1 and OP2 to values according to instruction
			OP1=R[memory[programCounter]&15];
			OP2=memory[programCounter+1];
			
		}
		else if(CPUoperation=="execute")
		{
			result=OP1+OP2;
		}
		else if(CPUoperation=="store")
		{
			R[memory[programCounter]&15]=result;
			programCounter+=2;
		}
	}
	
	//AND
	else if(currentInstruction==2)
	{
		if(CPUoperation=="decode")
		{
			printf("and instruction: ");
			printf("bytes: %x%x\n", memory[programCounter],memory[programCounter+1]);
			populatingOPRegisters();
			
		}
		else if(CPUoperation=="execute")
		{
			result=OP1&OP2;
		}
		else if(CPUoperation=="store")
		{
			R[memory[programCounter+1]&15]=result;
			programCounter+=2;
		}
	}
	
	//BRANCHIFEQUAL
	else if(currentInstruction==10)
	{
		if(CPUoperation=="decode")
		{
			printf("branchifequal instruction: ");
			printf("bytes: %x%x %x%x\n",
			memory[programCounter],
			memory[programCounter+1],
			memory[programCounter+2],
			memory[programCounter+3]);
			populatingOPRegisters();
			
	
			
		}
		else if(CPUoperation=="execute")
		{
			if(OP1==OP2)
			{
				//transcribing the individual bytes to a single value
				address=memory[programCounter+1]&15;
				address=(address<<8)|(memory[programCounter+2]);
				address=(address<<8)|(memory[programCounter+3]);
				
				//if value is negative
				if((memory[programCounter+1]&15)>7)
				{									
					address=-((address-1)^0xFFFFF); //reverse two's complement and set to negative		
				}
				
				//if address is out of range, set address to 0 (do nothing)
				if((programCounter+(2*address))<0||(programCounter+(2*address))>=sizeof(memory))
				{
					address=0;
				}
			}
			
			//if the two values are not equal
			else
			{
				address=0;
			}
		}
		else if(CPUoperation=="store")
		{
			programCounter=programCounter+(2*address); //update program counter
		}
	}
	
	//BRANCHIFLESS
	else if(currentInstruction==11)
	{
		if(CPUoperation=="decode")
		{
			printf("branchifless instruction: ");
			printf("bytes: %x%x %x%x\n",
			memory[programCounter],
			memory[programCounter+1],
			memory[programCounter+2],
			memory[programCounter+3]);
			populatingOPRegisters();
			
		}
		else if(CPUoperation=="execute")
		{
			//if the first value is less than the second value
			if(OP1<OP2)
			{
				address=memory[programCounter+1]&15;
				address=(address<<8)|(memory[programCounter+2]);
				address=(address<<8)|(memory[programCounter+3]);
				if((memory[programCounter+1]&15)>7)
				{									
					address=-((address-1)^0xFFFFF);			
				}
				if((programCounter+(2*address))<0||(programCounter+(2*address))>=sizeof(memory))
				{
					address=0;
				}
			}
			else
			{
				address=0;
			}
		}
		else if(CPUoperation=="store")
		{
			programCounter=programCounter+(2*address); //update program counter
		}
	}
	
	//CALL
	else if(currentInstruction==13)
	{
		if(CPUoperation=="decode")
		{
			printf("call instruction: ");
			printf("bytes: %x%x %x%x\n",
			memory[programCounter],
			memory[programCounter+1],
			memory[programCounter+2],
			memory[programCounter+3]);
			OP1=programCounter+4; //OP1 is set to the next instruction
			OP2=0;
			

		}
		else if(CPUoperation=="execute")
		{
			push(OP1); //pushes the next instruction onto the stack
			address=memory[programCounter]&15;
			address=(address<<8)|(memory[programCounter+1]);
			address=(address<<8)|(memory[programCounter+2]);
			address=(address<<8)|(memory[programCounter+3]);
			
			//if addresss is out of range, do nothing
			if((2*address)<0||(2*address)>=sizeof(memory))
			{
				address=0;
			}
		}
		else if(CPUoperation=="store")
		{
			programCounter=2*address; //Going to function at address
			programCounter=pop(); //returning to next instruction
		}
	}
	
	//DIVIDE
	else if(currentInstruction==3)
	{
		if(CPUoperation=="decode")
		{
			printf("divide instruction: ");
			printf("bytes: %x%x\n", memory[programCounter],memory[programCounter+1]);
			populatingOPRegisters();
			
		}
		else if(CPUoperation=="execute")
		{
			result=OP1/OP2;
		}
		else if(CPUoperation=="store")
		{
			R[memory[programCounter+1]&15]=result;
			programCounter+=2;
		}
	}
	
	//HALT
	else if(currentInstruction==0)
	{
		//setting flag to false to stop the program
		flag=0;
	}
	
	//INTERRUPT
	else if(currentInstruction==8)
	{
		if(CPUoperation=="decode")
		{
			printf("interrupt instruction: ");
			printf("bytes: %x%x\n", memory[programCounter],memory[programCounter+1]);
			OP1=memory[programCounter+1]; //setting OP1 to the interrupt number
			OP2=0;
			
		}
		else if(CPUoperation=="execute")
		{
			//if the interrupt number is 0 (print registers)
			if(OP1==0)
			{
				printf("Printing Registers:\n");
				for(i=0;i<15;i++)
				{		
					printf("R%d = %d\n", i, R[i]);
				}
			}
			
			//if the interrupt number is 1 (print memory)
			else if(OP1==1)
			{
				printf("Printing Memory:\n");
				
				//goes through memory from the beginning
				for(i=0;i<currentMemoryLocation;i++)
				{
					//if at the beginning, printing message
					if(i==0)
					{
						printf("Memory[0-%d] (file contents in memory):\n",sizeOfFile-1);
					}
					//since the instruction set is in memory, print that in a formatted way to save space
					if(i<sizeOfFile)
					{
						printf("%x ",memory[i]);
					}
					
					//prints the next thing in memory after the instruction set
					else
					{
						printf("Memory[%d] = %d", i, memory[i]);
					}
				}
				printf("\n");
			}
		}
		else if(CPUoperation=="store")
		{
			programCounter+=2; //updating program counter
		}
	}
	
	//JUMP
	else if(currentInstruction==12)
	{
		if(CPUoperation=="decode")
		{
			printf("jump instruction: ");
			printf("bytes: %x%x %x%x\n",
			memory[programCounter],
			memory[programCounter+1],
			memory[programCounter+2],
			memory[programCounter+3]);
			OP1=0;
			OP2=0;
			
		}
		else if(CPUoperation=="execute")
		{
			address=memory[programCounter]&15;
			address=(address<<8)|(memory[programCounter+1]);
			address=(address<<8)|(memory[programCounter+2]);
			address=(address<<8)|(memory[programCounter+3]);
			
			//if address is out of range, do nothing
			if((2*address)<0||(2*address)>=sizeof(memory))
			{
				address=0;
			}
		}
		else if(CPUoperation=="store")
		{
			programCounter=programCounter+(address*2); //updating program counter
		}
	}
	
	//LOAD
	else if(currentInstruction==14)
	{
		if(CPUoperation=="decode")
		{
			printf("load instruction: ");
			printf("bytes: %x%x\n", memory[programCounter],memory[programCounter+1]);
			OP1=memory[programCounter+1]>>4;
			OP2=memory[programCounter+1]&15;		
		}
		else if(CPUoperation=="execute")
		{	
			result = (memory[programCounter]&15); //result is equal to the result register
			address=R[OP1]+(2*OP2);	//setting the value to load into the result register	
		}
		else if(CPUoperation=="store")
		{
			if(address<0||address>=sizeof(memory)){} //if address is out of range, do nothing
			else
				R[result]=memory[address]; //load the register
			
			programCounter+=2;
		}		
	}
	
	//MULTIPLY
	else if(currentInstruction==4)
	{
		if(CPUoperation=="decode")
		{
			printf("multiply instruction: ");
			printf("bytes: %x%x\n", memory[programCounter],memory[programCounter+1]);
			populatingOPRegisters();
			
		}
		else if(CPUoperation=="execute")
		{
			result=OP1*OP2;
		}
		else if(CPUoperation=="store")
		{
			R[memory[programCounter+1]&15]=result;
			programCounter+=2;
		}
	}
	
	//OR
	else if(currentInstruction==6)
	{
		if(CPUoperation=="decode")
		{
			printf("or instruction: ");
			printf("bytes: %x%x\n", memory[programCounter],memory[programCounter+1]);
			populatingOPRegisters();
			
		}
		else if(CPUoperation=="execute")
		{
			result=OP1|OP2;
		}
		else if(CPUoperation=="store")
		{
			R[memory[programCounter+1]&15]=result;
			programCounter+=2;
		}
	}
	
	//pop/push/return
	else if(currentInstruction==7)
	{
		if(CPUoperation=="decode")
		{
			printf("pop/push/return instruction: ");
			printf("bytes: %x%x\n", memory[programCounter],memory[programCounter+1]);
			OP1=memory[programCounter]&15;
			OP2=memory[programCounter+1];
			
		}
		else if(CPUoperation=="execute")
		{
			//if the hex code is 10
			if(OP2==16)
			{
				printf("Pop!\n");
				result=pop(); //setting result to last thing in stack
			}
			
			//if the hex code is 01
			else if(OP2==1)
			{
				printf("Push!\n");
				push(R[OP1]); //pushing to stack
				result=0;
			}
			
			//if the hex code is 00
			else
			{
				printf("Return!\n");
				result=returnStack(); //gets the last thing in the stack but doesn't change the stack
				if(result<0||result>=sizeof(memory)) //if result is out of range, do nothing
				{
					result=0;
				}
			}
		}
		else if(CPUoperation=="store")
		{
			if(OP2==16)
			{
				R[memory[programCounter+1]&15]=result;
				programCounter+=2;
			}
			else if(OP2==1)
			{
				programCounter+=2;
			}
			else
			{
				programCounter=result;
			}
		}
	}
	
	//STORE
	else if(currentInstruction==15)
	{
		if(CPUoperation=="decode")
		{
			printf("store instruction: ");
			printf("bytes: %x%x\n", memory[programCounter],memory[programCounter+1]);
			OP1=memory[programCounter]&15;
			OP2=memory[programCounter+1]&15;
			
		}
		else if(CPUoperation=="execute")
		{
			result=R[memory[programCounter]&15];
			address=R[OP2]+(2*OP2);
		}
		else if(CPUoperation=="store")
		{
			if(address<0||address>=sizeof(memory)){} //if address is out of range, do nothing
			else
				memory[address]=result;	//updating memory
			programCounter+=2;
		}
	}
	
	//SUBTRACT
	else if(currentInstruction==5)
	{
		if(CPUoperation=="decode")
		{
			printf("subtract instruction: ");
			printf("bytes: %x%x\n", memory[programCounter],memory[programCounter+1]);
			populatingOPRegisters();
			
		}
		else if(CPUoperation=="execute")
		{
			result=OP1-OP2;
		}
		else if(CPUoperation=="store")
		{
			R[memory[programCounter+1]&15]=result;
			programCounter+=2;
		}
	}
	
	//if currentInstruction is not an accepted instruction
	else
	{
		flag = 0;
	}
}

//function for populating OP variables
void populatingOPRegisters()
{

	//populating OP registers
	OP1=R[memory[programCounter]&15];
	OP2=R[memory[programCounter+1]>>4];

}

//loading file into memory. Beginning run loop
void load(FILE *assemblyFile)
{	
	//initializing registers to 0
	for(i=0;i<15;i++)
	{
		R[i]=0;
	}
	
	//if file is empty, return error message
	if(assemblyFile==NULL)
		printf("error");
	
	//putting contents of file into memory
	fread(memory, sizeof(memory),1,assemblyFile);
	while(flag)
	{
		
		//calling functions
		fetch();
		decode();
		execute();
		store();
	}
	printf("halt!");
	
}

//calling fetch method: sets the current instruction
void fetch()
{
	currentInstruction=memory[programCounter]>>4;
}

//begin decoding instruction
void decode()
{	
	//sends method the current instruction along with the process
	checkInstruction(currentInstruction, "decode");	
}
//begin execution of instruction
void execute()
{
	checkInstruction(currentInstruction, "execute");
}
//begin storing the results
void store()
{
	checkInstruction(currentInstruction, "store");
}

int main(int argc, char **argv) 
{
	FILE *assemblyFile = fopen(argv[1],"rb"); //opening binary file
	
	fseek(assemblyFile, 0L, SEEK_END); //sets a pointer to the end of the file
	
	sizeOfFile=ftell(assemblyFile); //ftell gets the size of the file and sets it to a variable 
	currentMemoryLocation=sizeOfFile; //keeping track of location in memory
	
	//printf("sizeOfFile=%d\n",sizeOfFile);
	fseek(assemblyFile, 0L, SEEK_SET); //resetting pointer to beginning of file
	
	//begin loading file
	load(assemblyFile);
	return 0;
}