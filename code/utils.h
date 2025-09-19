#ifndef UTILS_H
#define UTILS_H

#endif // UTILS_H

#include <QMessageBox>
#include <QRegularExpression>
#include <QString>
extern "C"{
#include <libavutil/error.h>
#include <libavutil/avutil.h>
}

/**
 * Convert duration to time string"hh~h:mm:ss.zzz".
 * @param duration video duration from AVFormatContext
 * @return QString value
 */
QString duration2str(int64_t duration);

/**
 * Convert time string "hh~h:mm:ss.zzz" to time string.
 * @param QString value
 * @return -1 on failed, ffmpeg duration on success
 */
int64_t str2duration(QString timeStr);

/**
 * Detect whether two segments have overlapping area
 * @param segment1
 * @param segment2
 * @return true on has overlapping area, false on has not
 */
bool hasOverlap(QPair<int64_t, int64_t> segment1, QPair<int64_t, int64_t> segment2);

/**
 * Compare function or time slice
 * @param slice1
 * @param slice2
 * @return true on slice1 < slice2
 */
bool compareSlice(QPair<int64_t, int64_t> slice1, QPair<int64_t, int64_t> slice2);

/**
 * Process ffmpeg api function，print ffmpeg error message
 * @param ret
 * @return success on 1; failure on other number
 */
int processErrorMsg(int ret);
