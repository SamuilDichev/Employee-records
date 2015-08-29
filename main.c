// Student name: Samuil Dichev
// Student number: C1340032
// Module: CM2204 Advanced Programming
// Lecturer: Dr. Daniela Tsaneva


// Standard input/ouput
#include <stdio.h>
// String manipulation - strcpy(), etc.
#include <string.h>
// General functions - exit(0);
#include <stdlib.h>
// Testing / mapping characters - tolower(), etc.
#include <ctype.h>
// Improves portability by allowing the use of Booleans on platforms other than Windows.
#include <stdbool.h>


// Struct for the record
typedef struct {
	int ID;
	char firstName[50];
	char lastName[50];
	int age;
	int salary;
	char gender;
	char address[1024];
} Employee;

// Variables which will contain the user input.
char inFirstName[50];
char inLastName[50];
int inAge;
int inSalary;
char inGender;
char inAddress[1024];
char cConfirm;

// Function which cleans the input buffer. This helps avoid issues / bugs when multiple scanfs read from it.
void clean_stdin(void) {
	int c;
	do {
		c = getchar();
	} while (c != '\n' && c != EOF);
}


// Function to get user input.
// fflush(stdin) was used previously instead of clean_stdin();, but it's not portable and bad practice
void getInput() {
	printf ("\nPlease supply the customer details:\n");

	printf ("First name: ");
	scanf("%s", inFirstName);
	clean_stdin();

	printf ("Last name: ");
	scanf("%s", inLastName);
	clean_stdin();

	printf ("Age: ");
	scanf("%d", &inAge);
	clean_stdin();

	printf ("Gender (M / F): ");
	while (1) {
		scanf("%c", &inGender);
		clean_stdin();

		if (tolower (inGender) == 'm') {
			inGender = 'M';
			break;
		}

		else if (tolower (inGender) == 'f') {
			inGender = 'F';
			break;
		}

		else {
			printf("Invalid input. Please, type a valid gender (M / F): ");
		}
	}

	printf ("Salary: ");
	scanf("%d", &inSalary);
	clean_stdin();
	
	printf ("Address: ");
	scanf(" %[^\n]s", inAddress);
	clean_stdin();

	return;
}

// Function called when creating or updating record to write the record to a file.
// Takes an ID  and a file as arguments.
void saveRecord (int ID, FILE* output) {
	// Allocate memory	
	Employee *e;
	e = (Employee *) malloc (sizeof(Employee));
	// If e couldn't be allocated memory, print a MSG and return to menu.
	if (e == NULL) {
		printf("Not enough memory left!");
		return;
	}
	// Access through -> instead of . because e is a pointer
	e->ID = ID;
	strcpy(e->firstName, inFirstName);
	strcpy(e->lastName, inLastName);
	e->age = inAge;
	e->salary = inSalary;
	e->gender = inGender;
	strcpy(e->address, inAddress);
	fwrite(e, sizeof(Employee), 1, output);
	fclose(output);
	free(e);
}

// Function for creating a record, called by the menu. This calls the saveRecord function
void createRecord() {
	printf ("\nWould you like this person to be stored? (Y/N): ");
	
	// Keep doing forever. Break when one of the inner loops is entered.
	while (1) {
		scanf ("%c", &cConfirm);
		// User may type upper case input, so convert to lower before comparison.
		if (tolower (cConfirm) == 'y') {
			FILE* output;
			// a+ stands for append and read. If file doesn't exist, create it. B is just for binary.
			output = fopen("Data.dat", "a+b");

			// Go through the file and figure out the size.
			fseek(output, 0L, SEEK_END);
			int size = ftell(output);
			// Reset the pointer to the begining of the file after completion of size calculation. 
			fseek(output, 0, SEEK_SET);

			int recordCount;

			// If file is empty, set recordCount to 0
			if (size == -1) {
				recordCount = 0;
			}

			// Otherwise set it to the size / size of one record to get the number of records.
			else {
				recordCount = size / sizeof(Employee);
			}
			
			// Pass to save record the recordCount+1 (Used for automatic ID) and the file. Then break out of loop.
			saveRecord(recordCount+1, output);
			break;
		}

		// If user doesn't wish to save, break out of loop.
		else if (tolower(cConfirm) == 'n') {
			break;
		}
	}
	return;
}

// Function to updateRecord. Called by the menu.
void updateRecord(int upID) {
	Employee *e;
	e = (Employee *) malloc (sizeof(Employee));

	if (e == NULL) {
		printf("Not enough memory left!");
		return;
	}
	
	// Boolean to determine if a record has been found or not.
	bool found = false;
	FILE* output;
	// r+ stands for read and write. b for binary.
	output = fopen("Data.dat", "r+b");
	// Iterates through file
	while ((fread(e, sizeof(Employee), 1, output)) == 1) {
		// If the ID of a record matches the user input ID
		if (e->ID == upID) {
			// Set the pointer to it
			fseek(output, (upID-1) * sizeof(Employee), SEEK_SET);
			// Get user input
			getInput();
			// Pass the ID that's already being used and the file to the saveRecord function.
			saveRecord(e->ID, output);
			// Record has been found, so set the boolean to true.
			found = true;
			break;
		}
	}

	free(e);
	// Based on whether record has been found, display msg.
	(found) ? (printf("\nChanges saved")) : (printf("\nNo record with that ID exists"));
	return;
}

// Function to display records. Called from the menu.
void displayRecord() {
	// Allocate memory for Employee struct
	Employee *e;
	e = (Employee *) malloc (sizeof(Employee));

	if (e == NULL) {
		printf("Not enough memory left!");
		return;
	}

	FILE* input;
	// rb stands for read binary.
	input = fopen("Data.dat", "rb");
	// If file exists
	if (input) {
		// Iterate through it and print everything
		while ((fread(e, sizeof(Employee), 1, input)) == 1) {
	        printf("\nID: %d, Name: %s %s, Age: %d, Salary: %d, Gender: %c, Address: %s",
				e->ID,
				e->firstName,
				e->lastName,
				e->age,
				e->salary,
				e->gender,
				e->address);
	    }
	    // Close file.
		fclose(input);
		// Free memory used by struct
	}

	// If file does not exist, print msg.
	else {
		printf("No records file found!");
	}

	free(e);
	return;
}

// Menu
int main (int argc, char* argv[]) {
	// Keep doing forever.
	while (1) {
		// Variable to contain user input.
		int menuChoice;
		printf("\n\nMenu: \n");
		printf("1. Create a record.\n2. Display records\n3. Update record\n4. Exit\n\n");
		printf("Your choice: ");
		scanf("%d", &menuChoice);
		printf("\n\n");
		

		// Do something based on user input.
		switch (menuChoice) {
			// Default - if the below aren't matched.
			default:
				printf("Wrong input. Please, provide a number from 1 to 4!");
				break;
			case 1:
				getInput();
				createRecord();
				break;
			case 2:
				displayRecord();
				break;
			case 3:
				// Brackets needed to avoid errors on some compilers. 
				{
					// Varible for user input.
					int upID;
					printf("\nPlease, type the ID of the record you wish to update: ");
					scanf("%d", &upID);
					// Flush input to prevent future scanf problems.
					clean_stdin();
					//fflush(stdin);
					updateRecord(upID);
				}

				break;
			case 4:
				// Exit with a 0 flag (No problems)
				exit(0);
		}
	}
}