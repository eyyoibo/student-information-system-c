/**
 * @file grade.c
 * @brief Grade Management Module Implementation
 * 
 * Implements all grade management functionality including:
 * - Loading/saving grades from/to CSV files
 * - Recording and updating grades
 * - GPA calculation
 * - Grade statistics and distribution
 * - Transcript generation
 * 
 * @author Student Information System Team
 * @date January 2026
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "grade.h"
#include "utils.h"
#include "course.h"     
#include "enrollment.h" 
#include "student.h"

/**
 * @brief Gets the grade point weight for a letter grade
 * 
 * Converts letter grade to GPA weight for calculations.
 * Standard 4.0 scale: A=4.0, A-=3.7, B+=3.3, B=3.0, etc.
 * 
 * @param letter_grade The letter grade string
 * @return The corresponding grade point value
 */
float get_grade_weight(const char *letter_grade) {
    if (!letter_grade) return 0.0;
    if (strcmp(letter_grade, "A") == 0) return 4.0;
    if (strcmp(letter_grade, "A-") == 0) return 3.7;
    if (strcmp(letter_grade, "B+") == 0) return 3.3;
    if (strcmp(letter_grade, "B") == 0) return 3.0;
    if (strcmp(letter_grade, "B-") == 0) return 2.7;
    if (strcmp(letter_grade, "C+") == 0) return 2.3;
    if (strcmp(letter_grade, "C") == 0) return 2.0;
    if (strcmp(letter_grade, "F") == 0) return 0.0;
    return 0.0; 
}

/**
 * @brief Checks if a string represents a valid integer
 * 
 * @param str The string to check
 * @return 1 if valid integer, 0 otherwise
 */
static int is_string_integer(const char *str) {
    if (!str || *str == '\0') return 0;
    while (*str) {
        if (!isdigit((unsigned char)*str)) return 0;
        str++;
    }
    return 1;
}

/**
 * @brief Checks if a string represents a valid float
 * 
 * @param str The string to check
 * @return 1 if valid float, 0 otherwise
 */
static int is_string_float(const char *str) {
    if (!str || *str == '\0') return 0;
    int dot_seen = 0;
    while (*str) {
        if (*str == '.') {
            if (dot_seen) return 0; /* Only one decimal point allowed */
            dot_seen = 1;
        } else if (!isdigit((unsigned char)*str)) {
            return 0;
        }
        str++;
    }
    return 1;
}

/**
 * @brief Loads grade records from a CSV file
 * 
 * Reads grade data from CSV file, validates each record, and stores
 * valid grades in dynamically allocated memory.
 * 
 * @param filename Path to the CSV file
 * @param grades Pointer to the grade array pointer
 * @param count Pointer to store the number of loaded grades
 * @param enrollments Array of enrollments for validation
 * @param enr_count Number of enrollments
 * @return 1 on success, 0 on failure
 */
