#ifndef COORDGEODETIC_H_
#define COORDGEODETIC_H_

#include "Util.h"

#include <string>
#include <sstream>
#include <iomanip>

/**
 * Stores a geodetic position
 */
struct CoordGeodetic
{
public:
    /**
     * Default constructor
     */
    CoordGeodetic()
        : latitude(0.0),
        longitude(0.0),
        altitude(0.0)
    {
    }

    /**
     * Constructor
     * @param[in] lat the latitude (degrees by default)
     * @param[in] lon the longitude (degrees by default)
     * @param[in] alt the altitude in kilometers
     * @param[in] is_radians whether the latitude/longitude is in radians
     */
    CoordGeodetic(
            double lat,
            double lon,
            double alt,
            bool is_radians = false)
    {
        if (is_radians)
        {
            latitude = lat;
            longitude = lon;
        }
        else
        {
            latitude = Util::DegreesToRadians(lat);
            longitude = Util::DegreesToRadians(lon);
        }
        altitude = alt;
    }

    /**
     * Copy constructor
     * @param[in] geo object to copy from
     */
    CoordGeodetic(const CoordGeodetic& geo)
    {
        latitude = geo.latitude;
        longitude = geo.longitude;
        altitude = geo.altitude;
    }

    /**
     * Destructor
     */
    virtual ~CoordGeodetic()
    {
    }

    /**
     * Assignment operator
     * @param[in] geo object to copy from
     */
    CoordGeodetic& operator=(const CoordGeodetic& geo)
    {
        if (this != &geo)
        {
            latitude = geo.latitude;
            longitude = geo.longitude;
            altitude = geo.altitude;
        }
        return *this;
    }

    /**
     * Equality operator
     * @param[in] geo the object to compare with
     * @returns whether the object is equal
     */
    bool operator==(const CoordGeodetic& geo) const
    {
        return IsEqual(geo);
    }

    /**
     * Inequality operator
     * @param[in] geo the object to compare with
     * @returns whether the object is not equal
     */
    bool operator!=(const CoordGeodetic& geo) const
    {
        return !IsEqual(geo);
    }

    /**
     * Dump this object to a string
     * @returns string
     */
    std::string ToString() const
    {
        std::stringstream ss;
        ss << std::right << std::fixed << std::setprecision(3);
        ss << "Lat: " << std::setw(7) << Util::RadiansToDegrees(latitude);
        ss << ", Lon: " << std::setw(7) << Util::RadiansToDegrees(longitude);
        ss << ", Alt: " << std::setw(9) << altitude;
        return ss.str();
    }

    /** latitude in radians (-PI >= latitude < PI) */
    double latitude;
    /** latitude in radians (-PI/2 >= latitude <= PI/2) */
    double longitude;
    /** altitude in kilometers */
    double altitude;

private:
    bool IsEqual(const CoordGeodetic& geo) const
    {
        bool equal = false;
        if (latitude == geo.latitude &&
                longitude == geo.longitude &&
                altitude == geo.altitude)
        {
            equal = false;
        }
        return equal;
    }
};

/**
 * Dump a Coordgeodetic to a stream
 * @params[in,out] strm stream to output to
 * @params[in] g the CoordGeodetic to print
 */
inline std::ostream& operator<<(std::ostream& strm, const CoordGeodetic& g)
{
    return strm << g.ToString();
}

#endif
