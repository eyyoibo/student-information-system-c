/**
 * @file menu.c
 * @brief Menu implementation for the Student Information System
 * 
 * This file implements all menu-related functions for navigating the
 * Student Information System. It provides a text-based user interface
 * with submenus for each domain (students, courses, professors, etc.).
 * 
 * Features:
 * - Clear screen between menu transitions for better UX
 * - Input validation on all user inputs
 * - Consistent menu structure across all submenus
 * - Press-enter-to-continue for result viewing
 * 
 * @author Student
 * @date 2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* Include all module headers */
#include "menu.h"
#include "utils.h"
#include "student.h"
#include "course.h"
#include "professor.h"
#include "curriculum.h"
#include "enrollment.h"
#include "grade.h"

/**
 * @brief Pauses execution until user presses ENTER
 * 
 * Waits for user to press ENTER to continue.
 * Used after displaying results to allow reading before screen clear.
 * Note: Do not call clear_input_buffer() before this as it would
 * consume the newline character and require a second Enter press.
 */
void press_enter_to_continue() {
    printf("\nPress ENTER to continue...");
    int c;
    /* Wait for Enter key (newline) */
    while ((c = getchar()) != '\n' && c != EOF);
}

/**
 * @brief Displays the main menu
 * 
 * Shows the main navigation menu with all available system options.
 */
void print_main_menu() {
    printf("\n========================================\n");
    printf("       STUDENT INFORMATION SYSTEM       \n");
    printf("========================================\n");
    printf("1. Student Management\n");
    printf("2. Course Management\n");
    printf("3. Professor Management\n");
    printf("4. Enrollment Management\n");
    printf("5. Grade Management\n");
    printf("6. Reports\n");
    printf("7. System Options\n");
    printf("8. Curriculum Management\n");
    printf("0. Exit\n");
    printf("========================================\n");
    printf("Your Choice: ");
}

/**
 * @brief Student management submenu
 * 
 * Handles all student-related operations including add, edit, delete,
 * search, display details/transcript, and GPA calculation.
 */
void menu_student_management(Student **students, int *s_count, Grade *grades, int g_count, Course *courses, int c_count, Enrollment *enrollments, int e_count) {
    int choice;
    do {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
        printf("\n--- 1. STUDENT MANAGEMENT ---\n");
        printf("1. Add new student\n");
        printf("2. Edit student information\n");
        printf("3. Delete student\n");
        printf("4. Search for student\n");
        printf("5. Display student details and transcript\n");
        printf("6. Calculate and display student GPA\n");
        printf("7. List all students\n");
        printf("0. Back to Main Menu\n");
        printf("-----------------------------\n");
        printf("Choice: ");

        if (scanf("%d", &choice) != 1) { clear_input_buffer(); continue; }
        clear_input_buffer();

        int id, index;
        char search_buffer[100];

        switch (choice) {
            case 1:
                if (add_student(students, s_count)) printf(">> Student added successfully.\n");
                else printf(">> Error: Failed to add student.\n");
                break;
            case 2:
                printf("Enter Student ID to update: "); 
                if (scanf("%d", &id) != 1) { clear_input_buffer(); print_error("Invalid input."); break; }
                clear_input_buffer();
                if (!validate_student_id(id)) { break; }
                update_student(*students, *s_count, id);
                break;
            case 3: /* Delete Student */
                printf("Enter Student ID to delete: "); 
                if (scanf("%d", &id) != 1) { clear_input_buffer(); print_error("Invalid input."); break; }
                clear_input_buffer();
                if (!validate_student_id(id)) { break; }
                char confirm;
                printf("WARNING: This will delete the student AND all their enrollments/grades!\n");
                printf("Are you sure? (y/n): "); scanf("%c", &confirm); clear_input_buffer();
                
                if (confirm == 'y' || confirm == 'Y') {
                    /* Cascade delete: removes student and all related enrollments/grades */
                    if (delete_student(students, s_count, id, enrollments, &e_count, grades, &g_count)) {
                        printf(">> Student and all related data deleted successfully.\n");
                    } else {
                        printf(">> Error: Student ID not found.\n");
                    }
                }
                break;
            case 4:
                printf("1. ID\n2. Name\n3. Email\nChoice: ");
                int type; scanf("%d", &type); clear_input_buffer();
                index = -1;

                if (type == 1) {
                    /* ID is unique, returns single result */
                    printf("ID: "); scanf("%d", &id); clear_input_buffer();
                    index = search_student_by_id(*students, *s_count, id);
                    if (index != -1) print_student((*students)[index]);
                    else printf(">> Record not found.\n");

                } else if (type == 2) {
                    /* NAME SEARCH - returns multiple matches */
                    get_string_input("Name: ", search_buffer, 100);
                    int found = search_and_print_students_by_name(*students, *s_count, search_buffer);
                    
                    if (found == 0) {
                        printf(">> No students found with the name '%s'.\n", search_buffer);
                    } else {
                        printf(">> Total matches found: %d\n", found);
                    }

                } else if (type == 3) {
                    /* Email is unique, returns single result */
                    get_string_input("Email: ", search_buffer, 100);
                    index = search_student_by_email(*students, *s_count, search_buffer);
                    if (index != -1) print_student((*students)[index]);
                    else printf(">> Record not found.\n");
                }
                break;
            case 5:
                printf("Student ID: "); scanf("%d", &id); clear_input_buffer();
                index = search_student_by_id(*students, *s_count, id);
                if (index != -1) {
                    print_student((*students)[index]);
                    print_student_transcript(id, grades, g_count, courses, c_count);
                } else printf(">> Student not found.\n");
                break;
            case 6:
                printf("Student ID: "); scanf("%d", &id); clear_input_buffer();
                calculate_and_display_gpa(id, *students, *s_count, grades, g_count, courses, c_count);
                break;
            case 7:
                list_all_students(*students, *s_count);
                break;
            case 0: break;
            default: printf("Invalid choice.\n");
        }
        if (choice != 0) press_enter_to_continue();
    } while (choice != 0);
}

/**
 * @brief Course management submenu
 * 
 * Handles all course-related operations including add, edit, delete,
 * search, display details, set capacity, and assign prerequisites.
 */
