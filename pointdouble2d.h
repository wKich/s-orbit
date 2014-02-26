#ifndef POINTDOUBLE2D_H
#define POINTDOUBLE2D_H

#include <QPointF>
#include <QVector2D>

class PointDouble2D
{
public:
    PointDouble2D() : xp(0), yp(0) {}
    PointDouble2D(const QPointF& p) : xp(p.x()), yp(p.y()) {}
    PointDouble2D(const QVector2D& v) : xp(v.x()), yp(v.y()) {}
    PointDouble2D(const double& x, const double& y) : xp(x), yp(y) {}
    PointDouble2D(const PointDouble2D& other) : xp(other.xp), yp(other.yp) {}

    double x() const { return xp; }
    double y() const { return yp; }

    double& rx() { return xp; }
    double& ry() { return yp; }

    void setX(const double& x) { xp = x; }
    void setY(const double& y) { yp = y; }

    inline PointDouble2D& operator+=(const double& addend) { xp += addend; yp += addend; return *this; }
    inline PointDouble2D& operator+=(const PointDouble2D& other) { xp += other.xp; yp += other.yp; return *this; }
    inline PointDouble2D& operator-=(const double& subtrahend) { xp -= subtrahend; yp -= subtrahend; return *this; }
    inline PointDouble2D& operator-=(const PointDouble2D& other) { xp -= other.xp; yp -= other.yp; return *this; }
    inline PointDouble2D& operator*=(const double& multiplier) { xp *= multiplier; yp *= multiplier; return *this; }
    inline PointDouble2D& operator*=(const PointDouble2D& other) { xp *= other.xp; yp *= other.yp; return *this; }
    inline PointDouble2D& operator/=(const double& divisor) { xp /= divisor; yp /= divisor; return *this; }
    inline PointDouble2D& operator/=(const PointDouble2D& other) { xp /= other.xp; yp /= other.yp; return *this; }

    friend inline const PointDouble2D operator+(const PointDouble2D& first, const double& addend) { return PointDouble2D(first.xp + addend, first.yp + addend); }
    friend inline const PointDouble2D operator+(const PointDouble2D& first, const PointDouble2D& second) { return PointDouble2D(first.xp + second.xp, first.yp + second.yp); }
    friend inline const PointDouble2D operator-(const PointDouble2D& first, const double& subtrahend) { return PointDouble2D(first.xp - subtrahend, first.yp - subtrahend); }
    friend inline const PointDouble2D operator-(const PointDouble2D& first, const PointDouble2D& second) { return PointDouble2D(first.xp - second.xp, first.yp - second.yp); }
    friend inline const PointDouble2D operator*(const PointDouble2D& first, const double& multiplier) { return PointDouble2D(first.xp * multiplier, first.yp * multiplier); }
    friend inline const PointDouble2D operator*(const PointDouble2D& first, const PointDouble2D& second) { return PointDouble2D(first.xp * second.xp, first.yp * second.yp); }
    friend inline const PointDouble2D operator/(const PointDouble2D& first, const double& divisor) { return PointDouble2D(first.xp / divisor, first.yp / divisor); }
    friend inline const PointDouble2D operator/(const PointDouble2D& first, const PointDouble2D& second) { return PointDouble2D(first.xp / second.xp, first.yp / second.yp); }

    friend inline bool operator<(const PointDouble2D& first, const PointDouble2D& second) { return ((first.xp < second.xp) || (first.yp < second.yp)); }
    friend inline bool operator>(const PointDouble2D& first, const PointDouble2D& second) { return ((first.xp > second.xp) || (first.yp > second.yp)); }
    friend inline bool operator==(const PointDouble2D& first, const PointDouble2D& second) { return (qFuzzyCompare(first.xp, second.xp) && qFuzzyCompare(first.yp, second.yp)); }

    static inline PointDouble2D abs(const PointDouble2D& other) { return PointDouble2D(qAbs(other.xp), qAbs(other.yp)); }

    double distanceToPoint(const PointDouble2D& point) { double x = point.xp - xp; double y = point.yp - yp; return sqrt(x*x + y*y); }
    double lenght() const { return sqrt(squareLenght()); }
    double squareLenght() const { return (xp*xp + yp*yp); }
    QPointF toPointF() const { return QPointF(xp, yp); }
    QVector2D toVector2D() const { return QVector2D(xp, yp); }
private:
    double xp, yp;
};

#endif // POINTDOUBLE2D_H
