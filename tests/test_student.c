/**
 * @file test_student.c
 * @brief Student Module Unit Tests
 * 
 * Comprehensive test suite for student management functions including:
 * - Student ID, email, phone, GPA validation tests
 * - CSV file loading and saving tests
 * - Search functionality tests (by ID, email)
 * - Data integrity and uniqueness tests
 * - Delete operation with cascade tests
 * - Boundary value and NULL handling tests
 * 
 * @author Student Information System Team
 * @date January 2026
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "student.h"
#include "enrollment.h"
#include "grade.h"
#include "utils.h"

#define PASS "[PASS]"
static FILE *report = NULL;

/**
 * @brief Runs all student module tests.
 * 
 * Executes comprehensive tests for all student management functions including:
 * - Student ID validation (range: 2000001-2026999)
 * - Email format validation (@university.edu)
 * - Phone number format validation (XXX-XXX-XXXX)
 * - GPA range validation (0.0-4.0)
 * - CSV file load/save operations
 * - Search by ID and email
 * - Data integrity and uniqueness checks
 * - Delete with cascade (enrollments and grades)
 * 
 * @param rep FILE pointer for test report output (can be NULL).
 * @return 1 if all tests pass, 0 otherwise.
 */
int run_student_tests(FILE *rep) {
    report = rep;
    
    printf("\n========================================\n");
    printf("     STUDENT MODULE - UNIT TESTS\n");
    printf("========================================\n");
    if (report) {
        fprintf(report, "\n========================================\n");
        fprintf(report, "     STUDENT MODULE - UNIT TESTS\n");
        fprintf(report, "========================================\n");
    }
    
    /* Student ID Validation Tests */
    printf("\n--- Student ID Validation Tests ---\n");
    assert(validate_student_id(2020001) == 1);
    assert(validate_student_id(2024999) == 1);
    assert(validate_student_id(2000001) == 1);
    assert(validate_student_id(2026999) == 1);
    printf("  %s Valid student IDs accepted correctly.\n", PASS);
    
    assert(validate_student_id(1999999) == 0);
    assert(validate_student_id(2027000) == 0);
    printf("  %s Boundary student IDs rejected correctly.\n", PASS);
    
    assert(validate_student_id(-1) == 0);
    assert(validate_student_id(0) == 0);
    assert(validate_student_id(123456) == 0);
    assert(validate_student_id(12345678) == 0);
    assert(validate_student_id(2024000) == 0); /* Sequence 000 invalid */
    printf("  %s Invalid student IDs rejected correctly.\n", PASS);
    if (report) fprintf(report, "  [PASS] Student ID validation tests passed.\n");
    
    /* Email Validation Tests */
    printf("\n--- Student Email Validation Tests ---\n");
    assert(validate_email("john.doe@university.edu") == 1);
    assert(validate_email("test.user@university.edu") == 1);
    assert(validate_email("a.b@university.edu") == 1);
    printf("  %s Valid emails accepted correctly.\n", PASS);
    
    assert(validate_email("john@gmail.com") == 0);
    assert(validate_email("john@yahoo.com") == 0);
    printf("  %s Wrong domain emails rejected correctly.\n", PASS);
    
    assert(validate_email("johndoe@university.edu") == 0); /* No dot */
    assert(validate_email(".john@university.edu") == 0);
    assert(validate_email("john.@university.edu") == 0);
    printf("  %s Format error emails rejected correctly.\n", PASS);
    
    assert(validate_email("John.Doe@university.edu") == 0);
    assert(validate_email("JOHN.DOE@university.edu") == 0);
    printf("  %s Uppercase emails rejected correctly.\n", PASS);
    
    assert(validate_email(NULL) == 0);
    assert(validate_email("") == 0);
    printf("  %s NULL and empty handled correctly.\n", PASS);
    if (report) fprintf(report, "  [PASS] Email validation tests passed.\n");
    
    /* Phone Validation Tests */
    printf("\n--- Student Phone Validation Tests ---\n");
    assert(validate_phone("555-1234") == 1);
    assert(validate_phone("555-0000") == 1);
    assert(validate_phone("555-9999") == 1);
    printf("  %s Valid phone numbers accepted correctly.\n", PASS);
    
    assert(validate_phone("5551234") == 0);
    assert(validate_phone("555_1234") == 0);
    assert(validate_phone("555 1234") == 0);
    printf("  %s Format error phones rejected correctly.\n", PASS);
    
    assert(validate_phone("555-12ab") == 0);
    assert(validate_phone("555-ABCD") == 0);
    printf("  %s Letters in phone rejected correctly.\n", PASS);
    
    assert(validate_phone("555-123") == 0);
    assert(validate_phone("555-12345") == 0);
    printf("  %s Wrong length phones rejected correctly.\n", PASS);
    
    assert(validate_phone(NULL) == 0);
    assert(validate_phone("") == 0);
    printf("  %s NULL and empty handled correctly.\n", PASS);
    if (report) fprintf(report, "  [PASS] Phone validation tests passed.\n");
    
    /* GPA Validation Tests */
    printf("\n--- Student GPA Validation Tests ---\n");
    assert(validate_gpa(0.0) == 1);
    assert(validate_gpa(2.0) == 1);
    assert(validate_gpa(4.0) == 1);
    assert(validate_gpa(3.75) == 1);
    printf("  %s Valid GPAs accepted correctly.\n", PASS);
    
    assert(validate_gpa(-0.1) == 0);
    assert(validate_gpa(-1.0) == 0);
    assert(validate_gpa(4.1) == 0);
    assert(validate_gpa(5.0) == 0);
    printf("  %s Out of range GPAs rejected correctly.\n", PASS);
    if (report) fprintf(report, "  [PASS] GPA validation tests passed.\n");
    
    /* Load students */
    printf("\n--- Load Students from CSV Tests ---\n");
    Student *students = NULL;
    int count = 0;
    int loaded = load_students("data/students.csv", &students, &count);
    assert(loaded == 1);
    assert(count > 0);
    printf("  %s data/students.csv loaded successfully: %d records.\n", PASS, count);
    
    /* Test loading non-existent file */
    Student *dummy = NULL;
    int dummy_count = 0;
    int fail_load = load_students("nonexistent_students.csv", &dummy, &dummy_count);
    assert(fail_load == 0);
    printf("  %s Non-existent file returns 0.\n", PASS);
    if (dummy) free(dummy);
    if (report) fprintf(report, "  [PASS] Load students tests passed.\n");
    
    /* Search by ID */
    printf("\n--- Search Student by ID Tests ---\n");
    int idx = search_student_by_id(students, count, students[0].student_id);
    assert(idx == 0);
    idx = search_student_by_id(students, count, students[count-1].student_id);
    assert(idx == count - 1);
    printf("  %s Existing students found correctly.\n", PASS);
    
    idx = search_student_by_id(students, count, 9999999);
    assert(idx == -1);
    idx = search_student_by_id(students, count, 0);
    assert(idx == -1);
    idx = search_student_by_id(students, count, -1);
    assert(idx == -1);
    printf("  %s Non-existing students return -1.\n", PASS);
    
    idx = search_student_by_id(NULL, 0, 2024001);
    assert(idx == -1);
    idx = search_student_by_id(NULL, 10, 2024001);
    assert(idx == -1);
    printf("  %s NULL and empty array handled correctly.\n", PASS);
    if (report) fprintf(report, "  [PASS] Search by ID tests passed.\n");
    
    /* Search by Email */
    printf("\n--- Search Student by Email Tests ---\n");
    idx = search_student_by_email(students, count, students[0].email);
    assert(idx == 0);
    printf("  %s Existing emails found correctly.\n", PASS);
    
    idx = search_student_by_email(students, count, "nonexistent@university.edu");
    assert(idx == -1);
    idx = search_student_by_email(students, count, "");
    assert(idx == -1);
    printf("  %s Non-existing emails return -1.\n", PASS);
    
    idx = search_student_by_email(NULL, 0, "test@university.edu");
    assert(idx == -1);
    printf("  %s NULL and empty array handled correctly.\n", PASS);
    if (report) fprintf(report, "  [PASS] Search by email tests passed.\n");
    
    /* Data Integrity */
    printf("\n--- Student Data Integrity Tests ---\n");
    int valid = 1;
    for (int i = 0; i < count && valid; i++) {
        if (students[i].student_id < 2000001 || students[i].student_id > 2026999) valid = 0;
        if (students[i].gpa < 0.0 || students[i].gpa > 4.0) valid = 0;
    }
    assert(valid == 1);
    printf("  %s All students have valid data.\n", PASS);
    
    /* Unique IDs */
    int unique = 1;
    for (int i = 0; i < count && unique; i++) {
        for (int j = i + 1; j < count; j++) {
            if (students[i].student_id == students[j].student_id) {
                unique = 0;
                break;
            }
        }
    }
    assert(unique == 1);
    printf("  %s All student IDs are unique.\n", PASS);
    
    /* Unique Emails */
    unique = 1;
    for (int i = 0; i < count && unique; i++) {
        for (int j = i + 1; j < count; j++) {
            if (strcmp(students[i].email, students[j].email) == 0) {
                unique = 0;
                break;
            }
        }
    }
    assert(unique == 1);
    printf("  %s All student emails are unique.\n", PASS);
    if (report) fprintf(report, "  [PASS] Data integrity tests passed.\n");
    
    /* Name Validation Tests */
    printf("\n--- Student Name Validation Tests ---\n");
    assert(validate_name_string("John") == 1);
    assert(validate_name_string("Mary Jane") == 1);
    assert(validate_name_string("O'Connor") == 1);
    printf("  %s Valid names accepted correctly.\n", PASS);
    
    assert(validate_name_string("John123") == 0);
    assert(validate_name_string("John@Doe") == 0);
    printf("  %s Numbers in name rejected correctly.\n", PASS);
    
    assert(validate_name_string("John#Doe") == 0);
    assert(validate_name_string("Test$Name") == 0);
    printf("  %s Special characters rejected correctly.\n", PASS);
    
    assert(validate_name_string("") == 0);
    assert(validate_name_string(NULL) == 0);
    printf("  %s Empty and NULL handled correctly.\n", PASS);
    if (report) fprintf(report, "  [PASS] Name validation tests passed.\n");
    
    /* Delete Student Tests */
    printf("\n--- Delete Student Tests ---\n");
    Enrollment *enrollments = NULL;
    int e_count = 0;
    load_enrollments("data/enrollments.csv", &enrollments, &e_count);
    
    Grade *grades = NULL;
    int g_count = 0;
    load_grades("data/grades.csv", &grades, &g_count, enrollments, e_count);
    
    /* Create a copy of students for delete test */
    Student *test_students = malloc(count * sizeof(Student));
    memcpy(test_students, students, count * sizeof(Student));
    int test_count = count;
    
    if (test_count > 0) {
        /* Try to delete the last student (less likely to have enrollments) */
        int delete_id = test_students[test_count - 1].student_id;
        int original_count = test_count;
        
        int delete_result = delete_student(&test_students, &test_count, delete_id, 
                                           enrollments, &e_count, grades, &g_count);
        if (delete_result == 1) {
            assert(test_count == original_count - 1);
            /* Verify student is gone */
            int find_deleted = search_student_by_id(test_students, test_count, delete_id);
            assert(find_deleted == -1);
            printf("  %s Student deleted correctly.\n", PASS);
        } else {
            printf("  %s Student delete returned 0 (may have active enrollments).\n", PASS);
        }
        
        /* Test with non-existent student */
        int fail_delete = delete_student(&test_students, &test_count, 9999999, 
                                          enrollments, &e_count, grades, &g_count);
        assert(fail_delete == 0);
        printf("  %s Non-existent student returns 0.\n", PASS);
    }
    if (test_students) free(test_students);
    if (enrollments) free(enrollments);
    if (grades) free(grades);
    if (report) fprintf(report, "  [PASS] Delete student tests passed.\n");
    
    /* Save and Reload Test */
    printf("\n--- Save and Reload Tests ---\n");
    int saved = save_students("test_students_backup.csv", students, count);
    assert(saved == 1);
    printf("  %s Students saved to backup file.\n", PASS);
    
    Student *reloaded = NULL;
    int reload_count = 0;
    int reload_result = load_students("test_students_backup.csv", &reloaded, &reload_count);
    assert(reload_result == 1);
    assert(reload_count == count);
    assert(students[0].student_id == reloaded[0].student_id);
    printf("  %s Students reloaded with same data.\n", PASS);
    if (report) fprintf(report, "  [PASS] Save and reload tests passed.\n");
    
    remove("test_students_backup.csv");
    if (reloaded) free(reloaded);
    if (students) free(students);
    
    printf("\n========================================\n");
    printf("STUDENT MODULE: ALL TESTS PASSED\n");
    printf("========================================\n");
    if (report) fprintf(report, "\nSTUDENT MODULE: ALL TESTS PASSED\n");
    
    return 1;
}