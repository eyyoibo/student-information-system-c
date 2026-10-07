/**
 * @file course.c
 * @brief Course Management Module Implementation
 * 
 * Implements all course management functionality including:
 * - Loading/saving courses from/to CSV files
 * - CRUD operations (Create, Read, Update, Delete)
 * - Prerequisite management
 * - Instructor assignment
 * - Capacity management
 * 
 * @author Student Information System Team
 * @date January 2026
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "course.h"
#include "utils.h"
#include "grade.h"
#include "enrollment.h"
#include "professor.h"

/**
 * @brief Loads courses from a CSV file into the dynamic array
 * 
 * Reads course data from CSV file, validates each record, and stores
 * valid courses in dynamically allocated memory. Handles duplicate
 * detection for both course ID and course code.
 * 
 * @param filename Path to the CSV file
 * @param courses Pointer to the course array pointer (will be allocated)
 * @param count Pointer to store the number of loaded courses
 * @return 1 on success, 0 on failure
 */
int load_courses(const char *filename, Course **courses, int *count) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        print_error("Could not open courses file.");
        return 0;
    }

    char line[MAX_LINE_LEN];
    char *fields[7];           
    int capacity = 10;
    int line_number = 0;

    *count = 0;
    *courses = NULL;

    /* Skip header line */
    if (fgets(line, sizeof(line), file) == NULL) {
        fclose(file);
        return 1;
    }
    line_number++;

    *courses = (Course *)malloc(capacity * sizeof(Course));
    if (!*courses) {
        print_error("Initial memory allocation failed.");
        fclose(file);
        return 0;
    }

    while (fgets(line, sizeof(line), file)) {
        line_number++;
        line[strcspn(line, "\r\n")] = '\0';
        if (line[0] == '\0') continue;

        int field_count = split_csv_line(line, fields, 7);
        if (field_count != 7) {
             fprintf(stderr, "[ERROR] Line %d: Invalid column count. Skipped.\n", line_number);
             continue;
        }

        /* Extract and trim field values */
        char *str_id = fields[0];
        char *str_code = fields[1];
        char *str_name = fields[2];
        char *str_credits = fields[3];
        char *str_capacity = fields[4];
        char *str_dept = fields[5];
        char *str_prereq = fields[6];

        trim_whitespace(str_id);
        trim_whitespace(str_code);
        trim_whitespace(str_credits);
        trim_whitespace(str_capacity);

        /* 1. Numeric Format Validation */
        if (!is_numeric(str_id)) {
            fprintf(stderr, "[ERROR] Line %d: Course ID '%s' is not numeric. Skipped.\n", line_number, str_id);
            continue;
        }
        if (!is_numeric(str_credits)) {
            fprintf(stderr, "[ERROR] Line %d: Credits '%s' contains non-numeric characters. Skipped.\n", line_number, str_credits);
            continue;
        }
        if (!is_numeric(str_capacity)) {
            fprintf(stderr, "[ERROR] Line %d: Capacity '%s' contains non-numeric characters. Skipped.\n", line_number, str_capacity);
            continue;
        }

        int temp_id = atoi(str_id);
        int temp_credits = atoi(str_credits);
        int temp_capacity = atoi(str_capacity);

        /* 2. ID Range Validation (1000-9999) */
        if (!validate_course_id(temp_id)) {
            fprintf(stderr, "[ERROR] Line %d: Invalid Course ID (%d). Skipped.\n", line_number, temp_id);
            continue;
        }

        /* 3. Duplicate ID Check */
        if (search_course_by_id(*courses, *count, temp_id) != -1) {
            fprintf(stderr, "[ERROR] Line %d: Duplicate Course ID (%d). Already loaded. Skipped.\n", line_number, temp_id);
            continue;
        }

        /* 4. Duplicate Code Check */
        if (search_course_by_code(*courses, *count, str_code) != -1) {
            fprintf(stderr, "[ERROR] Line %d: Duplicate Course Code '%s'. Already loaded. Skipped.\n", line_number, str_code);
            continue;
        }

        /* Expand array if needed */
        if (*count >= capacity) {
            int new_capacity = capacity * 2;
            Course *temp = (Course *)realloc(*courses, new_capacity * sizeof(Course));
            if (!temp) {
                print_error("Memory reallocation failed.");
                free(*courses);
                *courses = NULL;
                *count = 0;
                fclose(file);
                return 0;
            }
            *courses = temp;
            capacity = new_capacity;
        }

        Course *c = &(*courses)[*count];
        c->course_id = temp_id;
        
        strncpy(c->course_code, str_code, MAX_CODE_LEN - 1); c->course_code[MAX_CODE_LEN - 1] = '\0';
        trim_whitespace(c->course_code);
        
        strncpy(c->course_name, str_name, MAX_NAME_LEN - 1); c->course_name[MAX_NAME_LEN - 1] = '\0';
        trim_whitespace(c->course_name);
        format_name(c->course_name);

        c->credits  = temp_credits;
        c->capacity = temp_capacity;

        strncpy(c->department, str_dept, MAX_DEPT_LEN - 1); c->department[MAX_DEPT_LEN - 1] = '\0';
        trim_whitespace(c->department);
        format_name(c->department);
        
        strncpy(c->prerequisites, str_prereq, MAX_PREREQ_LEN - 1); c->prerequisites[MAX_PREREQ_LEN - 1] = '\0';
        trim_whitespace(c->prerequisites);

        c->instructor_id = 0; 
        (*count)++;
    }

    fclose(file);
    if (*count == 0 && *courses != NULL) { free(*courses); *courses = NULL; }

    printf(">> Courses Loaded Successfully: %d\n", *count);
    return 1;
}

