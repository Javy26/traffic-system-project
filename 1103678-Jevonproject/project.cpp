#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <string.h>


// GLOBAL DECLARATIONS //


int myopt;
int suspectCount;	//used to keep track of how many suspect phone numbers are stored in the phone number file
const int TOT_RECS = 10000; //stores total number of records as a constant. 900 0000 t0 900 9999 is 10,000 records

FILE *SuspectsPhoneNum;
FILE *CallLog;
FILE *StatisticsFile;


typedef struct
{
	long Susnum;
	char FirstName[20];
	char LastName[20];
	int Casenum;
}Suspect;

typedef struct
{
	long phoneNum;
	int callsIn;	//number of calls in
	int callsOut;	//number of calls out
	int totCallsIn;	//total minutes on calls in
	int totCallsOut;//total minutes on calls out
}Stats;

Stats statistics[10000] = {{0}}; // create an array of Stats structures, and initialize all elements to 0 
Stats *stats_ptr;	// creates a pointer to a stats structure, this pointer will be used as a reference to the statistics array


// FUNCTION PROTOTYPES	//


//void setColor(int);
void Menu();
void clrscr();
void gotoxy(int, int);
void border();
void choice();
void InitializeSystem(Stats *, int);	// initalizeSystem and accepts a pointer to a Statistics  Structure 
void AddSuspectsPhoneNumber();
void SimulateCall(Stats *, int);
void DisplayCallLog();
void DisplayStatistics(Stats *, int);
void Exit();



// MAIN	//

void main()
{
	int i;
	char line1[] = {'W','e','l','c','o','m','e',' ', '2', ' ','J','C','F',' ','T','r','a','c','k','e','r',' ', 'b','y',' ','J',' ','D','A','V','I','S' };

	
	
	stats_ptr = statistics; //initalizes the stats pointer to point to the array of stats stuctures declared above		
			
	

	while(1)
	{
		border();
		gotoxy(28,2);
		

		for(i = 0; i < 32; i++)
		{
			printf("%c", line1[i]); _sleep(26);
		}
		Menu();
		choice();
	}
}


void Menu()
{
			
	char line2[] = {'S','e','l','e','c','t',' ','A', 'n',' ', 'I','t','e','m',' ','F','r','o','m',' ','T','h','e',' ','M','e','n','u'};
	int choice;
	int i;
	
			
	gotoxy(2,7);

	for(i = 0; i < 28; i++)
	{
		printf("%c", line2[i]); _sleep(30);
	}
	printf(":");
	gotoxy(2,8);
	printf("----------------------------- ---\n");
			

	_sleep(500); gotoxy(2,8);
	printf("1. Initialize System");
	_sleep(500); gotoxy(2,10);
	printf("2. Add Suspects Phone Number");
	_sleep(500); gotoxy(2,14);
	printf("3. Simulate Call");
	_sleep(500); gotoxy(2,16);
	printf("4. Display Call Log");
	_sleep(500); gotoxy(2,18);
	printf("5. Display Statistics");
	_sleep(500); gotoxy(2,20);
	printf("6. Exit ");gotoxy(32,7); 


}

void InitializeSystem(Stats *statObj, int size)
{
	int i;

	for(i = 0; i <= size; i++)
	{
		statObj[i].phoneNum = 9000000+i;
	}

	SuspectsPhoneNum = fopen ( "Suspects_Phone_Number.txt", "w");
	CallLog = fopen ( "Call_Log.txt", "w");
	StatisticsFile = fopen ( "Statistics_File.txt", "wb");

	fwrite(statObj, sizeof(Stats), size, StatisticsFile);

	fclose(SuspectsPhoneNum);
	fclose(CallLog);
	fclose(StatisticsFile);


}

