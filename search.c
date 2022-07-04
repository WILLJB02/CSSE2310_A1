#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_WORD_LENGTH 42

/* An Enum that defines all the possible search arguments within can be
 * specified in the program.
 */
typedef enum SearchArgument {
    EXACT, PREFIX, ANYWHERE, ERROR
} SearchType;

/* Function which determines how many option arguments (which begin with '-') 
 * the user has entered when running the program.
 * Parameters: 
 * argc - number of command arguments entered when running program.
 * argv - array containing the command arguments
 * Return:
 * int - number of command arguments which are option arguments. 
 */
int number_option_arugments(int argc, char** argv);

/* Function which determines weather a command argument is "-sort" (the 
 * argument to enable sort functionailty).
 * Paramaters:
 * commandArgument - command argument to be checked
 * Return:
 * bool - true if the command argument is "-sort", false otherwise.
 */
bool check_sort(char* commandArgument);

/* Function which returns the index of the "-sort" function when two option
 * arguments are present. If "-sort" is not present -1 is returned.
 * Paramters:
 * firstArgument - first option argument which the user inputted.
 * secondArgument - second option argument which the user entered.
 * Return:
 * int - index of the "-sort" function in the command arguments.
 * -1 if "-sort" is not present.
 */
int find_sort(char* firstArgument, char* secondArgument);

/* Function which prints an error message to stderr for when the pattern
 * presented by the user contains characters which are not alphabetic nor a
 * question mark.
 */
void pattern_error();

/* Function which return the Enum value assoicated with the search argument
 * presented by the user.
 * Paramters:
 * searchMode - search argument entered by the user.
 * Return:
 * SearchType - Enum representing the entered search argument.
 */
SearchType check_search_mode(char* searchMode);

/* Function which prints an error message to stderr for when there is an 
 * issue with the command argument format entered by the user.
 */
void command_line_format_error();

/* Function which prints an error message to stderr for when the filename 
 * entered by the user cannot be opened. 
 */
void file_error();

/* Function which determines whether a pattern entered by the user
 * contains only question marks or alphabetic characters.
 * Paramaters:
 * searchPattern - pattern entered by the user.
 * Return:
 * bool - true if the entered pattern only contains question marks or 
 * alphabetic characters, false otherwise.
 */
bool check_pattern(char* searchPattern);

/* Function which determines weather the given pattern matches a subtring of 
 * equal size located within a word read from a dictionary starting from a
 * specified index. Also ensures that if EXACT search is being used that the
 * string size of the file word is the same size as the entered pattern.
 * Parameters:
 * searchPattern - pattern entered by user.
 * dictionaryWord - word read from dictionary.
 * startingIndex - index from which the substring starts in the dictionary
 * word.
 * searchType - search mode that the program will run under as
 * specified in the command line by the user.
 * Return:
 * bool - true if the pattern matched a substring in the dictionaryWord
 * (and is the same length as the file word if using exact search), 
 * false otherwise.
 */
bool check_pattern_match(char* searchPattern, char* dictionaryWord, 
        int startingIndex, SearchType searchType);

/* A function which prints the matched words from the dictionary to the
 * standard output based on the search mode and whether sort mode has 
 * been enabled. After doing so it returns the number of matched words which
 * have been printed. 
 * Parameters: 
 * searchPattern - pattern entered by user.
 * dictionaryWord - word read from file.
 * startingIndex - inded from which the substring starts in the dictionary
 * searchType - search mode that the program will run under as
 * specified in the command line by the user.
 * Returns:
 * int - the number of words in the dictionary which match the given pattern.
 */
int pattern_search(char* searchPattern, char* filename, 
        bool sortModeEnabled, SearchType searchType);

/* Function which determines the number of option arguments is viable given
 * the total number of command arguments entered by the user.
 * eg: if only two command arguments are entered by the user, and two 
 * option arguments were deteceted, the function would determine this
 * combination as invalid.
 * Paramaters:
 * argc - number of program arguments
 * argv - array which stores the entered program arguments
 * optionArgumentCount - number of option arguments entered by the user.
 * Return:
 * bool - true if option/ command argument combination is invalid, false 
 * otherwise. 
 */
bool incorrect_argument_number(int argc, char** argv,
        int optionArgumentCount) {
    return ((argc == 2 && optionArgumentCount != 0) ||
            (argc == 3 && optionArgumentCount > 1) ||
            (argc == 4 && (optionArgumentCount == 0 ||
            optionArgumentCount > 2 )) || 
            (argc == 5 && optionArgumentCount != 2) ||
            argc > 5 || argc < 2);
}

