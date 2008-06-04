#ifndef __ZRTPUDP_H
#define __ZRTPUDP_H

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#include <ptlib.h>
#include <opal/rtpconn.h>

class OpalZrtp_UDP : public SecureRTP_UDP
{
    PCLASSINFO(OpalZrtp_UDP, SecureRTP_UDP);
  public:
    OpalZrtp_UDP(
      const PString & encapsulation,       ///<  identifies initial RTP encapsulation (RTP/AVP, UDPTL etc)
#if OPAL_RTP_AGGREGATE
      PHandleAggregator * aggregator, ///< RTP aggregator
#endif
      unsigned id,                    ///<  Session ID for RTP channel
      PBoolean remoteIsNAT                ///<  TRUE is remote is behind NAT
    );

    virtual ~OpalZrtp_UDP();

    virtual PBoolean WriteZrtpData(RTP_DataFrame & frame);

    virtual SendReceiveStatus OnSendData(RTP_DataFrame & frame);
    virtual SendReceiveStatus OnReceiveData(RTP_DataFrame & frame);
    virtual SendReceiveStatus OnSendControl(RTP_ControlFrame & frame, PINDEX & len);
    virtual SendReceiveStatus OnReceiveControl(RTP_ControlFrame & frame);
    virtual DWORD GetOutgoingSSRC();

  public:
    zrtp_stream_ctx_t *zrtpStream;
};


class OpalZrtpSecurityMode : public OpalSecurityMode
{
  PCLASSINFO(OpalZrtpSecurityMode, OpalSecurityMode);
};

class LibZrtpSecurityMode_Base : public OpalZrtpSecurityMode
{
    PCLASSINFO(LibZrtpSecurityMode_Base, OpalZrtpSecurityMode);
  public:
    LibZrtpSecurityMode_Base();
    ~LibZrtpSecurityMode_Base();

    RTP_UDP * CreateRTPSession(
#if OPAL_RTP_AGGREGATE
      PHandleAggregator *_aggregator,   ///< handle aggregator
#endif
      unsigned id,          ///<  Session ID for RTP channel
      PBoolean remoteIsNAT, ///<  TRUE is remote is behind NAT
      OpalConnection & conn
    );

    PBoolean Open();

    zrtp_profile_t *GetZrtpProfile();

    zrtp_conn_ctx_t	* zrtpSession;

  protected:
    // last element of each array mush be 0
    void Init(int *sas, int *pk, int *auth, int *cipher, int *hash);
    zrtp_profile_t *profile;
};


#endif //__ZRTPUDP_H
