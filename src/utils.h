/**
 * @file utils.h
 * @brief Utility functions header for Student Information System
 * 
 * Contains function prototypes for input validation, string manipulation,
 * and other helper functions used throughout the system.
 * 
 * @author Student Information System Team
 * @date January 2026
 */

#ifndef UTILS_H
#define UTILS_H

/**
 * @brief Validates the student ID format
 * 
 * Student ID must be 7 digits in YYYYXXX format where:
 * - YYYY is the enrollment year (2000-2026)
 * - XXX is the sequence number (001-999)
 *
 * @param id The student ID to validate
 * @return 1 if valid, 0 otherwise
 */
int validate_student_id(int id);

/**
 * @brief Validates the professor ID format
 * 
 * Professor ID must be a 4-digit number between 1001 and 9998.
 *
 * @param id The professor ID to validate
 * @return 1 if valid, 0 otherwise
 */
int validate_professor_id(int id);

/**
 * @brief Validates email address format
 * 
 * Email must follow the pattern: name.surname@university.edu
 *
 * @param email The email string to validate
 * @return 1 if valid, 0 otherwise
 */
int validate_email(const char *email);

/**
 * @brief Validates phone number format
 * 
 * Phone must follow the pattern: 555-XXXX (8 characters total)
 *
 * @param phone The phone string to validate
 * @return 1 if valid, 0 otherwise
 */
int validate_phone(const char *phone);

/**
 * @brief Validates GPA value
 * 
 * GPA must be between 0.00 and 4.00 inclusive.
 *
 * @param gpa The GPA value to validate
 * @return 1 if valid, 0 otherwise
 */
int validate_gpa(float gpa);

/**
 * @brief Validates date string format
 * 
 * Date must be in YYYY-MM-DD format with valid month and day values.
 * Handles leap years correctly.
 *
 * @param date The date string to validate
 * @return 1 if valid, 0 otherwise
 */
int validate_date(const char *date);

/**
 * Trims leading and trailing whitespace from a string.
 *
 * @param str The string to modify in-place
 */
void trim_whitespace(char *str);

/**
 * Splits a CSV line into fields.
 *
 * @param line The raw CSV line
 * @param fields Array of string pointers to store fields
 * @param max_fields Maximum number of fields to split
 * @return The number of fields found
 */
int split_csv_line(char *line, char **fields, int max_fields);

/**
 * Clears the standard input buffer to prevent scanning errors.
 */
void clear_input_buffer();

/**
 * Prints a standardized error message to stderr.
 *
 * @param message The error description
 */
void print_error(const char *message);

/**
 * Formats a name string (Capitalize first letter, lower case others).
 *
 * @param name The name string to format
 */
void format_name(char *name);

/**
 * Safely gets a string input from the user.
 *
 * @param prompt The message to display to the user
 * @param buffer The buffer to store the input
 * @param size The size of the buffer
 */
void get_string_input(const char *prompt, char *buffer, int size);

/**
 * Validates the course ID format.
 *
 * @param id The course ID (must be 4 digits: 1000-9999)
 * @return 1 if valid, 0 otherwise
 */
int validate_course_id(int id);

/**
 * Validates a numeric grade.
 *
 * @param grade The grade value (0.0 - 100.0)
 * @return 1 if valid, 0 otherwise
 */
int validate_numeric_grade(float grade);

/**
 * Validates a letter grade string.
 *
 * @param letter The letter grade (e.g., "A", "B+")
 * @return 1 if valid, 0 otherwise
 */
int validate_letter_grade(const char *letter);

/**
 * Computes a letter grade from a numeric grade.
 *
 * @param numeric The numeric score
 * @param out The buffer to store the result letter
 * @param out_size The size of the output buffer
 */
void compute_letter_grade(float numeric, char *out, int out_size);

/**
 * Validates that a string contains only alphabetic characters.
 *
 * @param str The string to check
 * @return 1 if valid, 0 otherwise
 */
int validate_name_string(const char *str);

/**
 * Compares two strings case-insensitively.
 *
 * @param s1 First string
 * @param s2 Second string
 * @return 1 if equal, 0 otherwise
 */
int str_equals_case_insensitive(const char *s1, const char *s2);

/**
 * Validates the semester format.
 *
 * @param semester The semester string (e.g., "2024-FALL")
 * @return 1 if valid, 0 otherwise
 */
int validate_semester(const char *semester);

/**
 * Validates the student ID format.
 * @param id The student ID
 * @return 1 if valid, 0 otherwise
 */
int validate_student_id(int id);

/**
 * Derives the semester string from a given date.
 * E.g., "2024-04-15" -> "2024-SPRING"
 * * @param date The date string (YYYY-MM-DD)
 * @param semester_out The buffer to write the semester string to
 */
void get_semester_from_date(const char *date, char *semester_out);

void pause_execution();

int is_numeric(const char *str);

#endif /* UTILS_H */