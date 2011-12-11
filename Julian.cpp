#include "Globals.h"
#include "Julian.h"

#include <cmath>
#include <ctime>
#include <cassert>
#include <cstring>
#include <sstream>
#include <iomanip>

#ifdef WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

Julian::Julian() {

#ifdef WIN32
    SYSTEMTIME st;
    GetSystemTime(&st);
    Initialize(st.wYear,
            st.wMonth,
            st.wDay,
            st.wHour,
            st.wMinute,
            (double) st.wSecond + (double) st.wMilliseconds / 1000.0);
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    struct tm gmt;
    gmtime_r(&tv.tv_sec, &gmt);
    Initialize(gmt.tm_year + 1900,
            gmt.tm_mon + 1,
            gmt.tm_mday,
            gmt.tm_hour,
            gmt.tm_min,
            (double) gmt.tm_sec + (double) tv.tv_usec / 1000000.0);
#endif
}

Julian::Julian(const Julian& jul) {

    date_ = jul.date_;
}

/*
 * create julian date given time_t value
 */
Julian::Julian(const time_t t) {

    struct tm ptm;
#if WIN32
    assert(gmtime_s(&ptm, &t));
#else
    if (gmtime_r(&t, &ptm) == NULL)
        assert(1);
#endif
    int year = ptm.tm_year + 1900;
    double day = ptm.tm_yday + 1 +
            (ptm.tm_hour +
            ((ptm.tm_min +
            (ptm.tm_sec / 60.0)) / 60.0)) / 24.0;

    Initialize(year, day);
}

/*
 * create julian date from year and day of year
 */
Julian::Julian(int year, double day) {

    Initialize(year, day);
}

/*
 * create julian date from individual components
 * year: 2004
 * mon: 1-12
 * day: 1-31
 * hour: 0-23
 * min: 0-59
 * sec: 0-59.99
 */
Julian::Julian(int year, int mon, int day, int hour, int min, double sec) {

    Initialize(year, mon, day, hour, min, sec);
}

/*
 * comparison
 */
bool Julian::operator==(const Julian &date) const {

    return date_ == date.date_ ? true : false;
}

bool Julian::operator!=(const Julian &date) const {

    return !(*this == date);
}

bool Julian::operator>(const Julian &date) const {

    return date_ > date.date_ ? true : false;
}

bool Julian::operator<(const Julian &date) const {

    return date_ < date.date_ ? true : false;
}

bool Julian::operator>=(const Julian &date) const {

    return date_ >= date.date_ ? true : false;
}

bool Julian::operator<=(const Julian &date) const {

    return date_ <= date.date_ ? true : false;
}

/*
 * assignment
 */
Julian& Julian::operator=(const Julian& b) {

    if (this != &b) {
        date_ = b.date_;
    }
    return (*this);
}

Julian& Julian::operator=(const double b) {

    date_ = b;
    return (*this);
}

/*
 * arithmetic
 */
Julian Julian::operator +(const Timespan& b) const {

    return Julian(*this) += b;
}

Julian Julian::operator-(const Timespan& b) const {

    return Julian(*this) -= b;
}

Timespan Julian::operator-(const Julian& b) const {

    return Timespan(date_ - b.date_);
}

/*
 * compound assignment
 */
Julian & Julian::operator +=(const Timespan& b) {

    date_ += b;
    return (*this);
}

Julian & Julian::operator -=(const Timespan& b) {

    date_ -= b;
    return (*this);
}

std::ostream & operator<<(std::ostream& stream, const Julian& julian) {

    std::stringstream out;
    struct Julian::DateTimeComponents datetime;
    julian.ToGregorian(&datetime);
    out << std::right << std::fixed << std::setprecision(6) << std::setfill('0');
    out << std::setw(4) << datetime.years << "-";
    out << std::setw(2) << datetime.months << "-";
    out << std::setw(2) << datetime.days << " ";
    out << std::setw(2) << datetime.hours << ":";
    out << std::setw(2) << datetime.minutes << ":";
    out << std::setw(9) << datetime.seconds << " UTC";
    stream << out.str();
    return stream;
}

/*
 * create julian date from year and day of year
 */
