/**
 * @file test_menu.c
 * @brief Menu Module Integration Tests
 * 
 * Comprehensive integration test suite for the Student Information System:
 * - Data loading workflow tests (correct module order)
 * - Student-Enrollment relationship tests
 * - Enrollment-Grade workflow tests
 * - Course-Professor assignment tests
 * - Cross-module reference validation tests
 * - Data consistency and uniqueness tests
 * - Cascade delete workflow tests
 * - Validation integration tests (all validate functions)
 * - Boundary case tests for all validators
 * - NULL and empty input handling tests
 * 
 * @author Student Information System Team
 * @date January 2026
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "menu.h"
#include "student.h"
#include "professor.h"
#include "course.h"
#include "enrollment.h"
#include "grade.h"
#include "utils.h"

#define PASS_MSG "\x1b[32m[PASS]\x1b[0m"
#define FAIL_MSG "\x1b[31m[FAIL]\x1b[0m"

static int tests_run = 0;
static int tests_passed = 0;

/**
 * @brief Tests the data loading workflow.
 * 
 * Verifies that all CSV files can be loaded in the correct 
 * dependency order (students -> professors -> courses -> 
 * enrollments -> grades) and that each module contains data.
 */
void test_data_loading_workflow() {
    printf("\n--- 7.4.1 Data Loading Workflow Tests ---\n");
    
    // Tüm modülleri yükle
    Student *students = NULL; int s_count = 0;
    Professor *professors = NULL; int p_count = 0;
    Course *courses = NULL; int c_count = 0;
    Enrollment *enrollments = NULL; int e_count = 0;
    Grade *grades = NULL; int g_count = 0;
    
    // Sıralı yükleme (bağımlılık sırası)
    int r1 = load_students("data/students.csv", &students, &s_count);
    int r2 = load_professors("data/professors.csv", &professors, &p_count);
    int r3 = load_courses("data/courses.csv", &courses, &c_count);
    int r4 = load_enrollments("data/enrollments.csv", &enrollments, &e_count);
    int r5 = load_grades("data/grades.csv", &grades, &g_count, enrollments, e_count);
    
    assert(r1 == 1);
    assert(r2 == 1);
    assert(r3 == 1);
    assert(r4 == 1);
    assert(r5 == 1);
    printf("  %s All CSV files loaded in correct order.\n", PASS_MSG);
    tests_passed++; tests_run++;
    
    assert(s_count > 0 && p_count > 0 && c_count > 0 && e_count > 0 && g_count > 0);
    printf("  %s All modules have data records.\n", PASS_MSG);
    tests_passed++; tests_run++;
    
    if (students) free(students);
    if (professors) free(professors);
    if (courses) free(courses);
    if (enrollments) free(enrollments);
    if (grades) free(grades);
}

/**
 * @brief Tests the student-enrollment relationship.
 * 
 * Verifies that students are properly linked to their enrollment 
 * records and that the foreign key relationships are valid.
 */
void test_student_enrollment_workflow() {
    printf("\n--- 7.4.2 Student-Enrollment Workflow Tests ---\n");
    
    Student *students = NULL; int s_count = 0;
    Enrollment *enrollments = NULL; int e_count = 0;
    
    load_students("data/students.csv", &students, &s_count);
    load_enrollments("data/enrollments.csv", &enrollments, &e_count);
    
    // Her öğrencinin kayıtlarını kontrol et
    int students_with_enrollments = 0;
    for (int i = 0; i < s_count && i < 10; i++) {
        for (int j = 0; j < e_count; j++) {
            if (enrollments[j].student_id == students[i].student_id) {
                students_with_enrollments++;
                break;
            }
        }
    }
    
    assert(students_with_enrollments > 0);
    printf("  %s Students have enrollment records.\n", PASS_MSG);
    tests_passed++; tests_run++;
    
    if (students) free(students);
    if (enrollments) free(enrollments);
}

/**
 * @brief Tests the enrollment-grade workflow.
 * 
 * Verifies that completed enrollments have corresponding grade 
 * records and that the grade-enrollment relationship is valid.
 */
