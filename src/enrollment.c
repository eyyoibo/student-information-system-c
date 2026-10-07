/**
 * @file enrollment.c
 * @brief Implementation of enrollment management functions.
 * 
 * This file contains the implementation of all enrollment-related operations
 * including loading, saving, searching, enrolling students, dropping courses,
 * and managing enrollment records.
 * 
 * @author Student Information System Team
 * @date January 2026
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 

#include "enrollment.h"
#include "utils.h"      /* is_numeric, str_equals_case_insensitive, validate_semester functions */
#include "grade.h"      
#include "course.h"     
#include "student.h"    
#include "professor.h"  

/**
 * @brief Searches for an enrollment record by its unique ID.
 * 
 * Helper function to find the index of an enrollment record in the array.
 * 
 * @param enrollments Array of Enrollment structures
 * @param count Total number of enrollments
 * @param enrollment_id The ID to search for
 * @return The index if found, -1 otherwise
 */
int search_enrollment_by_id(Enrollment *enrollments, int count, int enrollment_id) {
    for (int i = 0; i < count; i++) {
        if (enrollments[i].enrollment_id == enrollment_id) {
            return i;
        }
    }
    return -1;
}

/**
 * @brief Generates the next available enrollment ID.
 * 
 * Helper function that finds the maximum existing ID and returns max + 1.
 * 
 * @param enrollments Array of Enrollment structures
 * @param enr_count Total number of enrollments
 * @return The next available enrollment ID
 */
static int next_enrollment_id(Enrollment *enrollments, int enr_count) {
    int max_id = 0;
    for (int i = 0; i < enr_count; i++) {
        if (enrollments[i].enrollment_id > max_id) {
            max_id = enrollments[i].enrollment_id;
        }
    }
    return max_id + 1;
}

/**
 * @brief Checks if a student has passed the prerequisite course.
 * 
 * Validates whether a student has successfully completed the prerequisite
 * course (received a grade other than "F").
 * 
 * @param student_id The ID of the student to check
 * @param prereq_code The prerequisite course code
 * @param courses Array of Course structures
 * @param c_count Total number of courses
 * @param grades Array of Grade structures
 * @param g_count Total number of grades
 * @return 1 if prerequisite is met, 0 otherwise
 */
int has_passed_prerequisite(int student_id, const char *prereq_code, Course *courses, int c_count, Grade *grades, int g_count) {
    /* If prerequisite is "None", grant permission */
    if (prereq_code == NULL || strlen(prereq_code) == 0 || str_equals_case_insensitive(prereq_code, "None")) {
        return 1;
    }

    /* Find the course ID of the prerequisite course */
    int prereq_course_id = -1;
    for(int i=0; i<c_count; i++){
        if(str_equals_case_insensitive(courses[i].course_code, prereq_code)){
            prereq_course_id = courses[i].course_id;
            break;
        }
    }
    
    /* If prerequisite course not found in system, requirement cannot be met */
    if(prereq_course_id == -1) return 0;

    /* Check student's grades: Has the student passed the course? */
    for (int i = 0; i < g_count; i++) {
        if (grades[i].student_id == student_id && grades[i].course_id == prereq_course_id) {
            /* If grade is not "F", student has passed (A, B, C, D) */
            if (!str_equals_case_insensitive(grades[i].letter_grade, "F")) {
                return 1; 
            }
        }
    }

    return 0; /* No passing record found */
}

/**
 * @brief Checks if a student is already enrolled in a specific course.
 * 
 * Searches through enrollment records to find if a student is enrolled
 * in a given course for a specific semester.
 * 
 * @param enrollments Array of Enrollment structures
 * @param count Total number of enrollments
 * @param student_id The student ID to check
 * @param course_id The course ID to check
 * @param semester The semester string
 * @return 1 if enrolled, 0 otherwise
 */
int is_student_enrolled_in_course(Enrollment *enrollments, int count, int student_id, int course_id, const char *semester) {
    if (enrollments == NULL || count <= 0) return 0;
    
    for (int i = 0; i < count; i++) {
        if (enrollments[i].student_id == student_id && 
            enrollments[i].course_id == course_id &&
            str_equals_case_insensitive(enrollments[i].semester, semester)) {
            return 1;
        }
    }
    return 0;
}

