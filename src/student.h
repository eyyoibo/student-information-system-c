/**
 * @file student.h
 * @brief Header file for student management module
 * 
 * Contains Student structure definition and function prototypes
 * for all student-related operations in the Student Information System.
 * 
 * @author Student Information System Team
 * @date January 2026
 */

#ifndef STUDENT_H
#define STUDENT_H

/* Forward declarations for circular dependency resolution */
typedef struct Enrollment Enrollment;
typedef struct Grade Grade;

/* Maximum length constants for student fields */
#define MAX_NAME_LEN 50
#define MAX_EMAIL_LEN 50
#define MAX_PHONE_LEN 20
#define MAX_MAJOR_LEN 50
#define MAX_LINE_LEN 256

/**
 * @struct Student
 * @brief Structure representing a student in the system
 * 
 * Contains all information about a student including personal details,
 * contact information, and academic status.
 */
typedef struct Student {
    int student_id;                 /**< Unique student ID (YYYYXXX format, e.g., 2024001) */
    char first_name[MAX_NAME_LEN];  /**< Student's first name */
    char last_name[MAX_NAME_LEN];   /**< Student's last name */
    char email[MAX_EMAIL_LEN];      /**< University email (name.surname@university.edu) */
    char phone[MAX_PHONE_LEN];      /**< Phone number (555-XXXX format) */
    int enrollment_year;            /**< Year of enrollment (extracted from ID) */
    char major[MAX_MAJOR_LEN];      /**< Student's major/department */
    float gpa;                      /**< Current GPA (0.00 - 4.00) */
} Student;

/**
 * Loads student data from a CSV file into memory.
 *
 * @param filename The path to the CSV file
 * @param students Pointer to the student array
 * @param count Pointer to the student count variable
 * @return 1 if successful, 0 otherwise
 */
int load_students(const char *filename, Student **students, int *count);

/**
 * Searches for a student by their unique ID.
 *
 * @param students Array of Student structures
 * @param count Total number of students
 * @param id The student ID to search for
 * @return The index of the student if found, -1 otherwise
 */
int search_student_by_id(Student *students, int count, int id);

/**
 * Searches for students by name (first, last, or full name) and prints matches.
 *
 * @param students Array of Student structures
 * @param count Total number of students
 * @param name The name string to search for (case-insensitive)
 * @return The number of matches found
 */
int search_and_print_students_by_name(Student *students, int count, const char *name);

/**
 * Searches for a student by email address.
 *
 * @param students Array of Student structures
 * @param count Total number of students
 * @param email The email string to search for
 * @return The index of the student if found, -1 otherwise
 */
int search_student_by_email(Student *students, int count, const char *email);

/**
 * updates an existing student's information via a menu.
 *
 * @param students Array of Student structures
 * @param count Total number of students
 * @param id The ID of the student to update
 */
void update_student(Student *students, int count, int id);

/**
 * Deletes a student and all associated data (grades, enrollments).
 *
 * @param students Pointer to student array
 * @param s_count Pointer to student count
 * @param id The ID of the student to delete
 * @param enrollments Pointer to enrollment array
 * @param e_count Pointer to enrollment count
 * @param grades Pointer to grade array
 * @param g_count Pointer to grade count
 * @return 1 if successful, 0 otherwise
 */
int delete_student(Student **students, int *s_count, int id, Enrollment *enrollments, int *e_count, Grade *grades, int *g_count);

/**
 * Prints the details of a single student.
 *
 * @param s The Student structure to print
 */
void print_student(Student s);

/**
 * Lists all students in a formatted table.
 *
 * @param students Array of Student structures
 * @param count Total number of students
 */
void list_all_students(Student *students, int count);

/**
 * Frees the memory allocated for the student array.
 *
 * @param students Pointer to the student array
 */
void free_students(Student *students);

/**
 * Prompts user for input and adds a new student to the system.
 * Validates all inputs (ID, email, phone, etc.).
 *
 * @param students Pointer to the student array
 * @param count Pointer to the student count
 * @return 1 if successful, 0 otherwise
 */
int add_student(Student **students, int *count);

/**
 * Saves all student data to a CSV file.
 *
 * @param filename The path to the CSV file
 * @param students Array of Student structures
 * @param count Total number of students
 * @return 1 if successful, 0 otherwise
 */
int save_students(const char *filename, Student *students, int count);

#endif