/**
 * @brief Searches for a course by its ID
 * 
 * Performs linear search through the course array to find
 * a course with the specified ID.
 * 
 * @param courses Array of courses to search
 * @param count Number of courses in the array
 * @param id The course ID to search for
 * @return Index of the course if found, -1 otherwise
 */
int search_course_by_id(Course *courses, int count, int id){
    if(courses==NULL || count<=0) return -1;
    for(int i = 0; i < count; i++){
        if(courses[i].course_id==id)
            return i;
    }
    return -1;
}

/**
 * @brief Deletes a course and all associated data
 * 
 * Removes a course from the system along with all related
 * enrollments and grades. Uses cascade deletion to maintain
 * data integrity.
 * 
 * @param courses Array of courses
 * @param course_count Pointer to the course count (will be decremented)
 * @param course_id ID of the course to delete
 * @param enrollments Pointer to enrollments array
 * @param enrollment_count Pointer to enrollment count
 * @param grades Pointer to grades array
 * @param grade_count Pointer to grade count
 */
void delete_course(Course *courses, int *course_count, int course_id, 
                   Enrollment **enrollments, int *enrollment_count, 
                   Grade **grades, int *grade_count) {
    
    int index = search_course_by_id(courses, *course_count, course_id);
    if (index == -1) {
        print_error("Course ID not found.");
        return;
    }

    /* 1. First delete associated grades and enrollments */
    delete_grades_by_course_id(grades, grade_count, course_id);
    delete_enrollments_by_course_id(enrollments, enrollment_count, course_id);

    /* 2. Remove course from array (shift remaining elements) */
    for (int i = index; i < *course_count - 1; i++) {
        courses[i] = courses[i + 1];
    }
    (*course_count)--;

    /* Note: Memory is not shrunk here because courses pointer is passed by value.
       The allocated memory remains valid, just with one unused slot at the end.
       This is acceptable as the count is properly decremented. */

    printf("Course deleted successfully.\n");
}

