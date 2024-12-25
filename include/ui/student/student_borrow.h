#ifndef _student_borrow_h
#define _student_borrow_h

void borrow_book_menu(void *arg);
void display_book_details(void *arg);
void borrow_book(void *arg);
void borrow_book_config(void *arg);

// 前向声明
void student_preInfo(void *arg);
void student_postInfo(void *arg);

#endif // _student_borrow_h