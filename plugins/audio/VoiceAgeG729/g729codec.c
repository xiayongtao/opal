/*
 * Voice Age G.729A codec for OpenH323/OPAL
 *
 * Copyright (C) 2004 Post Increment, All Rights Reserved
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.0 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is Open H323 Library.
 *
 * The Initial Developer of the Original Code is Post Increment
 *
 * Contributor(s): ______________________________________.
 *
 * $Revision$
 * $Author$
 * $Date$
 */

#include <codec/opalplugin.h>


#include <stdlib.h>
#include <malloc.h>

#ifdef _WIN32_WCE

#include "va_g729_ARM/typedef.h"
#include "va_g729_ARM/g729ab_if.h"

#else // _WIN32_WCE

#include "va_g729/va_g729.h"

// All of PWLib/OpenH323 use MSVCRT.LIB or MSVCRTD.LIB, but vag729a.lib uses
// libcmt.lib, so we need to tell the linker to ignore it, can't have two
// Run Time libraries!
#pragma comment(linker, "/NODEFAULTLIB:libcmt.lib")

static int encoderInUse = 0;
static int decoderInUse = 0;

#endif // _WIN32_WCE


#define SAMPLES_PER_FRAME    80
#define BYTES_PER_FRAME      10
#define MICROSECONDSPERFRAME 10000
#define BITS_PER_SECOND      8000


/////////////////////////////////////////////////////////////////////////////

static void * create_encoder(const struct PluginCodec_Definition * codec)
{
#ifdef _WIN32_WCE

  void * context = malloc(E_IF_g729ab_queryBlockSize());
  if (context == NULL)
    return NULL;

  if (E_IF_g729ab_init(context) == 0)
    return context;

  free(context);
  return NULL;

#else

  if (encoderInUse)
    return NULL;

  va_g729a_init_encoder();
  encoderInUse = 1;
  return (void *)1;

#endif
}


static int codec_encoder(const struct PluginCodec_Definition * codec, 
                                           void * context,
                                     const void * from, 
                                       unsigned * fromLen,
                                           void * to,         
                                       unsigned * toLen,
                                   unsigned int * flag)
{
  if (*fromLen < SAMPLES_PER_FRAME*2 || *toLen < BYTES_PER_FRAME)
    return 0;

#ifdef _WIN32_WCE
  {
    UWord8 buffer[BYTES_PER_FRAME+1];
    if (E_IF_g729ab_encode(context, (Word16 *)from, buffer, toLen, 0) != 0)
      return 0;
    memcpy(to, &buffer[1], BYTES_PER_FRAME);
  }
#else

  va_g729a_encoder((short *)from, (unsigned char *)to);

#endif

  *fromLen = SAMPLES_PER_FRAME*2;
  *toLen   = BYTES_PER_FRAME;

  return 1; 
}


static void destroy_encoder(const struct PluginCodec_Definition * codec, void * context)
{
#ifdef _WIN32_WCE
  free(context);
#else
  encoderInUse = 0;
#endif
}


static void * create_decoder(const struct PluginCodec_Definition * codec)
{
#ifdef _WIN32_WCE

  void * context = malloc(D_IF_g729ab_queryBlockSize());
  if (context == NULL)
    return NULL;

  if (D_IF_g729ab_init(context) == 0)
    return context;

  free(context);
  return NULL;

#else

  if (decoderInUse)
    return NULL;

  decoderInUse = 1;

  va_g729a_init_decoder();

  return (void *)1;

#endif
}

static int codec_decoder(const struct PluginCodec_Definition * codec, 
                                           void * context,
                                     const void * from, 
                                       unsigned * fromLen,
                                           void * to,         
                                       unsigned * toLen,
                                   unsigned int * flag)
{
  if (*fromLen < BYTES_PER_FRAME || *toLen < SAMPLES_PER_FRAME*2)
    return 0;

#ifdef _WIN32_WCE
  {
    UWord8 buffer[BYTES_PER_FRAME+1];
    buffer[0] = 2;
    memcpy(&buffer[1], from, BYTES_PER_FRAME);
    if (D_IF_g729ab_decode(context, buffer, (Word16 *)to, 0) != 0)
      return 0;
  }
#else

  va_g729a_decoder((unsigned char *)from, (short *)to, 0);

#endif

  *fromLen = BYTES_PER_FRAME;
  *toLen = SAMPLES_PER_FRAME*2;

  return 1;
}

static void destroy_decoder(const struct PluginCodec_Definition * codec, void * context)
{
#ifdef _WIN32_WCE
  free(context);
#else
  decoderInUse = 0;
#endif
}


/////////////////////////////////////////////////////////////////////////////

static struct PluginCodec_information licenseInfo = {
  1073619586,                              // timestamp = Fri 09 Jan 2004 03:39:46 AM UTC = 

  "Craig Southeren, Post Increment",                           // source code author
  "1.0",                                                       // source code version
  "craigs@postincrement.com",                                  // source code email
  "http://www.postincrement.com",                              // source code URL
  "Copyright (C) 2004 by Post Increment, All Rights Reserved", // source code copyright
  "MPL 1.0",                                                   // source code license
  PluginCodec_License_MPL,                                     // source code license

  "G.729A",                                                    // codec description
  "VoiceAge Corporation",                                      // codec author
  NULL,                                                        // codec version
  NULL,                                                        // codec email
  "http://www.voiceage.com",                                   // codec URL
  "Copyright (C) 1995-2001 VoiceAge Corporation. All Rights Reserved",  // codec copyright information
  "NULL",                                                      // codec license
  PluginCodec_License_ResearchAndDevelopmentUseOnly            // codec license code
};

