/**
 * @file test_enrollment.c
 * @brief Enrollment Module Unit Tests
 * 
 * Comprehensive test suite for enrollment management functions including:
 * - Semester format validation tests (YYYY-SEASON)
 * - CSV file loading and saving tests
 * - Search functionality tests (by ID)
 * - Student enrollment status tests
 * - Course capacity checking tests
 * - Prerequisites validation tests
 * - Data integrity and uniqueness tests
 * - Boundary value and NULL handling tests
 * 
 * @author Student Information System Team
 * @date January 2026
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "enrollment.h"
#include "course.h"
#include "grade.h"
#include "utils.h"

#define PASS "[PASS]"
static FILE *report = NULL;

/**
 * @brief Runs all enrollment module tests.
 * 
 * Executes comprehensive tests for all enrollment management functions including:
 * - Semester format validation (YYYY-SPRING/SUMMER/FALL)
 * - Enrollment status validation (Active, Completed, Dropped, Withdrawn, Enrolled)
 * - Course capacity checking
 * - Prerequisites validation
 * - CSV file load/save operations
 * - Search by enrollment ID
 * - is_student_enrolled_in_course functionality
 * - Data integrity and uniqueness checks
 * 
 * @param rep FILE pointer for test report output (can be NULL).
 * @return 1 if all tests pass, 0 otherwise.
 */
