/**
 * @file professor.c
 * @brief Professor management module for Student Information System
 * 
 * This module handles all professor-related operations including:
 * - Loading professor data from CSV files
 * - Adding, updating, and deleting professors
 * - Searching professors by ID, email, or name
 * - Saving professor data back to CSV files
 * 
 * @author Student Information System Team
 * @date January 2026
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "professor.h"
#include "utils.h"

/**
 * @brief Loads professor data from a CSV file into memory
 * 
 * Reads a CSV file with the format:
 * professor_id,first_name,last_name,email,phone,department,title,office
 * 
 * Validates each record for:
 * - Valid professor ID (1001-9998)
 * - Valid email format (name.surname@university.edu)
 * - Valid phone format (555-XXXX)
 * 
 * @param filename Path to the CSV file
 * @param professors Pointer to the professor array (will be allocated)
 * @param count Pointer to store the number of professors loaded
 * @return 1 if successful, 0 if file cannot be opened or memory allocation fails
 */
int load_professors(const char *filename, Professor **professors, int *count) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        print_error("Could not open professors file.");
        return 0;
    }

    char line[MAX_LINE_LEN];
    char *fields[8];
    int capacity = 10;
    int line_number = 0;

    *count = 0;
    *professors = NULL;


    if (fgets(line, sizeof(line), file) == NULL) {
        fclose(file);
        return 1; 
    }
    line_number++;

    *professors = (Professor *)malloc(capacity * sizeof(Professor));
    if (!*professors) {
        print_error("Initial memory allocation failed.");
        fclose(file);
        return 0;
    }

    while (fgets(line, sizeof(line), file)) {
        line_number++;
        line[strcspn(line, "\r\n")] = '\0';
        if (line[0] == '\0') continue;

        int field_count = split_csv_line(line, fields, 8);
        
       
        if (field_count != 8) {
            fprintf(stderr, "[ERROR] File: %s, Line: %d -> Invalid format. Expected 8 columns, found %d. Skipped.\n", filename, line_number, field_count);
            continue;
        }

        // --- VALIDATION START ---
        int temp_id = atoi(fields[0]);
        char *temp_email = fields[3];
        char *temp_phone = fields[4];

        // Validate ID using validate_professor_id
        if (!validate_professor_id(temp_id)) {
            fprintf(stderr, "[ERROR] Line %d: Invalid Professor ID (%d). Must be 5000-9999. Skipped.\n", line_number, temp_id);
            continue;
        }

        // Validate Email
        trim_whitespace(temp_email);
        if (!validate_email(temp_email)) {
            fprintf(stderr, "[ERROR] Line %d: Invalid Email Format (%s). Skipped.\n", line_number, temp_email);
            continue;
        }

        // Validate Phone
        trim_whitespace(temp_phone);
        if (!validate_phone(temp_phone)) {
            fprintf(stderr, "[ERROR] Line %d: Invalid Phone Format (%s). Skipped.\n", line_number, temp_phone);
            continue;
        }
        // --- VALIDATION END ---

    
        if (*count >= capacity) {
            int new_capacity = capacity * 2;
            Professor *temp = (Professor *)realloc(*professors, new_capacity * sizeof(Professor));
            if (!temp) {
                print_error("Memory reallocation failed. Could not expand array.");
                free(*professors);
                *professors = NULL;
                *count = 0;
                fclose(file);
                return 0;
            }
            *professors = temp;
            capacity = new_capacity;
        }

        Professor *p = &(*professors)[*count];

        p->professor_id = temp_id;

        strncpy(p->first_name, fields[1], MAX_NAME_LEN - 1); p->first_name[MAX_NAME_LEN - 1] = '\0';
        trim_whitespace(p->first_name);
        format_name(p->first_name);

        strncpy(p->last_name, fields[2], MAX_NAME_LEN - 1); p->last_name[MAX_NAME_LEN - 1] = '\0';
        trim_whitespace(p->last_name);
        format_name(p->last_name);

        strcpy(p->email, temp_email);
        strcpy(p->phone, temp_phone);

        strncpy(p->department, fields[5], MAX_DEPT_LEN - 1); p->department[MAX_DEPT_LEN - 1] = '\0';
        trim_whitespace(p->department);
        format_name(p->department);

        strncpy(p->title, fields[6], MAX_TITLE_LEN - 1); p->title[MAX_TITLE_LEN - 1] = '\0';
        trim_whitespace(p->title);
        format_name(p->title);

        strncpy(p->office, fields[7], MAX_OFFICE_LEN - 1); p->office[MAX_OFFICE_LEN - 1] = '\0';
        trim_whitespace(p->office);

        (*count)++;
    }

    fclose(file);

    if (*count == 0) {
        free(*professors);
        *professors = NULL;
    }

    printf(">> Professors Loaded Successfully: %d (Check console for any line errors).\n", *count);
    return 1;
}