void menu_course_management(Course **courses, int *c_count, Enrollment *enrollments, int *e_count, Grade *grades, int *g_count) {
    int choice;
    do {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
        printf("\n--- 2. COURSE MANAGEMENT ---\n");
        printf("1. Add new course\n");
        printf("2. Edit course information\n");
        printf("3. Delete course\n");
        printf("4. Search for courses\n");
        printf("5. Display course details\n");
        printf("6. Set course capacity\n");
        printf("7. Assign prerequisites\n");
        printf("8. List all courses\n");
        printf("0. Back to Main Menu\n");
        printf("----------------------------\n");
        printf("Choice: ");

        if (scanf("%d", &choice) != 1) { clear_input_buffer(); continue; }
        clear_input_buffer();

        int id; /* Used in case 2 and 3 */

        switch (choice) {
            case 1:
                if (add_course(courses, c_count)) printf(">> Course added successfully.\n");
                else printf(">> Error: Failed to add course.\n");
                break;
            case 2:
                printf("Enter Course ID to update: "); 
                if (scanf("%d", &id) != 1) { clear_input_buffer(); print_error("Invalid input."); break; }
                clear_input_buffer();
                if (!validate_course_id(id)) { break; }
                update_course(*courses, *c_count, id);
                break;
            case 3: /* Delete Course */
                printf("Enter Course ID to delete: ");
                if (scanf("%d", &id) != 1) {
                     clear_input_buffer(); 
                     print_error("Invalid input."); 
                     break; 
                }
                clear_input_buffer();
                
                /* Cascade delete: removes course and all related enrollments/grades */
                delete_course(*courses, c_count, id, &enrollments, e_count, &grades, g_count);
                break;
            case 4: /* Search Courses */
              {
                    int search_choice;
                    printf("\n--- Search Course ---\n");
                    printf("1. Search by ID\n");
                    printf("2. Search by Code\n");
                    printf("3. List by Department\n");
                    printf("Choice: ");
                    
                    if (scanf("%d", &search_choice) != 1) {
                        clear_input_buffer();
                        print_error("Invalid input.");
                        break;
                    }
                    clear_input_buffer();

                    /* SEARCH BY ID */
                    if (search_choice == 1) {
                        int id;
                        printf("Enter Course ID: ");
                        if (scanf("%d", &id) != 1) {
                            clear_input_buffer();
                            print_error("Invalid input (Numeric ID required).");
                        } else {
                            clear_input_buffer();
                            int idx = search_course_by_id(*courses, *c_count, id);
                            
                            if (idx != -1) {
                                printf("\n%-10s %-10s %-30s %-15s %-8s %-10s\n", "ID", "Code", "Name", "Department", "Credits", "Capacity");
                                printf("--------------------------------------------------------------------------------------------------------\n");
                                Course *c = &(*courses)[idx];
                                printf("%-10d %-10s %-30s %-15s %-8d %-10d\n", 
                                    c->course_id, c->course_code, c->course_name, c->department, c->credits, c->capacity);
                                printf("--------------------------------------------------------------------------------------------------------\n");
                            } else {
                                printf(">> Error: Course with ID %d not found.\n", id);
                            }
                        }
                    }
                    /* SEARCH BY CODE */
                    else if (search_choice == 2) {
                        char code[MAX_CODE_LEN];
                        get_string_input("Enter Course Code: ", code, MAX_CODE_LEN);
                        trim_whitespace(code);
                        
                        int idx = search_course_by_code(*courses, *c_count, code);
                        if (idx != -1) {
                            printf("\n%-10s %-10s %-30s %-15s %-8s %-10s\n", "ID", "Code", "Name", "Department", "Credits", "Capacity");
                            printf("--------------------------------------------------------------------------------------------------------\n");
                            Course *c = &(*courses)[idx];
                            printf("%-10d %-10s %-30s %-15s %-8d %-10d\n", 
                                c->course_id, c->course_code, c->course_name, c->department, c->credits, c->capacity);
                            printf("--------------------------------------------------------------------------------------------------------\n");
                        } else {
                             printf(">> Error: Course with Code '%s' not found.\n", code);
                        }
                    }
                    /* LIST BY DEPARTMENT */
                    else if (search_choice == 3) {
                        char dept[MAX_DEPT_LEN];
                        get_string_input("Enter Department Name: ", dept, MAX_DEPT_LEN);
                        trim_whitespace(dept);
                        print_courses_by_department(*courses, *c_count, dept);
                    }
                    else {
                        print_error("Invalid search choice.");
                    }
                }
                break;
            case 5: /* Display Course Details */
                {
                    int search_type;
                    int target_index = -1;

                    printf("\n--- Display Course Details ---\n");
                    printf("1. Search by Course Code\n");
                    printf("2. Search by Course ID\n");
                    printf("Choice: ");
                    
                    if (scanf("%d", &search_type) != 1) {
                        clear_input_buffer();
                        print_error("Invalid input.");
                        break;
                    }
                    clear_input_buffer();

                    /* Search Logic */
                    if (search_type == 1) {
                        char code[MAX_CODE_LEN];
                        get_string_input("Enter Course Code: ", code, MAX_CODE_LEN);
                        trim_whitespace(code);
                        target_index = search_course_by_code(*courses, *c_count, code);
                        if (target_index == -1) printf(">> Error: Course '%s' not found.\n", code);
                    } 
                    else if (search_type == 2) {
                        int id;
                        printf("Enter Course ID: ");
                        if (scanf("%d", &id) != 1) {
                            clear_input_buffer();
                            print_error("Invalid ID format.");
                        } else {
                            clear_input_buffer();
                            target_index = search_course_by_id(*courses, *c_count, id);
                            if (target_index == -1) printf(">> Error: Course ID %d not found.\n", id);
                        }
                    } 
                    else {
                        print_error("Invalid search choice.");
                    }

                    /* Print Details (including Prerequisites) */
                    if (target_index != -1) {
                        Course *c = &(*courses)[target_index];
                        printf("\n========================================\n");
                        printf(" COURSE DETAILS\n");
                        printf("========================================\n");
                        printf("ID            : %d\n", c->course_id);
                        printf("Code          : %s\n", c->course_code);
                        printf("Name          : %s\n", c->course_name);
                        printf("Department    : %s\n", c->department);
                        printf("Credits       : %d\n", c->credits);
                        printf("Capacity      : %d\n", c->capacity);
                        printf("Prerequisites : %s\n", c->prerequisites);
                        printf("========================================\n");
                    }
                }
                break;
            case 6:
                printf("Course ID: "); 
                if (scanf("%d", &id) != 1) { clear_input_buffer(); print_error("Invalid input."); break; }
                clear_input_buffer();
                if (!validate_course_id(id)) { break; }
                set_course_capacity(*courses, *c_count, id, enrollments, *e_count);
                break;
            case 7: /* Assign Prerequisites */
                {
                    char target_code[MAX_CODE_LEN];
                    printf("\n--- Assign Prerequisites ---\n");                   
                    /* Request code instead of ID */
                    get_string_input("Enter Course Code to update: ", target_code, MAX_CODE_LEN);
                    trim_whitespace(target_code);

                    int idx = search_course_by_code(*courses, *c_count, target_code);
                    if (idx == -1) {
                        printf(">> Error: Course '%s' not found.\n", target_code);
                    } else {
                        /* Get course ID and call function */
                        int found_id = (*courses)[idx].course_id;
                        assign_prerequisites(*courses, *c_count, found_id);
                    }
                }
                break;
            case 8:
                list_all_courses(*courses, *c_count);
                break;
            case 0: break;
            default: printf("Invalid choice.\n");
        }
        if (choice != 0) press_enter_to_continue();
    } while (choice != 0);
}

