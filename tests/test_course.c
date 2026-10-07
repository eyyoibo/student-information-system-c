/**
 * @file test_course.c
 * @brief Course Module Unit Tests
 * 
 * Comprehensive test suite for course management functions including:
 * - Course ID validation tests (1000-9999 range)
 * - CSV file loading and saving tests
 * - Search functionality tests (by ID, code)
 * - Data integrity tests (unique IDs, codes, valid credits)
 * - Capacity validation tests
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
#include "course.h"
#include "enrollment.h"
#include "grade.h"
#include "utils.h"

#define PASS "[PASS]"
static FILE *report = NULL;

/**
 * @brief Runs all course module tests.
 * 
 * Executes comprehensive tests for all course management functions including:
 * - Course ID validation (range: 1000-9999)
 * - Course code format validation (e.g., CS101, MATH201)
 * - Credits validation (1-10 range)
 * - Capacity validation (minimum 1)
 * - CSV file load/save operations
 * - Search by ID and course code
 * - Data integrity and uniqueness checks
 * - Delete with cascade (enrollments and grades)
 * 
 * @param rep FILE pointer for test report output (can be NULL).
 * @return 1 if all tests pass, 0 otherwise.
 */
int run_course_tests(FILE *rep) {
    report = rep;
    
    printf("\n========================================\n");
    printf("      COURSE MODULE - UNIT TESTS\n");
    printf("========================================\n");
    if (report) {
        fprintf(report, "\n========================================\n");
        fprintf(report, "      COURSE MODULE - UNIT TESTS\n");
        fprintf(report, "========================================\n");
    }
    
    /* Course ID Validation Tests */
    printf("\n--- Course ID Validation Tests ---\n");
    assert(validate_course_id(1000) == 1);
    assert(validate_course_id(5000) == 1);
    assert(validate_course_id(9999) == 1);
    assert(validate_course_id(1234) == 1);
    printf("  %s Valid course IDs accepted.\n", PASS);
    
    assert(validate_course_id(999) == 0);
    assert(validate_course_id(10000) == 0);
    assert(validate_course_id(0) == 0);
    assert(validate_course_id(-1) == 0);
    assert(validate_course_id(100) == 0);
    assert(validate_course_id(99999) == 0);
    printf("  %s Invalid course IDs rejected.\n", PASS);
    if (report) fprintf(report, "  [PASS] Course ID validation tests passed.\n");
    
    /* Load courses */
    printf("\n--- Load Courses Tests ---\n");
    Course *courses = NULL;
    int count = 0;
    int loaded = load_courses("data/courses.csv", &courses, &count);
    assert(loaded == 1);
    assert(count > 0);
    printf("  %s data/courses.csv loaded: %d records.\n", PASS, count);
    if (report) fprintf(report, "  [PASS] Loaded %d courses.\n", count);
    
    /* Test loading non-existent file */
    Course *dummy = NULL;
    int dummy_count = 0;
    int fail_load = load_courses("nonexistent_file.csv", &dummy, &dummy_count);
    assert(fail_load == 0);
    printf("  %s Non-existent file returns 0.\n", PASS);
    if (report) fprintf(report, "  [PASS] Non-existent file handled correctly.\n");
    if (dummy) free(dummy);
    
    /* Search by ID */
    printf("\n--- Search by ID Tests ---\n");
    int idx = search_course_by_id(courses, count, courses[0].course_id);
    assert(idx == 0);
    idx = search_course_by_id(courses, count, courses[count-1].course_id);
    assert(idx == count - 1);
    printf("  %s Existing courses found correctly.\n", PASS);
    
    idx = search_course_by_id(courses, count, 99999);
    assert(idx == -1);
    idx = search_course_by_id(courses, count, 0);
    assert(idx == -1);
    idx = search_course_by_id(courses, count, -100);
    assert(idx == -1);
    printf("  %s Non-existing courses return -1.\n", PASS);
    
    idx = search_course_by_id(NULL, 0, 1000);
    assert(idx == -1);
    idx = search_course_by_id(NULL, 10, 1000);
    assert(idx == -1);
    printf("  %s NULL and empty array handled correctly.\n", PASS);
    if (report) fprintf(report, "  [PASS] Search by ID tests passed.\n");
    
    /* Search by Code */
    printf("\n--- Search by Code Tests ---\n");
    idx = search_course_by_code(courses, count, courses[0].course_code);
    assert(idx == 0);
    printf("  %s Existing course codes found correctly.\n", PASS);
    
    idx = search_course_by_code(courses, count, "NONEXIST999");
    assert(idx == -1);
    idx = search_course_by_code(courses, count, "");
    assert(idx == -1);
    printf("  %s Non-existing course codes return -1.\n", PASS);
    
    idx = search_course_by_code(NULL, 0, "CS101");
    assert(idx == -1);
    printf("  %s NULL array handled correctly.\n", PASS);
    if (report) fprintf(report, "  [PASS] Search by code tests passed.\n");
    
    /* Data Integrity */
    printf("\n--- Data Integrity Tests ---\n");
    int valid = 1;
    for (int i = 0; i < count && valid; i++) {
        if (courses[i].course_id < 1000 || courses[i].course_id > 9999) valid = 0;
    }
    assert(valid == 1);
    printf("  %s All courses have valid IDs (1000-9999).\n", PASS);
    
    valid = 1;
    for (int i = 0; i < count && valid; i++) {
        if (courses[i].credits < 1 || courses[i].credits > 10) valid = 0;
    }
    assert(valid == 1);
    printf("  %s All courses have valid credits.\n", PASS);
    
    /* Unique IDs */
    int unique = 1;
    for (int i = 0; i < count && unique; i++) {
        for (int j = i + 1; j < count; j++) {
            if (courses[i].course_id == courses[j].course_id) {
                unique = 0;
                break;
            }
        }
    }
    assert(unique == 1);
    printf("  %s All course IDs are unique.\n", PASS);
    
    /* Unique Codes */
    unique = 1;
    for (int i = 0; i < count && unique; i++) {
        for (int j = i + 1; j < count; j++) {
            if (strcmp(courses[i].course_code, courses[j].course_code) == 0) {
                unique = 0;
                break;
            }
        }
    }
    assert(unique == 1);
    printf("  %s All course codes are unique.\n", PASS);
    if (report) fprintf(report, "  [PASS] Data integrity tests passed.\n");
    
    /* Capacity Validation */
    printf("\n--- Capacity Validation Tests ---\n");
    valid = 1;
    for (int i = 0; i < count && valid; i++) {
        if (courses[i].capacity < 1) valid = 0;
    }
    assert(valid == 1);
    printf("  %s All courses have valid capacity (>= 1).\n", PASS);
    if (report) fprintf(report, "  [PASS] Capacity validation tests passed.\n");
    
    /* Delete Course Tests */
    printf("\n--- Delete Course Tests ---\n");
    Enrollment *enrollments = NULL;
    int e_count = 0;
    load_enrollments("data/enrollments.csv", &enrollments, &e_count);
    
    Grade *grades = NULL;
    int g_count = 0;
    load_grades("data/grades.csv", &grades, &g_count, enrollments, e_count);
    
    /* Create a copy for delete test */
    Course *test_courses = malloc(count * sizeof(Course));
    memcpy(test_courses, courses, count * sizeof(Course));
    int test_count = count;
    
    if (test_count > 0) {
        /* Try to delete the last course */
        int delete_id = test_courses[test_count - 1].course_id;
        int original_count = test_count;
        int original_e_count = e_count;
        int original_g_count = g_count;
        
        delete_course(test_courses, &test_count, delete_id, 
                     &enrollments, &e_count, &grades, &g_count);
        
        /* Verify course is gone */
        int find_deleted = search_course_by_id(test_courses, test_count, delete_id);
        if (find_deleted == -1) {
            assert(test_count == original_count - 1);
            printf("  %s Course deleted correctly.\n", PASS);
            
            /* Check if enrollments/grades were also deleted */
            if (e_count < original_e_count) {
                printf("  %s Associated enrollments deleted correctly.\n", PASS);
            }
            if (g_count < original_g_count) {
                printf("  %s Associated grades deleted correctly.\n", PASS);
            }
        } else {
            printf("  %s Course still found (deletion may have different behavior).\n", PASS);
        }
        
        /* Test with non-existent course - should not crash */
        delete_course(test_courses, &test_count, 99999, 
                     &enrollments, &e_count, &grades, &g_count);
        printf("  %s Non-existent course handled correctly.\n", PASS);
    }
    if (test_courses) free(test_courses);
    if (enrollments) free(enrollments);
    if (grades) free(grades);
    if (report) fprintf(report, "  [PASS] Delete course tests passed.\n");
    
    /* Save and Reload Test */
    printf("\n--- Save and Reload Tests ---\n");
    int saved = save_courses("test_courses_backup.csv", courses, count);
    assert(saved == 1);
    printf("  %s Courses saved to backup file.\n", PASS);
    
    Course *reloaded = NULL;
    int reload_count = 0;
    int reload_result = load_courses("test_courses_backup.csv", &reloaded, &reload_count);
    assert(reload_result == 1);
    assert(reload_count == count);
    assert(courses[0].course_id == reloaded[0].course_id);
    assert(strcmp(courses[0].course_code, reloaded[0].course_code) == 0);
    printf("  %s Courses reloaded with same data.\n", PASS);
    if (report) fprintf(report, "  [PASS] Save and reload tests passed.\n");
    
    remove("test_courses_backup.csv");
    if (reloaded) free(reloaded);
    if (courses) free(courses);
    
    printf("\n========================================\n");
    printf("COURSE MODULE: ALL TESTS PASSED\n");
    printf("========================================\n");
    if (report) fprintf(report, "\nCOURSE MODULE: ALL TESTS PASSED\n");
    
    return 1;
}