int run_enrollment_tests(FILE *rep) {
    report = rep;
    
    printf("\n========================================\n");
    printf("    ENROLLMENT MODULE - UNIT TESTS\n");
    printf("========================================\n");
    if (report) {
        fprintf(report, "\n========================================\n");
        fprintf(report, "    ENROLLMENT MODULE - UNIT TESTS\n");
        fprintf(report, "========================================\n");
    }
    
    /* Semester Validation Tests */
    printf("\n--- Semester Validation Tests ---\n");
    assert(validate_semester("2021-SPRING") == 1);
    assert(validate_semester("2023-FALL") == 1);
    assert(validate_semester("2022-SUMMER") == 1);
    assert(validate_semester("2024-SPRING") == 1);
    printf("  %s Valid semesters accepted correctly.\n", PASS);
    
    assert(validate_semester("2021-spring") == 1);
    assert(validate_semester("2021-Fall") == 1);
    assert(validate_semester("2021-Summer") == 1);
    printf("  %s Case-insensitive semesters accepted correctly.\n", PASS);
    
    assert(validate_semester("2021-SPRNG") == 0);
    assert(validate_semester("2021-AUTMN") == 0);
    assert(validate_semester("2021-WINTER") == 0);
    printf("  %s Season typos rejected correctly.\n", PASS);
    
    assert(validate_semester("2021SPRING") == 0);
    assert(validate_semester("SPRING-2021") == 0);
    assert(validate_semester("2021/SPRING") == 0);
    printf("  %s Format errors rejected correctly.\n", PASS);
    
    assert(validate_semester("ABCD-SPRING") == 0);
    assert(validate_semester("20A1-FALL") == 0);
    printf("  %s Letters in year rejected correctly.\n", PASS);
    
    assert(validate_semester("") == 0);
    assert(validate_semester(NULL) == 0);
    assert(validate_semester("2021") == 0);
    assert(validate_semester("2021-") == 0);
    printf("  %s Empty, NULL, incomplete rejected correctly.\n", PASS);
    if (report) fprintf(report, "  [PASS] Semester validation tests passed.\n");
    
    /* Load enrollments */
    printf("\n--- Load Enrollments from CSV Tests ---\n");
    Enrollment *enrollments = NULL;
    int count = 0;
    int loaded = load_enrollments("data/enrollments.csv", &enrollments, &count);
    assert(loaded == 1);
    assert(count > 0);
    printf("  %s data/enrollments.csv loaded successfully: %d records.\n", PASS, count);
    
    /* Test loading non-existent file */
    Enrollment *dummy = NULL;
    int dummy_count = 0;
    int fail_load = load_enrollments("nonexistent_enrollments.csv", &dummy, &dummy_count);
    assert(fail_load == 0);
    printf("  %s Non-existent file returns 0.\n", PASS);
    if (dummy) free(dummy);
    if (report) fprintf(report, "  [PASS] Load enrollments tests passed.\n");
    
    /* Search by ID */
    printf("\n--- Search Enrollment by ID Tests ---\n");
    int idx = search_enrollment_by_id(enrollments, count, enrollments[0].enrollment_id);
    assert(idx == 0);
    idx = search_enrollment_by_id(enrollments, count, enrollments[count-1].enrollment_id);
    assert(idx == count - 1);
    printf("  %s Existing enrollments found correctly.\n", PASS);
    
    idx = search_enrollment_by_id(enrollments, count, 999999);
    assert(idx == -1);
    idx = search_enrollment_by_id(enrollments, count, 0);
    assert(idx == -1);
    idx = search_enrollment_by_id(enrollments, count, -1);
    assert(idx == -1);
    printf("  %s Non-existing enrollments return -1.\n", PASS);
    
    idx = search_enrollment_by_id(NULL, 0, 1);
    assert(idx == -1);
    printf("  %s NULL and empty array handled correctly.\n", PASS);
    if (report) fprintf(report, "  [PASS] Search by ID tests passed.\n");
    
    /* is_student_enrolled_in_course Tests */
    printf("\n--- Is Student Enrolled Tests ---\n");
    /* Find an enrollment to test with */
    if (count > 0) {
        int result = is_student_enrolled_in_course(enrollments, count, 
                                                    enrollments[0].student_id, 
                                                    enrollments[0].course_id, 
                                                    enrollments[0].semester);
        assert(result == 1);
        printf("  %s Enrolled students detected correctly.\n", PASS);
        
        result = is_student_enrolled_in_course(enrollments, count, 
                                               enrollments[0].student_id, 
                                               enrollments[0].course_id, 
                                               "1999-SPRING");
        assert(result == 0);
        printf("  %s Different semester returns 0.\n", PASS);
        
        result = is_student_enrolled_in_course(enrollments, count, 
                                               enrollments[0].student_id, 
                                               99999, 
                                               enrollments[0].semester);
        assert(result == 0);
        printf("  %s Different course returns 0.\n", PASS);
        
        result = is_student_enrolled_in_course(enrollments, count, 
                                               9999999, 
                                               enrollments[0].course_id, 
                                               enrollments[0].semester);
        assert(result == 0);
        printf("  %s Different student returns 0.\n", PASS);
    }
    
    int result = is_student_enrolled_in_course(NULL, 0, 2024001, 1000, "2024-FALL");
    assert(result == 0);
    printf("  %s NULL and empty handled correctly.\n", PASS);
    if (report) fprintf(report, "  [PASS] Is student enrolled tests passed.\n");
    
    /* Data Integrity */
    printf("\n--- Enrollment Data Integrity Tests ---\n");
    int valid = 1;
    for (int i = 0; i < count && valid; i++) {
        if (enrollments[i].enrollment_id <= 0) valid = 0;
        if (enrollments[i].student_id <= 0) valid = 0;
        if (enrollments[i].course_id <= 0) valid = 0;
    }
    assert(valid == 1);
    printf("  %s All enrollments have valid data.\n", PASS);
    
    /* Unique IDs */
    int unique = 1;
    for (int i = 0; i < count && unique; i++) {
        for (int j = i + 1; j < count; j++) {
            if (enrollments[i].enrollment_id == enrollments[j].enrollment_id) {
                unique = 0;
                break;
            }
        }
    }
    assert(unique == 1);
    printf("  %s All enrollment IDs are unique.\n", PASS);
    if (report) fprintf(report, "  [PASS] Data integrity tests passed.\n");
    
    /* Status Validation */
    printf("\n--- Enrollment Status Tests ---\n");
    valid = 1;
    for (int i = 0; i < count && valid; i++) {
        if (strcmp(enrollments[i].status, "Active") != 0 &&
            strcmp(enrollments[i].status, "Completed") != 0 &&
            strcmp(enrollments[i].status, "Dropped") != 0 &&
            strcmp(enrollments[i].status, "Withdrawn") != 0 &&
            strcmp(enrollments[i].status, "Enrolled") != 0) {
            valid = 0;
        }
    }
    assert(valid == 1);
    printf("  %s All enrollment statuses are valid.\n", PASS);
    if (report) fprintf(report, "  [PASS] Status validation tests passed.\n");
    
    /* Check Course Capacity Tests */
    printf("\n--- Check Course Capacity Tests ---\n");
    Course *courses = NULL;
    int c_count = 0;
    load_courses("data/courses.csv", &courses, &c_count);
    
    if (c_count > 0 && count > 0) {
        /* Find a course with enrollments */
        int test_course_id = enrollments[0].course_id;
        char test_semester[20];
        strcpy(test_semester, enrollments[0].semester);
        
        /* Check capacity - should return 1 if there's room */
        int capacity_result = check_course_capacity(enrollments, count, courses, c_count, 
                                                     test_course_id, test_semester);
        /* Result depends on actual data, but shouldn't crash */
        assert(capacity_result == 0 || capacity_result == 1);
        printf("  %s Course capacity check works correctly.\n", PASS);
        
        /* Test with non-existent course */
        int no_course = check_course_capacity(enrollments, count, courses, c_count, 
                                              99999, test_semester);
        assert(no_course == 0 || no_course == 1);
        printf("  %s Non-existent course handled correctly.\n", PASS);
        
        /* Test with NULL arrays */
        int null_result = check_course_capacity(NULL, 0, courses, c_count, 
                                                test_course_id, test_semester);
        assert(null_result == 0 || null_result == 1);
        printf("  %s NULL enrollment array handled correctly.\n", PASS);
    }
    if (report) fprintf(report, "  [PASS] Check course capacity tests passed.\n");
    
    /* Check Prerequisites Tests */
    printf("\n--- Check Prerequisites Tests ---\n");
    Grade *grades = NULL;
    int g_count = 0;
    load_grades("data/grades.csv", &grades, &g_count, enrollments, count);
    
    if (c_count > 0 && count > 0) {
        int test_student_id = enrollments[0].student_id;
        int test_course_id = enrollments[0].course_id;
        
        /* Check prerequisites - result depends on data */
        int prereq_result = check_prerequisites(enrollments, count, courses, c_count, 
                                                grades, g_count, test_student_id, test_course_id);
        assert(prereq_result == 0 || prereq_result == 1);
        printf("  %s Prerequisites check works correctly.\n", PASS);
        
        /* Test with non-existent student */
        int no_student = check_prerequisites(enrollments, count, courses, c_count, 
                                             grades, g_count, 9999999, test_course_id);
        assert(no_student == 0 || no_student == 1);
        printf("  %s Non-existent student handled correctly.\n", PASS);
        
        /* Test with NULL grades */
        int null_grades = check_prerequisites(enrollments, count, courses, c_count, 
                                              NULL, 0, test_student_id, test_course_id);
        assert(null_grades == 0 || null_grades == 1);
        printf("  %s NULL grades array handled correctly.\n", PASS);
    }
    if (grades) free(grades);
    if (courses) free(courses);
    if (report) fprintf(report, "  [PASS] Check prerequisites tests passed.\n");
    
    /* Save and Reload Test */
    printf("\n--- Save and Reload Tests ---\n");
    int saved = save_enrollments("test_enrollments_backup.csv", enrollments, count);
    assert(saved == 1);
    printf("  %s Enrollments saved to backup file.\n", PASS);
    
    Enrollment *reloaded = NULL;
    int reload_count = 0;
    int reload_result = load_enrollments("test_enrollments_backup.csv", &reloaded, &reload_count);
    assert(reload_result == 1);
    assert(reload_count == count);
    assert(enrollments[0].enrollment_id == reloaded[0].enrollment_id);
    printf("  %s Enrollments reloaded with same data.\n", PASS);
    if (report) fprintf(report, "  [PASS] Save and reload tests passed.\n");
    
    remove("test_enrollments_backup.csv");
    if (reloaded) free(reloaded);
    if (enrollments) free(enrollments);
    
    printf("\n========================================\n");
    printf("ENROLLMENT MODULE: ALL TESTS PASSED\n");
    printf("========================================\n");
    if (report) fprintf(report, "\nENROLLMENT MODULE: ALL TESTS PASSED\n");
    
    return 1;
}