/**
 * @brief Saves all courses to a CSV file
 * 
 * Writes all course data to a CSV file with header row.
 * Overwrites existing file content.
 * 
 * @param filename Path to the output file
 * @param courses Array of courses to save
 * @param count Number of courses
 * @return 1 on success, 0 on failure
 */
int save_courses(const char *filename, Course *courses, int count) {
    FILE *file = fopen(filename, "w"); 
    if (!file) {
        print_error("Could not open file for saving courses.");
        return 0;
    }

    fprintf(file, "course_id,course_code,course_name,credits,capacity,department,prerequisites\n");

    for (int i = 0; i < count; i++) {
        fprintf(file, "%d,%s,%s,%d,%d,%s,%s\n", 
                courses[i].course_id, 
                courses[i].course_code, 
                courses[i].course_name, 
                courses[i].credits, 
                courses[i].capacity, 
                courses[i].department, 
                courses[i].prerequisites);
    }

    fclose(file);
    return 1;
}

/**
 * @brief Searches for a course by its code
 * 
 * Performs linear search through the course array to find
 * a course with the specified code (e.g., "CS101").
 * 
 * @param courses Array of courses to search
 * @param count Number of courses in the array
 * @param code The course code to search for
 * @return Index of the course if found, -1 otherwise
 */
int search_course_by_code(Course *courses, int count, const char *code) {
    if (courses == NULL || count <= 0) return -1;
    for (int i = 0; i < count; i++) {
        if (strcmp(courses[i].course_code, code) == 0)
            return i;
    }
    return -1;
}

/**
 * @brief Adds a new course to the system
 * 
 * Prompts the user for course details including ID, code, name,
 * credits, capacity, department, and prerequisites. Validates
 * all inputs and checks for duplicates before adding.
 * 
 * @param courses Pointer to the courses array (may be reallocated)
 * @param count Pointer to the course count (will be incremented)
 * @return 1 on success, 0 on failure
 */
int add_course(Course **courses, int *count) {
    Course new_course;
    char buffer[100]; /* Temporary buffer for input */

    /* 1. Course ID Input (Numeric and Duplicate Check) */
    while (1) {
        get_string_input("Enter Course ID: ", buffer, 100);
        trim_whitespace(buffer);

        if (!is_numeric(buffer)) {
            print_error("Invalid input. ID must be strictly numeric.");
            continue;
        }
        
        new_course.course_id = atoi(buffer);
        
        if (!validate_course_id(new_course.course_id)) {
            continue; /* Validation function prints error */
        }

        if (search_course_by_id(*courses, *count, new_course.course_id) != -1) {
            print_error("Course ID already exists.");
            continue;
        }
        break;
    }

    /* 2. Course Code Input (Duplicate Check) */
    while (1) {
        get_string_input("Course Code: ", new_course.course_code, MAX_CODE_LEN);
        trim_whitespace(new_course.course_code);
        
        if (strlen(new_course.course_code) == 0) continue;

        if (search_course_by_code(*courses, *count, new_course.course_code) != -1) {
            print_error("Course Code already exists! Please use a unique code.");
            continue;
        }
        break;
    }

    get_string_input("Course Name: ", new_course.course_name, MAX_NAME_LEN);
    trim_whitespace(new_course.course_name);
    format_name(new_course.course_name);

    /* 3. Credits Input (Numeric Check) */
    while (1) {
        get_string_input("Credits (Positive Integer): ", buffer, 100);
        trim_whitespace(buffer);

        if (!is_numeric(buffer)) {
            print_error("Invalid input. Credits must be a number.");
            continue;
        }
        new_course.credits = atoi(buffer);
        if (new_course.credits < 0) {
            print_error("Credits cannot be negative.");
            continue;
        }
        break;
    }

    /* 4. Capacity Input (Numeric Check) */
    while (1) {
        get_string_input("Capacity (Positive Integer): ", buffer, 100);
        trim_whitespace(buffer);

        if (!is_numeric(buffer)) {
            print_error("Invalid input. Capacity must be a number.");
            continue;
        }
        new_course.capacity = atoi(buffer);
        if (new_course.capacity < 0) {
            print_error("Capacity cannot be negative.");
            continue;
        }
        break;
    }

    /* Department Input */
    do {
        get_string_input("Department: ", new_course.department, MAX_DEPT_LEN);
        trim_whitespace(new_course.department);
        if(validate_name_string(new_course.department)) {
            format_name(new_course.department);
            break;
        }
    } while(1);

    /* Prerequisites Input (Code validation) */
    do {
        get_string_input("Prerequisites (Code or 'None'): ", new_course.prerequisites, MAX_PREREQ_LEN);
        trim_whitespace(new_course.prerequisites);
        if (strlen(new_course.prerequisites) == 0 || str_equals_case_insensitive(new_course.prerequisites, "None")) {
            strcpy(new_course.prerequisites, "None");
            break;
        }
        /* A course cannot be its own prerequisite */
        if (str_equals_case_insensitive(new_course.prerequisites, new_course.course_code)) {
             print_error("A course cannot be its own prerequisite.");
             continue;
        }
        /* Check if prerequisite course exists */
        if (search_course_by_code(*courses, *count, new_course.prerequisites) == -1) {
            printf(">> Error: Course '%s' not found.\n", new_course.prerequisites);
            continue;
        }
        break;
    } while(1);

    /* Memory allocation and addition */
    Course *temp = (Course *)realloc(*courses, (*count + 1) * sizeof(Course));
    if (!temp) {
        print_error("Memory allocation failed.");
        return 0;
    }
    *courses = temp;
    (*courses)[*count] = new_course;
    (*count)++;

    return 1;
}