/**
 * @brief Checks if a course has available capacity.
 * 
 * Counts current enrollments for a course in a specific semester
 * and compares with the course capacity.
 * 
 * @param enr Array of Enrollment structures
 * @param enr_count Total number of enrollments
 * @param courses Array of Course structures
 * @param crs_count Total number of courses
 * @param course_id The course ID to check
 * @param sem The semester string
 * @return 1 if space is available, 0 if full
 */
int check_course_capacity(Enrollment *enr, int enr_count, Course *courses, int crs_count, int course_id, const char *sem) {
    int c_idx = search_course_by_id(courses, crs_count, course_id);
    if (c_idx == -1) return 0;
    
    int current_count = 0;
    for (int i = 0; i < enr_count; i++) {
        if (enr[i].course_id == course_id && str_equals_case_insensitive(enr[i].semester, sem)) {
            current_count++;
        }
    }
    
    return (current_count < courses[c_idx].capacity) ? 1 : 0;
}

/**
 * @brief Checks if a student has passed the prerequisites for a course.
 * 
 * Validates whether a student has successfully completed the prerequisite
 * course(s) before allowing enrollment.
 * 
 * @param enr Array of Enrollment structures (unused in logic but kept for consistency)
 * @param enr_count Total number of enrollments
 * @param courses Array of Course structures
 * @param crs_count Total number of courses
 * @param grades Array of Grade structures
 * @param grd_count Total number of grades
 * @param student_id The student ID
 * @param course_id The course ID
 * @return 1 if prerequisites met, 0 otherwise
 */
int check_prerequisites(Enrollment *enr, int enr_count, Course *courses, int crs_count, Grade *grades, int grd_count, int student_id, int course_id) {
    (void)enr; /* Suppress unused parameter warning */
    (void)enr_count;
    
    int c_idx = search_course_by_id(courses, crs_count, course_id);
    if (c_idx == -1) return 0;
    
    return has_passed_prerequisite(student_id, courses[c_idx].prerequisites, courses, crs_count, grades, grd_count);
}

/**
 * @brief Loads enrollment records from a CSV file.
 * 
 * Reads and validates enrollment data from a CSV file. Performs validation
 * for numeric formats, duplicate IDs, status values, and semester format.
 * 
 * @param filename The path to the CSV file
 * @param enrollments Pointer to the array of Enrollment structures
 * @param count Pointer to the integer storing the number of enrollments
 * @return 1 if successful, 0 otherwise
 */
