//*********************************************************************/
//** Goal: Sunset/Sunrise Calculation.
//** Authors: Daniel Dagnino.
//*********************************************************************/

#include <math.h>

#include "SRS.h"

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void sun::calculate( void ) {
  //--------------------------------------------------------------//
	int february_corr, day_in_month, day_in_date;
  
	//--------------------------------------------------------------//
	// Day from 00:00:00 1/1/2017.
	if (month>2) february_corr = -2;
	else february_corr = 0;
	
	if (month>1) day_in_month = (month-1)*30 + (1+floor((month-2)/2)) + february_corr;
	else day_in_month = 0;
	
	day_in_date = (day-1) + day_in_month + (year-2017)*365;
	
	// Julian day at 00:00:00 1/1/2017.
	float JulianDay_000000_1_1_2017 = 2457754.5;
	
	// 
	float LocalTimeAfterMidnight = 0.5;
	
	//--------------------------------------------------------------//
	// Compute sunset and sunrise.
	float JulianDay = float(day_in_date)+JulianDay_000000_1_1_2017+LocalTimeAfterMidnight-timeZone/24.;
	float JulianCentury = (JulianDay-2451545.)/36525.;
	float GeomMeanLongSun = fmod(280.46646+JulianCentury*(36000.76983+JulianCentury*0.0003032),360.);
	float GeomMeanAnomSun = 357.52911+JulianCentury*(35999.05029-0.0001537*JulianCentury);
	float EccentEarthOrbit = 0.016708634-JulianCentury*(0.000042037+0.0000001267*JulianCentury);
	float SunEqCtr = sin(radians2(GeomMeanAnomSun))*(1.914602-JulianCentury*(0.004817+0.000014*JulianCentury))+ \
                   sin(radians2(2.*GeomMeanAnomSun))*(0.019993-0.000101*JulianCentury)+ \
                   sin(radians2(3.*GeomMeanAnomSun))*0.000289;
	float SunTrueLong = GeomMeanLongSun+SunEqCtr;
	float SunTrueAnom = GeomMeanAnomSun+SunEqCtr;
	float SunRadVector = (1.000001018*(1.-EccentEarthOrbit*EccentEarthOrbit))/(1.+EccentEarthOrbit*cos(radians2(SunTrueAnom)));
	float SunAppLong = SunTrueLong-0.00569-0.00478*sin(radians2(125.04-1934.136*JulianCentury));
	float MeanObliqEcliptic = 23.+(26.+((21.448-JulianCentury*(46.815+JulianCentury*(0.00059-JulianCentury*0.001813))))/60.)/60.;
	float ObliqCorr = MeanObliqEcliptic+0.00256*cos(radians2(125.04-1934.136*JulianCentury));
	float SunRtAscen = degrees2(atan2(cos(radians2(SunAppLong)),cos(radians2(ObliqCorr))*sin(radians2(SunAppLong))));
	float SunDeclin = degrees2(asin(sin(radians2(ObliqCorr))*sin(radians2(SunAppLong))));
	float VarY = tan(radians2(ObliqCorr/2.))*tan(radians2(ObliqCorr/2.));
	float EqTime = 4.*degrees2( \
		     VarY*sin(2.*radians2(GeomMeanLongSun)) \
		     -2.*EccentEarthOrbit*sin(radians2(GeomMeanAnomSun))+ \
		      4.*EccentEarthOrbit*VarY*sin(radians2(GeomMeanAnomSun))*cos(2.*radians2(GeomMeanLongSun)) \
		     -0.5*VarY*VarY*sin(4.*radians2(GeomMeanLongSun)) \
		     -1.25*EccentEarthOrbit*EccentEarthOrbit*sin(2.*radians2(GeomMeanAnomSun)) \
		     );
	float HASunrise = degrees2(acos(cos(radians2(90.833))/(cos(radians2(lat))*cos(radians2(SunDeclin)))- \
		        tan(radians2(lat))*tan(radians2(SunDeclin))));
	float SolarNoon = (720.-4.*lon-EqTime+timeZone*60.)/1440.;
	float Sunrise = (SolarNoon*1440.-HASunrise*4.)/1440.;
	float Sunset  = (SolarNoon*1440.+HASunrise*4.)/1440.;
	
	//--------------------------------------------------------------//
	// To time format.
	float H = Sunrise*24.;
	sunrise.min  = round((H-floor(H))*60.);
	sunrise.hour = floor(H);
	
	H = Sunset*24.;
	sunset.min  = round((H-floor(H))*60.);
	sunset.hour = floor(H);
	
	}




