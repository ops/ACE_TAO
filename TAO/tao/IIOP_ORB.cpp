// $Id$
//
// @(#)iioporb.cpp      1.8 95/09/19
// Copyright 1994-1995 by Sun Microsystems Inc.
// All Rights Reserved
//
// IIOP:                ORB pseudo-object
//
// This includes objref stringification/destringification for IIOP
// object references.

#include "tao/corba.h"

#if !defined (__ACE_INLINE__)
# include "tao/IIOP_ORB.i"
#endif /* ! __ACE_INLINE__ */

ACE_RCSID(tao, IIOP_ORB, "$Id$")

static const char ior_prefix [] = "IOR:";
static const char iiop_prefix [] = "iiop:";

// Objref stringification.

CORBA::String
IIOP_ORB::object_to_string (CORBA::Object_ptr obj,
                            CORBA::Environment &env)
{
  env.clear ();

  // Application writer controls what kind of objref strings they get,
  // maybe along with other things, by how they initialize the ORB.

  if (use_omg_ior_format_)
    {
      // By default, orbs use IOR strings; these are ugly (and error
      // prone) but specified by CORBA.
      //
      // XXX there should be a simple way to reuse this code in other
      // ORB implementations ...

      // @@ Is BUFSIZ the right size here?
      char buf [BUFSIZ];
      TAO_OutputCDR cdr (buf, sizeof buf, TAO_ENCAP_BYTE_ORDER);

      // support limited oref ACE_OS::strcmp.
      (void) ACE_OS::memset (buf, 0, BUFSIZ);

      // Marshal the objref into an encapsulation bytestream.
      (void) cdr.write_octet (TAO_ENCAP_BYTE_ORDER);
      if (cdr.encode (CORBA::_tc_Object,
                        &obj, 0,
                        env) != CORBA::TypeCode::TRAVERSE_CONTINUE)
        return 0;

      // Now hexify the encapsulated CDR data into a string, and
      // return that string.

      CORBA::String cp;
      size_t len = cdr.length ();

      CORBA::String string = CORBA::string_alloc (sizeof ior_prefix + 2 * len);

      ACE_OS::strcpy ((char *) string, ior_prefix);

      const char* bytes = cdr.buffer ();

      for (cp = (CORBA::String) ACE_OS::strchr ((char *) string, ':') + 1;
           len--;
           bytes++)
        {
          *cp++ = ACE::nibble2hex ((*bytes) >> 4);
          *cp++ = ACE::nibble2hex (*bytes);
        }

      *cp = 0;
      return string;
    }
  else
    {
      // The "internet" ORB uses readable URL style objrefs, as used
      // in the World Wide Web.
      //
      // NOTE: the version ID in the string is ugly but we can't
      // realistically eliminate it by any "assume 1.0" strategy...
      // Similarly with the port, because there's no single IIOP port
      // to which we could default.

      static const char digits [] = "0123456789";

      // This only works for IIOP objrefs.  If we're handed an objref
      // that's not an IIOP objref, fail -- application must use an
      // ORB that's configured differently.
      IIOP_Object *iiopobj = 
	ACE_dynamic_cast (IIOP_Object*, obj->_stubobj ());

      if (iiopobj == 0)
        return CORBA::string_copy ((CORBA::String) iiop_prefix);

      CORBA::String_var key;
      TAO_POA::encode_sequence_to_string (key.inout(),
                                          iiopobj->profile.object_key);

      u_int buflen = (ACE_OS::strlen (iiop_prefix) +
                      1 /* major # */ + 1 /* minor # */ +
                      2 /* double-slash separator */ +
                      ACE_OS::strlen (iiopobj->profile.host) +
                      1 /* colon separator */ +
                      5 /* port number */ +
                      1 /* slash separator */ +
                      ACE_OS::strlen (key) +
                      1 /* zero terminator */);
      CORBA::String buf = CORBA::string_alloc (buflen);

      ACE_OS::sprintf (buf,
                       "%s%c.%c//%s:%d/%s",
                       iiop_prefix,
                       digits [iiopobj->profile.iiop_version.major],
                       digits [iiopobj->profile.iiop_version.minor],
                       iiopobj->profile.host,
                       iiopobj->profile.port,
                       key.in ());
      return buf;
    }
}

// Destringify OMG-specified "IOR" string.
//
// XXX there should be a simple way to reuse this code in other ORB
// implementations ...