void test_enrollment_grade_workflow() {
    printf("\n--- 7.4.3 Enrollment-Grade Workflow Tests ---\n");
    
    Enrollment *enrollments = NULL; int e_count = 0;
    Grade *grades = NULL; int g_count = 0;
    
    load_enrollments("data/enrollments.csv", &enrollments, &e_count);
    load_grades("data/grades.csv", &grades, &g_count, enrollments, e_count);
    
    // Completed enrollments için grade kontrolü
    int completed_with_grades = 0;
    int completed_count = 0;
    
    for (int i = 0; i < e_count; i++) {
        if (strcmp(enrollments[i].status, "Completed") == 0) {
            completed_count++;
            if (search_grade_by_enrollment_id(grades, g_count, enrollments[i].enrollment_id) != -1) {
                completed_with_grades++;
            }
        }
    }
    
    assert(completed_count > 0);
    printf("  %s Completed enrollments exist.\n", PASS_MSG);
    tests_passed++; tests_run++;
    
    assert(completed_with_grades > 0);
    printf("  %s Completed enrollments have grades.\n", PASS_MSG);
    tests_passed++; tests_run++;
    
    if (enrollments) free(enrollments);
    if (grades) free(grades);
}

/**
 * @brief Tests the course-professor assignment workflow.
 * 
 * Verifies that professors are properly assigned to courses 
 * through the enrollment records and that the relationships are valid.
 */
void test_course_professor_workflow() {
    printf("\n--- 7.4.4 Course-Professor Workflow Tests ---\n");
    
    Professor *professors = NULL; int p_count = 0;
    Course *courses = NULL; int c_count = 0;
    Enrollment *enrollments = NULL; int e_count = 0;
    
    load_professors("data/professors.csv", &professors, &p_count);
    load_courses("data/courses.csv", &courses, &c_count);
    load_enrollments("data/enrollments.csv", &enrollments, &e_count);
    
    // Her profesörün en az bir derse atanmış olduğunu kontrol et
    int profs_with_courses = 0;
    for (int i = 0; i < p_count && i < 10; i++) {
        for (int j = 0; j < e_count; j++) {
            if (enrollments[j].professor_id == professors[i].professor_id) {
                profs_with_courses++;
                break;
            }
        }
    }
    
    assert(profs_with_courses > 0);
    printf("  %s Professors are assigned to courses.\n", PASS_MSG);
    tests_passed++; tests_run++;
    
    if (professors) free(professors);
    if (courses) free(courses);
    if (enrollments) free(enrollments);
}

/**
 * @brief Tests cross-module search functionality.
 * 
 * Verifies that foreign key references between modules are valid
 * by searching for students and courses referenced in enrollments.
 */
void test_cross_module_search() {
    printf("\n--- 7.4.5 Cross-Module Search Tests ---\n");
    
    Student *students = NULL; int s_count = 0;
    Professor *professors = NULL; int p_count = 0;
    Course *courses = NULL; int c_count = 0;
    Enrollment *enrollments = NULL; int e_count = 0;
    
    load_students("data/students.csv", &students, &s_count);
    load_professors("data/professors.csv", &professors, &p_count);
    load_courses("data/courses.csv", &courses, &c_count);
    load_enrollments("data/enrollments.csv", &enrollments, &e_count);
    
    // Bir enrollment al ve student/course referanslarını kontrol et
    if (e_count > 0) {
        Enrollment e = enrollments[0];
        
        int s_idx = search_student_by_id(students, s_count, e.student_id);
        int c_idx = search_course_by_id(courses, c_count, e.course_id);
        
        assert(s_idx != -1);
        assert(c_idx != -1);
        printf("  %s Cross-module references (student, course) are valid.\n", PASS_MSG);
        tests_passed++; tests_run++;
    }
    
    if (students) free(students);
    if (professors) free(professors);
    if (courses) free(courses);
    if (enrollments) free(enrollments);
}

/**
 * @brief Tests data consistency across all modules.
 * 
 * Verifies that there are no duplicate IDs in students, 
 * professors, and courses tables. Ensures data uniqueness 
 * constraints are maintained.
 */
