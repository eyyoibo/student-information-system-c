/**
 * @file student.c
 * @brief Student management module for Student Information System
 * 
 * This module handles all student-related operations including:
 * - Loading student data from CSV files
 * - Adding, updating, and deleting students
 * - Searching students by ID, email, or name
 * - Saving student data back to CSV files
 * 
 * @author Student Information System Team
 * @date January 2026
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "student.h"
#include "utils.h"
#include "enrollment.h"
#include "grade.h"

/**
 * @brief Loads student data from a CSV file into memory
 * 
 * Reads a CSV file with the format:
 * student_id,first_name,last_name,email,phone,enrollment_year,major,gpa
 * 
 * Validates each record for:
 * - Valid student ID format (YYYYXXX where YYYY is 2000-2026)
 * - Valid name format (no numbers or special characters)
 * - Valid major format
 * 
 * @param filename Path to the CSV file
 * @param students Pointer to the student array (will be allocated)
 * @param count Pointer to store the number of students loaded
 * @return 1 if successful, 0 if file cannot be opened or memory allocation fails
 */
int load_students(const char *filename, Student **students, int *count) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        print_error("Could not open students file.");
        return 0;
    }

    char line[MAX_LINE_LEN];
    char *fields[8];
    int capacity = 10;
    int line_number = 0;

    *count = 0;
    *students = NULL;

    /* Skip the header line */
    if (fgets(line, sizeof(line), file) == NULL) {
        fclose(file);
        return 1; 
    }
    line_number++;

    *students = (Student *)malloc(capacity * sizeof(Student));
    if (!*students) {
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
            fprintf(stderr, "[ERROR] Line %d: Invalid column count. Skipped.\n", line_number);
            continue;
        }
        
        /* --- VALIDATION START --- */
        int temp_id = atoi(fields[0]);
        char *temp_fname = fields[1];
        char *temp_lname = fields[2];
        char *temp_major = fields[6]; /* Major is also a name, must be validated */

        trim_whitespace(temp_fname);
        trim_whitespace(temp_lname);
        trim_whitespace(temp_major);

        /* 1. ID Validation */
        if (!validate_student_id(temp_id)) {
            fprintf(stderr, "[ERROR] Line %d: Invalid Student ID (%d). Skipped.\n", line_number, temp_id);
            continue;
        }

        /* 2. First Name Validation (Cannot contain numbers) */
        if (!validate_name_string(temp_fname)) {
            fprintf(stderr, "[ERROR] Line %d: Invalid First Name '%s'. (Contains numbers/symbols). Skipped.\n", line_number, temp_fname);
            continue;
        }

        /* 3. Last Name Validation (Cannot contain numbers) */
        if (!validate_name_string(temp_lname)) {
            fprintf(stderr, "[ERROR] Line %d: Invalid Last Name '%s'. (Contains numbers/symbols). Skipped.\n", line_number, temp_lname);
            continue;
        }

        /* 4. Major Validation (Optional but recommended) */
        if (!validate_name_string(temp_major)) {
            fprintf(stderr, "[ERROR] Line %d: Invalid Major Name '%s'. Skipped.\n", line_number, temp_major);
            continue;
        }
        /* --- VALIDATION END --- */

        if (*count >= capacity) {
            int new_capacity = capacity * 2;
            Student *temp = (Student *)realloc(*students, new_capacity * sizeof(Student));
            if (!temp) {
                print_error("Memory reallocation failed.");
                free(*students);
                *students = NULL;
                *count = 0;
                fclose(file);
                return 0;
            }
            *students = temp;
            capacity = new_capacity;
        }

        Student *s = &(*students)[*count];
        s->student_id = temp_id;
        
        strncpy(s->first_name, temp_fname, MAX_NAME_LEN - 1); s->first_name[MAX_NAME_LEN - 1] = '\0';
        trim_whitespace(s->first_name);
        format_name(s->first_name);
        
        strncpy(s->last_name, temp_lname, MAX_NAME_LEN - 1); s->last_name[MAX_NAME_LEN - 1] = '\0';
        trim_whitespace(s->last_name);
        format_name(s->last_name);
        
        strncpy(s->email, fields[3], MAX_EMAIL_LEN - 1); s->email[MAX_EMAIL_LEN - 1] = '\0';
        strncpy(s->phone, fields[4], MAX_PHONE_LEN - 1); s->phone[MAX_PHONE_LEN - 1] = '\0';
        
        s->enrollment_year = atoi(fields[5]);
        
        strncpy(s->major, temp_major, MAX_MAJOR_LEN - 1); s->major[MAX_MAJOR_LEN - 1] = '\0';
        trim_whitespace(s->major);
        format_name(s->major);
        
        s->gpa = (float)atof(fields[7]);

        (*count)++;
    }

    fclose(file);
    printf(">> Students Loaded Successfully: %d\n", *count);
    return 1;
}