/**
 * @brief Updates an existing course via interactive menu
 * 
 * Provides a menu to update individual fields of a course
 * including name, credits, capacity, department, and prerequisites.
 * 
 * @param courses Array of courses
 * @param count Number of courses
 * @param id ID of the course to update
 */
void update_course(Course *courses, int count, int id) {
    int index = search_course_by_id(courses, count, id);
    if (index == -1) {
        print_error("Course not found with this ID.");
        return;
    }

    Course *c = &courses[index];
    int choice;
    char buffer[MAX_LINE_LEN];

    do {
        printf("\n--- Update Menu for %s (%d) ---\n", c->course_code, c->course_id);
        printf("1. Update Course Name (Current: %s)\n", c->course_name);
        printf("2. Update Credits (Current: %d)\n", c->credits);
        printf("3. Update Capacity (Current: %d)\n", c->capacity);
        printf("4. Update Department (Current: %s)\n", c->department);
        printf("5. Update Prerequisites (Current: %s)\n", c->prerequisites);
        printf("0. Back to Main Menu\n");
        printf("Choice: ");

        if (scanf("%d", &choice) != 1) {
            clear_input_buffer();
            continue;
        }
        clear_input_buffer();

        switch (choice) {
            case 1:
                get_string_input("Enter New Name: ", buffer, MAX_NAME_LEN);
                trim_whitespace(buffer);
                if (strlen(buffer) > 0) strcpy(c->course_name, buffer);
                break;

            case 2:
                do {
                    printf("Enter New Credits (>=0): ");
                    if (scanf("%d", &c->credits) != 1) {
                        clear_input_buffer();
                        c->credits = -1; 
                        continue;
                    }
                    clear_input_buffer();
                    if (c->credits < 0) print_error("Credits cannot be negative!");
                } while (c->credits < 0);
                break;

            case 3:
                do {
                    printf("Enter New Capacity (>=0): ");
                    if (scanf("%d", &c->capacity) != 1) {
                        clear_input_buffer();
                        c->capacity = -1; 
                        continue;
                    }
                    clear_input_buffer();
                    if (c->capacity < 0) print_error("Capacity cannot be negative!");
                } while (c->capacity < 0);
                break;

            case 4:
                do {
                    get_string_input("Enter New Dept: ", buffer, MAX_DEPT_LEN);
                    trim_whitespace(buffer);
                    if (strlen(buffer) > 0) {
                        if(validate_name_string(buffer)) {
                            strcpy(c->department, buffer);
                            break;
                        }
                    } else {
                        break; 
                    }
                } while(1);
                break;

            case 5:
                do {
                    get_string_input("Enter New Prereqs (Code or 'None'): ", buffer, MAX_LINE_LEN);
                    trim_whitespace(buffer);

                    if (strlen(buffer) == 0 || str_equals_case_insensitive(buffer, "None")) {
                        strcpy(c->prerequisites, "None");
                        break;
                    }

                    if (str_equals_case_insensitive(buffer, c->course_code)) {
                        print_error("A course cannot be its own prerequisite.");
                        continue;
                    }

                    if (search_course_by_code(courses, count, buffer) == -1) {
                        printf(">> Error: Course '%s' not found.\n", buffer);
                        continue;
                    }
                    
                    /* Valid prerequisite */
                    strcpy(c->prerequisites, buffer);
                    break;
                } while(1);
                break;
            case 0:
                /* User wants to exit - do nothing */
                break;
            default: print_error("Invalid choice.");
        }
    } while (choice != 0);
}