int load_grades(const char *filename, Grade **grades, int *count, Enrollment *enrollments, int enr_count) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        print_error("Could not open grades file.");
        return 0;
    }

    char line[MAX_LINE_LEN];
    char *fields[7];
    int capacity = 10;
    int line_number = 0;

    *count = 0;
    *grades = NULL;

    /* Skip header line */
    if (fgets(line, sizeof(line), file) == NULL) {
        fclose(file);
        return 1; 
    }
    line_number++;

    *grades = (Grade *)malloc(capacity * sizeof(Grade));
    if (!*grades) {
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

        /* 1. DATA TYPE VALIDATION (String -> Numeric) */
        char *str_grade_id = fields[0];
        char *str_enr_id   = fields[1];
        char *str_stu_id   = fields[2];
        char *str_crs_id   = fields[3];
        char *str_numeric  = fields[5];
        char *temp_semester = fields[6];
        
        trim_whitespace(str_grade_id);
        trim_whitespace(str_enr_id);
        trim_whitespace(str_stu_id);
        trim_whitespace(str_crs_id);
        trim_whitespace(str_numeric);
        trim_whitespace(temp_semester);

        if (!is_string_integer(str_grade_id)) {
            fprintf(stderr, "[ERROR] Line %d: Grade ID '%s' is not numeric. Skipped.\n", line_number, str_grade_id); continue;
        }
        if (!is_string_integer(str_enr_id)) {
            fprintf(stderr, "[ERROR] Line %d: Enrollment ID '%s' is not numeric. Skipped.\n", line_number, str_enr_id); continue;
        }
        if (!is_string_integer(str_stu_id)) {
            fprintf(stderr, "[ERROR] Line %d: Student ID '%s' is not numeric. Skipped.\n", line_number, str_stu_id); continue;
        }
        if (!is_string_float(str_numeric)) {
            fprintf(stderr, "[ERROR] Line %d: Numeric Grade '%s' is not a valid number. Skipped.\n", line_number, str_numeric); continue;
        }

        int temp_grade_id = atoi(str_grade_id);
        int temp_enr_id   = atoi(str_enr_id);
        int temp_stu_id   = atoi(str_stu_id);
        float temp_numeric = (float)atof(str_numeric);

        /* 2. DUPLICATE GRADE ID CHECK */
        if (search_grade_by_id(*grades, *count, temp_grade_id) != -1) {
            fprintf(stderr, "[ERROR] Line %d: Duplicate Grade ID (%d). Already loaded. Skipped.\n", line_number, temp_grade_id);
            continue;
        }

        /* 3. STUDENT ID FORMAT CHECK */
        if (!validate_student_id(temp_stu_id)) {
            fprintf(stderr, "[ERROR] Line %d: Invalid Student ID (%d). Skipped.\n", line_number, temp_stu_id);
            continue;
        }

        /* 4. NUMERIC GRADE RANGE CHECK */
        if (!validate_numeric_grade(temp_numeric)) {
            fprintf(stderr, "[ERROR] Line %d: Invalid Numeric Grade (%.2f). Must be 0-100. Skipped.\n", line_number, temp_numeric);
            continue;
        }

        /* 5. ENROLLMENT ID EXISTENCE CHECK */
        /* Note: load_enrollments must be called before load_grades in main */
        int enrollment_found = 0;
        if (enrollments != NULL && enr_count > 0) {
            if (search_enrollment_by_id(enrollments, enr_count, temp_enr_id) != -1) {
                enrollment_found = 1;
            }
        }
        
        if (!enrollment_found) {
             fprintf(stderr, "[ERROR] Line %d: Enrollment ID (%d) not found in system. Cannot load grade. Skipped.\n", line_number, temp_enr_id);
             continue;
        }

        /* 6. SEMESTER FORMAT CHECK */
        if (!validate_semester(temp_semester)) {
            fprintf(stderr, "[ERROR] Line %d: Invalid Semester '%s'. Skipped.\n", line_number, temp_semester);
            continue;
        }

        /* MEMORY ALLOCATION AND RECORD CREATION */
        if (*count >= capacity) {
            int new_capacity = capacity * 2;
            Grade *temp = (Grade *)realloc(*grades, new_capacity * sizeof(Grade));
            if (!temp) {
                print_error("Memory reallocation failed.");
                free(*grades);
                *grades = NULL;
                *count = 0;
                fclose(file);
                return 0;
            }
            *grades = temp;
            capacity = new_capacity;
        }

        Grade *g = &(*grades)[*count];
        g->grade_id      = temp_grade_id;
        g->enrollment_id = temp_enr_id;
        g->student_id    = temp_stu_id;
        g->course_id     = atoi(str_crs_id); /* Simple conversion */
        
        strncpy(g->letter_grade, fields[4], MAX_LETTER_LEN - 1); g->letter_grade[MAX_LETTER_LEN - 1] = '\0';
        trim_whitespace(g->letter_grade);

        g->numeric_grade = temp_numeric;
        strncpy(g->semester, temp_semester, MAX_SEMESTER_LEN - 1); g->semester[MAX_SEMESTER_LEN - 1] = '\0';

        (*count)++;
    }
    fclose(file);
    
    if (*count == 0 && *grades != NULL) { free(*grades); *grades = NULL; }
    
    printf(">> Grades Loaded Successfully: %d\n", *count);
    return 1;
}

/**
 * @brief Searches for a grade record by ID
 * 
 * @param grades Array of grades to search
 * @param count Number of grades
 * @param grade_id The grade ID to find
 * @return Index of the grade if found, -1 otherwise
 */
int search_grade_by_id(Grade *grades, int count, int grade_id){
    if (grades == NULL || count <= 0) return -1;
    for (int i = 0; i < count; i++) {
        if (grades[i].grade_id == grade_id) return i;
    }
    return -1;
}