/**
 * @brief Searches for a student by ID
 * 
 * Performs linear search through the student array.
 * 
 * @param students Array of students to search
 * @param count Number of students
 * @param id The student ID to find
 * @return Index of the student if found, -1 otherwise
 */
int search_student_by_id(Student *students, int count, int id) {
    if (students == NULL || count <= 0) return -1;

    for (int i = 0; i < count; i++) {
        if (students[i].student_id == id)
            return i;
    }
    return -1;
}

/**
 * @brief Searches and prints students by name (case-insensitive)
 * 
 * Searches by first name or full name and prints matching results.
 * 
 * @param students Array of students
 * @param count Number of students
 * @param name Name to search for
 * @return Number of students found
 */
int search_and_print_students_by_name(Student *students, int count, const char *name) {
    if (students == NULL || count <= 0) return 0;

    int found_count = 0;
    char full_name[150];

    printf("\n--- Search Results for '%s' ---\n", name);

    for (int i = 0; i < count; i++) {
        sprintf(full_name, "%s %s", students[i].first_name, students[i].last_name);

        if (str_equals_case_insensitive(students[i].first_name, name) || 
            str_equals_case_insensitive(full_name, name)) {
            print_student(students[i]); 
            found_count++;
        }
    }
    return found_count; 
}

/**
 * @brief Searches for a student by email address
 * 
 * @param students Array of students
 * @param count Number of students
 * @param email Email to search for
 * @return Index of the student if found, -1 otherwise
 */
int search_student_by_email(Student *students, int count, const char *email) {
    if (students == NULL || count <= 0) return -1;

    for (int i = 0; i < count; i++) {
        if (strcmp(students[i].email, email) == 0) {
            return i;
        }
    }
    return -1;
}

/**
 * @brief Updates student details via interactive menu
 * 
 * Provides a menu to update individual fields of a student
 * including name, email, phone, major, and GPA.
 * 
 * @param students Array of students
 * @param count Number of students
 * @param id ID of the student to update
 */
void update_student(Student *students, int count, int id) {
    int index = search_student_by_id(students, count, id);
    if (index == -1) {
        print_error("Student not found with this ID.");
        return;
    }

    Student *s = &students[index];
    int choice;
    char buffer[MAX_LINE_LEN];

    do {
        printf("\n--- Update Menu for %s %s (%d) ---\n", s->first_name, s->last_name, s->student_id);
        printf("1. Update Name\n");
        printf("2. Update Email (Current: %s)\n", s->email);
        printf("3. Update Phone (Current: %s)\n", s->phone);
        printf("4. Update Major (Current: %s)\n", s->major);
        printf("5. Update GPA (Current: %.2f)\n", s->gpa);
        printf("0. Back to Main Menu\n");
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
                if (validate_name_string(buffer)) {
                    format_name(buffer);
                    strcpy(s->first_name, buffer);
                }

                get_string_input("Enter New Last Name: ", buffer, MAX_NAME_LEN);
                trim_whitespace(buffer);
                if (validate_name_string(buffer)) {
                    format_name(buffer);
                    strcpy(s->last_name, buffer);
                }
                break;

            case 2:
                get_string_input("Enter New Email: ", buffer, MAX_EMAIL_LEN);
                trim_whitespace(buffer);
                if (validate_email(buffer)) {
                    strcpy(s->email, buffer);
                }
                break;

            case 3:
                get_string_input("Enter New Phone (555-XXXX): ", buffer, MAX_PHONE_LEN);
                trim_whitespace(buffer);
                if (validate_phone(buffer)) {
                    strncpy(s->phone, buffer, MAX_PHONE_LEN - 1);
                    s->phone[MAX_PHONE_LEN - 1] = '\0';
                }
                break;

            case 4:
                get_string_input("Enter New Major: ", buffer, MAX_MAJOR_LEN);
                trim_whitespace(buffer);
                if (validate_name_string(buffer)) {
                    format_name(buffer);
                    strcpy(s->major, buffer);
                }
                break;

            case 5: {
                float new_gpa;
                printf("Enter New GPA: ");
                if (scanf("%f", &new_gpa) == 1) {
                    if (validate_gpa(new_gpa)) s->gpa = new_gpa;
                } else {
                    print_error("Invalid input. Enter a number.");
                }
                clear_input_buffer();
                break;
            }

            case 0: break;
            default: print_error("Invalid choice.");
        }
    } while (choice != 0);
}