/**
 * @brief Prints course information
 * 
 * Displays course details in either detailed format (all fields)
 * or list format (summary line).
 * 
 * @param c Pointer to the course to print
 * @param detailed 1 for detailed view, 0 for list format
 */
void print_course(Course *c, int detailed) {
    if (!c) return;
    
    if (detailed) {
        printf("\n--- Course Details ---\n");
        printf("ID          : %d\n", c->course_id);
        printf("Code        : %s\n", c->course_code);
        printf("Name        : %s\n", c->course_name);
        printf("Department  : %s\n", c->department);
        printf("Credit      : %d\n", c->credits);
        printf("Capacity    : %d\n", c->capacity);
        
        if (c->instructor_id != 0) {
            printf("Assigned Prof. ID: %d\n", c->instructor_id);
        } else {
            printf("Assigned Prof. ID: [None]\n");
        }
        
        printf("----------------------\n");
    } else {
        printf("%-5d %-10s %-25s %-5d\n", c->course_id, c->course_code, c->course_name, c->credits);
    }
}

/**
 * @brief Frees memory allocated for courses
 * 
 * Releases the dynamically allocated memory for the course array.
 * 
 * @param courses Pointer to the course array to free
 */
void free_courses(Course *courses) {
    if (courses != NULL) {
        free(courses);
        printf("Courses erased from memory.\n");
    }
}

/**
 * @brief Updates the capacity of a specific course
 * 
 * Validates that the new capacity is not less than the current
 * maximum enrollment in any semester for this course.
 * 
 * @param courses Array of courses
 * @param count Number of courses
 * @param id ID of the course to update
 * @param enrollments Array of enrollments for validation
 * @param enr_count Number of enrollments
 */
