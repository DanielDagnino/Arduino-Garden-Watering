  //=====================================================================
  //
  //       Garden watering system syncronized with sunrise/sunset
  //       ------------------------------------------------------
  //
  //                      Author: Daniel Dagnino
  //                        My garden, Spain
  //                       Copyright (C) 2017
  //
  // This program is free software; you can redistribute it and/or modify
  // it under the terms of the GNU General Public License as published by
  // the Free Software Foundation; either version 2 of the License, or
  // (at your option) any later version.
  //
  // This program is distributed in the hope that it will be useful,
  // but WITHOUT ANY WARRANTY; without even the implied warranty of
  // MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  // GNU General Public License for more details.
  //
  // You should have received a copy of the GNU General Public License
  // along with this program.  If not, see <http://www.gnu.org/licenses/>.
  //
  //=====================================================================

#if ARDUINO < 100
#include <WProgram.h>
#else
#include <Arduino.h>
#endif

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
class sun {
  
  class time {
    public:
      // Datos miembro de la clase.
      int hour;
      int min;
      int sec;
  };
  
	private:
    // Funciones miembro de la clase.
		float degrees2( float rad ){ return 360.*rad/(2.*3.1415927); }
		float radians2( float deg ){ return 2.*3.1415927*deg/360.; }
	
	public:
    // Datos miembro de la clase.
    int day;
    int month;
    int year;
    float lat, lon;
    float timeZone;
    time sunset, sunrise;
    // Funciones miembro de la clase.
    void calculate( void );
};

//--------------------------------------------------------------//





