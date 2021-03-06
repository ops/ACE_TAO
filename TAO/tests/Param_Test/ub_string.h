
//=============================================================================
/**
 *  @file    ub_string.h
 *
 *   Tests unbounded string
 *
 *  @author   Aniruddha Gokhale
 */
//=============================================================================

#ifndef PARAM_TEST_UNBOUNDED_STRING_H
#define PARAM_TEST_UNBOUNDED_STRING_H

#include "param_testC.h"

// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
//                        test unbounded strings
// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
class Test_Unbounded_String
{
public:
  /// ctor
  Test_Unbounded_String (void);

  /// dtor
  ~Test_Unbounded_String (void);

  /// run the SII test
  int run_sii_test (Param_Test_ptr objref);

  /// return operation name
  const char *opname () const;

  /// set values for parameters
  int init_parameters (Param_Test_ptr objref);

  /// reset values for CORBA
  int reset_parameters (void);

  /// check if results are valid
  CORBA::Boolean check_validity (void);

  /// check if results are valid. This is used for DII results
  CORBA::Boolean check_validity (CORBA::Request_ptr req);

  /// print all the values
  void print_values (void);

  /// invoke DII request with appropriate exception handling.
  void dii_req_invoke (CORBA::Request *);

private:
  /// operation name
  char *opname_;

  /// in parameter
  char *in_;

  /// inout parameter
  char *inout_;

  /// out parameter
  char *out_;

  /// return value
  char *ret_;
};

#endif /* PARAM_TEST_UNBOUNDED_STRING_H */
