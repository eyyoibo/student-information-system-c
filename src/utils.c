/**
 * @file utils.c
 * @brief Utility functions for Student Information System
 * 
 * This module provides common utility functions used throughout the system:
 * - Input validation (ID, email, phone, GPA, date, semester)
 * - String manipulation (trim, format)
 * - CSV parsing
 * - Error handling
 * 
 * @author Student Information System Team
 * @date January 2026
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"

/**
 * @brief Validates student ID based on year range and sequence
 * 
 * ID format: YYYYXXX where YYYY is 2000-2026, XXX is 001-999
 * Example: 2024001 is valid, 1999001 is invalid
 * 
 * @param id The student ID to validate
 * @return 1 if valid, 0 otherwise (prints error message)
 */
int validate_student_id(int id) {
    if (id <= 0) {
        print_error("ID cannot be zero or negative.");
        return 0;
    }
    if (id > 9999999) {
        print_error("Invalid ID format. ID must be 7 digits maximum (e.g., 2024001).");
        return 0;
    }
    if (id < 2000001 || id > 2026999) {
        print_error("Invalid ID format. ID must be 7 digits (e.g., 2024001) and start with 2000-2026.");
        return 0;
    }
    
    /* Extract year and sequence from ID: 2024001 -> Year: 2024, Seq: 001 */
    int year = id / 1000;
    int sequence = id % 1000;

    /* Validate year range: 2000-2026 */
    if (year < 2000 || year > 2026) {
        print_error("Invalid Student ID format. Year part (first 4 digits) must be 2000-2026.");
        return 0;
    }
    
    /* Sequence cannot be 000 */
    if (sequence == 0) {
        print_error("Invalid Student ID format. Sequence number (last 3 digits) cannot be 000.");
        return 0;
    }
    
    return 1;
}

/**
 * @brief Validates professor ID format
 * 
 * Professor ID must be a 4-digit number between 1001 and 9998.
 * 
 * @param id The professor ID to validate
 * @return 1 if valid, 0 otherwise (prints error message)
 */
int validate_professor_id(int id) {
    if (id <= 0) {
        print_error("Professor ID cannot be zero or negative.");
        return 0;
    }
    if (id <= 1000) {
        print_error("Invalid Professor ID. ID must be greater than 1000.");
        return 0;
    }
    if (id >= 9999) {
        print_error("Invalid Professor ID. ID must be less than 9999.");
        return 0;
    }
    return 1;
}

/**
 * @brief Validates email address format
 * 
 * Email must follow: name.surname@university.edu
 * - Must contain exactly one dot before @
 * - Must end with @university.edu
 * - Only lowercase letters allowed in local part
 * 
 * @param email The email string to validate
 * @return 1 if valid, 0 otherwise (prints error message)
 */
int validate_email(const char *email) {
    if (!email) {
        print_error("Email pointer is NULL.");
        return 0;
    }

    const char *domain = "@university.edu";
    int len = (int)strlen(email);
    int domain_len = (int)strlen(domain);

    // 1. Domain Check
    if (len <= domain_len || strcmp(email + len - domain_len, domain) != 0) {
        print_error("Email must end with @university.edu");
        return 0;
    }

    int local_len = len - domain_len;
    int dot_pos = -1;

    // 2. Character and Multi-dot Check
    for (int i = 0; i < local_len; i++) {
        char c = email[i];
        if (c == '.') {
            if (dot_pos != -1) {
                print_error("Email format must be name.surname (only one dot allowed).");
                return 0;
            }
            dot_pos = i;
        } else if (!(c >= 'a' && c <= 'z')) {
            print_error("Email contains invalid characters. Use lowercase only.");
            return 0;
        }
    }

    // 3. Dot Existence Check
    if (dot_pos == -1) {
        print_error("Email format must be name.surname (missing dot).");
        return 0;
    }

    // 4. Dot Position Check (Cannot be at start or end)
    if (dot_pos == 0 || dot_pos == local_len - 1) {
        print_error("Email format must be name.surname (dot cannot be at start or end).");
        return 0;
    }

    return 1;
}

/**
 * @brief Validates phone number format
 * 
 * Phone must follow the pattern: 555-XXXX
 * where X is a digit (0-9).
 * 
 * @param phone The phone string to validate
 * @return 1 if valid, 0 otherwise (prints error message)
 */
int validate_phone(const char *phone) {
    if (phone == NULL || strlen(phone) != 8) {
        print_error("Phone number must be exactly 8 characters (555-XXXX).");
        return 0;
    }
    if (phone[0] != '5' || phone[1] != '5' || phone[2] != '5') {
        print_error("Phone number must start with 555.");
        return 0;
    }
    if (phone[3] != '-') {
        print_error("Phone number must contain a hyphen at 4th position.");
        return 0;
    }
    for (int i = 4; i < 8; i++) {
        if (!isdigit((unsigned char)phone[i])) {
            print_error("Phone number must end with 4 digits.");
            return 0;
        }
    }
    return 1;
}