int load_enrollments(const char *filename, Enrollment **enrollments, int *count) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        print_error("Could not open enrollments file.");
        return 0;
    }
    char line[MAX_LINE_LEN];
    char *fields[7];
    int capacity = 10;
    int line_number = 0;

    *count = 0;
    *enrollments = NULL;

    /* Skip header line */
    if (fgets(line, sizeof(line), file) == NULL) { fclose(file); return 1; } 
    line_number++;

    *enrollments = (Enrollment *)malloc(capacity * sizeof(Enrollment));
    if (!*enrollments) { fclose(file); return 0; }
    
    while (fgets(line, sizeof(line), file)) {
        line_number++;
        line[strcspn(line, "\r\n")] = '\0';
        if (line[0] == '\0') continue;

        /* CSV parsing */
        if (split_csv_line(line, fields, 7) != 7) {
            fprintf(stderr, "[ERROR] Line %d: Invalid column count. Skipped.\n", line_number);
            continue;
        }

        char *str_id = fields[0];
        char *str_sid = fields[1];
        char *str_cid = fields[2];
        char *str_pid = fields[3];
        char *semester = fields[4];
        char *date = fields[5];
        char *status = fields[6];

        trim_whitespace(str_id);
        trim_whitespace(str_sid);
        trim_whitespace(str_cid);
        trim_whitespace(str_pid);
        trim_whitespace(semester);
        trim_whitespace(date);
        trim_whitespace(status);

        /* 1. NUMERIC FORMAT CHECK (using is_numeric from utils.h) */
        if (!is_numeric(str_id)) {
            fprintf(stderr, "[ERROR] Line %d: Enrollment ID '%s' is not numeric. Skipped.\n", line_number, str_id); continue;
        }
        if (!is_numeric(str_sid)) {
            fprintf(stderr, "[ERROR] Line %d: Student ID '%s' is not numeric. Skipped.\n", line_number, str_sid); continue;
        }
        if (!is_numeric(str_cid)) {
            fprintf(stderr, "[ERROR] Line %d: Course ID '%s' is not numeric. Skipped.\n", line_number, str_cid); continue;
        }
        if (!is_numeric(str_pid)) {
            fprintf(stderr, "[ERROR] Line %d: Professor ID '%s' is not numeric. Skipped.\n", line_number, str_pid); continue;
        }

        int temp_id = atoi(str_id);

        /* 2. DUPLICATE ID CHECK */
        if (search_enrollment_by_id(*enrollments, *count, temp_id) != -1) {
            fprintf(stderr, "[ERROR] Line %d: Duplicate Enrollment ID (%d). Skipped.\n", line_number, temp_id);
            continue;
        }

        /* 3. STATUS CHECK (Completed or Enrolled) */
        if (!str_equals_case_insensitive(status, "Enrolled") && 
            !str_equals_case_insensitive(status, "Completed")) {
            fprintf(stderr, "[ERROR] Line %d: Invalid Status '%s'. Must be 'Enrolled' or 'Completed'. Skipped.\n", line_number, status);
            continue;
        }

        /* 4. SEMESTER FORMAT CHECK */
        if (!validate_semester(semester)) {
            fprintf(stderr, "[ERROR] Line %d: Invalid Semester Format '%s'. Skipped.\n", line_number, semester);
            continue;
        }

        /* MEMORY ALLOCATION AND RECORD CREATION */
        if (*count >= capacity) {
            int new_capacity = capacity * 2;
            Enrollment *temp = (Enrollment *)realloc(*enrollments, new_capacity * sizeof(Enrollment));
            if (!temp) { free(*enrollments); *enrollments = NULL; *count = 0; fclose(file); return 0; }
            *enrollments = temp;
            capacity = new_capacity;
        }

        Enrollment *e = &(*enrollments)[*count];
        e->enrollment_id = temp_id;
        e->student_id    = atoi(str_sid);
        e->course_id     = atoi(str_cid);
        e->professor_id  = atoi(str_pid);

        strncpy(e->semester, semester, MAX_SEMESTER_LEN - 1); e->semester[MAX_SEMESTER_LEN - 1] = '\0';
        strncpy(e->enrollment_date, date, MAX_DATE_LEN - 1);  e->enrollment_date[MAX_DATE_LEN - 1] = '\0';
        strncpy(e->status, status, MAX_STATUS_LEN - 1);       e->status[MAX_STATUS_LEN - 1] = '\0';

        (*count)++;
    }
    fclose(file);
    printf(">> System loaded %d enrollments successfully.\n", *count);
    return 1;
}

/**
 * @brief Enrolls a student in a specific course for a given semester.
 * 
 * Validates capacity, prerequisites, and duplicate enrollments before
 * creating a new enrollment record. Automatically assigns instructor
 * and generates enrollment date based on semester.
 * 
 * @param enrollments Pointer to Enrollment array
 * @param e_count Pointer to enrollment count
 * @param students Student array for validation
 * @param s_count Student count
 * @param courses Course array for validation
 * @param c_count Course count
 * @param grades Grade array for prerequisite check
 * @param g_count Grade count
 * @return 1 if enrollment is successful, 0 otherwise
 */
