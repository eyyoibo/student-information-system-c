/**
 * @file curriculum.h
 * @brief Curriculum management module header
 * 
 * This header defines structures and functions for managing degree
 * requirements, tracking student progress, and checking graduation eligibility.
 * 
 * Features:
 * - Define curriculum requirements per department
 * - Track completed credits and courses
 * - Validate graduation requirements
 * 
 * @author Student
 * @date 2024
 */

#ifndef CURRICULUM_H
#define CURRICULUM_H

#include "student.h"
#include "course.h"
#include "grade.h"
#include "enrollment.h"

/** Minimum GPA required for graduation */
#define MIN_GRADUATION_GPA 2.00

/** Maximum number of required courses per curriculum */
#define MAX_REQUIRED_COURSES 50

/** Minimum passing grade (numeric) */
#define PASSING_GRADE_THRESHOLD 60.0

/** Maximum department name length */
#define MAX_DEPT_LEN 50 

/**
 * @brief Curriculum structure for degree requirements
 * 
 * Stores the degree requirements for a specific department,
 * including required courses and minimum credit requirements.
 */
typedef struct {
    char department[MAX_DEPT_LEN];              /**< Department name */
    int required_course_ids[MAX_REQUIRED_COURSES]; /**< Array of required course IDs */
    int num_required;                           /**< Number of required courses */
    int min_credits;                            /**< Minimum credits for graduation */
} Curriculum;

/**
 * @brief Initializes curricula from course data
 * 
 * Auto-generates department curricula based on available courses.
 * Each course is added to its department's curriculum.
 * 
 * @param curricula Array to store curriculum data
 * @param count Pointer to curriculum count
 * @param courses Array of courses
 * @param course_count Number of courses
 */
void init_curricula(Curriculum *curricula, int *count, Course *courses, int course_count);

/**
 * @brief Gets curriculum by department name
 * 
 * Performs case-insensitive search for department curriculum.
 * 
 * @param curricula Array of curricula
 * @param count Number of curricula
 * @param department Department name to search for
 * @return Pointer to curriculum if found, NULL otherwise
 */
Curriculum* get_curriculum_by_department(Curriculum *curricula, int count, const char *department);

/**
 * @brief Adds a course to curriculum requirements
 * 
 * Manually adds a course ID to the curriculum's required courses.
 * Supports cross-department requirements.
 * 
 * @param cur Pointer to curriculum to modify
 * @param course_id Course ID to add
 * @return 1 on success, 0 if already exists or capacity full
 */
int add_course_to_curriculum(Curriculum *cur, int course_id);

/**
 * @brief Calculates completed credits for a student
 * 
 * Counts unique course credits where student has a passing grade.
 * Handles retakes correctly (counts each course only once).
 * 
 * @param student_id Student ID to check
 * @param grades Array of grades
 * @param grade_count Number of grades
 * @param courses Array of courses
 * @param course_count Number of courses
 * @return Total completed credits
 */
int calculate_completed_credits(
    int student_id,
    Grade *grades, int grade_count,
    Course *courses, int course_count
);

/**
 * @brief Checks if student has passed a specific course
 * 
 * Searches grade records to determine if student passed the course
 * in any semester.
 * 
 * @param student_id Student ID to check
 * @param course_id Course ID to check
 * @param grades Array of grades
 * @param grade_count Number of grades
 * @return 1 if passed, 0 otherwise
 */
int has_passed_course(
    int student_id,
    int course_id,
    Grade *grades, int grade_count
);

/**
 * @brief Generates graduation status report
 * 
 * Comprehensive report showing:
 * - Required courses and completion status
 * - Earned credits vs minimum required
 * - GPA check against minimum
 * - Final graduation eligibility decision
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
void check_graduation_status(
    int student_id,
    Student *students, int stu_count,
    Course *courses, int course_count,
    Grade *grades, int grade_count,
    Curriculum *curricula, int cur_count
);

/**
 * @brief Shows student's degree progress
 * 
 * Displays progress toward degree completion including credits
 * and course completion with visual progress bar.
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
);

/**
 * @brief Shows remaining requirements for graduation
 * 
 * Lists missing courses and credits needed for graduation.
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
);

#endif /* CURRICULUM_H */