/**
 * @brief Validates GPA value
 * 
 * GPA must be a floating-point number between 0.0 and 4.0.
 * 
 * @param gpa The GPA value to validate
 * @return 1 if valid, 0 otherwise (prints error message)
 */
int validate_gpa(float gpa) {
    if (gpa < 0.00f || gpa > 4.00f) {
        print_error("GPA must be between 0.0 and 4.0.");
        return 0;
    }
    return 1;
}

/**
 * @brief Removes leading and trailing whitespace from a string
 * 
 * Modifies the string in-place by removing spaces, tabs, and other
 * whitespace characters from both ends of the string.
 * 
 * @param str The string to trim (modified in place)
 */
void trim_whitespace(char *str) {
    if (str == NULL || *str == '\0') return;

    int len = (int)strlen(str);
    while (len > 0 && isspace((unsigned char)str[len - 1])) {
        str[--len] = '\0';
    }

    int start = 0;
    while (str[start] != '\0' && isspace((unsigned char)str[start])) {
        start++;
    }

    if (start > 0) {
        int i;
        for (i = 0; str[i + start] != '\0'; i++) {
            str[i] = str[i + start];
        }
        str[i] = '\0';
    }
}

/**
 * @brief Formats a name with proper capitalization
 * 
 * Capitalizes the first letter of each word and lowercases
 * the rest. Handles multiple words separated by spaces.
 * Example: "jOHN DOE" becomes "John Doe"
 * 
 * @param name The name string to format (modified in place)
 */
void format_name(char *name) {
    if (name == NULL || name[0] == '\0') return;

    name[0] = (unsigned char)toupper(name[0]);
    for (int i = 1; name[i] != '\0'; i++) {
        if (name[i - 1] == ' ') {
            name[i] = (unsigned char)toupper(name[i]);
        } else {
            name[i] = (unsigned char)tolower(name[i]);
        }
    }
}

/**
 * @brief Splits a CSV line into individual fields
 * 
 * Parses a comma-separated line and stores pointers to each
 * field in the provided array. Modifies the input line by
 * replacing commas with null terminators.
 * 
 * @param line The CSV line to parse (modified in place)
 * @param fields Array to store pointers to each field
 * @param max_fields Maximum number of fields to extract
 * @return The number of fields extracted
 */
int split_csv_line(char *line, char **fields, int max_fields) {
    int count = 0;
    char *ptr = line;
    fields[count++] = ptr; 

    while (*ptr && count < max_fields) {
        if (*ptr == ',') {
            *ptr = '\0'; 
            fields[count++] = ptr + 1; 
        }
        ptr++;
    }
    return count;
}

/**
 * @brief Clears the stdin input buffer
 * 
 * Reads and discards all characters from stdin until a newline
 * or EOF is encountered. Use after scanf to prevent leftover
 * characters from affecting subsequent input operations.
 */
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/**
 * @brief Prints a formatted error message to stderr
 * 
 * Outputs the message with "[ERROR]:" prefix for consistent
 * error formatting throughout the application.
 * 
 * @param message The error message to display
 */
void print_error(const char *message) {
    if (message) {
        fprintf(stderr, "[ERROR]: %s\n", message);
    }
}

/**
 * @brief Gets a string input from user with prompt
 * 
 * Safely reads a string from stdin, handling newline characters
 * and buffer overflow. Displays the prompt before reading.
 * 
 * @param prompt The prompt message to display
 * @param buffer The buffer to store the input
 * @param size The size of the buffer
 */
void get_string_input(const char *prompt, char *buffer, int size) {
    if (strlen(prompt) > 0) {
        printf("%s", prompt);
    }

    if (fgets(buffer, size, stdin) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
    }
}

/**
 * @brief Validates course ID format
 * 
 * Course ID must be a 4-digit number between 1000 and 9999.
 * 
 * @param id The course ID to validate
 * @return 1 if valid, 0 otherwise (prints error message)
 */
int validate_course_id(int id) {
    if (id >= 1000 && id <= 9999) {
        return 1; 
    } else {
        print_error("Invalid Course ID! ID must be exactly 4 digits (1000-9999).");
        return 0; 
    }
}

/**
 * @brief Validates numeric grade value
 * 
 * Grade must be a floating-point number between 0.0 and 100.0.
 * 
 * @param grade The numeric grade to validate
 * @return 1 if valid, 0 otherwise
 */
int validate_numeric_grade(float grade) {
    if (grade < 0.0f || grade > 100.0f) {
        return 0;
    }
    return 1;
}

