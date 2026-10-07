/**
 * @file main.c
 * @brief Main entry point for the Student Information System
 * 
 * This is the main application file that initializes the system,
 * handles the main menu loop, and coordinates between all modules.
 * 
 * Features:
 * - UTF-8 console support for Windows and Unix
 * - Dynamic memory management for all data structures
 * - Menu-driven interface
 * - Proper cleanup on exit
 * 
 * @author Student
 * @date 2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

#ifdef _WIN32
    #include <windows.h>
    #define CLEAR_SCREEN() system("cls")
#else
    #define CLEAR_SCREEN() system("clear")
#endif

#include "menu.h"
#include "utils.h"
#include "curriculum.h"
#include "student.h"

/**
 * @brief Main entry point
 * 
 * Initializes the system with empty data structures and runs the main
 * menu loop. Data must be loaded via System Options > Load All Data.
 * 
 * @return 0 on successful exit
 */
int main() {
    /* UTF-8 console configuration */
    #ifdef _WIN32
        SetConsoleOutputCP(65001);  /* UTF-8 output */
        SetConsoleCP(65001);        /* UTF-8 input */
    #else
        setlocale(LC_ALL, "en_US.UTF-8");
    #endif
    
    /* Data Structures Initialization */
    Student *students = NULL;     int s_count = 0;
    Course *courses = NULL;       int c_count = 0;
    Grade *grades = NULL;         int g_count = 0;
    Enrollment *enrollments = NULL; int e_count = 0;
    Professor *professors = NULL; int p_count = 0;
    Curriculum curricula[20]; 
    int cur_count = 0;

    printf("\nWelcome to the Student Information System.\n");
    printf("NOTE: System started with empty data.\n");
    printf("Please go to '7. System Options' -> '2. Load All Data' to load from CSV files with validation.\n");

    int choice;
    do {
        /* Clear screen before showing main menu */
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
        
        print_main_menu();
        
        if (scanf("%d", &choice) != 1) {
            clear_input_buffer();
            continue;
        }
        clear_input_buffer();

        switch (choice) {
            case 1: /* Student Management */
                menu_student_management(&students, &s_count, grades, g_count, courses, c_count, enrollments, e_count);
                break;
            case 2: /* Course Management */
                menu_course_management(&courses, &c_count, enrollments, &e_count, grades, &g_count);
                break;
            case 3: /* Professor Management */
                menu_professor_management(&professors, &p_count, courses, c_count);
                break;
            case 4: /* Enrollment Management */
                menu_enrollment_management(&enrollments, &e_count, students, s_count, courses, c_count, professors, p_count, grades, g_count);
                break; 
            case 5: /* Grade Management */
                menu_grade_management(&grades, &g_count, enrollments, e_count, courses, c_count, students, s_count);
                break;
            case 6: /* Reports */
                menu_reports(students, s_count, courses, c_count, professors, p_count, enrollments, e_count, grades, g_count);
                break;
            case 7: /* System Options */
                menu_system_options(&students, &s_count, &courses, &c_count, &professors, &p_count, &enrollments, &e_count, &grades, &g_count);
                /* Initialize curriculum if courses are loaded */
                if (c_count > 0 && cur_count == 0) {
                     init_curricula(curricula, &cur_count, courses, c_count);
                }
                break;
            case 8: /* Curriculum Management */
                menu_curriculum_management(curricula, &cur_count, students, s_count, courses, c_count, grades, g_count);
                break;
            case 0:
                printf("Exiting the system...\n");
                break;
            default:
                printf("Invalid choice.\n");
                press_enter_to_continue();
        }
    } while (choice != 0);

    /* Memory cleanup */
    if (students) free(students);
    if (courses) free(courses);
    if (grades) free(grades);
    if (enrollments) free(enrollments);
    if (professors) free(professors);

    return 0;
}