static CORBA::Object_ptr
ior_string_to_object (const char *str,
                      CORBA::Environment &env)
{
  // Unhex the bytes, and make a CDR deencapsulation stream from the
  // resulting data.

  ACE_Message_Block mb (ACE_OS::strlen ((char *) str) / 2
                        + 1 + CDR::MAX_ALIGNMENT);

  CDR::mb_align (&mb);

  char *buffer = mb.rd_ptr ();

  const char *tmp = str;
  size_t len = 0;

  while (tmp [0] && tmp [1])
    {
      u_char byte;

      if (!(isxdigit (tmp [0]) && isxdigit (tmp [1])))
        break;

      byte = (u_char) (ACE::hex2byte (tmp [0]) << 4);
      byte |= ACE::hex2byte (tmp [1]);

      buffer [len++] = byte;
      tmp += 2;
    }

  if (tmp [0] && !isspace (tmp [0]))
    {
      env.exception (new CORBA::BAD_PARAM (CORBA::COMPLETED_NO));
      return CORBA::Object::_nil ();
    }

  // Create deencapsulation stream ... then unmarshal objref from that
  // stream.

  int byte_order = *(mb.rd_ptr ());
  mb.rd_ptr (1);
  mb.wr_ptr (len);
  TAO_InputCDR stream (&mb, byte_order);

  CORBA::Object_ptr objref;
  if (stream.decode (CORBA::_tc_Object,
		     &objref, 0,
		     env) != CORBA::TypeCode::TRAVERSE_CONTINUE)
    objref = 0;

  return objref;
}

// Destringify URL style IIOP objref.

static CORBA::Object_ptr
iiop_string_to_object (const char *string,
                       CORBA::Environment &env)
{
  // NIL objref encodes as just "iiop:" ... which has already been
  // removed, so we see it as an empty string.

  if (!string || !*string)
    return CORBA::Object::_nil ();

  // type ID not encoded in this string ... makes narrowing rather
  // expensive, though it does ensure that type-safe narrowing code
  // gets thoroughly excercised/debugged!  Without a typeID, the
  // _narrow will be required to make an expensive remote "is_a" call.

  IIOP_Object *data;

  // null type ID.
  ACE_NEW_RETURN (data, IIOP_Object ((char *) 0),
                  CORBA::Object::_nil ());

  // Remove the "N.N//" prefix, and verify the version's one
  // that we accept

  if (isdigit (string [0]) && isdigit (string [2]) && string [1] == '.'
      && string [3] == '/' && string [4] == '/')
    {
      data->profile.iiop_version.major = (char) (string [0] - '0');
      data->profile.iiop_version.minor = (char) (string [2] - '0');
      string += 5;
    }
  else
    {
      env.exception (new CORBA_DATA_CONVERSION (CORBA::COMPLETED_NO));
      data->_decr_refcnt ();
      return CORBA::Object::_nil ();
    }

  if (data->profile.iiop_version.major != IIOP::MY_MAJOR
      || data->profile.iiop_version.minor > IIOP::MY_MINOR)
    {
      env.exception (new CORBA_DATA_CONVERSION (CORBA::COMPLETED_NO));
      data->_decr_refcnt ();
      return CORBA::Object::_nil ();
    }

  // Pull off the "hostname:port/" part of the objref

  // Copy the string because we are going to modify it...
  CORBA::String_var copy = CORBA::string_dup (string);

  char *start = copy.inout ();
  char *cp = ACE_OS::strchr (start, ':');
  if (cp == 0)
    {
      env.exception (new CORBA_DATA_CONVERSION (CORBA::COMPLETED_NO));
      data->_decr_refcnt ();
      return CORBA::Object::_nil ();
    }

  data->profile.host = CORBA::string_alloc (1 + cp - start);
  for (cp = data->profile.host;
       *start != ':';
       *cp++ = *start++)
    continue;

  *cp = 0; start++;

  cp = ACE_OS::strchr (start, '/');

  if (cp == 0)
    {
      env.exception (new CORBA_DATA_CONVERSION (CORBA::COMPLETED_NO));
      CORBA::string_free (data->profile.host);
      data->profile.host = 0;
      data->_decr_refcnt ();
      return CORBA::Object::_nil ();
    }

  data->profile.port = (short) ACE_OS::atoi (start);
  data->profile.object_addr (0);
  start = ++cp;

  // Parse the object key
  TAO_POA::decode_string_to_sequence (data->profile.object_key,
                                      start);

  // Create the CORBA level proxy.
  TAO_ServantBase *servant =
    TAO_ORB_Core_instance ()->orb ()->_get_collocated_servant (data);

  // This will increase the ref_count on data by one
  CORBA_Object *obj = new CORBA_Object (data, servant, servant != 0);

  // Set the ref_count on data to 1, which is correct, because only
  // obj has now a reference to it.
  // data->_decr_refcnt ();

  return obj;
}

