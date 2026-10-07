/**
 * @file course.h
 * @brief Course Management Module Header
 * 
 * Defines the Course structure and function prototypes for course management
 * including CRUD operations, prerequisites, instructor assignment, and capacity.
 * 
 * @author Student Information System Team
 * @date January 2026
 */

#ifndef COURSE_H
#define COURSE_H

/* Forward Declarations */
typedef struct Enrollment Enrollment;
typedef struct Grade Grade;
#include "professor.h"  

#define MAX_CODE_LEN 10      /**< Maximum length for course code */
#define MAX_NAME_LEN 50      /**< Maximum length for course name */
#define MAX_DEPT_LEN 50      /**< Maximum length for department name */
#define MAX_PREREQ_LEN 100   /**< Maximum length for prerequisites string */
#define MAX_LINE_LEN 256     /**< Maximum length for CSV line */

/**
 * @brief Course structure representing a course in the system
 * 
 * Contains all information about a course including its code, name,
 * credits, capacity, department, prerequisites, and assigned instructor.
 */
typedef struct Course {
    int course_id;                        /**< Unique course identifier (1000-9999) */
    char course_code[MAX_CODE_LEN];       /**< Course code (e.g., "CS101") */
    char course_name[MAX_NAME_LEN];       /**< Course name */
    int credits;                          /**< Credit hours (1-6) */
    int capacity;                         /**< Maximum enrollment capacity */
    char department[MAX_DEPT_LEN];        /**< Department name */
    char prerequisites[MAX_PREREQ_LEN];   /**< Prerequisite course codes */
    int instructor_id;                    /**< Assigned professor ID */
} Course;

/**
 * Loads course data from a CSV file into the system memory.
 *
 * @param filename The path to the CSV file
 * @param courses Pointer to the array of Course structures
 * @param count Pointer to the integer storing the number of courses
 * @return 1 if successful, 0 otherwise
 */
int load_courses(const char *filename, Course **courses, int *count);

/**
 * Searches for a course by its unique ID.
 *
 * @param courses Array of Course structures
 * @param count Total number of courses
 * @param id The ID to search for
 * @return The index of the course if found, -1 otherwise
 */
int search_course_by_id(Course *courses, int count, int id);

/**
 * Prints the details of a specific course or list of courses.
 *
 * @param c Pointer to the Course structure (or array)
 * @param detailed Flag (1 for detailed view, 0 for list view)
 */
void print_course(Course *c, int detailed);

/**
 * Searches for a course by its code (e.g., "CS101").
 *
 * @param courses Array of Course structures
 * @param count Total number of courses
 * @param code The course code to search for
 * @return The index of the course if found, -1 otherwise
 */
int search_course_by_code(Course *courses, int count, const char *code);

/**
 * Prompts user for input and adds a new course to the system.
 *
 * @param courses Pointer to the array of Course structures
 * @param count Pointer to the integer storing the number of courses
 * @return 1 if successful, 0 otherwise
 */
int add_course(Course **courses, int *count);

/**
 * Updates details of an existing course.
 *
 * @param courses Array of Course structures
 * @param count Total number of courses
 * @param id The ID of the course to update
 */
void update_course(Course *courses, int count, int id);

/**
 * Updates the capacity of a specific course.
 *
 * @param courses Array of Course structures
 * @param count Total number of courses
 * @param id The ID of the course
 */
void set_course_capacity(Course *courses, int count, int id, Enrollment *enrollments, int enr_count);

/**
 * Assigns or updates prerequisites for a specific course.
 *
 * @param courses Array of Course structures
 * @param count Total number of courses
 * @param id The ID of the course
 */
void assign_prerequisites(Course *courses, int count, int id);

/**
 * Saves all course data to a CSV file.
 *
 * @param filename The path to the CSV file
 * @param courses Array of Course structures
 * @param count Total number of courses
 * @return 1 if successful, 0 otherwise
 */
int save_courses(const char *filename, Course *courses, int count);

/**
 * Frees the memory allocated for the course array.
 *
 * @param courses Pointer to the course array
 */
void free_courses(Course *courses);

/**
 * Assigns a professor to a course based on department matching.
 *
 * @param courses Array of Course structures
 * @param course_count Total number of courses
 * @param professors Array of Professor structures
 * @param prof_count Total number of professors
 */
void assign_course_by_department(Course *courses, int course_count, Professor *professors, int prof_count);

/**
 * Deletes a course and all associated data (grades, enrollments).
 *
 * @param courses Array of Course structures
 * @param course_count Pointer to total number of courses
 * @param course_id The ID of the course to delete
 * @param enrollments Array of Enrollment structures
 * @param enrollment_count Pointer to total number of enrollments
 * @param grades Array of Grade structures
 * @param grade_count Pointer to total number of grades
 */
void delete_course(Course *courses, int *course_count, int course_id, 
                   Enrollment **enrollments, int *enrollment_count, 
                   Grade **grades, int *grade_count);

/**
 * Lists all courses belonging to a specific department.
 *
 * @param courses Array of Course structures
 * @param count Total number of courses
 * @param dept_name The name of the department to filter by
 */
void print_courses_by_department(Course *courses, int count, const char *dept_name);

/**
 * Lists all courses in a formatted table.
 *
 * @param courses Array of Course structures
 * @param count Total number of courses
 */
void list_all_courses(Course *courses, int count);

#endif