/**
 * @brief Searches for a professor by their unique ID
 * 
 * Performs a linear search through the professor array.
 * 
 * @param professors Array of Professor structures
 * @param count Total number of professors in the array
 * @param id The professor ID to search for
 * @return Index of the professor if found, -1 otherwise
 */
int search_professor_by_id(Professor *professors, int count, int id) {
    if (professors == NULL || count <= 0) return -1;
    for (int i = 0; i < count; i++) {
        if (professors[i].professor_id == id)
            return i;
    }
    return -1;
}

/**
 * @brief Searches for professors by name and prints matching results
 * 
 * Performs case-insensitive search on first name, last name, or full name.
 * Prints all matching professors to stdout.
 * 
 * @param professors Array of Professor structures
 * @param count Total number of professors
 * @param name The name string to search for (first, last, or full name)
 */
void search_professor_by_name(Professor *professors, int count) {
    if (professors == NULL || count <= 0) {
        printf("No professors loaded.\n");
        return;
    }
    
    char search_name[100];
    get_string_input("Enter professor name to search: ", search_name, 100);
    trim_whitespace(search_name);
    
    if (strlen(search_name) == 0) {
        print_error("Search term cannot be empty.");
        return;
    }
    
    int found_count = 0;
    char full_name[150];
    
    printf("\n--- Search Results for '%s' ---\n", search_name);
    printf("%-6s %-20s %-30s %-15s\n", "ID", "Name", "Email", "Department");
    printf("------------------------------------------------------------------------\n");
    
    for (int i = 0; i < count; i++) {
        snprintf(full_name, sizeof(full_name), "%s %s", 
                 professors[i].first_name, professors[i].last_name);
        
        /* Check first name, last name, or full name (case-insensitive) */
        if (str_equals_case_insensitive(professors[i].first_name, search_name) ||
            str_equals_case_insensitive(professors[i].last_name, search_name) ||
            str_equals_case_insensitive(full_name, search_name)) {
            
            printf("%-6d %-20s %-30s %-15s\n",
                   professors[i].professor_id,
                   full_name,
                   professors[i].email,
                   professors[i].department);
            found_count++;
        }
    }
    
    printf("------------------------------------------------------------------------\n");
    if (found_count == 0) {
        printf("No professors found matching '%s'.\n", search_name);
    } else {
        printf("Total matches found: %d\n", found_count);
    }
}

/**
 * @brief Updates professor information via interactive menu
 * 
 * Displays a menu allowing the user to update various professor fields.
 * Validates all input before saving changes.
 * 
 * @param professors Array of Professor structures
 * @param count Total number of professors
 * @param id The ID of the professor to update
 */