static const char L16Desc[]  = { "L16" };

static const char g729Descr[]  = { "VoiceAge G729" };
static const char g729ADescr[] = { "VoiceAge G729A" };

static const char g729MediaFmt[]  = { "G.729" };
static const char g729AMediaFmt[] = { "G.729A" };

static const char ianaName[]   = { "G729" };
#define IANACODE  18

static struct PluginCodec_Definition g729CodecDefn[4] =
{
  { 
    // encoder
    PLUGIN_CODEC_VERSION,               // codec API version
    &licenseInfo,                       // license information

    PluginCodec_MediaTypeAudio |        // audio codec
    PluginCodec_InputTypeRaw |          // raw input data
    PluginCodec_OutputTypeRaw |         // raw output data
    PluginCodec_RTPTypeExplicit,        // explicit RTP type

    g729Descr,                          // text decription
    L16Desc,
    g729MediaFmt,

    NULL,                               // user data

    8000,                               // samples per second
    BITS_PER_SECOND,                    // raw bits per second
    MICROSECONDSPERFRAME,               // microseconds per frame
    SAMPLES_PER_FRAME,                  // samples per frame
    10,                                 // bytes per frame
    6,                                  // recommended number of frames per packet
    24,                                 // maximum number of frames per packet
    IANACODE,                           // IANA RTP payload code
    ianaName,                           // RTP Payload name

    create_encoder,                     // create codec function
    destroy_encoder,                    // destroy codec
    codec_encoder,                      // encode/decode
    NULL,                                // codec controls

    PluginCodec_H323AudioCodec_g729,    // h323CapabilityType
    NULL                                // h323CapabilityData
  },

  { 
    // decoder
    PLUGIN_CODEC_VERSION,               // codec API version
    &licenseInfo,                       // license information

    PluginCodec_MediaTypeAudio |        // audio codec
    PluginCodec_InputTypeRaw |          // raw input data
    PluginCodec_OutputTypeRaw |         // raw output data
    PluginCodec_RTPTypeExplicit,        // explicit RTP type

    g729Descr,                          // text decription
    g729MediaFmt,
    L16Desc,

    NULL,                               // user data

    8000,                               // samples per second
    BITS_PER_SECOND,                    // raw bits per second
    MICROSECONDSPERFRAME,               // microseconds per frame
    SAMPLES_PER_FRAME,                  // samples per frame
    BYTES_PER_FRAME,                    // bytes per frame
    6,                                  // recommended number of frames per packet
    24,                                 // maximum number of frames per packet
    IANACODE,                           // IANA RTP payload code
    ianaName,                           // RTP Payload name

    create_decoder,                     // create codec function
    destroy_decoder,                    // destroy codec
    codec_decoder,                      // encode/decode
    NULL,                                // codec controls

    PluginCodec_H323AudioCodec_g729,    // h323CapabilityType 
    NULL                                // h323CapabilityData
  },

  { 
    // encoder
    PLUGIN_CODEC_VERSION,                   // codec API version
    &licenseInfo,                           // license information

    PluginCodec_MediaTypeAudio |        // audio codec
    PluginCodec_InputTypeRaw |          // raw input data
    PluginCodec_OutputTypeRaw |         // raw output data
    PluginCodec_RTPTypeExplicit,        // explicit RTP type

    g729ADescr,                          // text decription
    L16Desc,
    g729AMediaFmt,

    NULL,                               // user data

    8000,                                   // samples per second
    BITS_PER_SECOND,                        // raw bits per second
    MICROSECONDSPERFRAME,                   // microseconds per frame
    SAMPLES_PER_FRAME,                      // samples per frame
    BYTES_PER_FRAME,                        // bytes per frame
    6,                                      // recommended number of frames per packet
    24,                                     // maximum number of frames per packet
    IANACODE,                           // IANA RTP payload code
    ianaName,                           // RTP Payload name

    create_encoder,                         // create codec function
    destroy_encoder,                        // destroy codec
    codec_encoder,                          // encode/decode
    NULL,                                // codec controls

    PluginCodec_H323AudioCodec_g729AnnexA,  // h323CapabilityType
    NULL                                    // h323CapabilityData
  },

  { 
    // decoder
    PLUGIN_CODEC_VERSION,                   // codec API version
    &licenseInfo,                           // license information

    PluginCodec_MediaTypeAudio |        // audio codec
    PluginCodec_InputTypeRaw |          // raw input data
    PluginCodec_OutputTypeRaw |         // raw output data
    PluginCodec_RTPTypeExplicit,        // explicit RTP type

    g729ADescr,                          // text decription
    g729AMediaFmt,
    L16Desc,

    NULL,                               // user data

    8000,                                   // samples per second
    BITS_PER_SECOND,                        // raw bits per second
    MICROSECONDSPERFRAME,                   // microseconds per frame
    SAMPLES_PER_FRAME,                      // samples per frame
    BYTES_PER_FRAME,                        // bytes per frame
    6,                                      // recommended number of frames per packet
    24,                                     // maximum number of frames per packet
    IANACODE,                           // IANA RTP payload code
    ianaName,                           // RTP Payload name

    create_decoder,                         // create codec function
    destroy_decoder,                        // destroy codec
    codec_decoder,                          // encode/decode
    NULL,                                // codec controls

    PluginCodec_H323AudioCodec_g729AnnexA,  // h323CapabilityType 
    NULL                                    // h323CapabilityData
  }
};


PLUGIN_CODEC_IMPLEMENT_ALL(VoiceAgeG729, g729CodecDefn, PLUGIN_CODEC_VERSION)

/////////////////////////////////////////////////////////////////////////////
