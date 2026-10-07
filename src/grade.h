/**
 * @file grade.h
 * @brief Grade management header file for the Student Information System.
 * 
 * This file contains the Grade structure definition and function prototypes
 * for grade-related operations including loading, saving, recording, updating,
 * and analyzing student grades.
 * 
 * @author Student Information System Team
 * @date January 2026
 */

#ifndef GRADE_H
#define GRADE_H

/* Forward Declarations */
typedef struct Course Course;
typedef struct Enrollment Enrollment;
typedef struct Student Student;

#define MAX_LETTER_LEN 3      /**< Maximum length for letter grade string */
#define MAX_SEMESTER_LEN 20   /**< Maximum length for semester string */

/**
 * @brief Structure representing a student grade record.
 * 
 * This structure stores all information related to a single grade entry,
 * including the associated enrollment, student, course, and grade values.
 */
typedef struct Grade {
    int grade_id;                         /**< Unique identifier for the grade record */
    int enrollment_id;                    /**< Associated enrollment ID */
    int student_id;                       /**< Student ID who received the grade */
    int course_id;                        /**< Course ID for which the grade was given */
    char letter_grade[MAX_LETTER_LEN];    /**< Letter grade (e.g., "A", "B+", "C") */
    float numeric_grade;                  /**< Numeric grade value (0.0 - 100.0) */
    char semester[MAX_SEMESTER_LEN];      /**< Semester when the grade was recorded */
} Grade;

/**
 * Loads grade data from a CSV file.
 *
 * @param filename The path to the CSV file
 * @param grades Pointer to the grade array
 * @param count Pointer to the grade count
 * @return 1 if successful, 0 otherwise
 */
int load_grades(const char *filename, Grade **grades, int *count, Enrollment *enrollments, int enr_count);

/**
 * Records a new grade for a specific enrollment.
 * Automatically computes the letter grade.
 *
 * @param grades Pointer to grade array
 * @param grade_count Pointer to grade count
 * @param enrollments Enrollment array
 * @param enr_count Enrollment count
 * @param enrollment_id The enrollment ID to grade
 * @param numeric_grade The numeric score (0-100)
 * @return 1 if successful, 0 otherwise
 */
int record_grade(Grade **grades, int *grade_count, Enrollment *enrollments, int enr_count, int enrollment_id, float numeric_grade);

/**
 * Updates an existing grade record.
 *
 * @param grades Array of grades
 * @param grade_count Total number of grades
 * @param grade_id The ID of the grade to update
 * @param new_numeric_grade The new numeric score
 * @return 1 if successful, 0 otherwise
 */
int update_grade(Grade *grades, int grade_count, int grade_id, float new_numeric_grade);

/**
 * Saves all grade data to a CSV file.
 *
 * @param filename The path to the CSV file
 * @param grades Array of Grade structures
 * @param count Total number of grades
 * @return 1 if successful, 0 otherwise
 */
int save_grades(const char *filename, Grade *grades, int count);

/**
 * Frees the memory allocated for the grade array.
 *
 * @param grades Pointer to the grade array
 */
void free_grades(Grade *grades);

/**
 * Searches for a grade by its unique ID.
 *
 * @param grades Array of grades
 * @param count Total number of grades
 * @param grade_id The ID to search for
 * @return The index if found, -1 otherwise
 */
int search_grade_by_id(Grade *grades, int count, int grade_id);

/**
 * Searches for a grade associated with a specific enrollment ID.
 *
 * @param grades Array of grades
 * @param count Total number of grades
 * @param enrollment_id The enrollment ID
 * @return The index if found, -1 otherwise
 */
int search_grade_by_enrollment_id(Grade *grades, int count, int enrollment_id);

/**
 * Calculates statistical data (average, min, max) for a specific course and semester.
 *
 * @param grades Array of grades
 * @param grade_count Total number of grades
 * @param course_id The course ID
 * @param semester The semester string
 * @param avg Pointer to store average
 * @param min Pointer to store minimum
 * @param max Pointer to store maximum
 * @return 1 if data found, 0 otherwise
 */
int calculate_course_statistics(Grade *grades, int grade_count, int course_id, const char *semester, float *avg, float *min, float *max);

/**
 * Displays the distribution of letter grades for a course.
 *
 * @param grades Array of grades
 * @param grade_count Total number of grades
 * @param course_id The course ID
 * @param semester The semester string
 */
void display_grade_distribution(Grade *grades, int grade_count, int course_id, const char *semester);

/**
 * Deletes all grade records associated with a specific course.
 *
 * @param grades Array of grades
 * @param grade_count Pointer to grade count
 * @param course_id The course ID
 */
void delete_grades_by_course_id(Grade **grades, int *grade_count, int course_id);

/**
 * Returns the GPA weight (e.g., 4.0 for "A") for a given letter grade.
 *
 * @param letter_grade The letter grade string
 * @return The GPA weight
 */
float get_grade_weight(const char *letter_grade); 

/**
 * Calculates and updates the GPA for a student based on their grades.
 *
 * @param student_id The student ID
 * @param students Student array
 * @param s_count Student count
 * @param grades Grade array
 * @param g_count Grade count
 * @param courses Course array
 * @param c_count Course count
 */
void calculate_and_display_gpa(int student_id, Student *students, int s_count, Grade *grades, int g_count, Course *courses, int c_count);

/**
 * Prints the academic transcript for a student.
 *
 * @param student_id The student ID
 * @param grades Grade array
 * @param g_count Grade count
 * @param courses Course array
 * @param c_count Course count
 */
void print_student_transcript(int student_id, Grade *grades, int g_count, Course *courses, int c_count);

/**
 * Combined function to display both course statistics and grade distribution.
 * Shows average, min, max grades and grade distribution in one view.
 *
 * @param grades Grade array
 * @param grade_count Total number of grades
 * @param course_id The course ID
 * @param semester The semester string
 */
void display_course_statistics_and_distribution(Grade *grades, int grade_count, int course_id, const char *semester);

#endif