void AddSuspectsPhoneNumber()
{
	Suspect newSuspect[15]; // create an array of Suspect structures, i.e an array to store suspects
	int numOfSuspects, i;

	system("cls");
	border();
	gotoxy(2,3);
	printf("How many suspects phone number do you have to enter? ");
	scanf("%d", &numOfSuspects);

	//update the file keeping track of the amount of suspect phone numbers added to the phone number file
	suspectCount += numOfSuspects;
	

	SuspectsPhoneNum = fopen ( "Suspects_Phone_Number.txt", "a");
	for(i=0; i<numOfSuspects; i++)
	{
		system("cls");
		border();
		do
		{
			system("cls");
			border();
			gotoxy(2,2); 
			printf("%d of %d", i+1, numOfSuspects);
			gotoxy(2,3); 
			printf("Enter suspect's phone number (9000000 and 9009999): ");
			scanf("%d", &newSuspect[i].Susnum);
		}while (newSuspect[i].Susnum < 9000000 || newSuspect[i].Susnum > 9009999);

		gotoxy(2,6);
		printf("Enter Suspect's First Name : ");
		scanf("%s", newSuspect[i].FirstName);
		gotoxy(2,8);
		printf("Enter suspect's Last name : ");
		scanf("%s", newSuspect[i].LastName);
		gotoxy(2,10);
		printf("Enter a case number : ");
		scanf("%d", &newSuspect[i].Casenum);

		//write suspect's information to file
		fprintf(SuspectsPhoneNum, "%7d ", newSuspect[i].Susnum);
		fprintf(SuspectsPhoneNum, "%10s ", newSuspect[i].FirstName);
		fprintf(SuspectsPhoneNum, "%10s ", newSuspect[i].LastName);
		fprintf(SuspectsPhoneNum, "%4d\r", newSuspect[i].Casenum);
	}
	
	fclose(SuspectsPhoneNum);
	gotoxy(2,14);
	
	printf("Press 1 to return to the main menu: ");
	gotoxy(2,16);

	printf("Press 2 to exit : ");
	gotoxy(21,16);
	scanf("%d",&myopt);

	if (myopt != 1)
	{
		Exit();
	}
}

