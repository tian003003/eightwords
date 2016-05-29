#include <sstream>

#include "calendar.h"

Calendar::Calendar(const std::string &dbFile)
{
    CC(sqlite3_open(dbFile.c_str(), &m_sqlite3));
}

Calendar::~Calendar()
{
    CC(sqlite3_close(m_sqlite3));
}

CalendarDay Calendar::queryChinaDay(int year, int month, int day)
{
    sqlite3_stmt *stmt;
    CC(sqlite3_prepare(m_sqlite3,
            "select cn_year, cn_month, cn_day from calendar where `year` = ? and `month` = ? and `day` = ?", -1, &stmt,
            nullptr));
    CC(sqlite3_bind_int(stmt, 1, year));
    CC(sqlite3_bind_int(stmt, 2, month));
    CC(sqlite3_bind_int(stmt, 3, day));

    CalendarDay chinaDay;

    int ret = sqlite3_step(stmt);
    if (ret == SQLITE_ROW)
    {
        // 找到
        chinaDay.chinaYear = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
        chinaDay.chinaMonth = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
        chinaDay.chinaDay = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
        CC(sqlite3_finalize(stmt));
    }
    else
    {
        CC(sqlite3_finalize(stmt));
        throw std::runtime_error("无法查询农历");
    }

    return chinaDay;
}

void Calendar::CC(int ret)
{
    if (ret == SQLITE_OK || ret == SQLITE_DONE)
    {
        return;
    }

    std::stringstream ssErr;
    ssErr << "sqlite error: " << sqlite3_errmsg(m_sqlite3);
    throw std::runtime_error(ssErr.str());
}
