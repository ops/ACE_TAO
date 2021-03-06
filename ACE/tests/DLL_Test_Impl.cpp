
//=============================================================================
/**
 *  @file    DLL_Test_Impl.cpp
 *
 *  This test illustrates the use of <ACE_DLL> wrapper class.
 *
 *  @author Kirthika Parameswaran  <kirthika@cs.wustl.edu>
 */
//=============================================================================


#include "DLL_Test_Impl.h"
#include "ace/ACE.h"
#include "ace/OS_Errno.h"
#include "ace/svc_export.h"
#include "ace/OS_NS_string.h"

Hello_Impl::Hello_Impl ()
{
  ACE_DEBUG ((LM_DEBUG, "Hello_Impl::Hello_Impl\n"));
}

Hello_Impl::~Hello_Impl ()
{
  ACE_DEBUG ((LM_DEBUG, "Hello_Impl::~Hello_Impl\n"));
}

void
Hello_Impl::say_next ()
{
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("How are you?\n")));
}

ACE_TCHAR *
Hello_Impl::new_info ()
{
  return ACE::strnew (ACE_TEXT ("Hello_Impl::new_info() allocated by ACE::strnew()"));
}

ACE_TCHAR *
Hello_Impl::malloc_info ()
{
  return ACE_OS::strdup (ACE_TEXT ("Hello_Impl::new_info() allocated by ACE_OS::malloc()"));
}

void *
Hello_Impl::operator new (size_t bytes)
{
  ACE_DEBUG ((LM_INFO, "Hello_Impl::new\n"));
  return ::new char[bytes];
}

/// Overloaded new operator, nothrow_t variant.
void *
Hello_Impl::operator new (size_t bytes, const std::nothrow_t &nt)
{
  ACE_DEBUG ((LM_INFO, "Hello_Impl::new\n"));
  return ::new (nt) char[bytes];
}

void
Hello_Impl::operator delete (void *ptr, const std::nothrow_t&) throw ()
{
  ACE_DEBUG ((LM_INFO, "Hello_Impl::delete\n"));
  ::delete [] static_cast<char *> (ptr);
}

void
Hello_Impl::operator delete (void *ptr)
{
  ACE_DEBUG ((LM_INFO, "Hello_Impl::delete\n"));
  ::delete [] static_cast<char *> (ptr);
}

extern "C" ACE_Svc_Export Hello *
get_hello (void)
{
  Hello *hello = 0;

  ACE_NEW_RETURN (hello,
                  Hello_Impl,
                  0);

  return hello;
}

class Static_Constructor_Test
{
public:
  Static_Constructor_Test ()
  {
    ACE_DEBUG ((LM_DEBUG,
                "Static_Constructor_Test::Static_Constructor_Test\n"));
  }
  ~Static_Constructor_Test ()
  {
    ACE_DEBUG ((LM_DEBUG,
                "Static_Constructor_Test::~Static_Constructor_Test\n"));
  }
};

static Static_Constructor_Test the_instance;

// --------------------------------------------------------

Child::Child ()
{
}

Child::~Child ()
{
}

void
Child::test ()
{
  ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("child called\n")));
}

// --------------------------------------------------------


// Test dynamic cast
extern "C" ACE_Svc_Export int
dynamic_cast_test (Parent *target)
{
  Child *c = 0;
  c = dynamic_cast<Child*> (target);
  ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("dynamic_cast_test: parent %@; child %@\n"),
              target, c));
  return target == c ? 0 : -1;
}