/**
 * @brief Deletes a student and all related data
 * 
 * Removes the student along with their enrollments and grades.
 * Uses cascade deletion to maintain data integrity.
 * 
 * @param students Pointer to student array
 * @param s_count Pointer to student count
 * @param id ID of student to delete
 * @param enrollments Array of enrollments
 * @param e_count Pointer to enrollment count
 * @param grades Array of grades
 * @param g_count Pointer to grade count
 * @return 1 on success, 0 if student not found
 */
int delete_student(Student **students, int *s_count, int id, 
                   Enrollment *enrollments, int *e_count, 
                   Grade *grades, int *g_count) {
    
    int index = search_student_by_id(*students, *s_count, id);
    if (index == -1) {
        return 0; 
    }

    if (grades != NULL && g_count != NULL) {
        for (int i = *g_count - 1; i >= 0; i--) {
            if (grades[i].student_id == id) {
                for (int j = i; j < *g_count - 1; j++) {
                    grades[j] = grades[j + 1];
                }
                (*g_count)--;
            }
        }
    }

    if (enrollments != NULL && e_count != NULL) {
        for (int i = *e_count - 1; i >= 0; i--) {
            if (enrollments[i].student_id == id) {
                for (int j = i; j < *e_count - 1; j++) {
                    enrollments[j] = enrollments[j + 1];
                }
                (*e_count)--;
            }
        }
    }

    for (int i = index; i < *s_count - 1; i++) {
        (*students)[i] = (*students)[i + 1];
    }

    (*s_count)--;

    if (*s_count > 0) {
        Student *temp = (Student *)realloc(*students, (*s_count) * sizeof(Student));
        if (temp != NULL) *students = temp;
    } else {
        free(*students);
        *students = NULL;
    }

    return 1; 
}

/**
 * @brief Prints detailed information for a single student
 * 
 * @param s The student structure to print
 */
void print_student(Student s) {
    printf("\n========================================\n");
    printf("STUDENT DETAILS\n");
    printf("========================================\n");
    printf("Student ID      : %d\n", s.student_id);
    printf("Full Name       : %s %s\n", s.first_name, s.last_name);
    printf("Email           : %s\n", s.email);
    printf("Phone           : %s\n", s.phone);
    printf("Enrollment Year : %d\n", s.enrollment_year);
    printf("Major           : %s\n", s.major);
    printf("GPA             : %.2f\n", s.gpa);
}

/**
 * @brief Frees memory allocated for students
 * 
 * @param students Pointer to student array to free
 */
void free_students(Student *students) {
    if (students != NULL) free(students);
}

/**
 * @brief Adds a new student to the system
 * 
 * Prompts for all required student information, validates each field,
 * and adds the student to the array if all validations pass.
 * 
 * @param students Pointer to student array (may be reallocated)
 * @param count Pointer to student count (will be incremented)
 * @return 1 on success, 0 on failure
 */
