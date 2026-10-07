/**
 * @file enrollment.h
 * @brief Header file for enrollment management functions and data structures.
 * 
 * This file contains the Enrollment structure definition and function prototypes
 * for managing student course enrollments, including loading, saving, searching,
 * and validating enrollment records.
 * 
 * @author Student Information System Team
 * @date January 2026
 */

#ifndef ENROLLMENT_H
#define ENROLLMENT_H

/* Forward Declarations */
typedef struct Course Course;
typedef struct Student Student;
typedef struct Professor Professor;
typedef struct Grade Grade;

#define MAX_SEMESTER_LEN 20   /**< Maximum length for semester string */
#define MAX_DATE_LEN 15       /**< Maximum length for date string */
#define MAX_STATUS_LEN 15     /**< Maximum length for status string */

/**
 * @struct Enrollment
 * @brief Structure representing a student's enrollment in a course.
 * 
 * This structure stores all information about a student's enrollment
 * in a specific course for a given semester.
 */
typedef struct Enrollment {
    int enrollment_id;                    /**< Unique identifier for the enrollment record */
    int student_id;                       /**< ID of the enrolled student */
    int course_id;                        /**< ID of the course */
    int professor_id;                     /**< ID of the instructor teaching the course */
    char semester[MAX_SEMESTER_LEN];      /**< Semester string (e.g., "2024-FALL") */
    char enrollment_date[MAX_DATE_LEN];   /**< Date of enrollment (YYYY-MM-DD format) */
    char status[MAX_STATUS_LEN];          /**< Enrollment status ("Enrolled" or "Completed") */
} Enrollment;

/**
 * Loads enrollment records from a CSV file.
 *
 * @param filename The path to the CSV file
 * @param enrollments Pointer to the array of Enrollment structures
 * @param count Pointer to the integer storing the number of enrollments
 * @return 1 if successful, 0 otherwise
 */
int load_enrollments(const char *filename, Enrollment **enrollments, int *count);

/**
 * Enrolls a student in a specific course for a given semester.
 * Validates capacity, prerequisites, and duplicate enrollments.
 *
 * @param enrollments Pointer to Enrollment array
 * @param e_count Pointer to enrollment count
 * @param students Student array for validation
 * @param s_count Student count
 * @param courses Course array for validation
 * @param c_count Course count
 * @param grades Grade array for prerequisite check
 * @param g_count Grade count
 * @param student_id The ID of the student
 * @param course_id The ID of the course
 * @param professor_id The ID of the instructor
 * @param semester The semester string (e.g., "2024-FALL")
 * @return 1 if enrollment is successful, 0 otherwise
 */
int enroll_student_in_course(Enrollment **enrollments, int *e_count, 
                             Student *students, int s_count, 
                             Course *courses, int c_count, 
                             Grade *grades, int g_count,
                             int student_id, int course_id,
                             int professor_id, const char *semester);

/**
 * Drops a student from a course (deletes the enrollment record).
 *
 * @param enrollments Pointer to Enrollment array
 * @param enr_count Pointer to enrollment count
 * @param student_id The ID of the student
 * @param course_id The ID of the course
 * @param semester The semester string
 * @return 1 if successful, 0 otherwise
 */
int drop_student_from_course(Enrollment **enrollments, int *e_count, 
                             Student *students, int s_count, 
                             Course *courses, int c_count,
                             int student_id, int course_id,
                             const char *semester);

/**
 * Saves all enrollment data to a CSV file.
 *
 * @param filename The path to the CSV file
 * @param enrollments Array of Enrollment structures
 * @param count Total number of enrollments
 * @return 1 if successful, 0 otherwise
 */
int save_enrollments(const char *filename, Enrollment *enrollments, int count);

/**
 * Prints the list of courses a specific student is enrolled in.
 *
 * @param enrollments Array of Enrollment structures
 * @param enr_count Total number of enrollments
 * @param courses Array of Course structures
 * @param course_count Total number of courses
 * @param student_id The ID of the student
 */
void print_student_enrollments(Enrollment *enrollments, int enr_count, Course *courses, int course_count, int student_id);

/**
 * Prints the list of students enrolled in a specific course (Roster).
 *
 * @param enrollments Array of Enrollment structures
 * @param enr_count Total number of enrollments
 * @param students Array of Student structures
 * @param stu_count Total number of students
 * @param course_id The ID of the course
 * @param semester The semester string
 */
void print_course_roster(Enrollment *enrollments, int enr_count, Student *students, int stu_count, int course_id, const char *semester);

/**
 * Frees the memory allocated for the enrollment array.
 *
 * @param enrollments Pointer to the enrollment array
 */
void free_enrollments(Enrollment *enrollments);

/**
 * Searches for an enrollment record by its unique ID.
 *
 * @param enrollments Array of Enrollment structures
 * @param count Total number of enrollments
 * @param enrollment_id The ID to search for
 * @return The index if found, -1 otherwise
 */
int search_enrollment_by_id(Enrollment *enrollments, int count, int enrollment_id);

/**
 * Checks if a student is already enrolled in a specific course.
 *
 * @param enrollments Array of Enrollment structures
 * @param count Total number of enrollments
 * @param student_id The student ID
 * @param course_id The course ID
 * @param semester The semester string
 * @return 1 if enrolled, 0 otherwise
 */
int is_student_enrolled_in_course(Enrollment *enrollments, int count, int student_id, int course_id, const char *semester);

/**
 * Checks if a course has available capacity.
 *
 * @param enr Array of Enrollment structures
 * @param enr_count Total number of enrollments
 * @param courses Array of Course structures
 * @param crs_count Total number of courses
 * @param course_id The course ID
 * @param sem The semester string
 * @return 1 if space is available, 0 if full
 */
int check_course_capacity(Enrollment *enr, int enr_count, Course *courses, int crs_count, int course_id, const char *sem);

/**
 * Checks if a student has passed the prerequisites for a course.
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
int check_prerequisites(Enrollment *enr, int enr_count, Course *courses, int crs_count, Grade *grades, int grd_count, int student_id, int course_id);

/**
 * Deletes all enrollment records associated with a specific course ID.
 *
 * @param enrollments Array of Enrollment structures
 * @param enrollment_count Pointer to enrollment count
 * @param course_id The course ID to clear
 */
void delete_enrollments_by_course_id(Enrollment **enrollments, int *enrollment_count, int course_id);

/**
 * Deletes a professor and all associated enrollments with "Enrolled" status (cascading delete).
 *
 * @param professors Pointer to professor array
 * @param prof_count Pointer to professor count
 * @param enrollments Pointer to enrollment array
 * @param e_count Pointer to enrollment count
 * @param prof_id The professor ID to delete
 * @return 1 if successful, 0 otherwise
 */
/**
 * @brief Checks if a student has passed the prerequisites for a course.
 *
 * @param student_id The ID of the student
 * @param prereq_code The prerequisite course code
 * @param courses Array of Course structures
 * @param c_count Total number of courses
 * @param grades Array of Grade structures
 * @param g_count Total number of grades
 * @return 1 if prerequisites met, 0 otherwise
 */
int has_passed_prerequisite(int student_id, const char *prereq_code, Course *courses, int c_count, Grade *grades, int g_count);

#endif