/**
 * @brief Validates letter grade string
 * 
 * Checks if the letter grade is one of the valid values:
 * A, A-, B+, B, B-, C+, C, F, FF
 * 
 * @param letter The letter grade string to validate
 * @return 1 if valid, 0 otherwise
 */
int validate_letter_grade(const char *letter) {
    if (!letter) return 0;
    const char *valid_letters[] = { "A", "A-", "B+", "B", "B-", "C+", "C", "F", "FF" };
    int count = sizeof(valid_letters) / sizeof(valid_letters[0]);

    for (int i = 0; i < count; i++) {
        if (strcmp(letter, valid_letters[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

/**
 * @brief Converts numeric score to letter grade
 * 
 * Grading scale:
 * - 95-100: A
 * - 90-94: A-
 * - 87-89: B+
 * - 84-86: B
 * - 81-83: B-
 * - 78-80: C+
 * - 70-77: C
 * - Below 70: F
 * 
 * @param numeric The numeric score (0-100)
 * @param out Buffer to store the letter grade
 * @param out_size Size of the output buffer
 */
void compute_letter_grade(float numeric, char *out, int out_size) {
    if (!out || out_size < 2) return;

    if (numeric >= 95.0f) strncpy(out, "A", out_size);
    else if (numeric >= 90.0f) strncpy(out, "A-", out_size);
    else if (numeric >= 87.0f) strncpy(out, "B+", out_size);
    else if (numeric >= 84.0f) strncpy(out, "B", out_size);
    else if (numeric >= 81.0f) strncpy(out, "B-", out_size);
    else if (numeric >= 78.0f) strncpy(out, "C+", out_size);
    else if (numeric >= 70.0f) strncpy(out, "C", out_size);
    else strncpy(out, "F", out_size);

    out[out_size - 1] = '\0';
}

/**
 * @brief Checks if a character is a Turkish letter
 * 
 * Recognizes Turkish-specific characters in Windows-1254 encoding:
 * lowercase: ğ, ü, ş, ı, ö, ç
 * uppercase: Ğ, Ü, Ş, İ, Ö, Ç
 * 
 * @param c The character to check (unsigned char)
 * @return 1 if Turkish letter, 0 otherwise
 */
static int is_turkish_letter(unsigned char c) {
    /* Turkish letters in Windows-1254 encoding */
    unsigned char turkish_chars[] = {
        240, 252, 254, 253, 246, 231,  /* ğ, ü, ş, ı, ö, ç (lowercase) */
        208, 220, 222, 221, 214, 199   /* Ğ, Ü, Ş, İ, Ö, Ç (uppercase) */
    };
    
    for (int i = 0; i < 12; i++) {
        if (c == turkish_chars[i]) {
            return 1;
        }
    }
    return 0;
}

/**
 * @brief Validates a name string for alphabetic characters
 * 
 * Allows:
 * - Alphabetic characters (a-z, A-Z)
 * - Spaces (for multi-word names)
 * - Apostrophes (for names like O'Connor)
 * - Turkish characters (both Windows-1254 and UTF-8 encodings)
 * 
 * @param str The name string to validate
 * @return 1 if valid, 0 otherwise (prints error message)
 */
int validate_name_string(const char *str) {
    if (!str || strlen(str) == 0) return 0;
    
    for (int i = 0; str[i] != '\0'; i++) {
        unsigned char c = (unsigned char)str[i];
        
        /* Allow spaces */
        if (c == ' ') {
            continue;
        }
        
        /* Allow apostrophe (') for names like O'Connor */
        if (c == '\'' || c == 0x27) {
            continue;
        }
        
        /* Allow regular alphabetic characters (a-z, A-Z) */
        if (isalpha(c)) {
            continue;
        }
        
        /* Allow Turkish characters (Windows-1254) */
        if (is_turkish_letter(c)) {
            continue;
        }
        
        /* UTF-8 Turkish characters check (for UTF-8 encoding) */
        /* Turkish characters in UTF-8 are 2-byte sequences starting with 0xC3, 0xC4, or 0xC5 */
        /* c is unsigned char (0-255), so range check is always valid */
        if (c >= 0xC0) {
            if (c == 0xC3 || c == 0xC4 || c == 0xC5) {
                /* This is likely a UTF-8 Turkish character start byte */
                /* Check if next byte exists and is a valid continuation byte */
                if (str[i+1] != '\0') {
                    unsigned char next = (unsigned char)str[i+1];
                    /* UTF-8 continuation bytes are in range 0x80-0xBF */
                    if (next >= 0x80 && next <= 0xBF) {
                        i++; /* Skip the continuation byte */
                        continue;
                    }
                }
            }
        }
        
        /* If we get here, the character is not valid */
        print_error("Name must contain only alphabetic characters!");
        return 0;
    }
    return 1;
}

/**
 * @brief Validates semester string format
 * 
 * Semester must be in format: YYYY-SEASON
 * Valid seasons: SPRING, FALL, SUMMER (case-insensitive)
 * Example: 2024-SPRING, 2023-fall
 * 
 * @param semester The semester string to validate
 * @return 1 if valid, 0 otherwise (prints error message)
 */
int validate_semester(const char *semester) {
    if (!semester || strlen(semester) < 6) {
        print_error("Invalid format. Example: 2021-SPRING (Year-Season).");
        return 0;
    }

    /* 1. Year Check - first 4 characters must be digits */
    for (int i = 0; i < 4; i++) {
        if (!isdigit((unsigned char)semester[i])) {
            print_error("Semester year must start with 4 digits (e.g. 2021).");
            return 0;
        }
    }

    /* 2. Hyphen Check */
    if (semester[4] != '-') {
        print_error("Hyphen (-) must follow the year. E.g., 2021-SPRING");
        return 0;
    }

    /* 3. Season Check (case-insensitive) */
    char season_part[50];
    strcpy(season_part, semester + 5); /* Get season part after hyphen */
    /* Convert to uppercase for comparison */
    for (int i = 0; season_part[i] != '\0'; i++) {
        season_part[i] = (char)toupper((unsigned char)season_part[i]);
    }
    
    /* Check if it's a valid season (SPRING, FALL, SUMMER) */
    if (strcmp(season_part, "SPRING") != 0 && 
        strcmp(season_part, "FALL") != 0 && 
        strcmp(season_part, "SUMMER") != 0) {
        print_error("Season must be SPRING, FALL, or SUMMER (case-insensitive).");
        return 0;
    }

    return 1;
}

/**
 * @brief Determines semester from a date string
 * 
 * Converts a date (YYYY-MM-DD) to semester format (YYYY-SEASON):
 * - Feb-Jun: SPRING
 * - Jul-Aug: SUMMER
 * - Sep-Jan: FALL
 * 
 * @param date Input date string in YYYY-MM-DD format
 * @param semester_out Buffer to store the resulting semester string
 */
void get_semester_from_date(const char *date, char *semester_out) {
    int year, month, day;
    if (sscanf(date, "%d-%d-%d", &year, &month, &day) != 3) {
        strcpy(semester_out, "UNKNOWN");
        return;
    }

    const char *season = "UNKNOWN";

    /* Semester logic based on month:
     * Spring: February(2) - June(6)
     * Summer: July(7) - August(8)
     * Fall:   September(9) - January(1)
     */
    if (month >= 2 && month <= 6) {
        season = "SPRING";
    } else if (month >= 7 && month <= 8) {
        season = "SUMMER";
    } else {
        season = "FALL";
    }

    sprintf(semester_out, "%d-%s", year, season);
}

/**
 * @brief Case-insensitive string comparison
 * 
 * Compares two strings character by character, ignoring case.
 * 
 * @param s1 First string
 * @param s2 Second string
 * @return 1 if strings are equal (case-insensitive), 0 otherwise
 */
int str_equals_case_insensitive(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        if (tolower((unsigned char)*s1) != tolower((unsigned char)*s2)) {
            return 0; 
        }
        s1++;
        s2++;
    }
    return (*s1 == '\0' && *s2 == '\0');
}

/**
 * @brief Validates a date string
 * 
 * Date must be in YYYY-MM-DD format with valid values:
 * - Year: 2000-2026
 * - Month: 1-12
 * - Day: Valid for the given month (considers leap years)
 * 
 * @param date The date string to validate
 * @return 1 if valid, 0 otherwise (prints error message)
 */
int validate_date(const char *date) {
    if (date == NULL || strlen(date) != 10) {
        print_error("Date must be in YYYY-MM-DD format (10 characters).");
        return 0;
    }
    int year, month, day;
    if (sscanf(date, "%d-%d-%d", &year, &month, &day) != 3) {
        print_error("Invalid date format.");
        return 0;
    }
    if (year < 2000 || year > 2026) return 0;
    if (month < 1 || month > 12) return 0;
    int daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    /* Check for leap year */
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) daysInMonth[1] = 29;
    if (day < 1 || day > daysInMonth[month - 1]) {
        print_error("Invalid day for the given month.");
        return 0;
    }
    return 1;
}

/**
 * @brief Pauses execution and waits for user input
 * 
 * Displays a prompt and waits for the user to press ENTER
 * before continuing. Used after displaying results.
 */
void pause_execution() {
    printf("\n>> Press ENTER to return to menu...");
    getchar(); 
}

/**
 * @brief Checks if a string contains only numeric characters
 * 
 * @param str The string to check
 * @return 1 if all characters are digits, 0 otherwise
 */
int is_numeric(const char *str) {
    if (str == NULL || *str == '\0') return 0;
    while (*str) {
        if (!isdigit((unsigned char)*str)) return 0;
        str++;
    }
    return 1;
}