void set_course_capacity(Course *courses, int count, int id, Enrollment *enrollments, int enr_count) {
    int index = search_course_by_id(courses, count, id);
    if (index == -1) {
        print_error("Course not found with this ID.");
        return;
    }

    /* Calculate maximum enrollment across all semesters */
    int max_occupied = 0;
    
    /* Check each enrollment record for this course */
    for (int i = 0; i < enr_count; i++) {
        /* Only count enrolled students for this course */
        if (enrollments[i].course_id == id && str_equals_case_insensitive(enrollments[i].status, "Enrolled")) {
            
            int current_sem_count = 0;
            /* Inner loop: Count students in the same semester */
            for (int j = 0; j < enr_count; j++) {
                if (enrollments[j].course_id == id && 
                    str_equals_case_insensitive(enrollments[j].status, "Enrolled") &&
                    str_equals_case_insensitive(enrollments[j].semester, enrollments[i].semester)) {
                    current_sem_count++;
                }
            }
            if (current_sem_count > max_occupied) {
                max_occupied = current_sem_count;
            }
        }
    }

    printf("Current Capacity: %d\n", courses[index].capacity);
    if (max_occupied > 0) {
        printf("NOTE: There is a semester with %d active students. Minimum capacity allowed is %d.\n", max_occupied, max_occupied);
    }

    int new_capacity;
    do {
        printf("Enter New Capacity: ");
        if (scanf("%d", &new_capacity) != 1) {
            clear_input_buffer();
            print_error("Invalid input! Please enter a number.");
            new_capacity = -1; /* Continue loop */
            continue;
        }
        clear_input_buffer();

        /* 1. Negative Check */
        if (new_capacity < 0) {
            print_error("Capacity cannot be negative.");
            continue;
        }

        /* 2. Enrollment Validation */
        if (new_capacity < max_occupied) {
            printf(">> Error: Cannot set capacity to %d. Existing enrollments (%d students) would exceed this limit!\n", new_capacity, max_occupied);
            continue;
        }

        /* Valid input, exit loop */
        break;

    } while(1);

    courses[index].capacity = new_capacity;
    printf("Capacity updated successfully for %s to %d.\n", courses[index].course_code, new_capacity);
}

/**
 * @brief Updates prerequisites for a course
 * 
 * Validates that the prerequisite course exists and prevents
 * circular dependencies (course cannot require itself).
 * 
 * @param courses Array of courses
 * @param count Number of courses
 * @param id ID of the course to update
 */
void assign_prerequisites(Course *courses, int count, int id) {
    int index = search_course_by_id(courses, count, id);
    if (index == -1) {
        print_error("Course not found with this ID.");
        return;
    }

    char buffer[MAX_LINE_LEN];
    printf("Current Prerequisites for %s: %s\n", courses[index].course_code, courses[index].prerequisites);
    
    do {
        get_string_input("Enter New Prerequisites (Code or 'None'): ", buffer, MAX_LINE_LEN);
        trim_whitespace(buffer);

        /* 1. Empty / None input */
        if (strlen(buffer) == 0 || str_equals_case_insensitive(buffer, "None")) {
            strcpy(courses[index].prerequisites, "None");
            printf("Prerequisites cleared (set to None).\n");
            break;
        }

        /* 2. Self-reference check */
        if (str_equals_case_insensitive(buffer, courses[index].course_code)) {
            print_error("Circular logic: A course cannot be its own prerequisite.");
            continue;
        }

        /* 3. Prerequisite existence check */
        int prereq_idx = search_course_by_code(courses, count, buffer);
        if (prereq_idx == -1) {
            printf(">> Error: The course '%s' does not exist in the system.\n", buffer);
            continue;
        }

        /* Valid prerequisite */
        strcpy(courses[index].prerequisites, buffer);
        printf("Prerequisites updated successfully for %s -> Requires %s.\n", courses[index].course_code, buffer);
        break;

    } while(1);
}

/**
 * @brief Assigns a professor to a course based on department matching
 * 
 * Lists professors in the same department as the course and allows
 * selection of an instructor from the matching list.
 * 
 * @param courses Array of courses
 * @param course_count Number of courses
 * @param professors Array of professors
 * @param prof_count Number of professors
 */
