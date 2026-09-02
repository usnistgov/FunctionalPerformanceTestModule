// Expert Assistant (EA) tool - This file by: DAV (NIST) - Tool version: 0 - File version: 0

/* Header declaring    
*/
//XXXXXXX1XXXXXXXXX2XXXXXXXXX3XXXXXXXXX4XXXXXXXXX5XXXXXXXXX6XXXXXXXXX7XXXXXXXXX8XXXXXXXXX9XXXXXXXXXCXXXXX

#ifndef DLLEXPORT_H
#define DLLEXPORT_H

#ifdef  COMPILINGDLL
#define AFDDGUI_API __declspec(dllexport)  // as compiled by a DLL project calling customTypes.hpp
#else
#define AFDDGUI_API __declspec(dllimport)  // when compiled in any Client project
#endif


#include <ctime>
#include <vector> // temp as C++ until C-style TBD
#include <string> // temp as C++ until C-style TBD

// TBD :  To preserve ANSI C compliance, #include nothing from C++ code

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8/////////9/////////C/////
// I


class AFDDGUI_API EInput {  

   public:

      virtual ~EInput( void ) { /* empty */ };

      virtual void SetCurrentTM( std::tm ) = 0;
      virtual void SetCurrentSamples(   double*,   // ptr to C-style array of doubles in order expected
                                          size_t,  // number of array elements (= number of "channels")
                                          size_t   // should always be result of sizeof(double)
                                          ) = 0;

      virtual int ClickKnobAndSetValue( unsigned int, size_t, double ) = 0;
 
      virtual void SingleStepAFDD( void ) = 0;
      virtual void RenderMpx( void ) = 0;

   protected:

      // EInput( void ) { /* empty */ };
 

};


 
class AFDDGUI_API EOutput {  

   public:

      virtual ~EOutput( void ) { /* empty */ };
 
      virtual double GetDouble( unsigned int ) = 0;
      virtual int GetIntCastFromBool( unsigned int ) = 0;
      virtual bool GetBool( unsigned int ) = 0;
      virtual time_t GetTimeStamp( unsigned int ) = 0;

     // Temporary (C++ STL dependent) methods until interface worked out with MAG
      virtual std::string GetLabel( unsigned int ) = 0;
      virtual std::string GetMsg( unsigned int ) = 0;
      virtual std::vector<std::string> GetAlertsFromKit( char ) = 0;

      virtual std::vector<std::string> GetKnobLabels( unsigned int ) = 0;
      virtual std::vector<double>      GetKnobValues( unsigned int ) = 0;

      // Maybe later
      //virtual char* GetChars( unsigned int, unsigned int ) = 0;
      //virtual std::vector<std::string> GetErrorLog( void ) = 0;

   protected:

      //EOutput( void ) { /* empty */ };

};



struct AFDDGUI_API EPortHandles {  // Not sure if this class can be rolled into those above

   public:

      //~EPortHandles( void );

      static EInput* GetPortIn( void );

      static EOutput* GetPortOut( void );

      static void SetPortIn( EInput* );

      static void SetPortOut( EOutput* );

   private:

      static EInput* p_inputPort;

      static EOutput* p_outputPort;

      //EPortHandles( void );
 
};



#endif


/* START FILE NOTES XXXXXXXXX3XXXXXXXXX4XXXXXXXXX5XXXXXXXXX6XXXXXXXXX7XXXXXXXXX8XXXXXXXXX9XXXXXXXXXCXXXXX

[1]   

--------------------------------------------------------------------------------
XXX END FILE NOTES */

//END-OF-FILE ZZZZZ2ZZZZZZZZZ3ZZZZZZZZZ4ZZZZZZZZZ5ZZZZZZZZZ6ZZZZZZZZZ7ZZZZZZZZZ8ZZZZZZZZZ9ZZZZZZZZZCZZZZZ