/* Function which determines if a invalid command line has be deteced. 
 * If an invalid command line has been deteced than the function for the
 * appropriate error message is called. 
 * Paramaters:
 * searchPattern - pattern entered by the user.
 * searchMode - search mode which has been read from option 
 * arguments in the inputed command line.
 * dicitonary - file which has been opened.
 * filename - name of file which has been opened.
 * Return:
 * bool - true if an ivalid command line has been deteceted by the progam, 
 * false otherwise.
 */
bool file_structure_error(char* searchPattern, SearchType searchMode,
        FILE* dictionary, char* filename) {
    if (searchMode == ERROR) {
        command_line_format_error();
        return true;
    } else if (dictionary == 0) {
        file_error(filename);
        return true;
    } else if (check_pattern(searchPattern) == false) {
        pattern_error();
        return true;
    }
    return false;
}

int main(int argc, char** argv) {
    // assigns deafult values to argument variables
    SearchType searchMode = EXACT;
    bool sortModeEnabled = false;
    char* filename = "/usr/share/dict/words";
    char* searchPattern;
    int sortArgumentPosition;
    int optionArgumentCount = number_option_arugments(argc, argv);

    //  Assigns users option arguments to variables if command line is valid
    if (incorrect_argument_number(argc, argv, optionArgumentCount) == true) {
        command_line_format_error();
        return 1;
    } else if (optionArgumentCount == 1) {
        sortModeEnabled = check_sort(argv[1]);
        if (sortModeEnabled == false) {
            searchMode = check_search_mode(argv[1]);
        }
    } else if (optionArgumentCount == 2) {
        sortArgumentPosition = find_sort(argv[1], argv[2]);
        if (sortArgumentPosition == -1) {
            command_line_format_error();
            return 1;
        } 
        sortModeEnabled = check_sort(argv[sortArgumentPosition]);
        searchMode = check_search_mode(argv[-1 * sortArgumentPosition + 3]);   
    }

    searchPattern = argv[optionArgumentCount + 1];
    // assigns filename to a variable if correct number of arguments is present
    if (optionArgumentCount == argc - 3) {
        filename = argv[argc - 1];
    }

    FILE* dictionary = fopen(filename, "r");
    if (file_structure_error(searchPattern, searchMode, dictionary, filename)
            == true) {
        return 1;
    } 
    fclose(dictionary);

    int matchedWordCounter = pattern_search(searchPattern, filename, 
            sortModeEnabled, searchMode);
    if (matchedWordCounter == 0) {
        return 1;
    }
    return 0;
}

void pattern_error() {
    fprintf(stderr, "search: pattern should only contain question marks and "
            "letters\n");
}

void command_line_format_error(){
    fprintf(stderr, "Usage: search [-exact|-prefix|-anywhere] [-sort] pattern "
            "[filename]\n");
}

int find_sort(char* firstArgument, char* secondArgument) {
    char sortMode[] = "-sort";
    if (strcmp(firstArgument, sortMode) == 0) {
        return 1;    
    } else if (strcmp(secondArgument, sortMode) == 0) {
        return 2;
    } else {
        return -1;
    }   
}

bool check_sort(char* commandArgument) {
    char sortMode[] = "-sort";
    if (strcmp(commandArgument, sortMode) == 0) {
        return true;    
    } else {
        return false;
    }
}

int number_option_arugments(int argc, char** argv) {
    int optionArgumentCount = 0;
    //iterating through all program
    //arguments and checking if the strings begin with '-'
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-' && i < 4) {
            optionArgumentCount++;
        }   
    } 
    return optionArgumentCount;
}

SearchType check_search_mode(char* searchMode) {
    char exactMode[] = "-exact";
    char prefixMode[] = "-prefix";
    char anywhereMode[] = "-anywhere";
    if (strcmp(searchMode, exactMode) == 0) {
        return EXACT;
    } else if (strcmp(searchMode, prefixMode) == 0) {
        return PREFIX;
    } else if (strcmp(searchMode, anywhereMode) == 0) {
        return ANYWHERE;   
    } else {
        return ERROR;
    }
}

/* Function which determines if a word read from the file only contians 
 * alphabetic characters.
 * Paramaters:
 * dictionaryWord - word read from dictionary.
 * Return:
 * bool - true if diciontary word only contians alphabetic characerts, false 
 * otherwise.
 */
bool word_alphabetic(char* dictionaryWord) {
    for (int i = 0; i < (strlen(dictionaryWord) - 1); i++) {
        if (isalpha(dictionaryWord[i]) == 0) {
            return false;   
        } 
    }
    return true;
}

bool check_pattern(char* pattern) {
    // iterating through each character in pattern and ensuring its either
    // alphabetic or a question mark
    for (int i = 0; i < strlen(pattern); i++) {
        if ((isalpha(pattern[i]) == 0 && pattern[i] != '?') ||
                pattern[i] == ' ') {
            return false;
        } 
    }
    return true;
}

void file_error(char* filename) {
    char prefix[] = "search: file \"";
    char suffix[] = "\" can not be opened\n";
    char* errorMessage = (char *)malloc(sizeof(prefix) + strlen(filename) + 
            sizeof(suffix) + 1);
    // combining all strings into error message string
    strcpy(errorMessage, prefix);
    strcat(filename, suffix);
    strcat(errorMessage, filename);
    fprintf(stderr, errorMessage);
    free(errorMessage);
}

/*
 * Function which compares two arguments (generally strings) character by 
 * character using strcmp. 
 * Paramters:
 * firstElement - first element to be compared.
 * secondElement - second element to be comapred.
 * Return:
 * int - 0 if the two elements are equal. >0 if the first argument occurs 
 * alphabetically before the second argument. <0 otherwise. 
 */
static int cmp_str(const void* firstElement, const void* secondElement) {
    char* const* str1 = firstElement;
    char* const* str2 = secondElement;
    return strcasecmp(*str1, *str2);
}

bool check_pattern_match(char* searchPattern, char* dictionaryWord, 
        int startingIndex, SearchType searchType) {
    if (word_alphabetic(dictionaryWord) == false) {
        return false;    
    } else if (searchType == EXACT && strlen(searchPattern) == 0) {
        return false;
    }
    // iterating through each character in the pattern and checking if the
    // corresponding character in the dictionary word matches 
    // (if the pattern character isnt a question mark)
    for (int i = 0; i < strlen(searchPattern); i++) {                   
        char dictionary = tolower(dictionaryWord[startingIndex + i]);
        char pattern = tolower(searchPattern[i]);      
        if (searchType == EXACT && ((pattern != '?' && pattern
                != dictionary)         
                || (strlen(searchPattern) != strlen(dictionaryWord) - 1))) {  
            return false;       
        } else if (searchType != EXACT && ((pattern != '?' && pattern
                != dictionary)         
                || (strlen(searchPattern) > strlen(dictionaryWord) - 1))) {  
            return false;       
        }                                         
    }       
    return true; 
}

/*
 * Function which determines if a dictionary word matches the given pattern
 * under a given search type.
 * Parameters:
 * searchPattern - pattern inputted by user.
 * dictionaryWord - word read from dictionary.
 * searchType - search mode program is being excuted in.
 * Return:
 * bool - true if the word from the dicitonary matches the pattern under the
 * given search type,  false otherwise
 */
bool matched_word(char* searchPattern, char* dictionaryWord,
        SearchType searchType) {
    bool matchedSection = false;        
    if (searchType == ANYWHERE) {
        // iterate through every starting index for a substring of the same 
        // length as searchPattern and then determining whether that 
        // substring matches the pattern
        int maxIndex = strlen(dictionaryWord) - strlen(searchPattern);
        for (int j = 0; j < maxIndex; j++) {             
            matchedSection = check_pattern_match(searchPattern,
                    dictionaryWord, j, searchType);                   
            if (matchedSection == true) {                    
                return matchedSection;                                  
            }                                                            
        }                             
    } else { 
        // checking if the substring at the beginning of the word matches the
        // pattern
        matchedSection = check_pattern_match(searchPattern, dictionaryWord, 0,
                searchType);         
    } 
    return matchedSection;
}

int pattern_search(char* searchPattern, char* filename, 
        bool sortModeEnabled, SearchType searchType) {       
    char dictionaryWord[MAX_WORD_LENGTH];           
    int matchedWordCounter = 0;   
    FILE* dictionary = fopen(filename, "r");

    // iterating through diciontary counting how many words match the pattern
    while (fgets(dictionaryWord, sizeof(dictionaryWord), dictionary) != NULL) {
        if (matched_word(searchPattern, dictionaryWord, searchType) == true) {
            matchedWordCounter++;  
        }                            
    }

    // re-opening file and allocating appropriate memory for array
    fclose(dictionary); 
    dictionary = fopen(filename, "r");
    char** matchedWords = (char**) malloc(sizeof(char*) * matchedWordCounter);
    matchedWordCounter = 0;

    // iterating through dictionary again, this time adding matched words to 
    // the above created array
    while (fgets(dictionaryWord, sizeof(dictionaryWord), dictionary) != NULL) {
        if (matched_word(searchPattern, dictionaryWord, searchType) == true) { 
            matchedWords[matchedWordCounter] = (char*)malloc(sizeof(char*) *
                    MAX_WORD_LENGTH);
            strcpy(matchedWords[matchedWordCounter], dictionaryWord); 
            matchedWordCounter++;          
        }  
    } 

    // sorting matched words if -sort was added in command line   
    if (sortModeEnabled == true) { 
       qsort(matchedWords, matchedWordCounter,
               sizeof(matchedWords), cmp_str);
    } 
    for (int i = 0; i < matchedWordCounter; i++) {
        printf("%s", matchedWords[i]);        
    }
    return matchedWordCounter;
}
