//Created by Emre Kumas.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

typedef struct recordData{
    unsigned long id;
    char firstName[32];
    char lastName[32];
    char phoneNum[100];
    char address[100];
}Record;

void listRecords();
void printRecord(Record);
void readFromFile(FILE *);
int isEmpty(FILE *);
int getNumericChoiceFromUser(char *);
int checkForBoth(char *, bool);
void characterLongText(char *);
void characterLongLong(unsigned long);
void updateRecord();
void getPhoneNumber(char *, char *);
void getString(char *, char *);
void writeToFile();
void createRecord();
void deleteRecord();

//Global Variables.
int numberOfAddress = 0;
Record *book = NULL;

int main() {

    //Variables.
    FILE *fP;
    bool fileFound = false;

    //First checking if the file exists.
    if(access("Database.txt", F_OK) != -1){

        //File exists.
        fileFound = true;

        //We're gonna read the file.
        fP = fopen("Database.txt", "r+");

        readFromFile(fP);

        fclose(fP);
    }

    //This loop will continue unless user enters 5.
    while(1){

        printf("ADDRESS BOOK\n");
        printf("1 - List all records\n");
        printf("2 - Update a record\n");
        printf("3 - Create a new record\n");
        printf("4 - Delete a record\n");
        printf("5 - Exit\n");

        //We're making sure the user doesn't type wrong characters because we only want numeric characters.
        int choice = getNumericChoiceFromUser("\nWhat do you want to do? (Enter a number between 1-5)");

        switch(choice){

            case 1:

                listRecords();

                break;

            case 2:

                updateRecord();

                break;

            case 3:

                createRecord();

                break;

            case 4:

                deleteRecord();

                break;

            case 5:

                goto endingOfProgram;

            default:

                printf("\n\n\n\n\n\n\n\n");
                system("clear");
                break;

        }
    }

    endingOfProgram:

    if(book != NULL){

        writeToFile();
        free(book);

    }else if(fileFound == true){

        //If the program reaches here it means, the book is NULL and there was a file at the beginning.
        //So what we need to do is, delete the file.
        int success = remove("Database.txt");

        if(success != 0)
            printf("\n\n\n\nFailed to delete the old file.\n");
    }

    return 0;
}

void listRecords(){

    //We need to check if the book contains anything.
    if(book == NULL)
        printf("\nThere is no record.\n\n\n\n\n\n\n\n");
    else{

        //Now we need to print the book.
        printf("\nID Number\t\tFirst Name\t\tLast Name\t\tPhone Num\t\tAddress\n");
        printf("*********\t\t**********\t\t*********\t\t*********\t\t*******\n");

        for(int i = 0; i < numberOfAddress; i++){

            printRecord(book[i]);

        }

        printf("\n\n\n\n\n\n\n\n");
    }
}

void printRecord(Record record){

    /*Okay. Now we need to print the values but we have a problem here. Since we want to add tabs
    * between words with \t, this will cause issues regarding to the length of words.
    * For example if name is Ali we need to type two \t because it is too short, if we use one \t the last name will shift to the left.
    * If name is Muhammed we need to type one \t because it is long so if we use two \t the last name will shift to the right.
    * So I created a function that checks the length of the text and puts \t accordingly. */

    printf("%lu", record.id);
    characterLongLong(record.id);
    printf("%s", record.firstName);
    characterLongText(record.firstName);
    printf("%s", record.lastName);
    characterLongText(record.lastName);
    printf("%s", record.phoneNum);
    characterLongText(record.phoneNum);
    printf("%s", record.address);
    characterLongText(record.address);
    printf("\n");

}

void readFromFile(FILE *file){

    //We need to read the file unless it the file is empty.
    if(isEmpty(file) == 0){

        //First we need to determine how many line it contains which means how many address it contains.
        int lineCalculator;

        do{

            lineCalculator = fgetc(file);

            //We increment the numberOfAddress by one if we encounter new line character.
            //If it is 0, we need to set it to 2 because it makes two lines.
            if(lineCalculator == '\n')
                numberOfAddress == 0 ? numberOfAddress = 2 : numberOfAddress++;

        }while(lineCalculator != EOF);

        //Now, we need to return the cursor to the beginning.
        rewind(file);

        //Now, lets create the address book.
        book = malloc(numberOfAddress * sizeof(Record));

        //Creating temporary variables.
        char input[300];
        char *strings;
        unsigned long tempId;
        char tempFirstName[32], tempLastName[32], tempPhoneNum[100], tempAddress[100];

        //Since we know how many address we have, we can loop the file and create the book.
        for(int i = 0; i < numberOfAddress; i++){

            //Read data from file.
            fgets(input, 299, file);
            tempId = strtoul(input, &strings, 10);
            sscanf(strings, "%s %s %s %s", tempFirstName, tempLastName, tempPhoneNum, tempAddress);

            book[i].id = tempId;
            strcpy(book[i].firstName, tempFirstName);
            strcpy(book[i].lastName, tempLastName);
            strcpy(book[i].phoneNum, tempPhoneNum);
            strcpy(book[i].address, tempAddress);

        }
    }
}

