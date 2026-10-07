/**
 * @file professor.h
 * @brief Header file for professor management module
 * 
 * Contains Professor structure definition and function prototypes
 * for all professor-related operations in the Student Information System.
 * 
 * @author Student Information System Team
 * @date January 2026
 */

#ifndef PROFESSOR_H
#define PROFESSOR_H

/* Maximum length constants for professor fields */
#define MAX_NAME_LEN 50
#define MAX_EMAIL_LEN 50
#define MAX_PHONE_LEN 20
#define MAX_DEPT_LEN 50
#define MAX_TITLE_LEN 50
#define MAX_OFFICE_LEN 20
#define MAX_LINE_LEN 256
#define MAX_PROFESSORS 100

/**
 * @struct Professor
 * @brief Structure representing a professor in the system
 * 
 * Contains all information about a professor including personal details,
 * contact information, and academic position.
 */
typedef struct Professor {
    int professor_id;               /**< Unique professor ID (1001-9998) */
    char first_name[MAX_NAME_LEN];  /**< Professor's first name */
    char last_name[MAX_NAME_LEN];   /**< Professor's last name */
    char email[MAX_EMAIL_LEN];      /**< University email address */
    char phone[MAX_PHONE_LEN];      /**< Phone number (555-XXXX format) */
    char department[MAX_DEPT_LEN];  /**< Academic department */
    char title[MAX_TITLE_LEN];      /**< Academic title (Professor, Associate Prof, etc.) */
    char office[MAX_OFFICE_LEN];    /**< Office location */
} Professor;

/* ============== Function Prototypes ============== */

/** @brief Loads professors from CSV file */
int load_professors(const char *filename, Professor **professors, int *count);

/** @brief Searches for professor by ID, returns index or -1 */
int search_professor_by_id(Professor *professors, int count, int id);

/** @brief Interactive menu to update professor information */
void update_professor(Professor *professors, int count, int id);

/** @brief Deletes a professor from the system */
int delete_professor(Professor **professors, int *count, int id);

/** @brief Displays all professors in a formatted table */
void print_professor(Professor *professors, int count);

/** @brief Frees memory allocated for professors */
void free_professors(Professor *professors);

/** @brief Searches for professor by email, returns index or -1 */
int search_professor_by_email(Professor *professors, int count, const char *email);

/** @brief Adds a new professor to the system */
int add_professor(Professor **professors, int *count);

/** @brief Saves all professors to CSV file */
int save_professors(const char *filename, Professor *professors, int count);

/** @brief Searches for professors by name and displays matches */
void search_professor_by_name(Professor *professors, int count);

#endif /* PROFESSOR_H */