// Destringify arbitrary objrefs.

CORBA::Object_ptr
IIOP_ORB::string_to_object (const char *str,
                            CORBA::Environment &env)
{
  env.clear ();

  CORBA::Object_ptr obj = 0;

  // Use the prefix code to choose which destringify algorithm to use.
  if (ACE_OS::strncmp (str,
                       iiop_prefix,
                       sizeof iiop_prefix - 1) == 0)
    obj = iiop_string_to_object (str + sizeof iiop_prefix - 1, env);

  else if (ACE_OS::strncmp (str,
                            ior_prefix,
                            sizeof ior_prefix - 1) == 0)
    obj = ior_string_to_object (str + sizeof ior_prefix - 1, env);

  // Return the object
  return obj;
}

TAO_ServantBase *
IIOP_ORB::_get_collocated_servant (STUB_Object *sobj)
{
  // ACE_DEBUG ((LM_DEBUG, "IIOP_ORB: get_collocated_servant\n"));

  if (this->optimize_collocation_objects_ && sobj != 0)
    {
      IIOP_Object *iiopobj = 
	ACE_dynamic_cast (IIOP_Object*, sobj);
      
      // Make sure users passed in an IIOP_Object otherwise, we don't
      // know what to do next.
      if (iiopobj == 0)
        {
#if 0
          ACE_ERROR ((LM_ERROR,
		      "%p: Passing IIOP ORB and non-IIOP object\n",
		      "_get_collocated_object"));
#endif
	  // Something must be wrong!
          return 0;
        }
#if 0
      ACE_DEBUG ((LM_DEBUG,
		  "IIOP_ORB: checking collocation for <%s:%d>\n",
		  iiopobj->profile.object_addr().get_host_name(),
		  iiopobj->profile.object_addr().get_port_number()));
#endif
      CORBA::Environment env;
      TAO_ObjectKey_var objkey = iiopobj->key (env);

      if (env.exception ())
        {
#if 0
          ACE_DEBUG ((LM_DEBUG,
                      "IIOP_ORB: cannot find key for <%s:%d>\n",
                      iiopobj->profile.object_addr().get_host_name(),
                      iiopobj->profile.object_addr().get_port_number()));
#endif
          return 0;
        }

      // Check if the object requested is a collocated object.
      TAO_POA *poa = TAO_ORB_Core_instance ()->
        get_collocated_poa (iiopobj->profile.object_addr());

      if (poa != 0)
        {
          PortableServer::Servant servant =
            poa->find_servant (objkey.in (), env);
          if (env.exception ())
            {
#if 0
              ACE_DEBUG ((LM_DEBUG,
                          "IIOP_ORB: cannot find servant for <%s:%d>\n",
                          iiopobj->profile.object_addr().get_host_name(),
                          iiopobj->profile.object_addr().get_port_number()));
#endif
              return 0;
            }

#if 0
          ACE_DEBUG ((LM_DEBUG,
                      "IIOP_ORB: object at <%s:%d> is collocated\n",
                      iiopobj->profile.object_addr().get_host_name(),
                      iiopobj->profile.object_addr().get_port_number()));
#endif
          return servant;
        }
    }

#if 0
  ACE_DEBUG ((LM_DEBUG,
	      "IIOP_ORB: collocation failed for \n"));
#endif

  return 0;
}

#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)
template class ACE_Singleton<IIOP_ORB, ACE_SYNCH_RECURSIVE_MUTEX>;
template class ACE_Unbounded_Set<ACE_INET_Addr>;
template class ACE_Unbounded_Set_Iterator<ACE_INET_Addr>;
#elif defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)
#pragma instantiate ACE_Singleton<IIOP_ORB, ACE_SYNCH_RECURSIVE_MUTEX>
#pragma instantiate ACE_Unbounded_Set<ACE_INET_Addr>
#pragma instantiate ACE_Unbounded_Set_Iterator<ACE_INET_Addr>
#endif /* ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION */
