/**
 * @file curriculum.c
 * @brief Curriculum management module implementation
 * 
 * This file implements curriculum management functionality including:
 * - Department curriculum initialization
 * - Credit calculation with retake handling
 * - Course completion checking
 * - Graduation eligibility validation
 * 
 * @author Student
 * @date 2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "curriculum.h"
#include "utils.h"

/**
 * @brief Checks if a letter grade is passing
 * 
 * Grades starting with A, B, or C are considered passing.
 * 
 * @param l Letter grade string
 * @return 1 if passing, 0 otherwise
 */
int is_passing_grade(const char *l) {
    if (l[0] == 'A' || l[0] == 'B' || l[0] == 'C') return 1;
    return 0;
}

/**
 * @brief Checks if student has passed a specific course
 * 
 * Searches through grade records to determine if the student
 * has ever passed this course (in any semester).
 */
int has_passed_course(int student_id, int course_id, Grade *grades, int grade_count) {
    for (int i = 0; i < grade_count; i++) {
        if (grades[i].student_id == student_id && grades[i].course_id == course_id) {
            if (is_passing_grade(grades[i].letter_grade)) {
                return 1; 
            }
        }
    }
    return 0; 
}

/**
 * @brief Gets curriculum by department name
 * 
 * Performs case-insensitive search for a curriculum matching
 * the given department name.
 */
Curriculum* get_curriculum_by_department(Curriculum *curricula, int count, const char *department) {
    for (int i = 0; i < count; i++) {
        /* Case-insensitive comparison */
        if (str_equals_case_insensitive(curricula[i].department, department)) {
            return &curricula[i];
        }
    }
    return NULL;
}

/**
 * @brief Adds a course to curriculum requirements
 * 
 * Adds a course ID to the curriculum's required courses list.
 * Checks for capacity limits and duplicate entries.
 */
int add_course_to_curriculum(Curriculum *cur, int course_id) {
    /* Check capacity */
    if (cur->num_required >= MAX_REQUIRED_COURSES) return 0;

    /* Check for duplicates */
    for (int i = 0; i < cur->num_required; i++) {
        if (cur->required_course_ids[i] == course_id) return 0;
    }

    /* Add the course */
    cur->required_course_ids[cur->num_required] = course_id;
    cur->num_required++;
    return 1;
}

/**
 * @brief Initializes curricula from course data
 * 
 * Auto-generates department curricula by scanning all courses
 * and grouping them by department. Each course is automatically
 * added as a requirement for its department.
 */
void init_curricula(Curriculum *curricula, int *count, Course *courses, int course_count) {
    *count = 0; 
    printf("Auto-generating default curricula based on course departments...\n");

    for (int i = 0; i < course_count; i++) {
        char *current_dept = courses[i].department;
        int found_index = -1;

        /* Check if department already has a curriculum */
        for (int j = 0; j < *count; j++) {
            if (strcmp(curricula[j].department, current_dept) == 0) {
                found_index = j; 
                break;
            }
        }

        if (found_index != -1) {
            /* Add to existing department curriculum */
            add_course_to_curriculum(&curricula[found_index], courses[i].course_id);
        } else {
            /* Create new department curriculum */
            if (*count < 20) { 
                int new_idx = *count;
                strcpy(curricula[new_idx].department, current_dept);
                curricula[new_idx].min_credits = 120; /* Default minimum */
                curricula[new_idx].num_required = 0;
                
                add_course_to_curriculum(&curricula[new_idx], courses[i].course_id);
                (*count)++;
            }
        }
    }
    printf("Initialization complete. %d departments found.\n", *count);
}

/**
 * @brief Calculates completed credits for a student
 * 
 * Counts credits for courses where the student has a passing grade.
 * Handles retakes correctly - each course is counted only once even
 * if taken multiple times.
 */
int calculate_completed_credits(int student_id, Grade *grades, int grade_count, Course *courses, int course_count) {
    int total_credits = 0;
    
    /* Track passed courses to avoid counting retakes */
    int passed_course_ids[100];
    int passed_count = 0;

    for (int i = 0; i < grade_count; i++) {
        if (grades[i].student_id == student_id) {
            
            /* Check for passing grade */
            if (is_passing_grade(grades[i].letter_grade)) {
                
                int cid = grades[i].course_id;
                
                /* Check if already counted (retake handling) */
                int already_counted = 0;
                for(int k=0; k<passed_count; k++) {
                    if(passed_course_ids[k] == cid) {
                        already_counted = 1;
                        break;
                    }
                }

                if (!already_counted) {
                    /* Find course and add credits */
                    int c_idx = search_course_by_id(courses, course_count, cid);
                    if (c_idx != -1) {
                        total_credits += courses[c_idx].credits;
                        
                        /* Mark as counted */
                        passed_course_ids[passed_count++] = cid;
                    }
                }
            }
        }
    }
    return total_credits;
}

/**
 * @brief Generates graduation status report
 * 
 * Comprehensive graduation check including:
 * - Required courses completion status
 * - Credit requirements
 * - GPA requirements
 * - Final eligibility decision
 */
void check_graduation_status(
    int student_id,
    Student *students, int stu_count,
    Course *courses, int course_count,
    Grade *grades, int grade_count,
    Curriculum *curricula, int cur_count
) {
    int s_idx = search_student_by_id(students, stu_count, student_id);
    if (s_idx == -1) { printf("Student not found.\n"); return; }
    
    Student s = students[s_idx];
    Curriculum *cur = get_curriculum_by_department(curricula, cur_count, s.major);

    printf("\n==========================================\n");
    printf("        GRADUATION STATUS REPORT          \n");
    printf("==========================================\n");
    printf("Student    : %s %s (%d)\n", s.first_name, s.last_name, s.student_id);
    printf("Department : %s\n", s.major);

    if (cur == NULL) {
        printf("[ERROR] No curriculum defined for '%s'.\n", s.major);
        printf("Tip: Go to Curriculum Management > Manage Requirements to create one.\n");
        return;
    }

    int earned_credits = calculate_completed_credits(student_id, grades, grade_count, courses, course_count);
    int missing_courses = 0;

    printf("\n--- REQUIRED COURSES STATUS ---\n");
    printf("%-10s %-30s %-10s\n", "Code", "Course Name", "Status");
    printf("----------------------------------------------------\n");

    for (int i = 0; i < cur->num_required; i++) {
        int cid = cur->required_course_ids[i];
        
        int c_idx = search_course_by_id(courses, course_count, cid);
        char code[20] = "???", name[50] = "Unknown";
        if (c_idx != -1) {
            strcpy(code, courses[c_idx].course_code);
            strcpy(name, courses[c_idx].course_name);
        }

        if (has_passed_course(student_id, cid, grades, grade_count)) {
            printf("%-10s %-30s [COMPLETED]\n", code, name);
        } else {
            printf("%-10s %-30s [MISSING] <--\n", code, name);
            missing_courses++;
        }
    }

    printf("\n--- SUMMARY ---\n");
    printf("Credits Earned  : %d / %d (Minimum)\n", earned_credits, cur->min_credits);
    printf("CGPA            : %.2f / %.2f (Minimum)\n", s.gpa, MIN_GRADUATION_GPA);
    printf("Missing Courses : %d\n", missing_courses);

    printf("----------------------------------------------------\n");
    
    /* GRADUATION DECISION LOGIC */
    int eligible = 1;
    
    if (missing_courses > 0) {
        printf("[-] Failed: Missing required courses.\n");
        eligible = 0;
    }
    if (earned_credits < cur->min_credits) {
        printf("[-] Failed: Not enough credits (%d needed).\n", cur->min_credits - earned_credits);
        eligible = 0;
    }
    if (s.gpa < MIN_GRADUATION_GPA) {
        printf("[-] Failed: GPA is below %.2f.\n", MIN_GRADUATION_GPA);
        eligible = 0;
    }

    if (eligible) {
        printf(" DECISION: ELIGIBLE FOR GRADUATION! (CONGRATULATIONS)\n");
    } else {
        printf(" DECISION: NOT ELIGIBLE.\n");
    }
    printf("====================================================\n");
}

/**
 * @brief Shows student's degree progress including completed and remaining courses
 * 
 * Displays a detailed breakdown of the student's progress toward their degree,
 * including credits earned, courses completed, and remaining requirements.
 * 
 * @param student_id Student ID to check
 * @param students Array of students
 * @param stu_count Number of students
 * @param courses Array of courses
 * @param course_count Number of courses
 * @param grades Array of grades
 * @param grade_count Number of grades
 * @param curricula Array of curricula
 * @param cur_count Number of curricula
 */
void show_degree_progress(
    int student_id,
    Student *students, int stu_count,
    Course *courses, int course_count,
    Grade *grades, int grade_count,
    Curriculum *curricula, int cur_count
) {
    int s_idx = search_student_by_id(students, stu_count, student_id);
    if (s_idx == -1) { printf("Student not found.\n"); return; }
    
    Student s = students[s_idx];
    Curriculum *cur = get_curriculum_by_department(curricula, cur_count, s.major);

    printf("\n+==================================================================+\n");
    printf("|               DEGREE PROGRESS REPORT                            |\n");
    printf("+==================================================================+\n");
    printf("| Student    : %-50s |\n", s.first_name);
    printf("| ID         : %-50d |\n", s.student_id);
    printf("| Major      : %-50s |\n", s.major);
    printf("| Current GPA: %-50.2f |\n", s.gpa);
    printf("+------------------------------------------------------------------+\n");

    if (cur == NULL) {
        printf("| [WARNING] No curriculum found for department: %s\n", s.major);
        printf("| Please initialize curriculum first.\n");
        printf("+==================================================================+\n");
        return;
    }

    int earned_credits = calculate_completed_credits(student_id, grades, grade_count, courses, course_count);
    int completed_required = 0;
    int remaining_required = 0;

    /* Count completed and remaining */
    for (int i = 0; i < cur->num_required; i++) {
        if (has_passed_course(student_id, cur->required_course_ids[i], grades, grade_count)) {
            completed_required++;
        } else {
            remaining_required++;
        }
    }

    /* Progress bar calculation */
    float progress_percent = (cur->min_credits > 0) ? 
        ((float)earned_credits / cur->min_credits * 100) : 0;
    if (progress_percent > 100) progress_percent = 100;
    
    int bar_filled = (int)(progress_percent / 5);  /* 20 chars = 100% */
    
    printf("| CREDIT PROGRESS: %d / %d credits\n", earned_credits, cur->min_credits);
    printf("| [");
    for (int i = 0; i < 20; i++) {
        if (i < bar_filled) printf("=");
        else printf(" ");
    }
    printf("] %.1f%%\n", progress_percent);
    
    printf("+------------------------------------------------------------------+\n");
    printf("| REQUIRED COURSES: %d completed, %d remaining\n", completed_required, remaining_required);
    printf("+==================================================================+\n");
}

/**
 * @brief Shows remaining requirements for a student
 * 
 * Displays only the courses and credits still needed for graduation.
 * 
 * @param student_id Student ID to check
 * @param students Array of students
 * @param stu_count Number of students
 * @param courses Array of courses
 * @param course_count Number of courses
 * @param grades Array of grades
 * @param grade_count Number of grades
 * @param curricula Array of curricula
 * @param cur_count Number of curricula
 */
void show_remaining_requirements(
    int student_id,
    Student *students, int stu_count,
    Course *courses, int course_count,
    Grade *grades, int grade_count,
    Curriculum *curricula, int cur_count
) {
    int s_idx = search_student_by_id(students, stu_count, student_id);
    if (s_idx == -1) { printf("Student not found.\n"); return; }
    
    Student s = students[s_idx];
    Curriculum *cur = get_curriculum_by_department(curricula, cur_count, s.major);

    printf("\n+==================================================================+\n");
    printf("|            REMAINING REQUIREMENTS FOR GRADUATION                |\n");
    printf("+==================================================================+\n");
    printf("| Student: %s %s (ID: %d)\n", s.first_name, s.last_name, s.student_id);
    printf("| Major  : %s\n", s.major);
    printf("+------------------------------------------------------------------+\n");

    if (cur == NULL) {
        printf("| [ERROR] No curriculum defined for '%s'.\n", s.major);
        printf("+==================================================================+\n");
        return;
    }

    int earned_credits = calculate_completed_credits(student_id, grades, grade_count, courses, course_count);
    int remaining_credits = cur->min_credits - earned_credits;
    if (remaining_credits < 0) remaining_credits = 0;
    
    int remaining_courses_count = 0;
    int remaining_course_credits = 0;

    printf("\n MISSING REQUIRED COURSES:\n");
    printf("+-----------+--------------------------------+----------+\n");
    printf("| Code      | Course Name                    | Credits  |\n");
    printf("+-----------+--------------------------------+----------+\n");

    for (int i = 0; i < cur->num_required; i++) {
        int cid = cur->required_course_ids[i];
        
        if (!has_passed_course(student_id, cid, grades, grade_count)) {
            int c_idx = search_course_by_id(courses, course_count, cid);
            if (c_idx != -1) {
                printf("| %-9s | %-30s | %-8d |\n", 
                       courses[c_idx].course_code,
                       courses[c_idx].course_name,
                       courses[c_idx].credits);
                remaining_courses_count++;
                remaining_course_credits += courses[c_idx].credits;
            }
        }
    }

    if (remaining_courses_count == 0) {
        printf("| (All required courses completed!)                      |\n");
    }
    printf("+-----------+--------------------------------+----------+\n");

    printf("\n SUMMARY:\n");
    printf("+------------------------------------------------------------------+\n");
    printf("| Remaining Required Courses: %-5d (%-3d credits needed)\n", 
           remaining_courses_count, remaining_course_credits);
    printf("| Additional Credits Needed : %-5d\n", remaining_credits);
    
    if (s.gpa < MIN_GRADUATION_GPA) {
        printf("| GPA Requirement           : Need %.2f (current: %.2f) [NOT MET]\n", 
               MIN_GRADUATION_GPA, s.gpa);
    } else {
        printf("| GPA Requirement           : %.2f / %.2f [MET]\n", 
               s.gpa, MIN_GRADUATION_GPA);
    }
    printf("+==================================================================+\n");
}