void Julian::Initialize(int year, double day) {

    year--;

    int A = (year / 100);
    int B = 2 - A + (A / 4);

    double new_years = static_cast<int> (365.25 * year) +
            static_cast<int> (30.6001 * 14) +
            1720994.5 + B;

    date_ = new_years + day;
}

/*
 * create julian date from individual components
 * year: 2004
 * mon: 1-12
 * day: 1-31
 * hour: 0-23
 * min: 0-59
 * sec: 0-59.99
 */
void Julian::Initialize(int year, int mon, int day, int hour, int min, double sec) {

    // Calculate N, the day of the year (1..366)
    int N;
    int F1 = (int) ((275.0 * mon) / 9.0);
    int F2 = (int) ((mon + 9.0) / 12.0);

    if (IsLeapYear(year)) {
        // Leap year
        N = F1 - F2 + day - 30;
    } else {
        // Common year
        N = F1 - (2 * F2) + day - 30;
    }

    double dblDay = N + (hour + (min + (sec / 60.0)) / 60.0) / 24.0;

    Initialize(year, dblDay);
}

/*
 * converts time to time_t
 * note: resolution to seconds only
 */
time_t Julian::ToTime() const {

    return static_cast<time_t> ((date_ - 2440587.5) * 86400.0);
}

/*
 * Greenwich Mean Sidereal Time
 */
double Julian::ToGreenwichSiderealTime() const {

#if 0
    double theta;
    double tut1;

    // tut1 = Julian centuries from 2000 Jan. 1 12h UT1 (since J2000 which is 2451545.0)
    // a Julian century is 36525 days
    tut1 = (date_ - 2451545.0) / 36525.0;

    // Rotation angle in arcseconds
    theta = 67310.54841 + (876600.0 * 3600.0 + 8640184.812866) * tut1 + 0.093104 * pow(tut1, 2.0) - 0.0000062 * pow(tut1, 3.0);

    // 360.0 / 86400.0 = 1.0 / 240.0
    theta = fmod(DegreesToRadians(theta / 240.0), kTWOPI);

    /*
     * check quadrants
     */
    if (theta < 0.0)
        theta += kTWOPI;

    return theta;
#endif

    static const double C1 = 1.72027916940703639e-2;
    static const double THGR70 = 1.7321343856509374;
    static const double FK5R = 5.07551419432269442e-15;

    /*
     * get integer number of days from 0 jan 1970
     */
    const double ts70 = date_ - 2433281.5 - 7305.0;
    const double ds70 = floor(ts70 + 1.0e-8);
    const double tfrac = ts70 - ds70;
    /*
     * find greenwich location at epoch
     */
    const double c1p2p = C1 + kTWOPI;
    double gsto = fmod(THGR70 + C1 * ds70 + c1p2p * tfrac + ts70 * ts70 * FK5R, kTWOPI);
    if (gsto < 0.0)
        gsto = gsto + kTWOPI;

    return gsto;
}

/*
 * Local Mean Sideral Time
 */
double Julian::ToLocalMeanSiderealTime(const double& lon) const {

    return fmod(ToGreenwichSiderealTime() + lon, kTWOPI);
}

void Julian::ToGregorian(struct DateTimeComponents* datetime) const {

    double jdAdj = GetDate() + 0.5;
    int Z = (int) jdAdj;
    double F = jdAdj - Z;

    int A = 0;

    if (Z < 2299161) {
        A = static_cast<int> (Z);
    } else {
        int a = static_cast<int> ((Z - 1867216.25) / 36524.25);
        A = static_cast<int> (Z + 1 + a - static_cast<int> (a / 4));
    }

    int B = A + 1524;
    int C = static_cast<int> ((B - 122.1) / 365.25);
    int D = static_cast<int> (365.25 * C);
    int E = static_cast<int> ((B - D) / 30.6001);

    datetime->hours = static_cast<int> (F * 24.0);
    F -= datetime->hours / 24.0;
    datetime->minutes = static_cast<int> (F * 1440.0);
    F -= datetime->minutes / 1440.0;
    datetime->seconds = F * 86400.0;

    datetime->days = B - D - static_cast<int> (30.6001 * E);
    datetime->months = E < 14 ? E - 1 : E - 13;
    datetime->years = datetime->months > 2 ? C - 4716 : C - 4715;
}