/**
 * @brief Professor management submenu
 * 
 * Handles all professor-related operations including add, edit, delete,
 * search, display details, assign courses, and list all professors.
 */
void menu_professor_management(Professor **professors, int *p_count, Course *courses, int c_count) {
    int choice;
    do {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
        printf("\n--- 3. PROFESSOR MANAGEMENT ---\n");
        printf("1. Add new professor\n");
        printf("2. Edit professor information\n");
        printf("3. Delete professor\n");
        printf("4. Search for professors\n");
        printf("5. Display professor details\n");
        printf("6. Assign courses to professors\n");
        printf("7. List ALL Professors\n");          
        printf("0. Back to Main Menu\n");
        printf("-------------------------------\n");
        printf("Choice: ");

        if (scanf("%d", &choice) != 1) { clear_input_buffer(); continue; }
        clear_input_buffer();

        int id, index, c_id, c_index;
        char search_buffer[100];

        switch (choice) {
            case 1: /* Add */
                if (add_professor(professors, p_count)) printf(">> Professor added successfully.\n");
                else printf(">> Error: Failed to add professor.\n");
                break;
            case 2: /* Edit */
                printf("Enter Professor ID to update: "); 
                if (scanf("%d", &id) != 1) { clear_input_buffer(); print_error("Invalid input."); break; }
                clear_input_buffer();
                if (!validate_professor_id(id)) { break; }
                update_professor(*professors, *p_count, id);
                break;
            case 3: /* Delete */
                printf("Enter Professor ID to delete: "); 
                if (scanf("%d", &id) != 1) { clear_input_buffer(); print_error("Invalid input."); break; }
                clear_input_buffer();
                if (!validate_professor_id(id)) { break; }
                if (delete_professor(professors, p_count, id)) printf(">> Professor deleted.\n");
                else printf(">> Error: ID not found.\n");
                break;
            case 4: /* Search */
                printf("1. ID\n2. Name\nChoice: ");
                int type; scanf("%d", &type); clear_input_buffer();
                index = -1;
                if (type == 1) {
                    printf("ID: "); scanf("%d", &id); clear_input_buffer();
                    index = search_professor_by_id(*professors, *p_count, id);
                } else if (type == 2) {
                    get_string_input("Name: ", search_buffer, 100);
                    for(int i=0; i<*p_count; i++) {
                        if(str_equals_case_insensitive((*professors)[i].first_name, search_buffer)) { 
                            index=i; break; 
                        }
                    }
                }
                
                if (index != -1) {
                    printf(">> Record found:\n");
                    print_professor(&(*professors)[index], 1);
                } else {
                    printf(">> Record not found.\n");
                }
                break;

            case 5: /* Display professor details */
                printf("Professor ID: "); 
                if (scanf("%d", &id) != 1) { clear_input_buffer(); print_error("Invalid input."); break; }
                clear_input_buffer();
                if (!validate_professor_id(id)) { break; }
                index = search_professor_by_id(*professors, *p_count, id);
                
                if (index != -1) {
                    /* Print professor information */
                    print_professor(&(*professors)[index], 1);
                    
                    /* List courses assigned to this professor */
                    printf("\n--- Assigned Courses for Prof. %s %s ---\n", 
                           (*professors)[index].first_name, (*professors)[index].last_name);
                    
                    int found_course = 0;
                    /* Scan all courses to find those assigned to this professor */
                    for(int i = 0; i < c_count; i++) {
                        if (courses[i].instructor_id == id) {
                            printf("* [%d] %s - %s\n", 
                                   courses[i].course_id, 
                                   courses[i].course_code, 
                                   courses[i].course_name);
                            found_course = 1;
                        }
                    }
                    
                    if (!found_course) {
                        printf(">> No courses assigned currently.\n");
                    }
                    printf("----------------------------------------\n");
                    
                } else printf(">> Record not found.\n");
                break;
            
            case 6: /* ASSIGN COURSES */
                printf("Enter Professor ID: "); 
                if (scanf("%d", &id) != 1) { clear_input_buffer(); print_error("Invalid input."); break; }
                clear_input_buffer();
                if (!validate_professor_id(id)) { break; }
                index = search_professor_by_id(*professors, *p_count, id);
                
                if (index == -1) {
                    printf(">> Error: Professor not found.\n");
                    break;
                }
                printf(">> Selected Professor: %s %s\n", (*professors)[index].first_name, (*professors)[index].last_name);

                printf("Enter Course ID to assign: "); scanf("%d", &c_id); clear_input_buffer();
                
                /* Find the course */
                c_index = -1;
                for(int i=0; i<c_count; i++) {
                    if (courses[i].course_id == c_id) { c_index = i; break; }
                }

                if (c_index == -1) {
                    printf(">> Error: Course not found.\n");
                } else {
                    /* ASSIGNMENT OPERATION (IN MEMORY) */
                    courses[c_index].instructor_id = id; 
                    
                    printf(">> ASSIGNMENT SUCCESSFUL (In Memory):\n");
                    printf("   Course: %s (%s)\n", courses[c_index].course_name, courses[c_index].course_code);
                    printf("   Now assigned to: Prof. %s %s\n", (*professors)[index].first_name, (*professors)[index].last_name);
                }
                break;

            case 7: /* List All */
                print_professor(*professors, *p_count);
                break;

            case 0: break;
            default: printf("Invalid choice.\n");
        }
        if (choice != 0) press_enter_to_continue();
    } while (choice != 0);
}