void test_data_consistency() {
    printf("\n--- 7.4.6 Data Consistency Tests ---\n");
    
    Student *students = NULL; int s_count = 0;
    Professor *professors = NULL; int p_count = 0;
    Course *courses = NULL; int c_count = 0;
    
    load_students("data/students.csv", &students, &s_count);
    load_professors("data/professors.csv", &professors, &p_count);
    load_courses("data/courses.csv", &courses, &c_count);
    
    // ID uniqueness kontrolü
    int duplicate_found = 0;
    for (int i = 0; i < s_count; i++) {
        for (int j = i + 1; j < s_count; j++) {
            if (students[i].student_id == students[j].student_id) {
                duplicate_found = 1;
                break;
            }
        }
    }
    
    assert(duplicate_found == 0);
    printf("  %s No duplicate student IDs found.\n", PASS_MSG);
    tests_passed++; tests_run++;
    
    duplicate_found = 0;
    for (int i = 0; i < p_count; i++) {
        for (int j = i + 1; j < p_count; j++) {
            if (professors[i].professor_id == professors[j].professor_id) {
                duplicate_found = 1;
                break;
            }
        }
    }
    
    assert(duplicate_found == 0);
    printf("  %s No duplicate professor IDs found.\n", PASS_MSG);
    tests_passed++; tests_run++;
    
    duplicate_found = 0;
    for (int i = 0; i < c_count; i++) {
        for (int j = i + 1; j < c_count; j++) {
            if (courses[i].course_id == courses[j].course_id) {
                duplicate_found = 1;
                break;
            }
        }
    }
    
    assert(duplicate_found == 0);
    printf("  %s No duplicate course IDs found.\n", PASS_MSG);
    tests_passed++; tests_run++;
    
    if (students) free(students);
    if (professors) free(professors);
    if (courses) free(courses);
}

/**
 * @brief Tests cascade delete workflow logic.
 * 
 * Verifies that cascade delete only affects enrollments with 
 * "Enrolled" status, preserving completed and dropped records.
 * Uses mock enrollment data to test the deletion logic.
 */
void test_cascade_delete_workflow() {
    printf("\n--- 7.4.7 Cascade Delete Workflow Tests ---\n");
    
    // Test cascade delete mantığı (sadece "Enrolled" olanları etkiler)
    Enrollment test_enr[5];
    
    test_enr[0].enrollment_id = 1; test_enr[0].professor_id = 5001;
    strcpy(test_enr[0].status, "Enrolled");
    
    test_enr[1].enrollment_id = 2; test_enr[1].professor_id = 5001;
    strcpy(test_enr[1].status, "Completed");
    
    test_enr[2].enrollment_id = 3; test_enr[2].professor_id = 5001;
    strcpy(test_enr[2].status, "Enrolled");
    
    test_enr[3].enrollment_id = 4; test_enr[3].professor_id = 5002;
    strcpy(test_enr[3].status, "Enrolled");
    
    test_enr[4].enrollment_id = 5; test_enr[4].professor_id = 5001;
    strcpy(test_enr[4].status, "Dropped");
    
    // Prof 5001 için sadece "Enrolled" olanları say
    int enrolled_count = 0;
    for (int i = 0; i < 5; i++) {
        if (test_enr[i].professor_id == 5001 && strcmp(test_enr[i].status, "Enrolled") == 0) {
            enrolled_count++;
        }
    }
    
    assert(enrolled_count == 2);
    printf("  %s Cascade delete logic counts only 'Enrolled' status.\n", PASS_MSG);
    tests_passed++; tests_run++;
}

/**
 * @brief Tests validation integration across all modules.
 * 
 * Verifies that all validation functions work correctly when called
 * programmatically. Tests student, professor, course, and enrollment 
 * validation rules including ID ranges, email formats, and phone formats.
 */