int isEmpty(FILE *file){

    //First we mark where the cursor is, then we move the cursor to the end of the file.
    long savedOffset = ftell(file);
    fseek(file, 0, SEEK_END);

    //If the cursor's position is 0, that means the file is empty. We return 1.
    if(ftell(file) == 0){
        return 1;
    }

    //Returning the cursor to the saved position.
    fseek(file, savedOffset, SEEK_SET);

    return 0;
}

int getNumericChoiceFromUser(char *text){

    char input[32];
    int number = 0;

    while(1){

        printf("%s: ", text);
        fgets(input, 31, stdin);

        //The first thing we need to check is if the user entered something. The user could press enter without
        //writing anything. We need to check this condition.
        if(strlen(input) < 2){
            //Means the user didn't type anything.
            printf("You need to type in order to progress.\n\n");
            continue;
        }

        //The second condition we need to check is if the user typed both numeric and non-numeric characters.
        //This is an error because we only want user to enter numeric characters.
        //But this is a little complex job.
        if(checkForBoth(input, true) == 1){
            //Means this is not we want. It contains non-numeric characters.
            printf("You can only type numeric characters.\n\n");
            continue;
        }

        number = (int)(strtol(input, NULL, 10));
        break;
    }

    return number;
}

int checkForBoth(char *input, bool NumericOrNot){

    //Now need to cast the char pointer to a char array or the while loop never ends until it hits a non-numeric character.
    char text[32];
    sscanf(input, "%s", text);

    char *c = text;

    //While c pointer points to something, we will check every character if it is numeric or not.
    while(*c){

        if(NumericOrNot == true && (*c < '0' || *c > '9'))
            return 1;
        else if(NumericOrNot == false && (*c < 'A' || *c > 'Z') && (*c < 'a' || *c > 'z') && *c != '/')
            return 1;

        c++;
    }

    //If the method reaches here, it means this input contains only numeric characters.
    return 0;
}

void characterLongText(char *text){
    if(strlen(text) >= 16){
        printf("\t");
    }else if(strlen(text) >= 8){
        printf("\t\t");
    }else{
        printf("\t\t\t");
    }
}

void characterLongLong(unsigned long input){

    char id[32];
    sprintf(id, "%lu", input);

    if(strlen(id) >= 16){
        printf("\t");
    }else if(strlen(id) >= 8){
        printf("\t\t");
    }else{
        printf("\t\t\t");
    }
}

void updateRecord(){

    //We need to check if the book contains anything.
    if(book == NULL)
        printf("\nThere is no record.\n\n\n\n\n\n\n\n");
    else{

        //Now we need to ask the user which record they want to change.
        int choice = getNumericChoiceFromUser("\nPlease enter the record number of the person you want to update");

        //Now the user typed a valid integer but we need to check if this number is recorded,
        //because you can't update a non-existing record.
        bool exists = false;
        int index = 0;

        for(int i = 0; i < numberOfAddress; i++){

            if(book[i].id == choice){
                exists = true;
                index = i;
                break;
            }
        }

        //If the exists equals to true, we found a match.
        if(exists == false){

            printf("\nThere is no such record with this given ID.\n\n\n\n\n\n\n\n");

        }else{

            //Lets print this persons info.
            printf("\n");
            printRecord(book[index]);


            //Now, lets inform the user about which values are changeable.
            printf("\n1 - Update phone number\n"
                           "2 - Update address\n");
            //Now we will need another user input.
            choice = getNumericChoiceFromUser("\nEnter your choice");

            switch(choice){

                case 1:

                    //Now we need to get the new phone number from the user.
                    getPhoneNumber("\nEnter the new phone number", book[index].phoneNum);

                    //Now, lets print the updated record.
                    printf("\n");
                    printRecord(book[index]);

                    break;
                case 2:

                    //Now we need to get the new address from the user.
                    getString("\nEnter the new address", book[index].address);

                    //Now, lets print the updated record.
                    printf("\n");
                    printRecord(book[index]);

                    break;
                default:
                    printf("You can only enter 1 or 2.\n");
                    break;

            }

            printf("\n\n\n\n\n\n\n\n");
        }
    }
}

void getPhoneNumber(char *text, char *input){

    while(1){

        printf("%s: ", text);
        fgets(input, 99, stdin);

        //The first thing we need to check is if the user entered something. The user could press enter without
        //writing anything. We need to check this condition.
        if(strlen(input) < 2){
            //Means the user didn't type anything.
            printf("You need to type in order to progress.\n\n");
            continue;
        }

        //The second condition we need to check is if the user typed both numeric and non-numeric characters.
        //This is an error because we only want user to enter numeric characters.
        //But this is a little complex job.
        if(checkForBoth(input, true) == 1){
            //Means this is not we want. It contains non-numeric characters.
            printf("You can only type numeric characters.\n\n");
            continue;
        }

        //The last thing we need to do is to remove the newline character at the end of input.
        strtok(input, "\n");

        break;
    }
}