/**
 * @brief Enrollment management submenu
 * 
 * Handles all enrollment-related operations including enrolling students,
 * dropping students, and displaying enrollment information.
 */
void menu_enrollment_management(Enrollment **enrollments, int *e_count, Student *students, int s_count, Course *courses, int c_count, Professor *professors, int p_count, Grade *grades, int g_count) {
    int choice;
    do {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
        printf("\n--- 4. ENROLLMENT MANAGEMENT ---\n");
        printf("1. Enroll student in course\n");
        printf("2. Drop student from course\n");
        printf("3. Display student's current enrollments\n");
        printf("4. Display course roster\n");
        printf("0. Back to Main Menu\n");
        printf("--------------------------------\n");
        printf("Choice: ");

        if (scanf("%d", &choice) != 1) { clear_input_buffer(); continue; }
        clear_input_buffer();

        int s_id, c_id;
        char semester[50];
        int s_idx, c_idx, p_idx;

        switch (choice) {
case 1: /* Enroll Student */
                /* 1. STUDENT SELECTION */
                printf("Student ID: "); scanf("%d", &s_id); clear_input_buffer();
                s_idx = search_student_by_id(students, s_count, s_id);
                if (s_idx == -1) { printf(">> Error: Student not found!\n"); break; }
                printf(">> Student: %s %s\n", students[s_idx].first_name, students[s_idx].last_name);

                /* 2. COURSE SELECTION */
                printf("Course ID: "); scanf("%d", &c_id); clear_input_buffer();
                c_idx = search_course_by_id(courses, c_count, c_id);
                if (c_idx == -1) { printf(">> Error: Course not found!\n"); break; }
                printf(">> Course: %s (%s)\n", courses[c_idx].course_name, courses[c_idx].course_code);

                /* 3. SEMESTER INPUT */
                do {
                    get_string_input("Semester (e.g. 2024-SPRING): ", semester, 50);
                    trim_whitespace(semester);
                    if (!validate_semester(semester)) {
                        print_error("Invalid semester format. Please use format: YYYY-SEASON (e.g., 2024-SPRING, 2024-FALL)");
                    }
                } while (!validate_semester(semester));

                /* 4. FIND ACTIVE SECTIONS (PROFESSORS) FOR THIS SEMESTER */
                int active_profs[10]; /* Max 10 different professors per semester */
                int active_count = 0;

                /* Scan enrollment history to find professors for this semester */
                for (int i = 0; i < *e_count; i++) {
                    if ((*enrollments)[i].course_id == c_id && 
                        str_equals_case_insensitive((*enrollments)[i].semester, semester)) {
                        
                        int p_current = (*enrollments)[i].professor_id;
                        int already_listed = 0;
                        
                        /* Check if already added to list */
                        for(int k=0; k<active_count; k++) {
                            if(active_profs[k] == p_current) already_listed = 1;
                        }

                        if (!already_listed && active_count < 10) {
                            active_profs[active_count++] = p_current;
                        }
                    }
                }

                int final_p_id = -1;

                /* CASE A: NO RECORDS (NEW SEMESTER) */
                if (active_count == 0) {
                    if (courses[c_idx].instructor_id != 0) {
                        final_p_id = courses[c_idx].instructor_id;
                        printf(">> [NEW SEMESTER] Assigning to default coordinator (ID: %d).\n", final_p_id);
                    } else {
                        printf(">> [NEW SEMESTER] No default instructor. Enter Professor ID manually: ");
                        scanf("%d", &final_p_id); clear_input_buffer();
                    }
                }
                /* CASE B: SINGLE PROFESSOR */
                else if (active_count == 1) {
                    final_p_id = active_profs[0];
                    int p_idx_temp = search_professor_by_id(professors, p_count, final_p_id);
                    printf(">> [FOUND EXISTING CLASS] Auto-assigning to Prof. %s %s.\n", 
                           professors[p_idx_temp].first_name, professors[p_idx_temp].last_name);
                }
                /* CASE C: MULTIPLE PROFESSORS (SHOW OPTIONS) */
                else {
                    printf("\n>> [MULTIPLE SECTIONS FOUND] Who is teaching this student?\n");
                    for (int i = 0; i < active_count; i++) {
                        int p_idx_temp = search_professor_by_id(professors, p_count, active_profs[i]);
                        if (p_idx_temp != -1) {
                            printf("%d. Prof. %s %s (ID: %d)\n", 
                                   i + 1, 
                                   professors[p_idx_temp].first_name, 
                                   professors[p_idx_temp].last_name, 
                                   professors[p_idx_temp].professor_id);
                        }
                    }
                    printf("0. Someone else (Open new section)\n");
                    printf("Choice: ");
                    
                    int selection;
                    scanf("%d", &selection); clear_input_buffer();

                    if (selection > 0 && selection <= active_count) {
                        final_p_id = active_profs[selection - 1];
                    } else if (selection == 0) {
                        printf("Enter New Professor ID: ");
                        scanf("%d", &final_p_id); clear_input_buffer();
                    } else {
                        printf(">> Invalid selection. Aborting.\n");
                        break;
                    }
                }

                /* Professor ID Validation */
                p_idx = search_professor_by_id(professors, p_count, final_p_id);
                if (p_idx == -1) {
                    printf(">> Error: Professor ID %d not found in database.\n", final_p_id);
                    break;
                }

                /* Department Check (Optional but recommended) */
                if (strcmp(courses[c_idx].department, professors[p_idx].department) != 0) {
                     printf("\n>> WARNING: Department Mismatch! (Course: %s, Prof: %s)\n", 
                            courses[c_idx].department, professors[p_idx].department);
                     printf(">> Proceed anyway? (y/n): ");
                     char confirm; scanf("%c", &confirm); clear_input_buffer();
                     if (confirm != 'y' && confirm != 'Y') {
                         printf(">> Enrollment cancelled.\n");
                         break;
                     }
                }

                /* 5. COMPLETE THE ENROLLMENT */
                if (enroll_student_in_course(enrollments, e_count, 
                                         students, s_count, 
                                         courses, c_count, 
                                         grades, g_count,
                                         s_id, c_id, final_p_id, semester)) {
                    /* Success */
                } else {
                    printf(">> Enrollment failed.\n");
                }
                break;

            case 2: /* Drop Student */
                printf("Student ID: "); scanf("%d", &s_id); clear_input_buffer();
                printf("Course ID: "); scanf("%d", &c_id); clear_input_buffer();
                
                /* SEMESTER VALIDATION */
                do {
                    get_string_input("Semester (e.g. 2021-SPRING): ", semester, 50);
                    trim_whitespace(semester);
                } while (!validate_semester(semester));

                if (drop_student_from_course(enrollments, e_count, students, s_count, courses, c_count, s_id, c_id, semester)) {
                    printf(">> Student dropped from course.\n");
                } else {
                    printf(">> Error: Enrollment record not found.\n");
                }
                break;

            case 3: /* Display Enrollments */
                printf("Student ID: "); scanf("%d", &s_id); clear_input_buffer();
                print_student_enrollments(*enrollments, *e_count, courses, c_count, s_id);
                break;

            case 4: /* Course Roster */
                printf("Course ID: "); scanf("%d", &c_id); clear_input_buffer();
                if (search_course_by_id(courses, c_count, c_id) == -1) {
                    printf(">> Error: Course not found.\n");
                } else {
                    /* SEMESTER VALIDATION */
                    do {
                        get_string_input("Semester (e.g. 2021-SPRING): ", semester, 50);
                        trim_whitespace(semester);
                    } while (!validate_semester(semester));
                    
                    print_course_roster(*enrollments, *e_count, students, s_count, c_id, semester);
                }
                break;

            case 0: break;
            default: printf("Invalid choice.\n");
        }
        if (choice != 0) press_enter_to_continue();
    } while (choice != 0);
}