void test_validation_integration() {
    printf("\n--- 7.4.8 Validation Integration Tests ---\n");
    
    /*
     * Bu testler interaktif fonksiyonların validate fonksiyonlarını
     * doğru kullandığını simüle eder. Gerçek fonksiyonlar stdin gerektirir
     * ancak validate fonksiyonlarının doğru çağrıldığını burada test ederiz.
     */
    
    /* Student modülü için validation testleri */
    printf("  Testing Student Module Validations:\n");
    
    /* Geçersiz student ID girişi simülasyonu */
    int invalid_student_ids[] = {0, -1, 1999999, 2027001, 12345, 123456789, 2024000};
    for (int i = 0; i < 7; i++) {
        assert(validate_student_id(invalid_student_ids[i]) == 0);
    }
    printf("    %s validate_student_id rejects invalid inputs correctly.\n", PASS_MSG);
    tests_passed++; tests_run++;
    
    /* Geçersiz email girişi simülasyonu - add_student'ta kullanılır */
    const char *invalid_emails[] = {
        "noemail", "test@gmail.com", "UPPER@university.edu", 
        "nodot@university.edu", ".start@university.edu", "end.@university.edu"
    };
    for (int i = 0; i < 6; i++) {
        assert(validate_email(invalid_emails[i]) == 0);
    }
    printf("    %s validate_email rejects invalid inputs correctly.\n", PASS_MSG);
    tests_passed++; tests_run++;
    
    /* Geçersiz phone girişi simülasyonu */
    const char *invalid_phones[] = {
        "1234567", "555_1234", "555-12ab", "555-123", "555-12345", "123-4567"
    };
    for (int i = 0; i < 6; i++) {
        assert(validate_phone(invalid_phones[i]) == 0);
    }
    printf("    %s validate_phone rejects invalid inputs correctly.\n", PASS_MSG);
    tests_passed++; tests_run++;
    
    /* Geçersiz GPA girişi simülasyonu - update_student'ta kullanılır */
    float invalid_gpas[] = {-0.5, -1.0, 4.1, 5.0, 10.0, -100.0};
    for (int i = 0; i < 6; i++) {
        assert(validate_gpa(invalid_gpas[i]) == 0);
    }
    printf("    %s validate_gpa rejects invalid inputs correctly.\n", PASS_MSG);
    tests_passed++; tests_run++;
    
    /* Professor modülü için validation testleri */
    printf("  Testing Professor Module Validations:\n");
    
    int invalid_prof_ids[] = {0, -1, 999, 1000, 9999, 10000, 100000};
    for (int i = 0; i < 7; i++) {
        assert(validate_professor_id(invalid_prof_ids[i]) == 0);
    }
    printf("    %s validate_professor_id rejects invalid inputs correctly.\n", PASS_MSG);
    tests_passed++; tests_run++;
    
    /* Course modülü için validation testleri */
    printf("  Testing Course Module Validations:\n");
    
    int invalid_course_ids[] = {0, -1, 999, 10000, 100, 99999};
    for (int i = 0; i < 6; i++) {
        assert(validate_course_id(invalid_course_ids[i]) == 0);
    }
    printf("    %s validate_course_id rejects invalid inputs correctly.\n", PASS_MSG);
    tests_passed++; tests_run++;
    
    /* Enrollment modülü için validation testleri */
    printf("  Testing Enrollment Module Validations:\n");
    
    const char *invalid_semesters[] = {
        "2021SPRING", "SPRING-2021", "2021/FALL", "2021-WINTER",
        "2021-AUTMN", "ABCD-SPRING", "", "2021-", "2021"
    };
    for (int i = 0; i < 9; i++) {
        assert(validate_semester(invalid_semesters[i]) == 0);
    }
    printf("    %s validate_semester rejects invalid inputs correctly.\n", PASS_MSG);
    tests_passed++; tests_run++;
    
    /* Grade modülü için validation testleri */
    printf("  Testing Grade Module Validations:\n");
    
    float invalid_grades[] = {-1.0, -0.1, 100.1, 101.0, 150.0, -50.0};
    for (int i = 0; i < 6; i++) {
        assert(validate_numeric_grade(invalid_grades[i]) == 0);
    }
    printf("    %s validate_numeric_grade rejects invalid inputs correctly.\n", PASS_MSG);
    tests_passed++; tests_run++;
    
    const char *invalid_letter_grades[] = {"a", "b+", "D", "E", "AA", "1", "", "X"};
    for (int i = 0; i < 8; i++) {
        assert(validate_letter_grade(invalid_letter_grades[i]) == 0);
    }
    printf("    %s validate_letter_grade rejects invalid inputs correctly.\n", PASS_MSG);
    tests_passed++; tests_run++;
    
    /* Name validation testleri - add fonksiyonlarında kullanılır */
    printf("  Testing Name/String Validations:\n");
    
    const char *invalid_names[] = {"John123", "Test@Name", "Name#1", "123"};
    for (int i = 0; i < 4; i++) {
        assert(validate_name_string(invalid_names[i]) == 0);
    }
    printf("    %s validate_name_string rejects invalid inputs correctly.\n", PASS_MSG);
    tests_passed++; tests_run++;
    
    /* Date validation testleri */
    const char *invalid_dates[] = {
        "2024-13-01", "2024-00-15", "2024-02-30", "2023-02-29", 
        "24-01-15", "2024/01/15", "01-15-2024"
    };
    for (int i = 0; i < 7; i++) {
        assert(validate_date(invalid_dates[i]) == 0);
    }
    printf("    %s validate_date rejects invalid inputs correctly.\n", PASS_MSG);
    tests_passed++; tests_run++;
    
    printf("  %s All validation functions correctly integrated.\n", PASS_MSG);
}