/**
 * @brief Updates an existing grade record
 * 
 * Updates the numeric grade value and automatically recalculates
 * the corresponding letter grade.
 * 
 * @param grades Array of grades
 * @param grade_count Number of grades
 * @param grade_id ID of the grade to update
 * @param new_numeric_grade New numeric score (0-100)
 * @return 1 on success, 0 on failure
 */
int update_grade(Grade *grades, int grade_count, int grade_id, float new_numeric_grade) {
    if (!grades || grade_count <= 0) return 0;
    
    int index = search_grade_by_id(grades, grade_count, grade_id);
    if (index == -1) { 
        print_error("Grade not found."); 
        return 0; 
    }
    
    if (!validate_numeric_grade(new_numeric_grade)) {
        print_error("Invalid numeric grade. Must be between 0.0 and 100.0");
        return 0;
    }

    /* Auto-update letter grade */
    grades[index].numeric_grade = new_numeric_grade;
    compute_letter_grade(new_numeric_grade, grades[index].letter_grade, MAX_LETTER_LEN);

    printf("Grade updated successfully to %.2f (%s).\n", grades[index].numeric_grade, grades[index].letter_grade);
    return 1;
}

/**
 * @brief Calculates statistics for a course in a specific semester
 * 
 * Computes average, minimum, and maximum grades for all students
 * enrolled in a course during a given semester.
 * 
 * @param grades Array of grades
 * @param grade_count Number of grades
 * @param course_id Course to calculate statistics for
 * @param semester Semester to filter by
 * @param avg Pointer to store average grade
 * @param min Pointer to store minimum grade
 * @param max Pointer to store maximum grade
 * @return 1 if statistics calculated, 0 if no grades found
 */
int calculate_course_statistics(Grade *grades, int grade_count, int course_id, const char *semester, float *avg, float *min, float *max){
    if (!grades || grade_count <= 0) return 0;
    float sum = 0.0;
    int count = 0;
    *min = 101.0; *max = -1.0;

    for (int i = 0; i < grade_count; i++) {
        if (grades[i].course_id == course_id && strcmp(grades[i].semester, semester) == 0) {
            float g = grades[i].numeric_grade;
            sum += g; count++;
            if (g < *min) *min = g;
            if (g > *max) *max = g;
        }
    }
    if (count == 0) return 0;
    *avg = sum / count;
    return 1;
}

/**
 * @brief Frees memory allocated for grades
 * 
 * @param grades Pointer to grade array to free
 */
void free_grades(Grade *grades) { if (grades) free(grades); }

/**
 * @brief Saves all grades to a CSV file
 * 
 * @param filename Path to the output file
 * @param grades Array of grades to save
 * @param count Number of grades
 * @return 1 on success, 0 on failure
 */
int save_grades(const char *filename, Grade *grades, int count){
    FILE *file =fopen(filename, "w");
    if(!file) {
        print_error("Could not open grades file for saving.");
        return 0;
    }
    fprintf(file, "grade_id,enrollment_id,student_id,course_id,letter_grade,numeric_grade,semester\n");
    for(int i = 0; i < count; i++){
        fprintf(file,"%d,%d,%d,%d,%s,%f,%s\n",                
                grades[i].grade_id, grades[i].enrollment_id, grades[i].student_id, 
                grades[i].course_id, grades[i].letter_grade, grades[i].numeric_grade, grades[i].semester);
    }
    fclose(file);
    return 1;
}

/**
 * @brief Searches for a grade by enrollment ID
 * 
 * @param grades Array of grades
 * @param count Number of grades
 * @param enrollment_id The enrollment ID to search for
 * @return Index of the grade if found, -1 otherwise
 */
int search_grade_by_enrollment_id(Grade *grades, int count, int enrollment_id){
    if(grades==NULL || count<=0) return -1;
    for(int i = 0; i < count; i++){
        if(grades[i].enrollment_id == enrollment_id) return i;
    }
    return -1;
}

/**
 * @brief Records a new grade for an enrollment
 * 
 * Creates a new grade record, automatically computes the letter grade,
 * and updates the enrollment status to "Completed".
 * 
 * @param grades Pointer to grade array (may be reallocated)
 * @param grade_count Pointer to grade count
 * @param enrollments Array of enrollments
 * @param enr_count Number of enrollments
 * @param enrollment_id The enrollment to grade
 * @param numeric_grade The numeric score (0-100)
 * @return 1 on success, 0 on failure
 */