/**
 * @brief Grade management submenu
 * 
 * Handles all grade-related operations including recording grades,
 * updating grades, calculating statistics, and displaying distributions.
 */
void menu_grade_management(Grade **grades, int *g_count, Enrollment *enrollments, int e_count, Course *courses, int c_count, Student *students, int s_count) {
    int choice;
    do {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
        printf("\n--- 5. GRADE MANAGEMENT ---\n");
        printf("1. Record grade for a student\n");
        printf("2. Update an existing grade\n");
        printf("3. Calculate course statistics (Avg, Min, Max)\n");
        printf("4. Display grade distribution\n");
        printf("0. Back to Main Menu\n");
        printf("---------------------------\n");
        printf("Choice: ");

        if (scanf("%d", &choice) != 1) { clear_input_buffer(); continue; }
        clear_input_buffer();

        int s_id, c_id, g_id;
        char semester[50];
        float numeric;

        switch (choice) {
            case 1: /* Record */
                /* STUDENT ID VALIDATION */
                printf("Student ID: ");
                if (scanf("%d", &s_id) != 1) {
                    clear_input_buffer();
                    print_error("Invalid input. Please enter a numeric student ID.");
                    break;
                }
                clear_input_buffer();
                
                if (!validate_student_id(s_id)) {
                    print_error("Invalid student ID format.");
                    break;
                }
                
                int s_idx = search_student_by_id(students, s_count, s_id);
                if (s_idx == -1) {
                    print_error("Student not found.");
                    break;
                }
                
                /* SHOW STUDENT'S ENROLLMENTS AND GRADES TABLE */
                printf("\n--- Student: %s %s (ID: %d) ---\n", students[s_idx].first_name, students[s_idx].last_name, s_id);
                printf("\n--- Current Enrollments ---\n");
                printf("%-10s %-10s %-30s %-15s %-15s %-10s\n", "Enroll ID", "Course ID", "Course Name", "Semester", "Status", "Grade");
                printf("--------------------------------------------------------------------------------------------------------\n");
                
                int enrollment_options[100];
                int option_count = 0;
                
                for (int i = 0; i < e_count; i++) {
                    if (enrollments[i].student_id == s_id) {
                        int c_idx = search_course_by_id(courses, c_count, enrollments[i].course_id);
                        char course_name[50] = "Unknown";
                        if (c_idx != -1) {
                            strncpy(course_name, courses[c_idx].course_name, 49);
                            course_name[49] = '\0';
                        }
                        
                        /* Check for existing grade */
                        int grade_idx = -1;
                        for (int j = 0; j < *g_count; j++) {
                            if ((*grades)[j].enrollment_id == enrollments[i].enrollment_id) {
                                grade_idx = j;
                                break;
                            }
                        }
                        
                        char grade_str[20] = "No Grade";
                        if (grade_idx != -1) {
                            sprintf(grade_str, "%.2f (%s)", (*grades)[grade_idx].numeric_grade, (*grades)[grade_idx].letter_grade);
                        }
                        
                        printf("%-10d %-10d %-30s %-15s %-15s %-10s\n",
                               enrollments[i].enrollment_id,
                               enrollments[i].course_id,
                               course_name,
                               enrollments[i].semester,
                               enrollments[i].status,
                               grade_str);
                        
                        enrollment_options[option_count++] = enrollments[i].enrollment_id;
                    }
                }
                
                if (option_count == 0) {
                    printf("No enrollments found for this student.\n");
                    break;
                }
                
                printf("--------------------------------------------------------------------------------------------------------\n");
                printf("\n--- Select Enrollment to Record/Update Grade ---\n");
                printf("Enter Enrollment ID (or 0 to enter new grade manually): ");
                int selected_enrollment_id;
                if (scanf("%d", &selected_enrollment_id) != 1) {
                    clear_input_buffer();
                    print_error("Invalid input.");
                    break;
                }
                clear_input_buffer();
                
                int enrollment_id = -1;
                
                if (selected_enrollment_id == 0) {
                    /* Manual entry */
                    printf("Course ID: ");
                    if (scanf("%d", &c_id) != 1) {
                        clear_input_buffer();
                        print_error("Invalid input.");
                        break;
                    }
                    clear_input_buffer();
                    
                    do {
                        get_string_input("Semester (e.g. 2024-SPRING): ", semester, 50);
                        trim_whitespace(semester);
                        if (!validate_semester(semester)) {
                            print_error("Invalid semester format. Please use format: YYYY-SEASON (e.g., 2024-SPRING, 2024-FALL)");
                        }
                    } while (!validate_semester(semester));
                    
                    for(int i = 0; i < e_count; i++) {
                        if(enrollments[i].student_id == s_id && 
                           enrollments[i].course_id == c_id && 
                           str_equals_case_insensitive(enrollments[i].semester, semester)) {
                            enrollment_id = enrollments[i].enrollment_id;
                            break;
                        }
                    }
                    
                    if (enrollment_id == -1) {
                        print_error("Student is not enrolled in this course for this semester.");
                        break;
                    }
                } else {
                    /* Validate selected enrollment ID */
                    int found = 0;
                    for (int i = 0; i < option_count; i++) {
                        if (enrollment_options[i] == selected_enrollment_id) {
                            enrollment_id = selected_enrollment_id;
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        print_error("Invalid enrollment ID selected.");
                        break;
                    }
                }
                
                /* Grade input (for both cases) */
                printf("Numeric Grade (0-100): ");
                if (scanf("%f", &numeric) != 1) {
                    clear_input_buffer();
                    print_error("Invalid input. Please enter a numeric value.");
                    break;
                }
                clear_input_buffer();
                
                if (!validate_numeric_grade(numeric)) {
                    print_error("Invalid grade. Grade must be between 0.0 and 100.0");
                    break;
                }
                
                if (record_grade(grades, g_count, enrollments, e_count, enrollment_id, numeric)) {
                    printf(">> Grade recorded successfully.\n");
                } else {
                    print_error("Failed to record grade.");
                }
                break;

            case 2: /* Update */
                printf("Enter Grade ID to update: "); scanf("%d", &g_id); clear_input_buffer();
                printf("New Numeric Grade: "); scanf("%f", &numeric); clear_input_buffer();
                
                update_grade(*grades, *g_count, g_id, numeric);
                break;

            case 3: /* Statistics */
                printf("Course ID: ");
                if (scanf("%d", &c_id) != 1) {
                    clear_input_buffer();
                    print_error("Invalid input. Please enter a numeric course ID.");
                    break;
                }
                clear_input_buffer();
                
                if (!validate_course_id(c_id)) {
                    print_error("Invalid course ID format.");
                    break;
                }
                
                if (search_course_by_id(courses, c_count, c_id) == -1) {
                    print_error("Course not found.");
                    break;
                }
                
                do {
                    get_string_input("Semester (e.g. 2024-SPRING): ", semester, 50);
                    trim_whitespace(semester);
                    if (!validate_semester(semester)) {
                        print_error("Invalid semester format. Please use format: YYYY-SEASON (e.g., 2024-SPRING, 2024-FALL)");
                    }
                } while (!validate_semester(semester));

                float avg, min_g, max_g;
                if (calculate_course_statistics(*grades, *g_count, c_id, semester, &avg, &min_g, &max_g)) {
                    printf("\n--- Course Statistics ---\nAverage: %.2f | Min: %.2f | Max: %.2f\n", avg, min_g, max_g);
                } else {
                    print_error("No grade records found for this course in the specified semester.");
                }
                break;

            case 4: /* Distribution */
                printf("Course ID: ");
                if (scanf("%d", &c_id) != 1) {
                    clear_input_buffer();
                    print_error("Invalid input. Please enter a numeric course ID.");
                    break;
                }
                clear_input_buffer();
                
                if (!validate_course_id(c_id)) {
                    print_error("Invalid course ID format.");
                    break;
                }
                
                if (search_course_by_id(courses, c_count, c_id) == -1) {
                    print_error("Course not found.");
                    break;
                }
                
                do {
                    get_string_input("Semester (e.g. 2024-SPRING): ", semester, 50);
                    trim_whitespace(semester);
                    if (!validate_semester(semester)) {
                        print_error("Invalid semester format. Please use format: YYYY-SEASON (e.g., 2024-SPRING, 2024-FALL)");
                    }
                } while (!validate_semester(semester));

                display_grade_distribution(*grades, *g_count, c_id, semester);
                break;

            case 0: break;
            default: printf("Invalid choice.\n");
        }
        if (choice != 0) press_enter_to_continue();
    } while (choice != 0);
}

/**
 * @brief Reports submenu
 * 
 * Provides various report options including student transcripts,
 * course rosters, professor course loads, and grade statistics.
 */
void menu_reports(Student *students, int s_count, Course *courses, int c_count, Professor *professors, int p_count, Enrollment *enrollments, int e_count, Grade *grades, int g_count) {
    int choice;
    do {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
        printf("\n--- 6. REPORTS ---\n");
        printf("1. Student Transcript\n");
        printf("2. Course Roster (Class List)\n");
        printf("3. Professor Course Load\n");
        printf("4. Grade Statistics\n");
        printf("0. Back to Main Menu\n");
        printf("------------------\n");
        printf("Choice: ");

        if (scanf("%d", &choice) != 1) { clear_input_buffer(); continue; }
        clear_input_buffer();

        int id;
        char semester[50];

        switch (choice) {
            case 1: /* Student Transcript */
                printf("Enter Student ID: ");
                scanf("%d", &id); clear_input_buffer();
                if (search_student_by_id(students, s_count, id) == -1) {
                    printf(">> Error: Student not found.\n");
                } else {
                    print_student_transcript(id, grades, g_count, courses, c_count);
                }
                break;

            case 2: /* Course Roster */
                do {
                    get_string_input("Semester (e.g. 2024-SPRING): ", semester, 50);
                    trim_whitespace(semester);
                    if (!validate_semester(semester)) {
                        print_error("Invalid semester format. Please use format: YYYY-SEASON (e.g., 2024-SPRING, 2024-FALL)");
                    }
                } while (!validate_semester(semester));
                
                /* Find all courses in the specified semester */
                int course_ids[100];
                int course_count_found = 0;
                
                for (int i = 0; i < e_count; i++) {
                    if (str_equals_case_insensitive(enrollments[i].semester, semester)) {
                        /* Check if course ID is already in the list */
                        int already_added = 0;
                        for (int j = 0; j < course_count_found; j++) {
                            if (course_ids[j] == enrollments[i].course_id) {
                                already_added = 1;
                                break;
                            }
                        }
                        if (!already_added && course_count_found < 100) {
                            course_ids[course_count_found++] = enrollments[i].course_id;
                        }
                    }
                }
                
                if (course_count_found == 0) {
                    printf(">> No courses found for semester %s.\n", semester);
                } else {
                    printf("\n--- Courses in Semester %s ---\n", semester);
                    for (int i = 0; i < course_count_found; i++) {
                        int c_idx = search_course_by_id(courses, c_count, course_ids[i]);
                        if (c_idx != -1) {
                            printf("\n");
                            print_course_roster(enrollments, e_count, students, s_count, course_ids[i], semester);
                        }
                    }
                }
                break;
case 3: /* Professor Course Load Report */
                printf("Enter Professor ID: ");
                if (scanf("%d", &id) != 1) {
                    clear_input_buffer();
                    print_error("Invalid input. Please enter a numeric professor ID.");
                    break;
                }
                clear_input_buffer();
                
                /* Professor ID validation (must be positive) */
                if (id <= 0) {
                    print_error("Invalid professor ID. ID must be a positive number.");
                    break;
                }
                
                int p_idx = search_professor_by_id(professors, p_count, id);
                
                if (p_idx == -1) {
                    print_error("Professor not found.");
                    break;
                }
                
                /* Email validation */
                if (!validate_email(professors[p_idx].email)) {
                    print_error("Professor email is invalid.");
                    break;
                }
                
                /* Request semester (report shows load for that semester) */
                do {
                    get_string_input("Semester (e.g. 2024-SPRING): ", semester, 50);
                    trim_whitespace(semester);
                    if (!validate_semester(semester)) {
                        print_error("Invalid semester format. Please use format: YYYY-SEASON (e.g., 2024-SPRING, 2024-FALL)");
                    }
                } while (!validate_semester(semester));

                printf("\n==================================================================================\n");
                printf(" COURSE LOAD REPORT: Prof. %s %s\n", professors[p_idx].first_name, professors[p_idx].last_name);
                printf(" Semester: %s\n", semester);
                printf("==================================================================================\n");
                /* COLUMNS: ID | Code | Course Name | Capacity | Enrolled | Status */
                printf("%-5s %-12s %-30s %-10s %-10s %-10s\n", "ID", "Code", "Course Name", "Capacity", "Enrolled", "Status");
                printf("----------------------------------------------------------------------------------\n");

                int total_courses = 0;
                int total_students = 0;

                for (int i = 0; i < c_count; i++) {
                    int enrolled_count = 0;
                    int is_related = 0;

                    /* Rule 1: Is the professor the course coordinator? */
                    if (courses[i].instructor_id == id) {
                        is_related = 1;
                    }

                    /* Rule 2: Are there students taking this course from this professor in this semester? */
                    for (int j = 0; j < e_count; j++) {
                        if (enrollments[j].course_id == courses[i].course_id &&
                            enrollments[j].professor_id == id && 
                            str_equals_case_insensitive(enrollments[j].semester, semester)) {
                            
                            if (str_equals_case_insensitive(enrollments[j].status, "Enrolled")) {
                                enrolled_count++;
                            }
                            is_related = 1;
                        }
                    }

                    if (is_related) {
                        int cap = courses[i].capacity; 
                        
                        /* Visual capacity status */
                        char status[20];
                        if (cap > 0 && enrolled_count >= cap) sprintf(status, "[FULL]");
                        else sprintf(status, "Open");

                        printf("%-5d %-12s %-30s %-10d %-10d %-10s\n", 
                               courses[i].course_id, 
                               courses[i].course_code, 
                               courses[i].course_name, 
                               cap,
                               enrolled_count,
                               status);
                        
                        total_courses++;
                        total_students += enrolled_count;
                    }
                }

                if (total_courses == 0) {
                    printf("\n>> No courses found for this professor in %s.\n", semester);
                } else {
                    printf("----------------------------------------------------------------------------------\n");
                    printf("SUMMARY: %d Courses | Total Students Handled: %d\n", total_courses, total_students);
                    printf("==================================================================================\n");
                }
                break;

            case 4: /* Grade Statistics */
                printf("Enter Course ID: ");
                if (scanf("%d", &id) != 1) {
                    clear_input_buffer();
                    print_error("Invalid input. Please enter a numeric course ID.");
                    break;
                }
                clear_input_buffer();
                
                if (!validate_course_id(id)) {
                    print_error("Invalid course ID format.");
                    break;
                }
                
                int c_idx = search_course_by_id(courses, c_count, id);
                if (c_idx == -1) {
                    print_error("Course not found.");
                    break;
                }
                
                do {
                    get_string_input("Semester (e.g. 2024-SPRING): ", semester, 50);
                    trim_whitespace(semester);
                    if (!validate_semester(semester)) {
                        print_error("Invalid semester format. Please use format: YYYY-SEASON (e.g., 2024-SPRING, 2024-FALL)");
                    }
                } while (!validate_semester(semester));
                
                /* Check for enrollment records */
                int has_enrollments = 0;
                for (int i = 0; i < e_count; i++) {
                    if (enrollments[i].course_id == id && 
                        str_equals_case_insensitive(enrollments[i].semester, semester)) {
                        has_enrollments = 1;
                        break;
                    }
                }
                
                if (!has_enrollments) {
                    print_error("No enrollment records found for this course in the specified semester.");
                    break;
                }
                
                /* Check for grade records */
                int has_grades = 0;
                for (int i = 0; i < g_count; i++) {
                    if (grades[i].course_id == id && 
                        str_equals_case_insensitive(grades[i].semester, semester)) {
                        has_grades = 1;
                        break;
                    }
                }
                
                if (!has_grades) {
                    print_error("No grade records found for this course in the specified semester.");
                    break;
                }

                float avg, min_g, max_g;
                if (calculate_course_statistics(grades, g_count, id, semester, &avg, &min_g, &max_g)) {
                    printf("\n--- Grade Statistics ---\n");
                    printf("Average Grade: %.2f\n", avg);
                    printf("Min Grade     : %.2f\n", min_g);
                    printf("Max Grade     : %.2f\n", max_g);
                    display_grade_distribution(grades, g_count, id, semester);
                } else {
                    print_error("Failed to calculate statistics.");
                }
                break;

            case 0: break;
            default: printf("Invalid choice.\n");
        }
        if (choice != 0) press_enter_to_continue();
    } while (choice != 0);
}

/**
 * @brief System options submenu
 * 
 * Provides system-level operations including saving all data,
 * reloading data from CSV files, and creating backups.
 */
void menu_system_options(Student **students, int *s_count, Course **courses, int *c_count, Professor **professors, int *p_count, Enrollment **enrollments, int *e_count, Grade **grades, int *g_count) {
    int choice;
    do {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
        printf("\n--- 7. SYSTEM OPTIONS ---\n");
        printf("1. Save All Data\n");
        printf("2. Load All Data (Reloads from CSV)\n");
        printf("3. Data Backup (Save as .bak files)\n");
        printf("0. Back to Main Menu\n");
        printf("-------------------------\n");
        printf("Choice: ");

        if (scanf("%d", &choice) != 1) { clear_input_buffer(); continue; }
        clear_input_buffer();

        switch (choice) {
            case 1: /* Save */
                save_students("data/students.csv", *students, *s_count);
                save_courses("data/courses.csv", *courses, *c_count);
                save_professors("data/professors.csv", *professors, *p_count);
                save_enrollments("data/enrollments.csv", *enrollments, *e_count);
                save_grades("data/grades.csv", *grades, *g_count);
                printf(">> All data saved successfully.\n");
                break;

            case 2: /* Load */
                printf("WARNING: Unsaved changes will be lost. Continue? (y/n): ");
                char confirm;
                scanf("%c", &confirm); clear_input_buffer();
                if (confirm == 'y' || confirm == 'Y') {
                    /* Free existing memory first */
                    if(*students) free(*students);
                    if(*courses) free(*courses);
                    if(*professors) free(*professors);
                    if(*enrollments) free(*enrollments);
                    if(*grades) free(*grades);

                printf("Reloading data...\n");
                load_students("data/students.csv", students, s_count);
                load_courses("data/courses.csv", courses, c_count);
                load_professors("data/professors.csv", professors, p_count);
                load_enrollments("data/enrollments.csv", enrollments, e_count);
                load_grades("data/grades.csv", grades, g_count, *enrollments, *e_count);
                printf(">> Data reloaded.\n");
                }
                break;

            case 3: /* Backup */
                save_students("data/students_backup.csv", *students, *s_count);
                save_courses("data/courses_backup.csv", *courses, *c_count);
                save_professors("data/professors_backup.csv", *professors, *p_count);
                save_enrollments("data/enrollments_backup.csv", *enrollments, *e_count);
                save_grades("data/grades_backup.csv", *grades, *g_count);
                printf(">> Backup completed. Files saved as data/*_backup.csv\n");
                break;

            case 0: break;
            default: printf("Invalid choice.\n");
        }
        if (choice != 0) press_enter_to_continue();
    } while (choice != 0);
}

/**
 * @brief Curriculum management submenu
 * 
 * Provides options for initializing curricula, managing degree
 * requirements, and checking student graduation status.
 */
void menu_curriculum_management(Curriculum *curricula, int *cur_count, Student *students, int s_count, Course *courses, int c_count, Grade *grades, int g_count) {
    int choice;
    do {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
        printf("\n--- 8. CURRICULUM MANAGEMENT ---\n");
        printf("1. Define Degree Requirements (Add/Edit)\n");
        printf("2. Track Student Progress Toward Degree\n");
        printf("3. Show Remaining Requirements\n");
        printf("4. Validate Graduation Eligibility\n");
        printf("5. Auto-Initialize All Curricula (Reset)\n");
        printf("0. Back to Main Menu\n");
        printf("--------------------------------\n");
        printf("Choice: ");

        if (scanf("%d", &choice) != 1) { clear_input_buffer(); continue; }
        clear_input_buffer();

        int s_id, c_id;
        char dept_name[50];

        switch (choice) {
            case 1: /* Define Degree Requirements (MANUAL EDITING) */
                printf("\n--- Define Degree Requirements ---\n");
                /* List existing departments */
                for(int i=0; i<*cur_count; i++) {
                    printf("%d. %s (Required: %d courses)\n", i+1, curricula[i].department, curricula[i].num_required);
                }
                
                printf("Enter Department Name to edit (exactly): ");
                get_string_input("", dept_name, 50);
                trim_whitespace(dept_name);
                
                Curriculum *cur = get_curriculum_by_department(curricula, *cur_count, dept_name);
                if (cur == NULL) {
                    printf(">> Department not found! (Try Auto-Initialize first)\n");
                } else {
                    printf(">> Editing Curriculum for: %s\n", cur->department);
                    printf("   Current required courses: %d\n", cur->num_required);
                    
                    printf("Enter Course ID to ADD as Requirement: ");
                    scanf("%d", &c_id); clear_input_buffer();
                    
                    int c_idx = search_course_by_id(courses, c_count, c_id);
                    if (c_idx == -1) {
                        printf(">> Error: Course ID %d not found in system.\n", c_id);
                    } else {
                        if (add_course_to_curriculum(cur, c_id)) {
                            printf(">> Success: %s (%s) added to %s requirements.\n", 
                                courses[c_idx].course_code, courses[c_idx].course_name, cur->department);
                        } else {
                            printf(">> Error: Course already added or list full.\n");
                        }
                    }
                }
                break;

            case 2: /* Track Student Progress Toward Degree */
                printf("Enter Student ID: ");
                scanf("%d", &s_id); clear_input_buffer();
                show_degree_progress(s_id, students, s_count, courses, c_count, grades, g_count, curricula, *cur_count);
                break;

            case 3: /* Show Remaining Requirements */
                printf("Enter Student ID: ");
                scanf("%d", &s_id); clear_input_buffer();
                show_remaining_requirements(s_id, students, s_count, courses, c_count, grades, g_count, curricula, *cur_count);
                break;

            case 4: /* Validate Graduation Eligibility */
                printf("Enter Student ID: ");
                scanf("%d", &s_id); clear_input_buffer();
                check_graduation_status(s_id, students, s_count, courses, c_count, grades, g_count, curricula, *cur_count);
                break;

            case 5: /* Auto-Initialize */
                init_curricula(curricula, cur_count, courses, c_count);
                break;

            case 0: break;
            default: printf("Invalid choice.\n");
        }
        if (choice != 0) press_enter_to_continue();
    } while (choice != 0);
}