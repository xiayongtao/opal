/*
 * Inter Asterisk Exchange 2
 * 
 * The entity which receives all manages weirdo iax2 packets that are 
 * sent outside of a regular call.
 * 
 * Open Phone Abstraction Library (OPAL)
 *
 * Copyright (c) 2006 Stephen Cook 
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
 * The Original Code is Open Phone Abstraction Library.
 *
 * The Initial Developer of the Original Code is Indranet Technologies Ltd
 *
 * The author of this code is Stephen Cook
 *
 *  $Log: jitter.h,v $
 *  Revision 1.1  2007/01/11 03:02:15  dereksmithies
 *  Remove the previous audio buffering code, and switch to using the jitter
 *  buffer provided in Opal. Reduce the verbosity of the log mesasges.
 *
 *  Revision 1.2  2006/09/11 03:12:51  dereksmithies
 *  Add logging and MPL license statements.
 *
 *
 */

#ifndef SPECIALPROCESSOR_H
#define SPECIALPROCESSOR_H

#include <ptlib.h>
#include <opal/connection.h>

#include <iax2/processor.h>
#include <iax2/frame.h>
#include <iax2/iedata.h>
#include <iax2/remote.h>
#include <iax2/safestrings.h>
#include <iax2/sound.h>

/**This is the special processor which is created to handle the weirdo iax2 packets
   that are sent outside of a particular call. Examples of weirdo packets are the
   ping/pong/lagrq/lagrp.
  */
class IAX2SpecialProcessor : public IAX2Processor
{
  PCLASSINFO(IAX2SpecialProcessor, IAX2Processor);
  
 public:
  /**Construct this class */
  IAX2SpecialProcessor(IAX2EndPoint & ep);

  /**Destructor */
  virtual ~IAX2SpecialProcessor();
  
 protected:
  /**Go through the three lists for incoming data (ethernet/sound/UI commands.  */
  virtual void ProcessLists();
  
  /**Processes a full frame*/
  virtual void ProcessFullFrame(IAX2FullFrame & fullFrame);
  
  /**Processes are mini frame*/
  virtual void ProcessNetworkFrame(IAX2MiniFrame * src);
  
  /**Print information about the class on to a stream*/
  virtual void PrintOn(ostream & strm) const;
  
  /**Called when there is no response to a request*/
  virtual void OnNoResponseTimeout();
  
  /**Process an IAX2FullFrameProtocol */
  void ProcessNetworkFrame(IAX2FullFrameProtocol * src);
  
  /**Process a poke command*/
  void ProcessIaxCmdPoke(IAX2FullFrameProtocol * src);
};

#endif /*SPECIALPROCESSOR_H*/