int record_grade(Grade **grades, int *grade_count, Enrollment *enrollments, int enr_count, int enrollment_id, float numeric_grade){
    
    /* 1. Validate numeric grade */
    if (!validate_numeric_grade(numeric_grade)) {
        print_error("Invalid numeric grade. Must be 0-100.");
        return 0;
    }

    /* 2. Find the enrollment record */
    int target_index = -1;
    for(int i = 0; i < enr_count; i++){
        if(enrollments[i].enrollment_id == enrollment_id){ 
            target_index = i; 
            break; 
        }
    }
    
    if(target_index == -1){ 
        print_error("Enrollment ID not found. (Internal Error)"); 
        return 0; 
    }
    
    /* 3. Status Check (Case-insensitive) */
    /* Cannot record grade if already Completed or Dropped */
    if (!str_equals_case_insensitive(enrollments[target_index].status, "Enrolled")) {
        printf(">> Error: Cannot record grade. Student status is '%s' (Must be 'Enrolled').\n", enrollments[target_index].status);
        return 0;
    }
    
    /* 4. Duplicate grade check */
    for(int i = 0; i < *grade_count; i++){
        if((*grades)[i].enrollment_id == enrollment_id){ 
            print_error("Grade already exists for this enrollment. Use 'Update Grade' instead."); 
            return 0; 
        }
    }

    /* 5. Generate new grade ID */
    int max_id = 0;
    for (int i = 0; i < *grade_count; i++) { 
        if ((*grades)[i].grade_id > max_id) max_id = (*grades)[i].grade_id; 
    }
    
    /* 6. Allocate memory */
    Grade *temp = (Grade *)realloc(*grades, sizeof(Grade) * (*grade_count + 1));
    if (!temp) {
        print_error("Memory allocation failed.");
        return 0;
    }
    *grades = temp;

    /* 7. Save grade data */
    Grade *new_record = &(*grades)[*grade_count];
    new_record->grade_id = max_id + 1;
    new_record->enrollment_id = enrollment_id;
    new_record->student_id = enrollments[target_index].student_id;
    new_record->course_id  = enrollments[target_index].course_id;
    
    /* Copy semester from enrollment */
    strncpy(new_record->semester, enrollments[target_index].semester, MAX_SEMESTER_LEN - 1);
    new_record->semester[MAX_SEMESTER_LEN - 1] = '\0';
    
    /* Calculate letter grade */
    new_record->numeric_grade = numeric_grade;
    compute_letter_grade(numeric_grade, new_record->letter_grade, MAX_LETTER_LEN);

    /* 8. Update enrollment status to Completed */
    strcpy(enrollments[target_index].status, "Completed");

    (*grade_count)++;
    
    printf("\n>> SUCCESS: Grade recorded: %.2f (%s)\n", new_record->numeric_grade, new_record->letter_grade);
    printf(">> System Update: Enrollment status changed to 'Completed'.\n");
    
    return 1;
}

/**
 * @brief Displays grade distribution for a course
 * 
 * Shows a bar chart of letter grade distribution with percentages.
 * 
 * @param grades Array of grades
 * @param grade_count Number of grades
 * @param course_id Course to display distribution for
 * @param semester Semester to filter by
 */
void display_grade_distribution(Grade *grades, int grade_count, int course_id, const char *semester) {
    const char *LABELS[] = {"A", "A-", "B+", "B", "B-", "C+", "C"};
    int counts[7] = {0}; 
    int total = 0;
    
    for (int i = 0; i < grade_count; i++) {
        if (grades[i].course_id == course_id && strcmp(grades[i].semester, semester) == 0) {
            total++;
            for (int j = 0; j < 7; j++) {
                if (strcmp(grades[i].letter_grade, LABELS[j]) == 0) { counts[j]++; break; }
            }
        }
    }
    
    printf("\n");
    printf("+--------------------------------------------------------------+\n");
    printf("| Grade Distribution: Course %-5d Semester: %-15s |\n", course_id, semester);
    printf("+--------------------------------------------------------------+\n");
    
    if (total == 0) {
        printf("| No grades found.                                             |\n");
    } else {
        printf("| %-5s | Count | Percentage | Visual Distribution           |\n", "Grade");
        printf("+-------+-------+------------+-------------------------------+\n");
        
        for (int i = 0; i < 7; i++) {
            float percentage = (float)counts[i]/total*100.0;
            int bar_length = (int)(percentage / 5.0); // Max 20 chars
            
            printf("| %-5s | %5d | %8.1f%% | ", LABELS[i], counts[i], percentage);
            for (int j = 0; j < bar_length && j < 20; j++) printf("#");
            for (int j = bar_length; j < 20; j++) printf(" ");
            printf("          |\n");
        }
    }
    printf("+--------------------------------------------------------------+\n");
    printf("| Total Grades: %-45d |\n", total);
    printf("+--------------------------------------------------------------+\n");
}