int enroll_student_in_course(Enrollment **enrollments, int *e_count, 
                             Student *students, int s_count, 
                             Course *courses, int c_count, 
                             Grade *grades, int g_count,
                             int student_id, int course_id,
                             int professor_id, const char *semester) {

    /* 1. VALIDATE INPUT */
    int s_idx = search_student_by_id(students, s_count, student_id);
    if (s_idx == -1) { print_error("Student not found."); return 0; }

    int c_idx = search_course_by_id(courses, c_count, course_id);
    if (c_idx == -1) { print_error("Course not found."); return 0; }

    if (!validate_semester(semester)) return 0;

    /* 2. VALIDATION CHECKS */
    /* A) Already enrolled check */
    for (int i = 0; i < *e_count; i++) {
        if ((*enrollments)[i].student_id == student_id && 
            (*enrollments)[i].course_id == course_id &&
            str_equals_case_insensitive((*enrollments)[i].semester, semester)) {
            print_error("Student is already enrolled in this course for this semester.");
            return 0;
        }
    }

    /* B) Prerequisite check */
    if (!has_passed_prerequisite(student_id, courses[c_idx].prerequisites, courses, c_count, grades, g_count)) {
        printf(">> Error: Prerequisite not met! Requires: %s\n", courses[c_idx].prerequisites);
        return 0;
    }

    /* C) Capacity check */
    int current_enrolled_count = 0;
    for (int i = 0; i < *e_count; i++) {
        if ((*enrollments)[i].course_id == course_id && 
            str_equals_case_insensitive((*enrollments)[i].semester, semester)) {
            current_enrolled_count++;
        }
    }
    if (current_enrolled_count >= courses[c_idx].capacity) {
        print_error("Course is full for this semester!");
        return 0;
    }

    /* 3. ENROLLMENT RECORD CREATION */
    Enrollment *temp = (Enrollment *)realloc(*enrollments, (*e_count + 1) * sizeof(Enrollment));
    if (!temp) { print_error("Memory allocation failed."); return 0; }
    *enrollments = temp;

    int new_id = next_enrollment_id(*enrollments, *e_count);

    Enrollment *new_e = &(*enrollments)[*e_count];
    new_e->enrollment_id = new_id;
    new_e->student_id = student_id;
    new_e->course_id = course_id;
    new_e->professor_id = professor_id; /* Use professor_id from parameter */
    
    strncpy(new_e->semester, semester, MAX_SEMESTER_LEN - 1);
    new_e->semester[MAX_SEMESTER_LEN - 1] = '\0';
    
    /* DATE ASSIGNMENT LOGIC */
    /* Semester Format: "YYYY-SEASON" (e.g., 2024-FALL) */
    int year;
    
    /* Parse year and season (e.g., 2024-FALL -> year=2024, season=FALL) */
    /* Using '-' as delimiter in parsing */
    
    /* Simple parser for semester string */
    char temp_sem[MAX_SEMESTER_LEN];
    strcpy(temp_sem, semester);
    char *token_year = strtok(temp_sem, "-");
    char *token_season = strtok(NULL, "-");

    if (token_year && token_season && is_numeric(token_year)) {
        year = atoi(token_year);
        if (str_equals_case_insensitive(token_season, "SPRING")) {
            sprintf(new_e->enrollment_date, "%d-02-01", year); /* Spring: February 1 */
        } else if (str_equals_case_insensitive(token_season, "FALL")) {
            sprintf(new_e->enrollment_date, "%d-09-01", year); /* Fall: September 1 */
        } else if (str_equals_case_insensitive(token_season, "SUMMER")) {
            sprintf(new_e->enrollment_date, "%d-06-01", year); /* Summer: June 1 */
        } else {
             sprintf(new_e->enrollment_date, "%d-09-01", year); /* Default */
        }
    } else {
        /* If format doesn't match, use default date */
        strcpy(new_e->enrollment_date, "2024-09-01");
    }

    strcpy(new_e->status, "Enrolled"); 

    (*e_count)++;
    
    printf(">> SUCCESS: Student enrolled in %s (%s). Quota: %d/%d\n", 
           courses[c_idx].course_code, semester, current_enrolled_count + 1, courses[c_idx].capacity);
    return 1;
}

/**
 * @brief Drops a student from a course (deletes the enrollment record).
 * 
 * Removes an enrollment record for a student from a specific course.
 * Completed courses cannot be dropped.
 * 
 * @param enrollments Pointer to Enrollment array
 * @param e_count Pointer to enrollment count
 * @param students Student array for reference (reserved for future validation)
 * @param s_count Student count (reserved for future use)
 * @param courses Course array for reference (reserved for future validation)
 * @param c_count Course count (reserved for future use)
 * @param semester The semester string
 * @return 1 if successful, 0 otherwise
 */