void update_professor(Professor *professors, int count, int id) {
    int index = search_professor_by_id(professors, count, id);
    if (index == -1) {
        print_error("Professor not found with this ID.");
        return;
    }

    Professor *p = &professors[index];
    int choice;
    char buffer[MAX_LINE_LEN];

    do {
        printf("\n--- Update Menu for %s %s (%d) ---\n", p->first_name, p->last_name, p->professor_id);
        printf("1. Update Name\n");
        printf("2. Update Email (Current: %s)\n", p->email);
        printf("3. Update Phone (Current: %s)\n", p->phone);
        printf("4. Update Department (Current: %s)\n", p->department);
        printf("5. Update Title (Current: %s)\n", p->title);
        printf("6. Update Office (Current: %s)\n", p->office);
        printf("0. Back\n");
        printf("Choice: ");

        if (scanf("%d", &choice) != 1) {
            clear_input_buffer();
            continue;
        }
        clear_input_buffer();

        switch (choice) {
            case 1:
                get_string_input("Enter New First Name: ", buffer, MAX_NAME_LEN);
                trim_whitespace(buffer);
                format_name(buffer);
                if (strlen(buffer) > 0) strcpy(p->first_name, buffer);

                get_string_input("Enter New Last Name: ", buffer, MAX_NAME_LEN);
                trim_whitespace(buffer);
                format_name(buffer);
                if (strlen(buffer) > 0) strcpy(p->last_name, buffer);
                break;

            case 2:
                get_string_input("Enter New Email: ", buffer, MAX_EMAIL_LEN);
                trim_whitespace(buffer);
                if (validate_email(buffer)) strcpy(p->email, buffer);
                break;

            case 3:
                get_string_input("Enter New Phone (555-XXXX): ", buffer, MAX_PHONE_LEN);
                trim_whitespace(buffer);
                if (validate_phone(buffer)) {
                    strncpy(p->phone, buffer, MAX_PHONE_LEN - 1);
                    p->phone[MAX_PHONE_LEN - 1] = '\0';
                }
                break;

            case 4:
                get_string_input("Enter New Department: ", buffer, MAX_DEPT_LEN);
                trim_whitespace(buffer);
                if (strlen(buffer) > 0) strcpy(p->department, buffer);
                break;

            case 5:
                get_string_input("Enter New Title: ", buffer, MAX_TITLE_LEN);
                trim_whitespace(buffer);
                if (strlen(buffer) > 0) strcpy(p->title, buffer);
                break;

            case 6:
                get_string_input("Enter New Office: ", buffer, MAX_OFFICE_LEN);
                trim_whitespace(buffer);
                if (strlen(buffer) > 0) strcpy(p->office, buffer);
                break;

            case 0:
                break;

            default:
                print_error("Invalid menu option. Please enter 0-6.");
        }
    } while (choice != 0);
}

/**
 * @brief Deletes a professor from the system
 * 
 * Removes the professor with the given ID from the array.
 * Reallocates memory to shrink the array after deletion.
 * 
 * @param professors Pointer to the professor array
 * @param count Pointer to the professor count (will be decremented)
 * @param id The ID of the professor to delete
 * @return 1 if successful, 0 if professor not found
 */
int delete_professor(Professor **professors, int *count, int id) {
    int index = search_professor_by_id(*professors, *count, id);
    if (index == -1) {
        print_error("Professor ID not found.");
        return 0;
    }

    for (int i = index; i < *count - 1; i++) {
        (*professors)[i] = (*professors)[i + 1];
    }

    (*count)--;

    if (*count > 0) {
        Professor *temp = (Professor *)realloc(*professors, (*count) * sizeof(Professor));
        if (temp != NULL) *professors = temp;
    } else {
        free(*professors);
        *professors = NULL;
    }

    return 1;
}

/**
 * @brief Displays all professors in a formatted table
 * 
 * Prints a table with professor ID, name, email, department, and title.
 * 
 * @param professors Array of Professor structures
 * @param count Total number of professors
 */
void print_professor(Professor *professors, int count) {
    if (professors == NULL || count <= 0) {
        printf("\nNo professors in the system. Please load data first.\n");
        return;
    }

    printf("\n%-5s %-20s %-25s %-15s %-15s\n", "ID", "Name", "Email", "Dept", "Title");
    printf("------------------------------------------------------------------------------------\n");

    for (int i = 0; i < count; i++) {
        char full_name[MAX_NAME_LEN * 2];
        snprintf(full_name, sizeof(full_name), "%s %s", professors[i].first_name, professors[i].last_name);

        printf("%-5d %-20s %-25s %-15s %-15s\n",
            professors[i].professor_id,
            full_name,
            professors[i].email,
            professors[i].department,
            professors[i].title
        );
    }
    printf("------------------------------------------------------------------------------------\n");
    printf("Total Professors: %d\n", count);
}

/**
 * @brief Frees memory allocated for the professor array
 * 
 * @param professors Pointer to the professor array to free
 */
void free_professors(Professor *professors) {
    if (professors != NULL) free(professors);
}