/**
 * @brief Deletes all grades for a specific course
 * 
 * Used when a course is deleted to maintain data integrity.
 * 
 * @param grades Pointer to grade array
 * @param grade_count Pointer to grade count
 * @param course_id ID of the course to delete grades for
 */
void delete_grades_by_course_id(Grade **grades, int *grade_count, int course_id) {
    if (grades == NULL || *grades == NULL) return;

    int new_count = 0;
    /* Move non-deleted grades to the beginning */
    for (int i = 0; i < *grade_count; i++) {
        if ((*grades)[i].course_id != course_id) {
            (*grades)[new_count++] = (*grades)[i];
        }
    }

    if (new_count < *grade_count) {
        if (new_count == 0) {
            free(*grades);
            *grades = NULL;
        } else {
            Grade *temp = (Grade *)realloc(*grades, new_count * sizeof(Grade));
            if (temp) *grades = temp;
        }
        *grade_count = new_count;
    }
    printf(">> Related grades deleted. New grade count: %d\n", *grade_count);
}

/**
 * @brief Prints a complete student transcript with CGPA
 * 
 * Displays all grades grouped by semester with running GPA
 * calculation. Shows course details and cumulative GPA.
 * 
 * @param student_id ID of the student
 * @param grades Array of grades
 * @param g_count Number of grades
 * @param courses Array of courses for name lookup
 * @param c_count Number of courses
 */