int drop_student_from_course(Enrollment **enrollments, int *e_count, 
                             Student *students, int s_count, 
                             Course *courses, int c_count,
                             int student_id, int course_id,
                             const char *semester) {
    /* Suppress unused parameter warnings - parameters kept for future extensibility */
    (void)students;
    (void)s_count;
    (void)courses;
    (void)c_count;

    int found_index = -1;
    for (int i = 0; i < *e_count; i++) {
        if ((*enrollments)[i].student_id == student_id && 
            (*enrollments)[i].course_id == course_id &&
            str_equals_case_insensitive((*enrollments)[i].semester, semester)) {
            /* Completed course cannot be dropped rule */
            if (str_equals_case_insensitive((*enrollments)[i].status, "Completed")) {
                print_error("Cannot drop a completed course!");
                return 0;
            }
            found_index = i;
            break;
        }
    }

    if (found_index == -1) {
        print_error("Enrollment record not found.");
        return 0;
    }

    /* Delete operation (Shift elements) */
    for (int i = found_index; i < *e_count - 1; i++) {
        (*enrollments)[i] = (*enrollments)[i + 1];
    }
    (*e_count)--;

    if (*e_count > 0) {
        Enrollment *temp = (Enrollment *)realloc(*enrollments, (*e_count) * sizeof(Enrollment));
        if (temp) *enrollments = temp;
    } else {
        free(*enrollments);
        *enrollments = NULL;
    }

    printf(">> Student dropped from course successfully.\n");
    return 1;
}

/**
 * @brief Displays the course roster for a specific course and semester.
 * 
 * Prompts for course ID and semester, then displays all enrolled students
 * with their information in a formatted table.
 * 
 * @param enrollments Array of Enrollment structures
 * @param e_count Total number of enrollments
 * @param courses Array of Course structures
 * @param c_count Total number of courses
 * @param students Array of Student structures
 * @param stu_count Total number of students
 */
void display_course_roster(Enrollment *enrollments, int e_count, Course *courses, int c_count, Student *students, int stu_count) {
    int course_id;
    printf("Enter Course ID: ");
    if (scanf("%d", &course_id) != 1) { clear_input_buffer(); return; }
    clear_input_buffer();

    int c_idx = search_course_by_id(courses, c_count, course_id);
    if (c_idx == -1) { print_error("Course not found."); return; }

    char semester[MAX_SEMESTER_LEN];
    get_string_input("Enter Semester (e.g., 2024-FALL): ", semester, MAX_SEMESTER_LEN);
    trim_whitespace(semester);

    printf("\n+-----------------------------------------------------------------------------+\n");
    printf("| Class Roster: %-10s %-30s %-15s |\n", 
           courses[c_idx].course_code, courses[c_idx].course_name, semester);
    printf("+-----------------------------------------------------------------------------+\n");
    printf("| %-10s | %-20s | %-20s | %-12s |\n", "Stud. ID", "First Name", "Last Name", "Status");
    printf("+------------+----------------------+----------------------+--------------+\n");

    int count = 0;
    for (int i = 0; i < e_count; i++) {
        if (enrollments[i].course_id == course_id && 
            str_equals_case_insensitive(enrollments[i].semester, semester) &&
            str_equals_case_insensitive(enrollments[i].status, "Enrolled")) {
            
            int student_found = 0;
            for (int j = 0; j < stu_count; j++) {
                if (students[j].student_id == enrollments[i].student_id) {
                    printf("| %-10d | %-20s | %-20s | %-12s |\n", 
                           students[j].student_id, 
                           students[j].first_name, 
                           students[j].last_name,
                           enrollments[i].status);
                    count++;
                    student_found = 1;
                    break;
                }
            }
            if (!student_found) {
                printf("| %-10d | %-20s | %-20s | %-12s |\n", enrollments[i].student_id, "UNKNOWN", "UNKNOWN", enrollments[i].status);
            }
        }
    }

    if (count == 0) {
        printf("| No active students found for this course in %-26s |\n", semester);
    }
    printf("+-----------------------------------------------------------------------------+\n");
    printf("| Total Students: %-56d |\n", count);
    printf("+-----------------------------------------------------------------------------+\n");
}

/**
 * @brief Prints the list of courses a specific student is enrolled in.
 * 
 * Displays all enrollment records for a given student ID in a formatted table
 * including course code, course name, semester, and status.
 * 
 * @param enrollments Array of Enrollment structures
 * @param enr_count Total number of enrollments
 * @param courses Array of Course structures
 * @param course_count Total number of courses
 * @param student_id The ID of the student
 */