void getString(char *text, char *input){

    while(1){

        printf("%s: ", text);
        fgets(input, 99, stdin);

        //The first thing we need to check is if the user entered something. The user could press enter without
        //writing anything. We need to check this condition.
        if(strlen(input) < 2){
            //Means the user didn't type anything.
            printf("You need to type in order to progress.\n\n");
            continue;
        }

        //The second condition we need to check is if the user typed both numeric and non-numeric characters.
        //This is an error because we only want user to enter non-numeric characters.
        //But this is a little complex job.
        if(checkForBoth(input, false) == 1){
            //Means this is not we want. It contains non-numeric characters.
            printf("You can only type non-numeric characters.\n\n");
            continue;
        }

        //The last thing we need to do is to remove the newline character at the end of input.
        strtok(input, "\n");

        break;
    }
}

void writeToFile(){

    FILE *fp = fopen("Database.txt", "w+");

    //Lets write the book to the file.
    for(int i = 0; i < numberOfAddress; i++){

        //The only thing we need to make sure is, while writing the last record don't write the newline character.
        if(i != numberOfAddress - 1)
            fprintf(fp, "%lu %s %s %s %s\n", book[i].id, book[i].firstName, book[i].lastName,
                                                book[i].phoneNum, book[i].address);
        else
            fprintf(fp, "%lu %s %s %s %s", book[i].id, book[i].firstName, book[i].lastName,
                    book[i].phoneNum, book[i].address);

    }

    fclose(fp);
}

void createRecord(){

    //First, we need to ask the user the ID of the new record.
    unsigned long newID = (unsigned long)(getNumericChoiceFromUser("\nEnter the ID number to create a new record"));

    //Now, we need to make sure this ID doesn't exist in the book.
    bool exists = false;

    if(book != NULL){

        for(int i = 0; i < numberOfAddress; i++){

            if(book[i].id == newID){

                //Means a record with this ID already created.
                printf("\nA record with this ID already created.");
                exists = true;
                break;
            }
        }
    }

    if(exists == false){

        //To add a new record, we need to add a space in the book.
        if(book == NULL){
            book = malloc(sizeof(Record));
        }else{
            Record *newBook = realloc(book, (numberOfAddress + 1) * sizeof(Record));

            //We need to check if the realloc function successfully worked.
            if(newBook)
                book = newBook;
            else{
                printf("\nReallocation failed.");
                return;
            }
        }

        //Now, we need to get the other informations from the user.
        book[numberOfAddress].id = newID;
        getString("Enter the first name", book[numberOfAddress].firstName);
        getString("Enter the last name", book[numberOfAddress].lastName);
        getPhoneNumber("Enter the phone number", book[numberOfAddress].phoneNum);
        getString("Enter the address", book[numberOfAddress].address);

        numberOfAddress++;
    }

    printf("\n\n\n\n\n\n\n\n");
}

void deleteRecord(){

    //First thing to do is to check if there is any record. If not, we don't need to do anything else.
    if(book == NULL){

        printf("\nThere is no record.\n\n\n\n\n\n\n\n");
        return;

    }

    //Now, we need to ask the user the ID which will be deleted.
    unsigned long deleteID = (unsigned long)(getNumericChoiceFromUser("\nEnter the ID number to create a new record"));

    //Now, we need to make sure this ID exists in the book.
    bool exists = false;
    int index = 0;

    for(int i = 0; i < numberOfAddress; i++){

        if(book[i].id == deleteID){

            //Means a record with this ID found.
            exists = true;
            index = i;
            break;
        }
    }

    if(exists == false){

        //Means there is no match.
        printf("\nThere is no record with this ID.");

    }else{

        //We found a match, now we need to delete it.
        //First, lets show the record which will be deleted.
        printRecord(book[index]);

        //Now, we will create a temporary book with decreased size by one.
        Record *tempBook = malloc((numberOfAddress - 1) * sizeof(Record));

        //Copy all the contents except the deleted one from book to the temp book.
        for(int i = 0, correctIndex = 0; i < numberOfAddress; i++){

            if(i != index){

                //While we're not on the deleted one...
                tempBook[correctIndex].id = book[i].id;
                strcpy(tempBook[correctIndex].firstName, book[i].firstName);
                strcpy(tempBook[correctIndex].lastName, book[i].lastName);
                strcpy(tempBook[correctIndex].phoneNum, book[i].phoneNum);
                strcpy(tempBook[correctIndex].address, book[i].address);

                correctIndex++;
            }
        }

        //So now, we copied all except the deleted one to our new tempBook. Now, lets free the old book.
        free(book);

        //And set the book pointer to the temp book.
        book = tempBook;

        //Deletion is completed.
        printf("Deleted.");
        numberOfAddress--;

        //The last thing we need to check, if all the records are deleted, we need to set the book to NULL.
        if(numberOfAddress == 0)
            book = NULL;
    }

    printf("\n\n\n\n\n\n\n\n");
}