/**
 * @brief Searches for a professor by email address
 * 
 * Performs exact match search on email addresses.
 * 
 * @param professors Array of Professor structures
 * @param count Total number of professors
 * @param email The email address to search for
 * @return Index of the professor if found, -1 otherwise
 */
int search_professor_by_email(Professor *professors, int count, const char *email) {
    if (professors == NULL || count <= 0) return -1;
    for (int i = 0; i < count; i++) {
        if (strcmp(professors[i].email, email) == 0)
            return i;
    }
    return -1;
}

/**
 * @brief Adds a new professor to the system
 * 
 * Prompts the user for all professor information, validates input,
 * checks for duplicate ID and email, then adds to the array.
 * 
 * @param professors Pointer to the professor array (may be reallocated)
 * @param count Pointer to the professor count (will be incremented)
 * @return 1 if successful, 0 if validation fails or memory allocation fails
 */
int add_professor(Professor **professors, int *count) {
    Professor new_prof;

    /* Get and validate professor ID */
    do {
        printf("Enter Professor ID (5000-9999): ");
        if (scanf("%d", &new_prof.professor_id) != 1) {
            clear_input_buffer();
            printf("Error: ID must be a number.\n");
            continue;
        }
        clear_input_buffer();
        if (!validate_professor_id(new_prof.professor_id)) {
            printf("Please enter a valid Professor ID (5000-9999).\n");
        }
    } while (!validate_professor_id(new_prof.professor_id));

    if (search_professor_by_id(*professors, *count, new_prof.professor_id) != -1) {
        print_error("Professor with this ID already exists!");
        return 0;
    }

    get_string_input("First Name: ", new_prof.first_name, MAX_NAME_LEN);
    trim_whitespace(new_prof.first_name);
    format_name(new_prof.first_name);

    get_string_input("Last Name: ", new_prof.last_name, MAX_NAME_LEN);
    trim_whitespace(new_prof.last_name);
    format_name(new_prof.last_name);

    get_string_input("Email: ", new_prof.email, MAX_EMAIL_LEN);
    trim_whitespace(new_prof.email);
    if (!validate_email(new_prof.email)) return 0;

    /* Check for duplicate email address */
    if (search_professor_by_email(*professors, *count, new_prof.email) != -1) {
        printf("Error: The email '%s' is already registered to another professor.\n", new_prof.email);
        return 0;
    }

    get_string_input("Phone (555-XXXX): ", new_prof.phone, MAX_PHONE_LEN);
    trim_whitespace(new_prof.phone);
    if (!validate_phone(new_prof.phone)) return 0;

    get_string_input("Department: ", new_prof.department, MAX_DEPT_LEN);
    trim_whitespace(new_prof.department);
    format_name(new_prof.department);

    get_string_input("Title: ", new_prof.title, MAX_TITLE_LEN);
    trim_whitespace(new_prof.title);
    format_name(new_prof.title);

    get_string_input("Office: ", new_prof.office, MAX_OFFICE_LEN);
    trim_whitespace(new_prof.office);

    Professor *temp = (Professor *)realloc(*professors, (*count + 1) * sizeof(Professor));
    if (!temp) {
        print_error("Memory allocation failed! Could not add professor.");
        return 0;
    }

    *professors = temp;
    (*professors)[*count] = new_prof;
    (*count)++;

    return 1;
}

/**
 * @brief Saves all professor data to a CSV file
 * 
 * Writes all professors to the specified file in CSV format.
 * Overwrites any existing file content.
 * 
 * @param filename Path to the output CSV file
 * @param professors Array of Professor structures
 * @param count Total number of professors
 * @return 1 if successful, 0 if file cannot be opened
 */
int save_professors(const char *filename, Professor *professors, int count) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        print_error("Could not open file for saving professors.");
        return 0;
    }

    fprintf(file, "professor_id,first_name,last_name,email,phone,department,title,office\n");

    for (int i = 0; i < count; i++) {
        fprintf(file, "%d,%s,%s,%s,%s,%s,%s,%s\n",
                professors[i].professor_id,
                professors[i].first_name,
                professors[i].last_name,
                professors[i].email,
                professors[i].phone,
                professors[i].department,
                professors[i].title,
                professors[i].office);
    }

    fclose(file);
    return 1;
}