void print_student_enrollments(Enrollment *enrollments, int enr_count, Course *courses, int course_count, int student_id) {
    printf("\n+-------------------------------------------------------------------------------+\n");
    printf("| Enrollments for Student ID: %-48d |\n", student_id);
    printf("+-----------+-------------+--------------------------+-------------+----------+\n");
    printf("| Enroll ID | Code        | Course Name              | Semester    | Status   |\n");
    printf("+-----------+-------------+--------------------------+-------------+----------+\n");
    
    int count = 0;
    for (int i = 0; i < enr_count; i++) {
        if (enrollments[i].student_id == student_id) {
            /* Find course details */
            int c_idx = search_course_by_id(courses, course_count, enrollments[i].course_id);
            if (c_idx != -1) {
                printf("| %-9d | %-11s | %-24s | %-11s | %-8s |\n",
                       enrollments[i].enrollment_id,
                       courses[c_idx].course_code,
                       courses[c_idx].course_name,
                       enrollments[i].semester,
                       enrollments[i].status);
                count++;
            } else {
                printf("| %-9d | %-11s | %-24s | %-11s | %-8s |\n",
                       enrollments[i].enrollment_id,
                       "UNKNOWN",
                       "Course Not Found",
                       enrollments[i].semester,
                       enrollments[i].status);
                count++;
            }
        }
    }
    
    if (count == 0) {
        printf("| No enrollments found for this student.                                        |\n");
    }
    printf("+-----------+-------------+--------------------------+-------------+----------+\n");
    printf("| Total Enrollments: %-56d |\n", count);
    printf("+-------------------------------------------------------------------------------+\n");
}

/**
 * @brief Prints the list of students enrolled in a specific course.
 * 
 * Displays all students enrolled in a course for a given semester,
 * showing both "Enrolled" and "Completed" status records.
 * 
 * @param enrollments Array of Enrollment structures
 * @param enr_count Total number of enrollments
 * @param students Array of Student structures
 * @param stu_count Total number of students
 * @param course_id The ID of the course
 * @param semester The semester string
 */
void print_course_roster(Enrollment *enrollments, int enr_count, Student *students, int stu_count, int course_id, const char *semester) {
    printf("\n+-------------------------------------------------------------------------+\n");
    printf("| Course Roster: Course ID %-5d Semester: %-25s |\n", course_id, semester);
    printf("+--------------+--------------------+--------------------+---------------+\n");
    printf("| Student ID   | First Name         | Last Name          | Status        |\n");
    printf("+--------------+--------------------+--------------------+---------------+\n");
    
    int count = 0;
    for (int i = 0; i < enr_count; i++) {
        if (enrollments[i].course_id == course_id && 
            str_equals_case_insensitive(enrollments[i].semester, semester)) {
            /* Show all statuses (Enrolled + Completed) */
            
            int student_found = 0;
            for (int j = 0; j < stu_count; j++) {
                if (students[j].student_id == enrollments[i].student_id) {
                    printf("| %-12d | %-18s | %-18s | %-13s |\n",
                           students[j].student_id,
                           students[j].first_name,
                           students[j].last_name,
                           enrollments[i].status);
                    count++;
                    student_found = 1;
                    break;
                }
            }
            if (!student_found) {
                printf("| %-12d | %-18s | %-18s | %-13s |\n",
                       enrollments[i].student_id,
                       "UNKNOWN",
                       "UNKNOWN",
                       enrollments[i].status);
                count++;
            }
        }
    }
    
    if (count == 0) {
        printf("| No students found for this course in %-30s |\n", semester);
    }
    printf("+--------------+--------------------+--------------------+---------------+\n");
    printf("| Total Students: %-54d |\n", count);
    printf("+-------------------------------------------------------------------------+\n");
}

/**
 * @brief Saves all enrollment data to a CSV file.
 * 
 * Writes all enrollment records to a CSV file with proper header row.
 * 
 * @param filename The path to the CSV file
 * @param enrollments Array of Enrollment structures
 * @param count Total number of enrollments
 * @return 1 if successful, 0 otherwise
 */
int save_enrollments(const char *filename, Enrollment *enrollments, int count) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        print_error("Could not open file for saving enrollments.");
        return 0;
    }

    fprintf(file, "enrollment_id,student_id,course_id,professor_id,semester,enrollment_date,status\n");

    for (int i = 0; i < count; i++) {
        fprintf(file, "%d,%d,%d,%d,%s,%s,%s\n",
                enrollments[i].enrollment_id,
                enrollments[i].student_id,
                enrollments[i].course_id,
                enrollments[i].professor_id,
                enrollments[i].semester,
                enrollments[i].enrollment_date,
                enrollments[i].status);
    }

    fclose(file);
    return 1;
}