void assign_course_by_department(Course *courses, int course_count, Professor *professors, int prof_count) {
    printf("\n=== DEPARTMENT BASED ASSIGNMENT ===\n");

    int course_id, c_index = -1;
    printf("Enter Course ID to assign: ");
    if (scanf("%d", &course_id) != 1) { clear_input_buffer(); return; }
    clear_input_buffer();

    c_index = search_course_by_id(courses, course_count, course_id);
    if (c_index == -1) {
        print_error("Course not found.");
        return;
    }

    char *target_dept = courses[c_index].department;
    printf("\nSelected Course: %s (%s)\n", courses[c_index].course_name, courses[c_index].course_code);
    printf("Looking for professors in department: '%s'...\n", target_dept);

    printf("\n--- Matching Professors ---\n");
    printf("%-5s %-20s %-20s\n", "ID", "Name", "Title");
    printf("------------------------------------------------\n");

    int match_count = 0;

    for (int i = 0; i < prof_count; i++) {
        if (strcmp(professors[i].department, target_dept) == 0) {
            printf("%-5d %s %-10s %-20s\n", 
                   professors[i].professor_id, 
                   professors[i].first_name, 
                   professors[i].last_name, 
                   professors[i].title);
            match_count++;
        }
    }
    printf("------------------------------------------------\n");

    if (match_count == 0) {
        printf("No professors found in %s department!\n", target_dept);
        return;
    }

    int prof_id, p_index = -1;
    printf("Enter Professor ID from the list above: ");
    if (scanf("%d", &prof_id) != 1) { clear_input_buffer(); return; }
    clear_input_buffer();

    p_index = search_professor_by_id(professors, prof_count, prof_id);
    
    if (p_index == -1) {
        print_error("Invalid Professor ID.");
        return;
    }
    if (strcmp(professors[p_index].department, target_dept) != 0) {
        print_error("This professor is not in the required department!");
        return;
    }

    courses[c_index].instructor_id = prof_id;

    printf("\nSUCCESS: %s is assigned to %s %s.\n", 
           courses[c_index].course_code, 
           professors[p_index].first_name, 
           professors[p_index].last_name);
}

/**
 * @brief Lists all courses in a specific department
 * 
 * Displays a formatted table of courses that belong to the
 * specified department (case-insensitive matching).
 * 
 * @param courses Array of courses
 * @param count Number of courses
 * @param dept_name Name of the department to filter by
 */
void print_courses_by_department(Course *courses, int count, const char *dept_name) {
    int found = 0;
    
    printf("\n--- Courses in Department: %s ---\n", dept_name);
    printf("%-10s %-10s %-30s %-8s %-10s\n", "ID", "Code", "Name", "Credits", "Capacity");
    printf("-----------------------------------------------------------------------------\n");

    for (int i = 0; i < count; i++) {
        /* Case-insensitive department comparison */
        if (str_equals_case_insensitive(courses[i].department, dept_name)) {
            printf("%-10d %-10s %-30s %-8d %-10d\n", 
                courses[i].course_id,
                courses[i].course_code,
                courses[i].course_name,
                courses[i].credits,
                courses[i].capacity
            );
            found = 1;
        }
    }
    
    if (!found) {
        printf("No courses found in this department.\n");
    }
    printf("-----------------------------------------------------------------------------\n");
}

/**
 * @brief Lists all courses in a formatted table
 * 
 * Displays all courses with their basic information in a tabular format.
 * 
 * @param courses Array of courses
 * @param count Number of courses
 */
void list_all_courses(Course *courses, int count) {
    if (courses == NULL || count == 0) {
        printf("\n>> No courses in the system.\n");
        return;
    }
    
    printf("\n");
    printf("+------------------------------------------------------------------------------+\n");
    printf("|                            ALL COURSES LIST                                 |\n");
    printf("+------+----------+------------------------------+--------+-------------------+\n");
    printf("| ID   | Code     | Course Name                  | Credit | Department        |\n");
    printf("+------+----------+------------------------------+--------+-------------------+\n");
    
    for (int i = 0; i < count; i++) {
        printf("| %-4d | %-8s | %-28s | %-6d | %-17s |\n",
               courses[i].course_id,
               courses[i].course_code,
               courses[i].course_name,
               courses[i].credits,
               courses[i].department);
    }
    
    printf("+------+----------+------------------------------+--------+-------------------+\n");
    printf("| Total: %-70d |\n", count);
    printf("+------------------------------------------------------------------------------+\n");
}