void SimulateCall(Stats *statObj, int size)
{
	//store the name of the months and days corressponding to thier position in the array 0=sun 1=mon etc 
	static const char wday_name[][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
	static const char mon_name[][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun","Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

	time_t timeNow = time(0);					// get the number of seconds since 1970
	struct tm * dateTime = localtime(&timeNow);	// a structure containing calendar date and time info, localtime() converts the time to local time
	Suspect activeSusp;							//create a new suspect (this suspect is an active suspect recieving or making a call)
	int i=0, minsOnCall, cType, randomNum;				// randomNum determines which number will be selected from the phone number file
	long otherParty;							// otherParty stores the number of the person on the other end of the call i.e the number being called or the number a call is coming from

	srand(time(NULL));
	randomNum = rand() % suspectCount + 0;

	//randomly selects a phone number from the phone number file
	SuspectsPhoneNum = fopen ( "Suspects_Phone_Number.txt", "r");
	while(!feof(SuspectsPhoneNum))
	{
		fscanf(SuspectsPhoneNum, "%ld %s %s %d", &activeSusp.Susnum, activeSusp.FirstName, activeSusp.LastName, &activeSusp.Casenum);
		i++;
		if(i == randomNum)
			break;
	}
	fclose(SuspectsPhoneNum);
	
	StatisticsFile = fopen("Statistics_File.txt", "rb");	//opens statistics file in read binary mode
		fread(statObj, sizeof(Stats), size, StatisticsFile);	//dumps the file contents into an array of stats structures using a pointer to the array
	fclose(StatisticsFile);

	//randomly determines the call type either incoming or outgoing
	cType = rand() % 2 + 1;
	if(cType == 2)	//test if call is an incoming call
	{
		otherParty = rand() % 9009999 + 9000000; //randomly select a phone number for the incoming call
		minsOnCall = rand() % 1440 + 1;		//random number between 1 and 1440. 1440 is the number of minutes in one day

		//prints call information to the screen (date, time, incoming, the number of the incoming call & the amount of minutes spent on the call)
		system("cls");
		border();
		gotoxy(2,3); 
		printf("%10s %17s %16s %15s %10s", "Date", "Time", "Out/In", "Number", "Minutes");
		gotoxy(2,5); 
		printf("%s %s %d %d\t%d:%.2d:%.2d %15s %14d %8d", wday_name[dateTime->tm_wday], mon_name[dateTime->tm_mon], dateTime->tm_mday,
			1900 + dateTime->tm_year, dateTime->tm_hour, dateTime->tm_min, dateTime->tm_sec, "Incoming", activeSusp.Susnum, minsOnCall);
		
		//write call data to call log file
		CallLog = fopen ( "Call_Log.txt", "a");
		fprintf(CallLog, "%s %s %d %d %d:%d:%d %s %d %d\r", wday_name[dateTime->tm_wday], mon_name[dateTime->tm_mon], dateTime->tm_mday, 
			1900 + dateTime->tm_year, dateTime->tm_hour, dateTime->tm_min, dateTime->tm_sec, "Incoming", activeSusp.Susnum, minsOnCall);
		fclose(CallLog);

		//update the statistics record of the active suspect
		for(i=0; i<size;i++)
		{
			if(statObj[i].phoneNum == activeSusp.Susnum)	// searches for the active suspect's phone number
			{
				statObj[i].callsIn += 1;				//callsIn = callsIn + 1, just increment the value of callsIn by 1
				statObj[i].totCallsIn += minsOnCall;	//totCallsIn = totCallsIn + minsOnCall, basically just adding the mins spent on the call to what was already stored in the file
				break;	// exits the loop after finding the suspect's phone number and updating the record, no need to search the rest of the file
			}
		} //end for
	}
	else	// the call is an out going call
	{
		otherParty = rand() % 9009999 + 9000000;
		minsOnCall = rand() % 1440 + 1;

		//prints call information to the screen
		system("cls");
		border();
		gotoxy(2,3); 
		printf("%10s %17s %16s %15s %10s", "Date", "Time", "Out/In", "Number", "Minutes");
		gotoxy(2,5); 
		printf("%s %s %d %d\t%d:%.2d:%.2d %15s %14d %8d", wday_name[dateTime->tm_wday], mon_name[dateTime->tm_mon], dateTime->tm_mday,
			1900 + dateTime->tm_year, dateTime->tm_hour, dateTime->tm_min, dateTime->tm_sec, "Outgoing", activeSusp.Susnum, minsOnCall);

		//write call data to call log file
		CallLog = fopen ( "Call_Log.txt", "a");
		fprintf(CallLog, "%s %s %d %d %d:%d:%d %s %d %d\r", wday_name[dateTime->tm_wday], mon_name[dateTime->tm_mon], dateTime->tm_mday, 
			1900 + dateTime->tm_year, dateTime->tm_hour, dateTime->tm_min, dateTime->tm_sec, "Outgoing", activeSusp.Susnum, minsOnCall);
		fclose(CallLog);
		
		//update the statistics record of the active suspect
		for(i=0; i<size;i++)
		{
			if(statObj[i].phoneNum == activeSusp.Susnum)	// searches for the active suspect's phone number
			{
				statObj[i].callsOut += 1;				//callsOut = callsOut + 1, just increment the value of callsOut by 1
				statObj[i].totCallsOut += minsOnCall;	//totCallsOut = totCallsOut + minsOnCall, basically just adding the mins spent on the call to what was already stored in the file
				break;	// exits the loop after finding the suspect's phone number and updating the record, no need to search the rest of the file
			}
		}
	}

	//update the statistics file with the new information
	StatisticsFile = fopen("Statistics_File.txt", "wb");
		fwrite(statObj, sizeof(Stats), size, StatisticsFile);
	fclose(StatisticsFile);
	
	
	gotoxy(25,14);
	system("pause");
	
}

void DisplayCallLog()
{
	char wday[5], month[4], inOut[9], col;
	int day, year, hour, min, sec, numMins, i;
	long num;

	system("cls");
	border();
	gotoxy(2,2); 

	printf("%10s %17s %16s %15s %10s", "Date", "Time", "Out/In","Number", "Minutes");
	
	if(CallLog = fopen("Call_Log.txt", "r"))
	{
		i =4;
		while(!feof(CallLog))
		{
			fscanf(CallLog, "%s %s %d %d %d%c%d%c%d %s %ld %d", wday, month, &day, &year, &hour, &col, &min, &col, &sec, inOut, &num, &numMins);

			gotoxy(2,i);
			printf("%s %s %d %d\t%d:%.2d:%.2d %14s %15d %8d", wday, month, day, year, hour, min, sec, inOut, num, numMins);
			_sleep(130);
			i+=2;

			if(i == 28) //if screen is full
			{
				gotoxy(25,28);
				system("pause");
				system("cls");
				border();
				gotoxy(2,2);
				printf("%10s %17s %16s %15s %10s", "Date", "Time", "Out/In","Number", "Minutes");
				
				i =4;
			}
		} getch();
	}
	fclose(CallLog);
}

void DisplayStatistics(Stats *statObj, int size)
{
	int found = FALSE;
	int i;
	long queryNum;
	Suspect availSuspects[15];

	system("cls");
	border();
	gotoxy(2,3);

	SuspectsPhoneNum = fopen("Suspects_Phone_Number.txt", "r");
		for(i=0; i < suspectCount; i++)
		{
			fscanf(SuspectsPhoneNum, "%ld %s %s %d", &availSuspects[i].Susnum, availSuspects[i].FirstName, availSuspects[i].LastName, &availSuspects[i].Casenum);
		}
	fclose(SuspectsPhoneNum);

	StatisticsFile = fopen("Statistics_File.txt", "rb");	//opens statistics file in binary read mode
	fread(statObj, sizeof(Stats), size, StatisticsFile);	//dumps the file contents into an array of stats structures using a pointer to the array
	fclose(StatisticsFile);

	printf("Enter a number from the phone number file: ");
	scanf("%ld", &queryNum);

	for(i = 0; i < suspectCount; i++)
	{
		if(queryNum == availSuspects[i].Susnum)
		{
			found = TRUE;
			break;
		}
	}

	system("cls");
	border();
	gotoxy(2,3);

	if(found)
	{
		
		printf("%7s  %8s  %9s  %13s  %14s", "Number", "Calls In", "Calls Out", "Total Mins In", "Total Mins Out");
		gotoxy(2,5); 
		for(i = 0; i < size; i++)
		{
			if(queryNum == statObj[i].phoneNum)
			{
				printf("%7ld %8d %9d %13d %14d", statObj[i].phoneNum, statObj[i].callsIn, statObj[i].callsOut, statObj[i].totCallsIn, statObj[i].totCallsOut);
				break;
			}
		}
	}
	else
		printf("The number is not in the phone numbers file. Please try again.");
	getch();
}

void choice (){
	int x;
	printf(" ");
	scanf("%d",&x); 
	switch (x){
	case 1:
		InitializeSystem(stats_ptr, TOT_RECS);
		system("cls");
		border();
		
		gotoxy(30,2);
		printf("3 Files were created");
		
		gotoxy(2,4);
		printf("1. Return to main menu");
		gotoxy(2,6);
		printf("2. Exit Program");
		gotoxy(2,8);
		printf("-> Your choice : "); 
		scanf("%d",&myopt);
		if(myopt != 1)
			Exit();
		break;
	case 2:
		AddSuspectsPhoneNumber();
		break;
	case 3:
		//Simulate Call
		SimulateCall(stats_ptr, TOT_RECS);
		break;
	case 4:
		//Display Call log
		DisplayCallLog();
		break;
	case 5:
		//Display Statistics
		DisplayStatistics(stats_ptr, TOT_RECS);
		break;
	case 6:
		Exit();
		break;
	default:
		gotoxy(24,22);
		
		printf("No such item! Please try again.");
		getch();
		fflush(stdin);
	}
}
void Exit()
{
		system("cls");
		border();
		gotoxy(32,14);
		
		printf("Jamaica Constabulary Force Program\n\n");
		printf("\t\t\t\tis Now Closing");
		
		exit(1);
}

//changes background and foreground colour

// Moves the cursor to x, y in console window
// x = horizontal axis, y = vertical axis
void gotoxy(int x, int y)
{
	HANDLE con=GetStdHandle(STD_OUTPUT_HANDLE);
	COORD Coord;
	Coord.X=x;
	Coord.Y=y;	
	SetConsoleCursorPosition(con,Coord);
}

//border
void border()
{
    int xCountr, yCountr;
	const int BORDR = 178;

	system( "mode con:cols=81 lines=31" );
   
	system("cls");

	// horizontal borders
    for(xCountr = 0; xCountr < 81; xCountr++)
    {
        gotoxy(xCountr, 0);
        printf("%c", BORDR);

        gotoxy(xCountr, 29);
        printf("%c", BORDR);
    }

	// vertical borders
    for(yCountr = 0; yCountr < 29; yCountr++)
    {
        gotoxy(0, yCountr);
        printf("%c", BORDR);

        gotoxy(80, yCountr);
        printf("%c", BORDR);
    }

	
	gotoxy(30,30);
	
}