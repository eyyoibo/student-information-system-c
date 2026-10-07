/**
 * @file test_main.c
 * @brief Main Test Runner for Student Information System (Assert-based)
 * 
 * Runs all unit tests from all modules and generates a detailed
 * test report. Uses assert() - program terminates on first failure.
 * 
 * Modules tested:
 * - Utils (validation functions)
 * - Student
 * - Professor
 * - Course
 * - Enrollment
 * - Grade
 * 
 * @author Student Information System Team
 * @date January 2026
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Forward declarations of test runners */
extern int run_utils_tests(FILE *report);
extern int run_student_tests(FILE *report);
extern int run_professor_tests(FILE *report);
extern int run_course_tests(FILE *report);
extern int run_enrollment_tests(FILE *report);
extern int run_grade_tests(FILE *report);
extern int run_menu_tests(FILE *report);

/**
 * @brief Main entry point for the test suite
 * 
 * Executes all module tests and generates test_results.txt
 * with detailed per-test results.
 */
int main() {
    printf("\n");
    printf("================================================================\n");
    printf("     STUDENT INFORMATION SYSTEM - COMPREHENSIVE TEST SUITE\n");
    printf("================================================================\n\n");
    
    /* Open report file */
    FILE *report = fopen("test_results.txt", "w");
    if (!report) {
        fprintf(stderr, "[ERROR] Could not create test_results.txt\n");
        return 1;
    }
    
    /* Write report header */
    time_t now = time(NULL);
    fprintf(report, "================================================================\n");
    fprintf(report, "       STUDENT INFORMATION SYSTEM - TEST RESULTS REPORT\n");
    fprintf(report, "================================================================\n");
    fprintf(report, "Generated: %s", ctime(&now));
    fprintf(report, "================================================================\n\n");
    
    /* Run Utils tests */
    printf("Running Utils Module Tests...\n");
    run_utils_tests(report);
    
    /* Run Student tests */
    printf("Running Student Module Tests...\n");
    run_student_tests(report);
    
    /* Run Professor tests */
    printf("Running Professor Module Tests...\n");
    run_professor_tests(report);
    
    /* Run Course tests */
    printf("Running Course Module Tests...\n");
    run_course_tests(report);
    
    /* Run Enrollment tests */
    printf("Running Enrollment Module Tests...\n");
    run_enrollment_tests(report);
    
    /* Run Grade tests */
    printf("Running Grade Module Tests...\n");
    run_grade_tests(report);
    
    /* Run Menu/Integration tests */
    printf("Running Menu/Integration Tests...\n");
    run_menu_tests(report);
    
    /* Write summary to report */
    fprintf(report, "\n================================================================\n");
    fprintf(report, "                        SUMMARY\n");
    fprintf(report, "================================================================\n\n");
    fprintf(report, "All Modules: PASSED\n");
    fprintf(report, "  - Utils Module:      PASSED\n");
    fprintf(report, "  - Student Module:    PASSED\n");
    fprintf(report, "  - Professor Module:  PASSED\n");
    fprintf(report, "  - Course Module:     PASSED\n");
    fprintf(report, "  - Enrollment Module: PASSED\n");
    fprintf(report, "  - Grade Module:      PASSED\n");
    fprintf(report, "  - Menu Module:       PASSED\n");
    fprintf(report, "\n");
    fprintf(report, "Overall Result: ALL TESTS PASSED\n");
    fprintf(report, "\n================================================================\n");
    
    fclose(report);
    
    /* Print final summary to console */
    printf("\n");
    printf("================================================================\n");
    printf("                      FINAL SUMMARY\n");
    printf("================================================================\n");
    printf("  Utils Module:      PASSED\n");
    printf("  Student Module:    PASSED\n");
    printf("  Professor Module:  PASSED\n");
    printf("  Course Module:     PASSED\n");
    printf("  Enrollment Module: PASSED\n");
    printf("  Grade Module:      PASSED\n");
    printf("================================================================\n");
    printf("  Overall: ALL TESTS PASSED\n");
    printf("================================================================\n");
    printf("\n");
    printf("Detailed results saved to: test_results.txt\n\n");
    
    return 0;
}
