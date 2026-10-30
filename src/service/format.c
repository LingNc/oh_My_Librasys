#include "service/format.h"

#include <stdio.h>
#include <time.h>

#define BORROW_PERIOD_SECONDS (30 * 86400)

void format_student_row(student item, char *out, size_t cap) {
    snprintf(out, cap, "%-10zu %-20s %-12s %-12s %-10d",
             item->studentID,
             item->name->c_str(item->name),
             item->class->c_str(item->class),
             item->department->c_str(item->department),
             item->borrowedCount);
}

void format_book_row(book item, char *out, size_t cap) {
    int used = snprintf(out, cap, "%-20s %-30s %-24s %-24s",
                        item->ISBN->c_str(item->ISBN),
                        item->name->c_str(item->name),
                        item->author->c_str(item->author),
                        item->publisher->c_str(item->publisher));
    if (used < 0 || (size_t)used >= cap) return;

    if (item->status != 1) {
        snprintf(out + used, cap - (size_t)used, "%-10s", "可借");
        return;
    }

    time_t now = time(NULL);
    time_t return_time = item->borrowData + BORROW_PERIOD_SECONDS;
    time_t duration = return_time - now;
    if (duration < 0) duration = -duration;
    size_t days = (size_t)(duration / 86400);
    size_t hours = (size_t)((duration - (time_t)days * 86400) / 3600);

    if (now > return_time) {
        snprintf(out + used, cap - (size_t)used,
                 "%-10s %zu天%zu小时", "已逾期", days, hours);
    } else {
        snprintf(out + used, cap - (size_t)used,
                 "%-10s 剩余%zu天%zu小时归还", "已借出", days, hours);
    }
}

void format_manager_row(manager item, char *out, size_t cap) {
    snprintf(out, cap, "%-10zu %-20s %-20s",
             item->id,
             item->name->c_str(item->name),
             item->registration_date->c_str(item->registration_date));
}