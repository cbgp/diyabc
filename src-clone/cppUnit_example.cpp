#include "data.cpp"
#include <vector>
#ifndef ALGORITHM
#include <algorithm>
#define ALGORITHM
#endif



#include <cppunit/TestResult.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/BriefTestProgressListener.h>

#include <cppunit/TestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#ifndef MATHH
#include <math.h>
#define MATHH
#endif

using namespace std;

class DiyTest : public CppUnit::TestFixture
{
public:
   CPPUNIT_TEST_SUITE( DiyTest );
   //CPPUNIT_TEST( testsRates );
   CPPUNIT_TEST( testsDataC );

   CPPUNIT_TEST_SUITE_END();
   /// Méthode dont les tests réussissent
   void testsDataC()
   {
     DataC d;
     int *a;
     a = new int[2];
     a[0] = 3;
     d.loadfromfile("/home/julien/vcs/git/diyabc.git/src-JMC-C++/gptestA_001.txt");
     CPPUNIT_ASSERT(d.nsample == 3);
     CPPUNIT_ASSERT(d.sexratio == 0.76);
     CPPUNIT_ASSERT(d.nloc == 14);

     //d.nind = &a;
     //CPPUNIT_ASSERT(*(d.nind) == 3);
     //cout << "plop";
     //d.libere();
     //CPPUNIT_ASSERT(d.nind == NULL);
   }
   /// Méthode dont les tests ratent
   void testsRates()
   {
     CPPUNIT_ASSERT( !(1 == 1) );
     CPPUNIT_ASSERT( 1 == 2 );
   }
};

CPPUNIT_TEST_SUITE_REGISTRATION(DiyTest);

int main( int ac, char **av )
{
  //--- Create the event manager and test controller
  CPPUNIT_NS::TestResult controller;

  //--- Add a listener that colllects test result
  CPPUNIT_NS::TestResultCollector result;
  controller.addListener( &result );        

  //--- Add a listener that print dots as test run.
  CPPUNIT_NS::BriefTestProgressListener progress;
  controller.addListener( &progress );      

  CPPUNIT_NS::TestRunner runner;
  runner.addTest( CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest() );
  runner.run( controller );

  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write();
    return 0;
}