int add_student(Student **students, int *count) {
    Student new_student;
    int valid_id = 0;

    /* 1. ID INPUT AND VALIDATION LOOP */
    while (!valid_id) {
        printf("Enter Student ID: ");
        
        /* A) Numeric Input Check */
        if (scanf("%d", &new_student.student_id) != 1) {
            clear_input_buffer();
            print_error("Invalid input. Student ID must be numeric.");
            continue;
        }
        clear_input_buffer();

        /* B) Format Check (uses validate function from utils.c) */
        if (!validate_student_id(new_student.student_id)) {
            continue; 
        }

        /* C) Uniqueness Check */
        if (search_student_by_id(*students, *count, new_student.student_id) != -1) {
            print_error("Student with this ID already exists. Please try another.");
            continue;
        }

        /* All validations passed, exit loop */
        valid_id = 1;
    }

    /* 2. OTHER FIELD INPUT */
    
    /* Name validation (alphabetic only) */
    do {
        get_string_input("First Name: ", new_student.first_name, MAX_NAME_LEN);
        trim_whitespace(new_student.first_name);
        if (validate_name_string(new_student.first_name)) {
            format_name(new_student.first_name);
            break;
        }
    } while(1);

    do {
        get_string_input("Last Name: ", new_student.last_name, MAX_NAME_LEN);
        trim_whitespace(new_student.last_name);
        if (validate_name_string(new_student.last_name)) {
            format_name(new_student.last_name);
            break;
        }
    } while(1);

    do {
        get_string_input("Email: ", new_student.email, MAX_EMAIL_LEN);
        trim_whitespace(new_student.email);
        if (validate_email(new_student.email)) break;
    } while(1);

    do {
        get_string_input("Phone (555-XXXX): ", new_student.phone, MAX_PHONE_LEN);
        trim_whitespace(new_student.phone);
        if (validate_phone(new_student.phone)) break;
    } while(1);

    do {
        get_string_input("Major (Department): ", new_student.major, MAX_MAJOR_LEN);
        trim_whitespace(new_student.major);
        if (validate_name_string(new_student.major)) {
            format_name(new_student.major);
            break;
        }
    } while(1);
    
    /* Automatic Assignments */
    new_student.enrollment_year = new_student.student_id / 1000;
    new_student.gpa = 0.0;
    
    /* 3. MEMORY ALLOCATION AND RECORD */
    Student *temp = (Student *)realloc(*students, (*count + 1) * sizeof(Student));
    if (!temp) {
        print_error("Memory allocation failed! Could not add student.");
        return 0;
    }

    *students = temp;
    (*students)[*count] = new_student;
    (*count)++;

    return 1;
}

/**
 * @brief Saves all students to a CSV file
 * 
 * Writes all student data to a CSV file with header row.
 * 
 * @param filename Path to the output file
 * @param students Array of students to save
 * @param count Number of students
 * @return 1 on success, 0 on failure
 */
int save_students(const char *filename, Student *students, int count) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        print_error("Could not open file.");
        return 0;
    }

    fprintf(file, "student_id,first_name,last_name,email,phone,enrollment_year,major,gpa\n");

    for (int i = 0; i < count; i++) {
        fprintf(file, "%d,%s,%s,%s,%s,%d,%s,%.2f\n",
                students[i].student_id,
                students[i].first_name,
                students[i].last_name,
                students[i].email,
                students[i].phone,
                students[i].enrollment_year,
                students[i].major,
                students[i].gpa);
    }

    fclose(file);
    return 1;
}

/**
 * @brief Lists all students in a formatted table
 * 
 * Displays all students with their basic information in a tabular format.
 * 
 * @param students Array of students
 * @param count Number of students
 */
void list_all_students(Student *students, int count) {
    if (students == NULL || count == 0) {
        printf("\n>> No students in the system.\n");
        return;
    }
    
    printf("\n");
    printf("+------------------------------------------------------------------------------+\n");
    printf("|                           ALL STUDENTS LIST                                 |\n");
    printf("+-----------+----------------------+----------------------+-------------------+\n");
    printf("| ID        | First Name           | Last Name            | Major             |\n");
    printf("+-----------+----------------------+----------------------+-------------------+\n");
    
    for (int i = 0; i < count; i++) {
        printf("| %-9d | %-20s | %-20s | %-17s |\n",
               students[i].student_id,
               students[i].first_name,
               students[i].last_name,
               students[i].major);
    }
    
    printf("+-----------+----------------------+----------------------+-------------------+\n");
    printf("| Total: %-70d |\n", count);
    printf("+------------------------------------------------------------------------------+\n");
}