/**
 * @file menu.h
 * @brief Menu interface for the Student Information System
 * 
 * This header defines all menu-related functions for navigating the
 * Student Information System. Each menu handles a specific domain
 * (students, courses, professors, etc.) and provides a consistent
 * user interface.
 * 
 * @author Student
 * @date 2024
 */

#ifndef MENU_H
#define MENU_H

#include "student.h"
#include "course.h"
#include "professor.h"
#include "enrollment.h"
#include "grade.h"
#include "curriculum.h"

/**
 * @brief Pauses execution until user presses ENTER
 * 
 * Used after displaying results to allow user to read output
 * before the screen is cleared.
 */
void press_enter_to_continue();

/**
 * @brief Displays the main menu of the system
 * 
 * Shows all available menu options including Student, Course,
 * Professor, Enrollment, Grade, Reports, System Options, and
 * Curriculum Management.
 */
void print_main_menu();

/**
 * @brief Student management submenu
 * 
 * Provides options for adding, editing, deleting, and searching
 * students. Also allows viewing student details and calculating GPA.
 * 
 * @param students Pointer to student array
 * @param s_count Pointer to student count
 * @param grades Array of grades (for transcript display)
 * @param g_count Number of grades
 * @param courses Array of courses (for transcript display)
 * @param c_count Number of courses
 * @param enrollments Array of enrollments
 * @param e_count Number of enrollments
 */
void menu_student_management(Student **students, int *s_count, Grade *grades, int g_count, Course *courses, int c_count, Enrollment *enrollments, int e_count);

/**
 * @brief Course management submenu
 * 
 * Provides options for adding, editing, deleting, and searching
 * courses. Also allows setting capacity and assigning prerequisites.
 * 
 * @param courses Pointer to course array
 * @param c_count Pointer to course count
 * @param enrollments Pointer to enrollment array (for cascading delete)
 * @param e_count Pointer to enrollment count
 * @param grades Pointer to grade array (for cascading delete)
 * @param g_count Pointer to grade count
 */
void menu_course_management(Course **courses, int *c_count, Enrollment *enrollments, int *e_count, Grade *grades, int *g_count);

/**
 * @brief Professor management submenu
 * 
 * Provides options for adding, editing, deleting, and searching
 * professors. Also allows assigning courses to professors.
 * 
 * @param professors Pointer to professor array
 * @param p_count Pointer to professor count
 * @param courses Array of courses (for course assignment display)
 * @param c_count Number of courses
 */
void menu_professor_management(Professor **professors, int *p_count, Course *courses, int c_count);

/**
 * @brief Enrollment management submenu
 * 
 * Provides options for enrolling students in courses, dropping
 * students from courses, and viewing enrollment information.
 * 
 * @param enrollments Pointer to enrollment array
 * @param e_count Pointer to enrollment count
 * @param students Array of students
 * @param s_count Number of students
 * @param courses Array of courses
 * @param c_count Number of courses
 * @param professors Array of professors
 * @param p_count Number of professors
 * @param grades Array of grades
 * @param g_count Number of grades
 */
void menu_enrollment_management(Enrollment **enrollments, int *e_count, Student *students, int s_count, Course *courses, int c_count, Professor *professors, int p_count, Grade *grades, int g_count);

/**
 * @brief Grade management submenu
 * 
 * Provides options for recording and updating grades, as well as
 * calculating course statistics and displaying grade distributions.
 * 
 * @param grades Pointer to grade array
 * @param g_count Pointer to grade count
 * @param enrollments Array of enrollments
 * @param e_count Number of enrollments
 * @param courses Array of courses
 * @param c_count Number of courses
 * @param students Array of students
 * @param s_count Number of students
 */
void menu_grade_management(Grade **grades, int *g_count, Enrollment *enrollments, int e_count, Course *courses, int c_count, Student *students, int s_count);

/**
 * @brief Reports submenu
 * 
 * Provides various report options including student transcripts,
 * course rosters, professor course loads, and grade statistics.
 * 
 * @param students Array of students
 * @param s_count Number of students
 * @param courses Array of courses
 * @param c_count Number of courses
 * @param professors Array of professors
 * @param p_count Number of professors
 * @param enrollments Array of enrollments
 * @param e_count Number of enrollments
 * @param grades Array of grades
 * @param g_count Number of grades
 */
void menu_reports(Student *students, int s_count, Course *courses, int c_count, Professor *professors, int p_count, Enrollment *enrollments, int e_count, Grade *grades, int g_count);

/**
 * @brief System options submenu
 * 
 * Provides system-level operations including saving all data,
 * reloading data from CSV files, and creating backups.
 * 
 * @param students Pointer to student array
 * @param s_count Pointer to student count
 * @param courses Pointer to course array
 * @param c_count Pointer to course count
 * @param professors Pointer to professor array
 * @param p_count Pointer to professor count
 * @param enrollments Pointer to enrollment array
 * @param e_count Pointer to enrollment count
 * @param grades Pointer to grade array
 * @param g_count Pointer to grade count
 */
void menu_system_options(Student **students, int *s_count, Course **courses, int *c_count, Professor **professors, int *p_count, Enrollment **enrollments, int *e_count, Grade **grades, int *g_count);

/**
 * @brief Curriculum management submenu
 * 
 * Provides options for initializing curricula, managing degree
 * requirements, and checking student graduation status.
 * 
 * @param curricula Array of curriculum data
 * @param cur_count Pointer to curriculum count
 * @param students Array of students
 * @param s_count Number of students
 * @param courses Array of courses
 * @param c_count Number of courses
 * @param grades Array of grades
 * @param g_count Number of grades
 */
void menu_curriculum_management(Curriculum *curricula, int *cur_count, Student *students, int s_count, Course *courses, int c_count, Grade *grades, int g_count);

#endif