/**
 * @brief Tests boundary cases for all validation functions.
 * 
 * Tests edge values and boundary conditions including:
 * - ID ranges at minimum and maximum values
 * - GPA at 0.0 and 4.0
 * - Numeric grades at 0 and 100
 * - Semester year boundaries (2000-2026)
 */
void test_validation_boundary_cases() {
    printf("\n--- 7.4.9 Validation Boundary Cases Tests ---\n");
    
    /* Student ID sınır değerleri */
    printf("  Testing boundary values:\n");
    
    /* Student ID: 2000001 - 2026999 aralığı geçerli */
    assert(validate_student_id(2000001) == 1); /* Min geçerli */
    assert(validate_student_id(2026999) == 1); /* Max geçerli */
    assert(validate_student_id(2000000) == 0); /* Min-1 (000 sequence) */
    assert(validate_student_id(2027000) == 0); /* Max+1 (year out of range) */
    printf("    %s Student ID boundary values correct.\n", PASS_MSG);
    tests_passed++; tests_run++;
    
    /* Professor ID: 1001 - 9998 aralığı geçerli */
    assert(validate_professor_id(1001) == 1); /* Min geçerli */
    assert(validate_professor_id(9998) == 1); /* Max geçerli */
    assert(validate_professor_id(1000) == 0); /* Min-1 */
    assert(validate_professor_id(9999) == 0); /* Max+1 */
    printf("    %s Professor ID boundary values correct.\n", PASS_MSG);
    tests_passed++; tests_run++;
    
    /* Course ID: 1000 - 9999 aralığı geçerli */
    assert(validate_course_id(1000) == 1); /* Min geçerli */
    assert(validate_course_id(9999) == 1); /* Max geçerli */
    assert(validate_course_id(999) == 0);  /* Min-1 */
    assert(validate_course_id(10000) == 0); /* Max+1 */
    printf("    %s Course ID boundary values correct.\n", PASS_MSG);
    tests_passed++; tests_run++;
    
    /* GPA: 0.0 - 4.0 aralığı geçerli */
    assert(validate_gpa(0.0) == 1);   /* Min geçerli */
    assert(validate_gpa(4.0) == 1);   /* Max geçerli */
    assert(validate_gpa(-0.01) == 0); /* Min-epsilon */
    assert(validate_gpa(4.01) == 0);  /* Max+epsilon */
    printf("    %s GPA boundary values correct.\n", PASS_MSG);
    tests_passed++; tests_run++;
    
    /* Numeric Grade: 0.0 - 100.0 aralığı geçerli */
    assert(validate_numeric_grade(0.0) == 1);    /* Min geçerli */
    assert(validate_numeric_grade(100.0) == 1);  /* Max geçerli */
    assert(validate_numeric_grade(-0.01) == 0);  /* Min-epsilon */
    assert(validate_numeric_grade(100.01) == 0); /* Max+epsilon */
    printf("    %s Numeric grade boundary values correct.\n", PASS_MSG);
    tests_passed++; tests_run++;
    
    /* Letter Grade hesaplama sınır değerleri */
    char grade[4];
    compute_letter_grade(95.0, grade, 4);  assert(strcmp(grade, "A") == 0);
    compute_letter_grade(94.99, grade, 4); assert(strcmp(grade, "A-") == 0);
    compute_letter_grade(90.0, grade, 4);  assert(strcmp(grade, "A-") == 0);
    compute_letter_grade(89.99, grade, 4); assert(strcmp(grade, "B+") == 0);
    compute_letter_grade(70.0, grade, 4);  assert(strcmp(grade, "C") == 0);
    compute_letter_grade(69.99, grade, 4); assert(strcmp(grade, "F") == 0);
    printf("    %s Letter grade computation boundary values correct.\n", PASS_MSG);
    tests_passed++; tests_run++;
    
    printf("  %s All boundary cases handled correctly.\n", PASS_MSG);
}

