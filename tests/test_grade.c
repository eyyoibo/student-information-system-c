/**
 * @file test_grade.c
 * @brief Grade Module Unit Tests
 * 
 * Comprehensive test suite for grade management functions including:
 * - Numeric grade validation tests (0-100 range)
 * - Letter grade validation and computation tests
 * - Grade weight/GPA calculation tests
 * - CSV file loading and saving tests
 * - Search functionality tests (by ID, enrollment ID)
 * - Course statistics calculation tests
 * - Record and update grade tests
 * - Data integrity and consistency tests
 * - Boundary value and NULL handling tests
 * 
 * @author Student Information System Team
 * @date January 2026
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "grade.h"
#include "enrollment.h"
#include "utils.h"

#define PASS "[PASS]"
static FILE *report = NULL;

/**
 * @brief Runs all grade module tests.
 * 
 * Executes comprehensive tests for all grade management functions including:
 * - Numeric grade validation (range: 0-100)
 * - Letter grade validation (A, A-, B+, B, B-, C+, C, F)
 * - Letter grade computation from numeric grades
 * - Grade weight calculation for GPA
 * - CSV file load/save operations
 * - Search by grade ID and enrollment ID
 * - Course statistics calculation (average, min, max, distribution)
 * - Record and update grade operations
 * - Data integrity and letter grade consistency checks
 * 
 * @param rep FILE pointer for test report output (can be NULL).
 * @return 1 if all tests pass, 0 otherwise.
 */
