/**
 * @file test_utils.c
 * @brief Utils Module Unit Tests
 * 
 * Comprehensive test suite for utility functions including:
 * - ID validation tests (student, professor, course)
 * - Email and phone format validation tests
 * - GPA and grade validation tests
 * - Semester and date format validation tests
 * - String manipulation function tests
 * - Boundary value and edge case testing
 * 
 * @author Student Information System Team
 * @date January 2026
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "utils.h"

#define PASS "[PASS]"
static FILE *report = NULL;

/**
 * @brief Runs all utility module tests.
 * 
 * Executes comprehensive tests for all validation and string manipulation 
 * functions in the utils module. Tests include:
 * - ID validation (student, professor, course, enrollment)
 * - Email and phone format validation
 * - GPA and grade validation (numeric and letter)
 * - Semester and date format validation
 * - String manipulation (trim, split, format, compare)
 * 
 * @param rep FILE pointer for test report output (can be NULL).
 * @return 1 if all tests pass, 0 otherwise.
 */
int run_utils_tests(FILE *rep) {
    report = rep;
    
    printf("\n========================================\n");
    printf("      UTILS MODULE - UNIT TESTS\n");
    printf("========================================\n");
    if (report) {
        fprintf(report, "\n========================================\n");
        fprintf(report, "      UTILS MODULE - UNIT TESTS\n");
        fprintf(report, "========================================\n");
    }
    /* Student ID Tests */
    printf("\n--- Student ID Validation Tests ---\n");
    assert(validate_student_id(2020001) == 1);
    assert(validate_student_id(2024999) == 1);
    printf("  %s Valid student IDs accepted.\n", PASS);
    
    assert(validate_student_id(1999001) == 0);
    assert(validate_student_id(2027001) == 0);
    assert(validate_student_id(-1) == 0);
    assert(validate_student_id(0) == 0);
    printf("  %s Invalid student IDs rejected.\n", PASS);
    if (report) fprintf(report, "  [PASS] Student ID validation tests passed.\n");
    
    /* Professor ID Tests */
    printf("\n--- Professor ID Validation Tests ---\n");
    assert(validate_professor_id(1001) == 1);
    assert(validate_professor_id(5000) == 1);
    assert(validate_professor_id(9998) == 1);
    printf("  %s Valid professor IDs accepted.\n", PASS);
    
    assert(validate_professor_id(1000) == 0);
    assert(validate_professor_id(9999) == 0);
    assert(validate_professor_id(0) == 0);
    assert(validate_professor_id(-1) == 0);
    printf("  %s Invalid professor IDs rejected.\n", PASS);
    if (report) fprintf(report, "  [PASS] Professor ID validation tests passed.\n");
    
    /* Course ID Tests */
    printf("\n--- Course ID Validation Tests ---\n");
    assert(validate_course_id(1000) == 1);
    assert(validate_course_id(5555) == 1);
    assert(validate_course_id(9999) == 1);
    printf("  %s Valid course IDs accepted.\n", PASS);
    
    assert(validate_course_id(999) == 0);
    assert(validate_course_id(10000) == 0);
    assert(validate_course_id(0) == 0);
    printf("  %s Invalid course IDs rejected.\n", PASS);
    if (report) fprintf(report, "  [PASS] Course ID validation tests passed.\n");
    /* Email Tests */
    printf("\n--- Email Validation Tests ---\n");
    assert(validate_email("john.doe@university.edu") == 1);
    assert(validate_email("test.user@university.edu") == 1);
    printf("  %s Valid emails accepted.\n", PASS);
    
    assert(validate_email("john@gmail.com") == 0);
    assert(validate_email("johndoe@university.edu") == 0);
    assert(validate_email("John.Doe@university.edu") == 0);
    assert(validate_email(NULL) == 0);
    printf("  %s Invalid emails rejected.\n", PASS);
    if (report) fprintf(report, "  [PASS] Email validation tests passed.\n");
    
    /* Phone Tests */
    printf("\n--- Phone Validation Tests ---\n");
    assert(validate_phone("555-1234") == 1);
    assert(validate_phone("555-9999") == 1);
    printf("  %s Valid phones accepted.\n", PASS);
    
    assert(validate_phone("123-4567") == 0);
    assert(validate_phone("5551234") == 0);
    assert(validate_phone(NULL) == 0);
    printf("  %s Invalid phones rejected.\n", PASS);
    if (report) fprintf(report, "  [PASS] Phone validation tests passed.\n");
    
    /* GPA Tests */
    printf("\n--- GPA Validation Tests ---\n");
    assert(validate_gpa(0.0) == 1);
    assert(validate_gpa(2.5) == 1);
    assert(validate_gpa(4.0) == 1);
    printf("  %s Valid GPAs accepted.\n", PASS);
    
    assert(validate_gpa(-0.1) == 0);
    assert(validate_gpa(4.1) == 0);
    printf("  %s Invalid GPAs rejected.\n", PASS);
    if (report) fprintf(report, "  [PASS] GPA validation tests passed.\n");
    
    /* Semester Tests */
    printf("\n--- Semester Validation Tests ---\n");
    assert(validate_semester("2021-SPRING") == 1);
    assert(validate_semester("2023-FALL") == 1);
    assert(validate_semester("2022-SUMMER") == 1);
    printf("  %s Valid semesters accepted.\n", PASS);
    
    assert(validate_semester("2021SPRING") == 0);
    assert(validate_semester("SPRING-2021") == 0);
    assert(validate_semester(NULL) == 0);
    printf("  %s Invalid semesters rejected.\n", PASS);
    if (report) fprintf(report, "  [PASS] Semester validation tests passed.\n");
    
    /* Letter Grade Tests */
    printf("\n--- Letter Grade Tests ---\n");
    char grade[4];
    compute_letter_grade(95, grade, 4); assert(strcmp(grade, "A") == 0);
    compute_letter_grade(100, grade, 4); assert(strcmp(grade, "A") == 0);
    compute_letter_grade(90, grade, 4); assert(strcmp(grade, "A-") == 0);
    compute_letter_grade(94, grade, 4); assert(strcmp(grade, "A-") == 0);
    compute_letter_grade(87, grade, 4); assert(strcmp(grade, "B+") == 0);
    compute_letter_grade(89, grade, 4); assert(strcmp(grade, "B+") == 0);
    compute_letter_grade(84, grade, 4); assert(strcmp(grade, "B") == 0);
    compute_letter_grade(86, grade, 4); assert(strcmp(grade, "B") == 0);
    compute_letter_grade(81, grade, 4); assert(strcmp(grade, "B-") == 0);
    compute_letter_grade(83, grade, 4); assert(strcmp(grade, "B-") == 0);
    compute_letter_grade(78, grade, 4); assert(strcmp(grade, "C+") == 0);
    compute_letter_grade(80, grade, 4); assert(strcmp(grade, "C+") == 0);
    compute_letter_grade(70, grade, 4); assert(strcmp(grade, "C") == 0);
    compute_letter_grade(77, grade, 4); assert(strcmp(grade, "C") == 0);
    compute_letter_grade(50, grade, 4); assert(strcmp(grade, "F") == 0);
    compute_letter_grade(0, grade, 4); assert(strcmp(grade, "F") == 0);
    compute_letter_grade(69, grade, 4); assert(strcmp(grade, "F") == 0);
    printf("  %s Letter grade computation correct.\n", PASS);
    if (report) fprintf(report, "  [PASS] Letter grade computation tests passed.\n");
    
    /* Letter Grade Validation Tests */
    printf("\n--- Letter Grade Validation Tests ---\n");
    assert(validate_letter_grade("A") == 1);
    assert(validate_letter_grade("A-") == 1);
    assert(validate_letter_grade("B+") == 1);
    assert(validate_letter_grade("B") == 1);
    assert(validate_letter_grade("B-") == 1);
    assert(validate_letter_grade("C+") == 1);
    assert(validate_letter_grade("C") == 1);
    assert(validate_letter_grade("F") == 1);
    printf("  %s Valid letter grades accepted.\n", PASS);
    
    assert(validate_letter_grade("a") == 0);
    assert(validate_letter_grade("b+") == 0);
    assert(validate_letter_grade("D") == 0);
    assert(validate_letter_grade("E") == 0);
    assert(validate_letter_grade("") == 0);
    assert(validate_letter_grade(NULL) == 0);
    assert(validate_letter_grade("AB") == 0);
    assert(validate_letter_grade("1") == 0);
    printf("  %s Invalid letter grades rejected.\n", PASS);
    if (report) fprintf(report, "  [PASS] Letter grade validation tests passed.\n");
    
    /* Numeric Grade Validation Tests */
    printf("\n--- Numeric Grade Validation Tests ---\n");
    assert(validate_numeric_grade(0.0) == 1);
    assert(validate_numeric_grade(50.0) == 1);
    assert(validate_numeric_grade(100.0) == 1);
    assert(validate_numeric_grade(75.5) == 1);
    printf("  %s Valid numeric grades accepted.\n", PASS);
    
    assert(validate_numeric_grade(-1.0) == 0);
    assert(validate_numeric_grade(-0.1) == 0);
    assert(validate_numeric_grade(100.1) == 0);
    assert(validate_numeric_grade(150.0) == 0);
    printf("  %s Invalid numeric grades rejected.\n", PASS);
    if (report) fprintf(report, "  [PASS] Numeric grade validation tests passed.\n");
    
    /* Name String Validation Tests */
    printf("\n--- Name String Validation Tests ---\n");
    assert(validate_name_string("John") == 1);
    assert(validate_name_string("Mary Jane") == 1);
    assert(validate_name_string("O'Connor") == 1);
    printf("  %s Valid names accepted.\n", PASS);
    
    assert(validate_name_string("John123") == 0);
    assert(validate_name_string("Test@Name") == 0);
    assert(validate_name_string("") == 0);
    assert(validate_name_string(NULL) == 0);
    printf("  %s Invalid names rejected.\n", PASS);
    if (report) fprintf(report, "  [PASS] Name string validation tests passed.\n");
    
    /* Date Validation Tests */
    printf("\n--- Date Validation Tests ---\n");
    assert(validate_date("2024-01-15") == 1);
    assert(validate_date("2020-12-31") == 1);
    assert(validate_date("2024-02-29") == 1); /* Leap year */
    printf("  %s Valid dates accepted.\n", PASS);
    
    assert(validate_date("2024-13-01") == 0); /* Invalid month */
    assert(validate_date("2024-00-15") == 0); /* Invalid month */
    assert(validate_date("2024-02-30") == 0); /* Invalid day */
    assert(validate_date("2023-02-29") == 0); /* Not leap year */
    assert(validate_date("24-01-15") == 0); /* Wrong format */
    assert(validate_date(NULL) == 0);
    printf("  %s Invalid dates rejected.\n", PASS);
    if (report) fprintf(report, "  [PASS] Date validation tests passed.\n");
    
    /* Trim Whitespace Tests */
    printf("\n--- Trim Whitespace Tests ---\n");
    char ws1[] = "  hello  ";
    trim_whitespace(ws1);
    assert(strcmp(ws1, "hello") == 0);
    
    char ws2[] = "\t\ttest\t\t";
    trim_whitespace(ws2);
    assert(strcmp(ws2, "test") == 0);
    
    char ws3[] = "   ";
    trim_whitespace(ws3);
    assert(strcmp(ws3, "") == 0);
    
    char ws4[] = "nowhitespace";
    trim_whitespace(ws4);
    assert(strcmp(ws4, "nowhitespace") == 0);
    
    char ws5[] = "";
    trim_whitespace(ws5);
    assert(strcmp(ws5, "") == 0);
    
    char ws6[] = "  middle space  ";
    trim_whitespace(ws6);
    assert(strcmp(ws6, "middle space") == 0);
    printf("  %s Whitespace trimmed correctly.\n", PASS);
    if (report) fprintf(report, "  [PASS] Trim whitespace tests passed.\n");
    
    /* Split CSV Line Tests */
    printf("\n--- Split CSV Line Tests ---\n");
    char csv1[] = "field1,field2,field3";
    char *fields1[5];
    int num1 = split_csv_line(csv1, fields1, 5);
    assert(num1 == 3);
    assert(strcmp(fields1[0], "field1") == 0);
    assert(strcmp(fields1[1], "field2") == 0);
    assert(strcmp(fields1[2], "field3") == 0);
    printf("  %s Basic CSV split works.\n", PASS);
    
    char csv2[] = "one";
    char *fields2[5];
    int num2 = split_csv_line(csv2, fields2, 5);
    assert(num2 == 1);
    assert(strcmp(fields2[0], "one") == 0);
    printf("  %s Single field CSV works.\n", PASS);
    
    char csv3[] = "a,b,c,d,e,f";
    char *fields3[3];
    int num3 = split_csv_line(csv3, fields3, 3);
    assert(num3 == 3); /* Limited by max_fields */
    printf("  %s Max fields limit works.\n", PASS);
    if (report) fprintf(report, "  [PASS] Split CSV line tests passed.\n");
    
    /* Format Name Tests */
    printf("\n--- Format Name Tests ---\n");
    char name1[] = "john";
    format_name(name1);
    assert(strcmp(name1, "John") == 0);
    
    char name2[] = "MARY";
    format_name(name2);
    assert(strcmp(name2, "Mary") == 0);
    
    char name3[] = "jOHN";
    format_name(name3);
    assert(strcmp(name3, "John") == 0);
    
    char name4[] = "a";
    format_name(name4);
    assert(strcmp(name4, "A") == 0);
    printf("  %s Names formatted correctly.\n", PASS);
    if (report) fprintf(report, "  [PASS] Format name tests passed.\n");
    
    /* Case Insensitive String Compare Tests */
    printf("\n--- Case Insensitive Compare Tests ---\n");
    assert(str_equals_case_insensitive("Hello", "hello") == 1);
    assert(str_equals_case_insensitive("HELLO", "hello") == 1);
    assert(str_equals_case_insensitive("HeLLo", "hElLO") == 1);
    assert(str_equals_case_insensitive("test", "test") == 1);
    printf("  %s Equal strings detected correctly.\n", PASS);
    
    assert(str_equals_case_insensitive("hello", "world") == 0);
    assert(str_equals_case_insensitive("test", "testing") == 0);
    assert(str_equals_case_insensitive("a", "b") == 0);
    printf("  %s Different strings detected correctly.\n", PASS);
    
    assert(str_equals_case_insensitive("", "") == 1);
    assert(str_equals_case_insensitive("test", "") == 0);
    printf("  %s Empty strings handled correctly.\n", PASS);
    if (report) fprintf(report, "  [PASS] Case insensitive compare tests passed.\n");
    
    /* Get Semester From Date Tests */
    printf("\n--- Get Semester From Date Tests ---\n");
    char semester[20];
    
    /* Spring: February(2) - June(6) */
    get_semester_from_date("2024-02-15", semester);
    assert(strcmp(semester, "2024-SPRING") == 0);
    
    get_semester_from_date("2024-04-15", semester);
    assert(strcmp(semester, "2024-SPRING") == 0);
    
    get_semester_from_date("2024-06-15", semester);
    assert(strcmp(semester, "2024-SPRING") == 0);
    
    /* Summer: July(7) - August(8) */
    get_semester_from_date("2024-07-15", semester);
    assert(strcmp(semester, "2024-SUMMER") == 0);
    
    get_semester_from_date("2024-08-15", semester);
    assert(strcmp(semester, "2024-SUMMER") == 0);
    
    /* Fall: September(9) - January(1) */
    get_semester_from_date("2024-09-15", semester);
    assert(strcmp(semester, "2024-FALL") == 0);
    
    get_semester_from_date("2024-12-15", semester);
    assert(strcmp(semester, "2024-FALL") == 0);
    
    get_semester_from_date("2024-01-15", semester);
    assert(strcmp(semester, "2024-FALL") == 0);
    
    printf("  %s Semester derived from date correctly.\n", PASS);
    if (report) fprintf(report, "  [PASS] Get semester from date tests passed.\n");
    
    /* Is Numeric Tests */
    printf("\n--- Is Numeric Tests ---\n");
    assert(is_numeric("12345") == 1);
    assert(is_numeric("0") == 1);
    assert(is_numeric("999999") == 1);
    printf("  %s Numeric strings detected correctly.\n", PASS);
    
    assert(is_numeric("12a45") == 0);
    assert(is_numeric("abc") == 0);
    assert(is_numeric("12.34") == 0);
    assert(is_numeric("-123") == 0);
    assert(is_numeric("") == 0);
    printf("  %s Non-numeric strings rejected correctly.\n", PASS);
    if (report) fprintf(report, "  [PASS] Is numeric tests passed.\n");
    
    printf("\n========================================\n");
    printf("UTILS MODULE: ALL TESTS PASSED\n");
    printf("========================================\n");
    if (report) fprintf(report, "\nUTILS MODULE: ALL TESTS PASSED\n");
    
    return 1;
}
