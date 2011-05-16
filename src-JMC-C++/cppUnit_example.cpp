
#include <cppunit/TestResult.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/BriefTestProgressListener.h>

#include <cppunit/TestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

class DiyTest : public CppUnit::TestFixture
{
public:
   CPPUNIT_TEST_SUITE( DiyTest );
   CPPUNIT_TEST( testsRates );
   CPPUNIT_TEST( testsReussis );
   CPPUNIT_TEST_SUITE_END();
   /// Méthode dont les tests réussissent
   void testsReussis()
   {
     CPPUNIT_ASSERT( 1 == 1 );
     CPPUNIT_ASSERT( !(1 == 2) );
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