void print_student_transcript(int student_id, Grade *grades, int g_count, Course *courses, int c_count) {
    printf("\n======================================================\n");
    printf("               OFFICIAL TRANSCRIPT                    \n");
    printf("======================================================\n");
    printf("Student ID: %d\n", student_id);
    
    /* Step 1: Find all unique semesters for this student */
    char semesters[20][50]; /* Max 20 semesters assumed */
    int sem_count = 0;

    for (int i = 0; i < g_count; i++) {
        if (grades[i].student_id == student_id) {
            /* Check if semester already in list */
            int exists = 0;
            for (int j = 0; j < sem_count; j++) {
                if (strcmp(semesters[j], grades[i].semester) == 0) {
                    exists = 1;
                    break;
                }
            }
            /* Add if not exists */
            if (!exists && sem_count < 20) {
                strcpy(semesters[sem_count], grades[i].semester);
                sem_count++;
            }
        }
    }

    /* Step 2: Sort semesters chronologically */
    /* Simple bubble sort (year strings are sortable: "2021-FALL", "2022-SPRING") */
    for (int i = 0; i < sem_count - 1; i++) {
        for (int j = 0; j < sem_count - i - 1; j++) {
            if (strcmp(semesters[j], semesters[j+1]) > 0) {
                char temp[50];
                strcpy(temp, semesters[j]);
                strcpy(semesters[j], semesters[j+1]);
                strcpy(semesters[j+1], temp);
            }
        }
    }

    /* Step 3: Print grades grouped by semester */
    float total_gpa_points = 0;  /* Sum of (credits * grade_weight) */
    int total_credits = 0;

    if (sem_count == 0) {
        printf("\n[No academic records found for this student]\n");
    }

    for (int s = 0; s < sem_count; s++) {
        printf("\n+------------------------------------------------------------------------+\n");
        printf("| SEMESTER: %-62s |\n", semesters[s]);
        printf("+------------+--------------------------------+-------+---------+-----------+\n");
        printf("| Code       | Course Name                    | Cred  | Grade   | Numeric   |\n");
        printf("+------------+--------------------------------+-------+---------+-----------+\n");

        float term_gpa_points = 0;  /* Sum of (credits * grade_weight) for this term */
        int term_credits = 0;
        float term_numeric_sum = 0;

        for (int i = 0; i < g_count; i++) {
            /* Only print grades for current semester */
            if (grades[i].student_id == student_id && strcmp(grades[i].semester, semesters[s]) == 0) {
                
                /* Find course details */
                int c_idx = -1;
                for(int k=0; k<c_count; k++) { 
                    if(courses[k].course_id == grades[i].course_id) { c_idx = k; break; } 
                }

                if (c_idx != -1) {
                    float grade_weight = get_grade_weight(grades[i].letter_grade);
                    
                    printf("| %-10s | %-30s | %-5d | %-7s | %-9.2f |\n", 
                           courses[c_idx].course_code, 
                           courses[c_idx].course_name, 
                           courses[c_idx].credits, 
                           grades[i].letter_grade, 
                           grades[i].numeric_grade);

                    term_credits += courses[c_idx].credits;
                    term_gpa_points += courses[c_idx].credits * grade_weight;
                    term_numeric_sum += grades[i].numeric_grade;
                    total_credits += courses[c_idx].credits;
                    total_gpa_points += courses[c_idx].credits * grade_weight;
                }
            }
        }
        
        /* Term Summary */
        printf("+------------+--------------------------------+-------+---------+-----------+\n");
        if (term_credits > 0) {
            float term_gpa = term_gpa_points / term_credits;
            int course_count_term = 0;
            for (int i = 0; i < g_count; i++) {
                if (grades[i].student_id == student_id && strcmp(grades[i].semester, semesters[s]) == 0) {
                    course_count_term++;
                }
            }
            float term_avg = term_numeric_sum / course_count_term;
            printf("| TERM AVG   | Credits: %-21d | %-5d | GPA:%-3.2f| %-9.2f |\n", 
                   term_credits, term_credits, term_gpa, term_avg);
        } else {
            printf("| TERM AVG   | No courses in this semester    |       |         |           |\n");
        }
        printf("+------------+--------------------------------+-------+---------+-----------+\n");
    }

    printf("\n+------------------------------------------------------------------------+\n");
    printf("| CUMULATIVE GPA: ");
    if (total_credits > 0) {
        float cumulative_gpa = total_gpa_points / total_credits;
        printf("%-53.2f |\n", cumulative_gpa);
    } else {
        printf("N/A (no grades)                                       |\n");
    }
    printf("+------------------------------------------------------------------------+\n");
}

/**
 * @brief Calculates and displays GPA for a student
 * 
 * Computes cumulative GPA using the best grade for repeated courses.
 * Updates the student record with the calculated GPA.
 * 
 * @param student_id ID of the student
 * @param students Array of students
 * @param s_count Number of students
 * @param grades Array of grades
 * @param g_count Number of grades
 * @param courses Array of courses for credit lookup
 * @param c_count Number of courses
 */
