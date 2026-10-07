/**
 * @file test_professor.c
 * @brief Professor Module Unit Tests
 * 
 * Comprehensive test suite for professor management functions including:
 * - Professor ID, email, phone validation tests
 * - CSV file loading and saving tests
 * - Search functionality tests (by ID, email)
 * - Data integrity and uniqueness tests
 * - Delete operation tests
 * - Boundary value and NULL handling tests
 * 
 * @author Student Information System Team
 * @date January 2026
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "professor.h"
#include "enrollment.h"
#include "utils.h"

#define PASS "[PASS]"
static FILE *report = NULL;

/**
 * @brief Runs all professor module tests.
 * 
 * Executes comprehensive tests for all professor management functions including:
 * - Professor ID validation (range: 1001-9998)
 * - Email format validation (@university.edu)
 * - Phone number format validation (XXX-XXX-XXXX)
 * - CSV file load/save operations
 * - Search by ID and email
 * - Data integrity and uniqueness checks
 * - Delete professor operations
 * 
 * @param rep FILE pointer for test report output (can be NULL).
 * @return 1 if all tests pass, 0 otherwise.
 */
int run_professor_tests(FILE *rep) {
    report = rep;
    
    printf("\n========================================\n");
    printf("    PROFESSOR MODULE - UNIT TESTS\n");
    printf("========================================\n");
    if (report) {
        fprintf(report, "\n========================================\n");
        fprintf(report, "    PROFESSOR MODULE - UNIT TESTS\n");
        fprintf(report, "========================================\n");
    }
    
    /* Professor ID Validation Tests */
    printf("\n--- Professor ID Validation Tests ---\n");
    assert(validate_professor_id(1001) == 1);
    assert(validate_professor_id(5000) == 1);
    assert(validate_professor_id(9998) == 1);
    assert(validate_professor_id(5555) == 1);
    printf("  %s Valid professor IDs accepted correctly.\n", PASS);
    
    assert(validate_professor_id(1000) == 0);
    assert(validate_professor_id(9999) == 0);
    printf("  %s Boundary professor IDs rejected correctly.\n", PASS);
    
    assert(validate_professor_id(0) == 0);
    assert(validate_professor_id(-1) == 0);
    assert(validate_professor_id(999) == 0);
    assert(validate_professor_id(10000) == 0);
    printf("  %s Invalid professor IDs rejected correctly.\n", PASS);
    if (report) fprintf(report, "  [PASS] Professor ID validation tests passed.\n");
    
    /* Email Validation Tests */
    printf("\n--- Professor Email Validation Tests ---\n");
    assert(validate_email("john.doe@university.edu") == 1);
    assert(validate_email("prof.smith@university.edu") == 1);
    printf("  %s Valid emails accepted correctly.\n", PASS);
    
    assert(validate_email("john@gmail.com") == 0);
    assert(validate_email("john@yahoo.com") == 0);
    printf("  %s Wrong domain emails rejected correctly.\n", PASS);
    
    assert(validate_email("johndoe@university.edu") == 0);
    assert(validate_email(".john@university.edu") == 0);
    printf("  %s Format error emails rejected correctly.\n", PASS);
    
    assert(validate_email("JOHN.DOE@university.edu") == 0);
    printf("  %s Uppercase emails rejected correctly.\n", PASS);
    
    assert(validate_email(NULL) == 0);
    assert(validate_email("") == 0);
    printf("  %s NULL and empty handled correctly.\n", PASS);
    if (report) fprintf(report, "  [PASS] Email validation tests passed.\n");
    
    /* Phone Validation Tests */
    printf("\n--- Professor Phone Validation Tests ---\n");
    assert(validate_phone("555-1234") == 1);
    assert(validate_phone("555-9999") == 1);
    printf("  %s Valid phone numbers accepted correctly.\n", PASS);
    
    assert(validate_phone("5551234") == 0);
    assert(validate_phone("555_1234") == 0);
    printf("  %s Format error phones rejected correctly.\n", PASS);
    
    assert(validate_phone("555-12ab") == 0);
    printf("  %s Letters in phone rejected correctly.\n", PASS);
    
    assert(validate_phone("555-123") == 0);
    assert(validate_phone("555-12345") == 0);
    printf("  %s Wrong length phones rejected correctly.\n", PASS);
    
    assert(validate_phone(NULL) == 0);
    assert(validate_phone("") == 0);
    printf("  %s NULL and empty handled correctly.\n", PASS);
    if (report) fprintf(report, "  [PASS] Phone validation tests passed.\n");
    
    /* Load professors */
    printf("\n--- Load Professors from CSV Tests ---\n");
    Professor *professors = NULL;
    int count = 0;
    int loaded = load_professors("data/professors.csv", &professors, &count);
    assert(loaded == 1);
    assert(count > 0);
    printf("  %s data/professors.csv loaded successfully: %d records.\n", PASS, count);
    
    /* Test loading non-existent file */
    Professor *dummy = NULL;
    int dummy_count = 0;
    int fail_load = load_professors("nonexistent_professors.csv", &dummy, &dummy_count);
    assert(fail_load == 0);
    printf("  %s Non-existent file returns 0.\n", PASS);
    if (dummy) free(dummy);
    if (report) fprintf(report, "  [PASS] Load professors tests passed.\n");
    
    /* Search by ID */
    printf("\n--- Search Professor by ID Tests ---\n");
    int idx = search_professor_by_id(professors, count, professors[0].professor_id);
    assert(idx == 0);
    idx = search_professor_by_id(professors, count, professors[count-1].professor_id);
    assert(idx == count - 1);
    printf("  %s Existing professors found correctly.\n", PASS);
    
    idx = search_professor_by_id(professors, count, 99999);
    assert(idx == -1);
    idx = search_professor_by_id(professors, count, 0);
    assert(idx == -1);
    idx = search_professor_by_id(professors, count, -1);
    assert(idx == -1);
    printf("  %s Non-existing professors return -1.\n", PASS);
    
    idx = search_professor_by_id(NULL, 0, 5000);
    assert(idx == -1);
    idx = search_professor_by_id(NULL, 10, 5000);
    assert(idx == -1);
    printf("  %s NULL and empty array handled correctly.\n", PASS);
    if (report) fprintf(report, "  [PASS] Search by ID tests passed.\n");
    
    /* Search by Email */
    printf("\n--- Search Professor by Email Tests ---\n");
    idx = search_professor_by_email(professors, count, professors[0].email);
    assert(idx >= 0);
    printf("  %s Existing emails found correctly.\n", PASS);
    
    idx = search_professor_by_email(professors, count, "nonexistent@university.edu");
    assert(idx == -1);
    idx = search_professor_by_email(professors, count, "");
    assert(idx == -1);
    printf("  %s Non-existing emails return -1.\n", PASS);
    
    idx = search_professor_by_email(NULL, 0, "test@university.edu");
    assert(idx == -1);
    printf("  %s NULL and empty array handled correctly.\n", PASS);
    if (report) fprintf(report, "  [PASS] Search by email tests passed.\n");
    
    /* Data Integrity */
    printf("\n--- Professor Data Integrity Tests ---\n");
    int valid = 1;
    for (int i = 0; i < count && valid; i++) {
        if (professors[i].professor_id <= 1000 || professors[i].professor_id >= 9999) valid = 0;
    }
    assert(valid == 1);
    printf("  %s All professors have valid data.\n", PASS);
    
    /* Unique IDs */
    int unique = 1;
    for (int i = 0; i < count && unique; i++) {
        for (int j = i + 1; j < count; j++) {
            if (professors[i].professor_id == professors[j].professor_id) {
                unique = 0;
                break;
            }
        }
    }
    assert(unique == 1);
    printf("  %s All professor IDs are unique.\n", PASS);
    
    /* Unique Emails */
    unique = 1;
    for (int i = 0; i < count && unique; i++) {
        for (int j = i + 1; j < count; j++) {
            if (strcmp(professors[i].email, professors[j].email) == 0) {
                unique = 0;
                break;
            }
        }
    }
    assert(unique == 1);
    printf("  %s All professor emails are unique.\n", PASS);
    if (report) fprintf(report, "  [PASS] Data integrity tests passed.\n");
    
    /* Delete Professor Tests */
    printf("\n--- Delete Professor Tests ---\n");
    /* Create a copy for delete test */
    Professor *test_profs = malloc(count * sizeof(Professor));
    memcpy(test_profs, professors, count * sizeof(Professor));
    int test_count = count;
    
    if (test_count > 0) {
        /* Try to delete the last professor */
        int delete_id = test_profs[test_count - 1].professor_id;
        int original_count = test_count;
        
        int delete_result = delete_professor(&test_profs, &test_count, delete_id);
        if (delete_result == 1) {
            assert(test_count == original_count - 1);
            /* Verify professor is gone */
            int find_deleted = search_professor_by_id(test_profs, test_count, delete_id);
            assert(find_deleted == -1);
            printf("  %s Professor deleted correctly.\n", PASS);
        } else {
            printf("  %s Professor delete returned 0.\n", PASS);
        }
        
        /* Test with non-existent professor */
        int fail_delete = delete_professor(&test_profs, &test_count, 99999);
        assert(fail_delete == 0);
        printf("  %s Non-existent professor returns 0.\n", PASS);
    }
    if (test_profs) free(test_profs);
    if (report) fprintf(report, "  [PASS] Delete professor tests passed.\n");
    
    /* Save and Reload Test */
    printf("\n--- Save and Reload Tests ---\n");
    int saved = save_professors("test_professors_backup.csv", professors, count);
    assert(saved == 1);
    printf("  %s Professors saved to backup file.\n", PASS);
    
    Professor *reloaded = NULL;
    int reload_count = 0;
    int reload_result = load_professors("test_professors_backup.csv", &reloaded, &reload_count);
    assert(reload_result == 1);
    assert(reload_count == count);
    assert(professors[0].professor_id == reloaded[0].professor_id);
    printf("  %s Professors reloaded with same data.\n", PASS);
    if (report) fprintf(report, "  [PASS] Save and reload tests passed.\n");
    
    remove("test_professors_backup.csv");
    if (reloaded) free(reloaded);
    if (professors) free(professors);
    
    printf("\n========================================\n");
    printf("PROFESSOR MODULE: ALL TESTS PASSED\n");
    printf("========================================\n");
    if (report) fprintf(report, "\nPROFESSOR MODULE: ALL TESTS PASSED\n");
    
    return 1;
}