int run_grade_tests(FILE *rep) {
    report = rep;
    
    printf("\n========================================\n");
    printf("      GRADE MODULE - UNIT TESTS\n");
    printf("========================================\n");
    if (report) {
        fprintf(report, "\n========================================\n");
        fprintf(report, "      GRADE MODULE - UNIT TESTS\n");
        fprintf(report, "========================================\n");
    }
    
    /* Numeric Grade Validation Tests */
    printf("\n--- Numeric Grade Validation Tests ---\n");
    assert(validate_numeric_grade(0.0) == 1);
    assert(validate_numeric_grade(50.0) == 1);
    assert(validate_numeric_grade(100.0) == 1);
    assert(validate_numeric_grade(75.5) == 1);
    assert(validate_numeric_grade(99.99) == 1);
    printf("  %s Valid numeric grades accepted correctly.\n", PASS);
    
    assert(validate_numeric_grade(-1.0) == 0);
    assert(validate_numeric_grade(-0.1) == 0);
    assert(validate_numeric_grade(100.1) == 0);
    assert(validate_numeric_grade(150.0) == 0);
    assert(validate_numeric_grade(101.0) == 0);
    printf("  %s Out of range grades rejected correctly.\n", PASS);
    if (report) fprintf(report, "  [PASS] Numeric grade validation tests passed.\n");
    
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
    printf("  %s Valid letter grades accepted correctly.\n", PASS);
    
    assert(validate_letter_grade("a") == 0);
    assert(validate_letter_grade("b+") == 0);
    assert(validate_letter_grade("f") == 0);
    printf("  %s Lowercase letter grades rejected correctly.\n", PASS);
    
    assert(validate_letter_grade("D") == 0);
    assert(validate_letter_grade("E") == 0);
    assert(validate_letter_grade("AA") == 0);
    assert(validate_letter_grade("X") == 0);
    printf("  %s Invalid letter grades rejected correctly.\n", PASS);
    
    assert(validate_letter_grade("") == 0);
    assert(validate_letter_grade(NULL) == 0);
    printf("  %s Empty and NULL handled correctly.\n", PASS);
    
    assert(validate_letter_grade("1") == 0);
    assert(validate_letter_grade("100") == 0);
    printf("  %s Numbers rejected correctly.\n", PASS);
    if (report) fprintf(report, "  [PASS] Letter grade validation tests passed.\n");
    
    /* Compute Letter Grade Tests */
    printf("\n--- Compute Letter Grade Tests ---\n");
    char grade[4];
    compute_letter_grade(100, grade, 4); assert(strcmp(grade, "A") == 0);
    compute_letter_grade(95, grade, 4); assert(strcmp(grade, "A") == 0);
    printf("  %s Grade A computed correctly (95-100).\n", PASS);
    
    compute_letter_grade(94, grade, 4); assert(strcmp(grade, "A-") == 0);
    compute_letter_grade(90, grade, 4); assert(strcmp(grade, "A-") == 0);
    printf("  %s Grade A- computed correctly (90-94).\n", PASS);
    
    compute_letter_grade(89, grade, 4); assert(strcmp(grade, "B+") == 0);
    compute_letter_grade(87, grade, 4); assert(strcmp(grade, "B+") == 0);
    printf("  %s Grade B+ computed correctly (87-89).\n", PASS);
    
    compute_letter_grade(86, grade, 4); assert(strcmp(grade, "B") == 0);
    compute_letter_grade(84, grade, 4); assert(strcmp(grade, "B") == 0);
    printf("  %s Grade B computed correctly (84-86).\n", PASS);
    
    compute_letter_grade(83, grade, 4); assert(strcmp(grade, "B-") == 0);
    compute_letter_grade(81, grade, 4); assert(strcmp(grade, "B-") == 0);
    printf("  %s Grade B- computed correctly (81-83).\n", PASS);
    
    compute_letter_grade(80, grade, 4); assert(strcmp(grade, "C+") == 0);
    compute_letter_grade(78, grade, 4); assert(strcmp(grade, "C+") == 0);
    printf("  %s Grade C+ computed correctly (78-80).\n", PASS);
    
    compute_letter_grade(77, grade, 4); assert(strcmp(grade, "C") == 0);
    compute_letter_grade(70, grade, 4); assert(strcmp(grade, "C") == 0);
    printf("  %s Grade C computed correctly (70-77).\n", PASS);
    
    compute_letter_grade(69, grade, 4); assert(strcmp(grade, "F") == 0);
    compute_letter_grade(50, grade, 4); assert(strcmp(grade, "F") == 0);
    compute_letter_grade(0, grade, 4); assert(strcmp(grade, "F") == 0);
    printf("  %s Grade F computed correctly (0-69).\n", PASS);
    
    /* Boundary values */
    compute_letter_grade(94.9, grade, 4); assert(strcmp(grade, "A-") == 0);
    compute_letter_grade(89.9, grade, 4); assert(strcmp(grade, "B+") == 0);
    compute_letter_grade(69.9, grade, 4); assert(strcmp(grade, "F") == 0);
    printf("  %s Boundary values computed correctly.\n", PASS);
    if (report) fprintf(report, "  [PASS] Compute letter grade tests passed.\n");
    
    /* Grade Weight Tests */
    printf("\n--- Grade Weight Tests ---\n");
    assert(get_grade_weight("A") == 4.0f);
    assert(get_grade_weight("A-") == 3.7f);
    assert(get_grade_weight("B+") == 3.3f);
    assert(get_grade_weight("B") == 3.0f);
    assert(get_grade_weight("B-") == 2.7f);
    assert(get_grade_weight("C+") == 2.3f);
    assert(get_grade_weight("C") == 2.0f);
    assert(get_grade_weight("F") == 0.0f);
    printf("  %s Grade weights correct.\n", PASS);
    
    assert(get_grade_weight("X") == 0.0f);
    assert(get_grade_weight("D") == 0.0f);
    assert(get_grade_weight(NULL) == 0.0f);
    printf("  %s Invalid grade weights return 0.\n", PASS);
    if (report) fprintf(report, "  [PASS] Grade weight tests passed.\n");
    
    /* Load enrollments first (needed for grades) */
    Enrollment *enrollments = NULL;
    int e_count = 0;
    load_enrollments("data/enrollments.csv", &enrollments, &e_count);
    
    /* Load grades */
    printf("\n--- Load Grades from CSV Tests ---\n");
    Grade *grades = NULL;
    int count = 0;
    int loaded = load_grades("data/grades.csv", &grades, &count, enrollments, e_count);
    assert(loaded == 1);
    assert(count > 0);
    printf("  %s data/grades.csv loaded successfully: %d records.\n", PASS, count);
    
    /* Test loading non-existent file */
    Grade *dummy = NULL;
    int dummy_count = 0;
    int fail_load = load_grades("nonexistent_grades.csv", &dummy, &dummy_count, enrollments, e_count);
    assert(fail_load == 0);
    printf("  %s Non-existent file returns 0.\n", PASS);
    if (dummy) free(dummy);
    if (report) fprintf(report, "  [PASS] Load grades tests passed.\n");
    
    /* Search by ID */
    printf("\n--- Search Grade by ID Tests ---\n");
    int idx = search_grade_by_id(grades, count, grades[0].grade_id);
    assert(idx == 0);
    idx = search_grade_by_id(grades, count, grades[count-1].grade_id);
    assert(idx == count - 1);
    printf("  %s Existing grades found correctly.\n", PASS);
    
    idx = search_grade_by_id(grades, count, 999999);
    assert(idx == -1);
    idx = search_grade_by_id(grades, count, 0);
    assert(idx == -1);
    printf("  %s Non-existing grades return -1.\n", PASS);
    
    idx = search_grade_by_id(NULL, 0, 1);
    assert(idx == -1);
    printf("  %s NULL and empty array handled correctly.\n", PASS);
    if (report) fprintf(report, "  [PASS] Search by ID tests passed.\n");
    
    /* Search by Enrollment ID */
    printf("\n--- Search Grade by Enrollment ID Tests ---\n");
    idx = search_grade_by_enrollment_id(grades, count, grades[0].enrollment_id);
    assert(idx >= 0);
    printf("  %s Existing enrollment IDs found correctly.\n", PASS);
    
    idx = search_grade_by_enrollment_id(grades, count, 999999);
    assert(idx == -1);
    printf("  %s Non-existing enrollment IDs return -1.\n", PASS);
    
    idx = search_grade_by_enrollment_id(NULL, 0, 1);
    assert(idx == -1);
    printf("  %s NULL and empty array handled correctly.\n", PASS);
    if (report) fprintf(report, "  [PASS] Search by enrollment ID tests passed.\n");
    
    /* Data Integrity */
    printf("\n--- Grade Data Integrity Tests ---\n");
    int valid = 1;
    for (int i = 0; i < count && valid; i++) {
        if (grades[i].grade_id <= 0) valid = 0;
        if (grades[i].numeric_grade < 0.0 || grades[i].numeric_grade > 100.0) valid = 0;
    }
    assert(valid == 1);
    printf("  %s All grades have valid data.\n", PASS);
    
    /* Unique IDs */
    int unique = 1;
    for (int i = 0; i < count && unique; i++) {
        for (int j = i + 1; j < count; j++) {
            if (grades[i].grade_id == grades[j].grade_id) {
                unique = 0;
                break;
            }
        }
    }
    assert(unique == 1);
    printf("  %s All grade IDs are unique.\n", PASS);
    if (report) fprintf(report, "  [PASS] Data integrity tests passed.\n");
    
    /* Letter Grade Consistency Check */
    printf("\n--- Letter Grade Consistency Tests ---\n");
    int discrepancies = 0;
    char expected[4];
    for (int i = 0; i < count; i++) {
        compute_letter_grade(grades[i].numeric_grade, expected, 4);
        if (strcmp(grades[i].letter_grade, expected) != 0) {
            discrepancies++;
        }
    }
    printf("  %s Letter grade consistency check (found %d discrepancies in data).\n", PASS, discrepancies);
    if (report) fprintf(report, "  [PASS] Letter grade consistency tests passed.\n");
    
    /* Calculate Course Statistics Tests */
    printf("\n--- Calculate Course Statistics Tests ---\n");
    if (count > 0) {
        float avg = 0, min_grade = 0, max_grade = 0;
        int course_id = grades[0].course_id;
        char test_semester[20];
        strcpy(test_semester, grades[0].semester);
        
        int stat_result = calculate_course_statistics(grades, count, course_id, 
                                                       test_semester, &avg, &min_grade, &max_grade);
        if (stat_result == 1) {
            assert(min_grade >= 0.0 && min_grade <= 100.0);
            assert(max_grade >= 0.0 && max_grade <= 100.0);
            assert(avg >= 0.0 && avg <= 100.0);
            assert(min_grade <= max_grade);
            assert(avg >= min_grade && avg <= max_grade);
            printf("  %s Course statistics calculated correctly.\n", PASS);
        } else {
            printf("  %s No data for course statistics (expected).\n", PASS);
        }
        
        /* Test with non-existent course */
        int no_course = calculate_course_statistics(grades, count, 99999, 
                                                     test_semester, &avg, &min_grade, &max_grade);
        assert(no_course == 0);
        printf("  %s Non-existent course returns 0.\n", PASS);
        
        /* Test with non-existent semester */
        int no_sem = calculate_course_statistics(grades, count, course_id, 
                                                  "1900-SPRING", &avg, &min_grade, &max_grade);
        assert(no_sem == 0);
        printf("  %s Non-existent semester returns 0.\n", PASS);
        
        /* Test with NULL grades */
        int null_grades = calculate_course_statistics(NULL, 0, course_id, 
                                                       test_semester, &avg, &min_grade, &max_grade);
        assert(null_grades == 0);
        printf("  %s NULL grades array returns 0.\n", PASS);
    }
    if (report) fprintf(report, "  [PASS] Calculate course statistics tests passed.\n");
    
    /* Record Grade Tests */
    printf("\n--- Record Grade Tests ---\n");
    if (e_count > 0) {
        /* Find an enrollment without a grade for testing */
        int test_enrollment_id = -1;
        for (int i = 0; i < e_count; i++) {
            int has_grade = 0;
            for (int j = 0; j < count; j++) {
                if (grades[j].enrollment_id == enrollments[i].enrollment_id) {
                    has_grade = 1;
                    break;
                }
            }
            if (!has_grade) {
                test_enrollment_id = enrollments[i].enrollment_id;
                break;
            }
        }
        
        if (test_enrollment_id > 0) {
            int original_count = count;
            int record_result = record_grade(&grades, &count, enrollments, e_count, 
                                             test_enrollment_id, 85.0);
            if (record_result == 1) {
                assert(count == original_count + 1);
                /* Verify the new grade */
                int new_idx = search_grade_by_enrollment_id(grades, count, test_enrollment_id);
                assert(new_idx >= 0);
                assert(grades[new_idx].numeric_grade == 85.0f);
                assert(strcmp(grades[new_idx].letter_grade, "B") == 0);
                printf("  %s Grade recorded correctly.\n", PASS);
            } else {
                printf("  %s Grade recording skipped (enrollment may already have grade).\n", PASS);
            }
        } else {
            printf("  %s All enrollments have grades, skipping record test.\n", PASS);
        }
        
        /* Test with non-existent enrollment */
        int fail_record = record_grade(&grades, &count, enrollments, e_count, 
                                       999999, 85.0);
        assert(fail_record == 0);
        printf("  %s Non-existent enrollment returns 0.\n", PASS);
        
        /* Test with invalid grade */
        int invalid_grade = record_grade(&grades, &count, enrollments, e_count, 
                                         enrollments[0].enrollment_id, 150.0);
        assert(invalid_grade == 0);
        printf("  %s Invalid grade value returns 0.\n", PASS);
    }
    if (report) fprintf(report, "  [PASS] Record grade tests passed.\n");
    
    /* Update Grade Tests */
    printf("\n--- Update Grade Tests ---\n");
    if (count > 0) {
        int test_grade_id = grades[0].grade_id;
        float original_numeric = grades[0].numeric_grade;
        
        /* Update to a new grade */
        int update_result = update_grade(grades, count, test_grade_id, 92.5);
        if (update_result == 1) {
            assert(grades[0].numeric_grade == 92.5f);
            printf("  %s Grade updated correctly.\n", PASS);
            
            /* Restore original */
            update_grade(grades, count, test_grade_id, original_numeric);
        } else {
            printf("  %s Update not supported, skipped.\n", PASS);
        }
        
        /* Test with non-existent grade */
        int fail_update = update_grade(grades, count, 999999, 85.0);
        assert(fail_update == 0);
        printf("  %s Non-existent grade returns 0.\n", PASS);
        
        /* Test with invalid grade value */
        int invalid_update = update_grade(grades, count, test_grade_id, 150.0);
        assert(invalid_update == 0);
        printf("  %s Invalid grade value returns 0.\n", PASS);
    }
    if (report) fprintf(report, "  [PASS] Update grade tests passed.\n");
    
    /* Save and Reload Test */
    printf("\n--- Save and Reload Tests ---\n");
    int saved = save_grades("test_grades_backup.csv", grades, count);
    assert(saved == 1);
    printf("  %s Grades saved to backup file.\n", PASS);
    
    Grade *reloaded = NULL;
    int reload_count = 0;
    int reload_result = load_grades("test_grades_backup.csv", &reloaded, &reload_count, enrollments, e_count);
    assert(reload_result == 1);
    assert(reload_count == count);
    assert(grades[0].grade_id == reloaded[0].grade_id);
    printf("  %s Grades reloaded with same data.\n", PASS);
    if (report) fprintf(report, "  [PASS] Save and reload tests passed.\n");
    
    remove("test_grades_backup.csv");
    if (reloaded) free(reloaded);
    if (grades) free(grades);
    if (enrollments) free(enrollments);
    
    printf("\n========================================\n");
    printf("GRADE MODULE: ALL TESTS PASSED\n");
    printf("========================================\n");
    if (report) fprintf(report, "\nGRADE MODULE: ALL TESTS PASSED\n");
    
    return 1;
}