void calculate_and_display_gpa(int student_id, Student *students, int s_count, Grade *grades, int g_count, Course *courses, int c_count) {
    
    int s_idx = search_student_by_id(students, s_count, student_id);
    if (s_idx == -1) {
        print_error("Student not found.");
        return;
    }

    float total_points = 0.0;
    int total_credits = 0;

    typedef struct {
        int course_id;
        float max_grade_point; 
    } UniqueCourse;

    UniqueCourse processed_courses[100]; 
    int processed_count = 0;

    for (int i = 0; i < g_count; i++) {
        if (grades[i].student_id == student_id) {
            
            float current_point = get_grade_weight(grades[i].letter_grade);
            int course_already_processed = 0;
            int found_index = -1;

            for (int k = 0; k < processed_count; k++) {
                if (processed_courses[k].course_id == grades[i].course_id) {
                    course_already_processed = 1;
                    found_index = k;
                    break;
                }
            }

            if (course_already_processed) {
                if (current_point > processed_courses[found_index].max_grade_point) {
                    processed_courses[found_index].max_grade_point = current_point;
                }
            } else {
                if (processed_count < 100) {
                    processed_courses[processed_count].course_id = grades[i].course_id;
                    processed_courses[processed_count].max_grade_point = current_point;
                    processed_count++;
                }
            }
        }
    }

    printf("\n--- GPA Calculation for %s %s ---\n", students[s_idx].first_name, students[s_idx].last_name);
    printf("%-10s %-30s %-10s %-10s %-12s\n", "Course ID", "Course Name", "Credits", "Grade Pt", "Weighted");
    printf("------------------------------------------------------------------------\n");

    for (int i = 0; i < processed_count; i++) {
        int c_idx = search_course_by_id(courses, c_count, processed_courses[i].course_id);
        
        if (c_idx != -1) {
            int credits = courses[c_idx].credits;
            float grade_point = processed_courses[i].max_grade_point;
            float weighted_points = grade_point * credits;
            
            printf("%-10d %-30s %-10d %-10.2f %-12.2f\n", 
                   courses[c_idx].course_id, 
                   courses[c_idx].course_name, 
                   credits, 
                   grade_point,
                   weighted_points);

            total_points += weighted_points;
            total_credits += credits;
        }
    }

    printf("------------------------------------------------------------------------\n");
    
    float calculated_gpa = 0.0;
    if (total_credits > 0) {
        calculated_gpa = total_points / total_credits;
        printf("Total Credits : %d\n", total_credits);
        printf("Total Points  : %.2f\n", total_points);
        printf("Calculated GPA: %.2f / 4.00\n", calculated_gpa);
    } else {
        printf("GPA           : 0.00 (No graded courses found)\n");
    }

    students[s_idx].gpa = calculated_gpa;
    printf(">> System Update: Student record updated with new GPA (%.2f).\n", calculated_gpa);
    printf(">> (Don't forget to 'Save All Changes' from main menu!)\n");
}

/**
 * @brief Displays course statistics and grade distribution
 * 
 * Combines statistics (average, min, max) with a visual grade
 * distribution chart for a specific course and semester.
 * 
 * @param grades Array of grades
 * @param grade_count Number of grades
 * @param course_id Course to analyze
 * @param semester Semester to filter by
 */
void display_course_statistics_and_distribution(Grade *grades, int grade_count, int course_id, const char *semester) {
    /* 1. Calculate statistics */
    float avg = 0.0f, min = 100.0f, max = 0.0f;
    int count = 0;
    
    for (int i = 0; i < grade_count; i++) {
        if (grades[i].course_id == course_id && strcmp(grades[i].semester, semester) == 0) {
            if (grades[i].numeric_grade < min) min = grades[i].numeric_grade;
            if (grades[i].numeric_grade > max) max = grades[i].numeric_grade;
            avg += grades[i].numeric_grade;
            count++;
        }
    }
    
    // Display statistics and distribution
    printf("\n+------------------------------------------------------------------------------+\n");
    printf("|                          COURSE GRADE REPORT                                |\n");
    printf("| Course ID: %-5d  Semester: %-45s |\n", course_id, semester);
    printf("+------------------------------------------------------------------------------+\n");
    
    if (count == 0) {
        printf("| No grades found for this course.                                            |\n");
    } else {
        avg /= count;
        
        // Statistics section
        printf("| STATISTICS:                                                                 |\n");
        printf("|   Total Students: %-56d |\n", count);
        printf("|   Average Score:  %-56.2f |\n", avg);
        printf("|   Highest Score:  %-56.2f |\n", max);
        printf("|   Lowest Score:   %-56.2f |\n", min);
        
        printf("|                                                                             |\n");
        
        // Distribution section
        const char *LABELS[] = {"A", "A-", "B+", "B", "B-", "C+", "C"};
        int counts[7] = {0};
        
        for (int i = 0; i < grade_count; i++) {
            if (grades[i].course_id == course_id && strcmp(grades[i].semester, semester) == 0) {
                for (int j = 0; j < 7; j++) {
                    if (strcmp(grades[i].letter_grade, LABELS[j]) == 0) { 
                        counts[j]++; 
                        break; 
                    }
                }
            }
        }
        
        printf("| GRADE DISTRIBUTION:                                                         |\n");
        for (int i = 0; i < 7; i++) {
            float percentage = (float)counts[i] / count * 100.0;
            int bar_length = (int)(percentage / 5.0);
            
            printf("|   %-3s: %5d (%5.1f%%) | ", LABELS[i], counts[i], percentage);
            for (int j = 0; j < bar_length && j < 20; j++) printf("#");
            for (int j = bar_length; j < 20; j++) printf(" ");
            printf("                      |\n");
        }
    }
    
    printf("+------------------------------------------------------------------------------+\n");
}