/**
 * @brief Frees the memory allocated for the enrollment array.
 * 
 * @param enrollments Pointer to the enrollment array
 */
void free_enrollments(Enrollment *enrollments) {
    if (enrollments != NULL) free(enrollments);
}

/**
 * @brief Deletes all enrollment records associated with a specific course ID.
 * 
 * Removes all enrollments for a given course and resizes the array.
 * 
 * @param enrollments Pointer to Enrollment array
 * @param enrollment_count Pointer to enrollment count
 * @param course_id The course ID to clear
 */
void delete_enrollments_by_course_id(Enrollment **enrollments, int *enrollment_count, int course_id) {
    if (enrollments == NULL || *enrollments == NULL) return;

    int new_count = 0;
    /* Shift non-deleted records to the front of the array */
    for (int i = 0; i < *enrollment_count; i++) {
        if ((*enrollments)[i].course_id != course_id) {
            (*enrollments)[new_count++] = (*enrollments)[i];
        }
    }
    
    /* If records were deleted (new count is less than old count) */
    if (new_count < *enrollment_count) {
        if (new_count == 0) {
            /* If all records deleted, free memory */
            free(*enrollments);
            *enrollments = NULL;
        } else {
            /* Shrink array size */
            Enrollment *temp = (Enrollment *)realloc(*enrollments, new_count * sizeof(Enrollment));
            if (temp) {
                *enrollments = temp; 
            }
        }
        *enrollment_count = new_count;
    }
    printf(">> Enrollment records updated. New count: %d\n", *enrollment_count);
}

/**
 * @brief Deletes a professor and all associated enrollments with "Enrolled" status.
 * 
 * Performs a cascading delete: removes all active enrollments for the professor
 * and then removes the professor record itself.
 * 
 * @param professors Pointer to professor array
 * @param prof_count Pointer to professor count
 * @param enrollments Pointer to enrollment array
 * @param e_count Pointer to enrollment count
 * @param prof_id The professor ID to delete
 * @return 1 if successful, 0 otherwise
 */
int delete_professor_cascade(Professor **professors, int *prof_count, Enrollment **enrollments, int *e_count, int prof_id) {
    int index = search_professor_by_id(*professors, *prof_count, prof_id);
    if (index == -1) {
        print_error("Professor ID not found.");
        return 0;
    }

    /* 1. Delete all enrollments where professor_id matches AND status is "Enrolled" */
    if (enrollments != NULL && e_count != NULL && *enrollments != NULL) {
        int deleted_count = 0;
        for (int i = *e_count - 1; i >= 0; i--) {
            /* Only delete if professor_id matches AND status is "Enrolled" */
            if ((*enrollments)[i].professor_id == prof_id && strcmp((*enrollments)[i].status, "Enrolled") == 0) {
                /* Shift remaining enrollments */
                for (int j = i; j < *e_count - 1; j++) {
                    (*enrollments)[j] = (*enrollments)[j + 1];
                }
                (*e_count)--;
                deleted_count++;
            }
        }
        
        /* Resize enrollment array if needed */
        if (*e_count > 0) {
            Enrollment *temp_ptr = (Enrollment *)realloc(*enrollments, (*e_count) * sizeof(Enrollment));
            if (temp_ptr != NULL) *enrollments = temp_ptr;
        } else if (*e_count == 0) {
            free(*enrollments);
            *enrollments = NULL;
        }
        
        printf(">> Deleted %d 'Enrolled' enrollments associated with professor ID %d.\n", deleted_count, prof_id);
    }

    /* 2. Delete the professor record */
    for (int i = index; i < *prof_count - 1; i++) {
        (*professors)[i] = (*professors)[i + 1];
    }

    (*prof_count)--;

    if (*prof_count > 0) {
        Professor *temp = (Professor *)realloc(*professors, (*prof_count) * sizeof(Professor));
        if (temp != NULL) *professors = temp;
    } else {
        free(*professors);
        *professors = NULL;
    }

    printf(">> Professor deleted successfully.\n");
    return 1;
}