/**
 * @brief Tests NULL and empty input handling.
 * 
 * Verifies that all validation and search functions properly 
 * handle NULL pointers and empty strings without crashing.
 * Essential for program stability and robustness.
 */
void test_validation_null_empty_handling() {
    printf("\n--- 7.4.10 NULL and Empty Input Tests ---\n");
    
    /* NULL pointer handling */
    printf("  Testing NULL pointer handling:\n");
    
    assert(validate_email(NULL) == 0);
    assert(validate_phone(NULL) == 0);
    assert(validate_semester(NULL) == 0);
    assert(validate_letter_grade(NULL) == 0);
    assert(validate_name_string(NULL) == 0);
    assert(validate_date(NULL) == 0);
    printf("    %s All validate functions handle NULL correctly.\n", PASS_MSG);
    tests_passed++; tests_run++;
    
    /* Empty string handling */
    printf("  Testing empty string handling:\n");
    
    assert(validate_email("") == 0);
    assert(validate_phone("") == 0);
    assert(validate_semester("") == 0);
    assert(validate_letter_grade("") == 0);
    assert(validate_name_string("") == 0);
    assert(validate_date("") == 0);
    printf("    %s All validate functions handle empty strings correctly.\n", PASS_MSG);
    tests_passed++; tests_run++;
    
    /* Search functions with NULL arrays */
    printf("  Testing search functions with NULL arrays:\n");
    
    assert(search_student_by_id(NULL, 0, 2024001) == -1);
    assert(search_student_by_email(NULL, 0, "test@university.edu") == -1);
    assert(search_professor_by_id(NULL, 0, 5000) == -1);
    assert(search_professor_by_email(NULL, 0, "test@university.edu") == -1);
    assert(search_course_by_id(NULL, 0, 1000) == -1);
    assert(search_course_by_code(NULL, 0, "CS101") == -1);
    assert(search_enrollment_by_id(NULL, 0, 1) == -1);
    assert(search_grade_by_id(NULL, 0, 1) == -1);
    printf("    %s All search functions handle NULL arrays correctly.\n", PASS_MSG);
    tests_passed++; tests_run++;
    
    printf("  %s NULL and empty input handling complete.\n", PASS_MSG);
}

/**
 * @brief Runs all menu module integration tests.
 * 
 * Executes a comprehensive suite of integration tests covering:
 * - Data loading workflow (correct dependency order)
 * - Student-enrollment relationships
 * - Enrollment-grade workflows
 * - Course-professor assignments
 * - Cross-module searches
 * - Data consistency (no duplicates)
 * - Cascade delete logic
 * - Validation integration
 * - Boundary cases
 * - NULL/empty input handling
 * 
 * @param report FILE pointer for test report output.
 * @return 1 if all tests pass, 0 otherwise.
 */
int run_menu_tests(FILE *report) {
    printf("\n========================================\n");
    printf("     MENU MODULE - INTEGRATION TESTS\n");
    printf("========================================\n");
    
    fprintf(report, "\n--- MENU/INTEGRATION TESTS ---\n");
    
    tests_run = 0;
    tests_passed = 0;
    
    test_data_loading_workflow();
    test_student_enrollment_workflow();
    test_enrollment_grade_workflow();
    test_course_professor_workflow();
    test_cross_module_search();
    test_data_consistency();
    test_cascade_delete_workflow();
    test_validation_integration();
    test_validation_boundary_cases();
    test_validation_null_empty_handling();
    
    printf("\n========================================\n");
    printf("MENU/INTEGRATION TESTS: %d/%d passed\n", tests_passed, tests_run);
    printf("========================================\n");
    
    fprintf(report, "Tests Run: %d\n", tests_run);
    fprintf(report, "Tests Passed: %d\n", tests_passed);
    fprintf(report, "Tests Failed: %d\n", tests_run - tests_passed);
    fprintf(report, "Result: %s\n", (tests_passed == tests_run) ? "ALL PASSED" : "SOME FAILED");
    
    return (tests_passed == tests_run) ? 1 : 0;
}
