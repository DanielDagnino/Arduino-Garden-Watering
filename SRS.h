//*********************************************************************/
//** Goal: Sunset/Sunrise Calculation.
//** Authors: Daniel Dagnino.
//*********************************************************************/

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





