/*
 * main.h
 *
 * An OPAL GUI phone application.
 *
 * Open Phone Abstraction Library (OPAL)
 * Formally known as the Open H323 project.
 *
 * Copyright (c) 2004 Post Increment
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
 * The Original Code is Open Phone client.
 *
 * The Initial Developer of the Original Code is Post Increment
 *
 * Contributor(s): ______________________________________.
 *
 * $Revision$
 * $Author$
 * $Date$
 */

#ifndef _OpenPhone_MAIN_H
#define _OpenPhone_MAIN_H

#ifndef _PTLIB_H
#include <ptlib.h>
#endif

#include <wx/wx.h>
#include <wx/dataobj.h>

#include <opal/manager.h>
#include <opal/pcss.h>

#if OPAL_H323
#include <h323/h323.h>
#include <h323/gkclient.h>
#endif

#if OPAL_SIP
#include <sip/sip.h>
#endif


#include <list>


class MyManager;

class OpalPOTSEndPoint;
class OpalIVREndPoint;

class wxSplitterWindow;
class wxSplitterEvent;
class wxListCtrl;
class wxListEvent;
class wxGrid;


class PwxString : public wxString
{
  public:
    PwxString() { }
    PwxString(const wxString & str) : wxString(str) { }
    PwxString(const PString & str) : wxString((const char *)str) { }
    PwxString(const char * str) : wxString(str) { }

    PwxString & operator=(const wxString & str) { wxString::operator=(str); return *this; }
    PwxString & operator=(const PString & str) { wxString::operator=((const char *)str); return *this; }
    PwxString & operator=(const char * str) { wxString::operator=(str); return *this; }

    operator PString() const { return c_str(); }
    operator PIPSocket::Address() const { return PIPSocket::Address(PString(c_str())); }
    friend ostream & operator<<(ostream & stream, const PwxString & string) { return stream << string.c_str(); }

    bool operator==(const OpalMediaFormat & other) const { return IsSameAs((const char *)other); }
};


class MyPCSSEndPoint : public OpalPCSSEndPoint
{
  PCLASSINFO(MyPCSSEndPoint, OpalPCSSEndPoint);

  public:
    MyPCSSEndPoint(MyManager & manager);

  private:
    virtual BOOL OnShowIncoming(const OpalPCSSConnection & connection);
    virtual BOOL OnShowOutgoing(const OpalPCSSConnection & connection);

    MyManager & m_manager;
};


#if OPAL_H323
class MyH323EndPoint : public H323EndPoint
{
  public:
    MyH323EndPoint(MyManager & manager);

  private:
    virtual void OnRegistrationConfirm();

    MyManager & m_manager;
};
#endif


#if OPAL_SIP
class MySIPEndPoint : public SIPEndPoint
{
  public:
    MySIPEndPoint(MyManager & manager);

  private:
    virtual void OnRegistrationStatus(
      const PString & aor,
      BOOL wasRegistering,
      BOOL reRegistering,
      SIP_PDU::StatusCodes reason
    );

    MyManager & m_manager;
};
#endif


class CallDialog : public wxDialog
{
  public:
    CallDialog(wxFrame * parent);

    PwxString m_Address;

  private:
    void OnOK(wxCommandEvent & event);
    void OnAddressChange(wxCommandEvent & event);

    wxComboBox * m_AddressCtrl;
    wxButton   * m_ok;

    DECLARE_EVENT_TABLE()
};


class CallingPanel : public wxPanel
{
  public:
    CallingPanel(MyManager & manager, wxWindow * parent);

  private:
    void OnHangUp(wxCommandEvent & event);

    MyManager & m_manager;

    DECLARE_EVENT_TABLE()
};


class AnswerPanel : public wxPanel
{
  public:
    AnswerPanel(MyManager & manager, wxWindow * parent);

  private:
    void OnAnswer(wxCommandEvent & event);
    void OnReject(wxCommandEvent & event);

    MyManager & m_manager;

    DECLARE_EVENT_TABLE()
};


class InCallPanel : public wxPanel
{
  public:
    InCallPanel(MyManager & manager, wxWindow * parent);
    virtual bool Show(bool show = true);

  private:
    void OnHangUp(wxCommandEvent & event);
    void OnUserInput1(wxCommandEvent & event);
    void OnUserInput2(wxCommandEvent & event);
    void OnUserInput3(wxCommandEvent & event);
    void OnUserInput4(wxCommandEvent & event);
    void OnUserInput5(wxCommandEvent & event);
    void OnUserInput6(wxCommandEvent & event);
    void OnUserInput7(wxCommandEvent & event);
    void OnUserInput8(wxCommandEvent & event);
    void OnUserInput9(wxCommandEvent & event);
    void OnUserInput0(wxCommandEvent & event);
    void OnUserInputStar(wxCommandEvent & event);
    void OnUserInputHash(wxCommandEvent & event);
    void OnUserInputFlash(wxCommandEvent & event);
    void OnUpdateVU(wxTimerEvent& event);

    void SpeakerVolume(wxScrollEvent & event);
    void MicrophoneVolume(wxScrollEvent & event);
    void SetVolume(bool microphone, int value);

    MyManager & m_manager;
    wxSlider  * m_SpeakerVolume;
    wxSlider  * m_MicrophoneVolume;
    wxGauge   * m_vuSpeaker;
    wxGauge   * m_vuMicrophone;
    wxTimer     m_vuTimer;
    bool        m_FirstTime;

    DECLARE_EVENT_TABLE()
};


class SpeedDialDialog : public wxDialog
{
  public:
    SpeedDialDialog(MyManager *parent);

    wxString m_Name;
    wxString m_Number;
    wxString m_Address;
    wxString m_Description;

  private:
    void OnChange(wxCommandEvent & event);

    MyManager & m_manager;

    wxButton     * m_ok;
    wxTextCtrl   * m_nameCtrl;
    wxTextCtrl   * m_numberCtrl;
    wxTextCtrl   * m_addressCtrl;
    wxStaticText * m_inUse;
    wxStaticText * m_ambiguous;

    DECLARE_EVENT_TABLE()
};


#if OPAL_SIP

class RegistrarInfo
{
  public:
    RegistrarInfo()
      : m_Active(false)
      , m_TimeToLive(300)
    {
    }

    bool operator==(const RegistrarInfo & other) const
    {
      return m_Active     == other.m_Active &&
             m_Domain     == other.m_Domain &&
             m_User       == other.m_User &&
             m_Password   == other.m_Password &&
             m_TimeToLive == other.m_TimeToLive &&
             m_Proxy      == other.m_Proxy;
    }

    bool      m_Active;
    PwxString m_Domain;
    PwxString m_User;
    PwxString m_Password;
    int       m_TimeToLive;
    PwxString m_Proxy;
};

typedef list<RegistrarInfo> RegistrarList;

#endif


class OptionsDialog : public wxDialog
{
  public:
    OptionsDialog(MyManager *parent);
    ~OptionsDialog();
    virtual bool TransferDataFromWindow();

  private:
    MyManager & m_manager;

    ////////////////////////////////////////
    // General fields
    PwxString m_Username;
    PwxString m_DisplayName;
    PwxString m_RingSoundDeviceName;
    PwxString m_RingSoundFileName;
    bool      m_AutoAnswer;
#if P_EXPAT
    PwxString m_IVRScript;
#endif

    void BrowseSoundFile(wxCommandEvent & event);
    void PlaySoundFile(wxCommandEvent & event);

    ////////////////////////////////////////
    // Networking fields
    wxString     m_Bandwidth;
    int          m_TCPPortBase;
    int          m_TCPPortMax;
    int          m_UDPPortBase;
    int          m_UDPPortMax;
    int          m_RTPPortBase;
    int          m_RTPPortMax;
    int          m_RTPTOS;
    PwxString    m_STUNServer;
    PwxString    m_NATRouter;
    wxListBox  * m_LocalInterfaces;
    wxTextCtrl * m_InterfaceToAdd;
    wxButton   * m_AddInterface;
    wxButton   * m_RemoveInterface;
    void BandwidthClass(wxCommandEvent & event);
    void SelectedLocalInterface(wxCommandEvent & event);
    void ChangedInterfaceToAdd(wxCommandEvent & event);
    void AddInterface(wxCommandEvent & event);
    void RemoveInterface(wxCommandEvent & event);

    ////////////////////////////////////////
    // Sound fields
    PwxString m_SoundPlayer;
    PwxString m_SoundRecorder;
    int       m_SoundBuffers;
    PwxString m_LineInterfaceDevice;
    int       m_AEC;
    PwxString m_Country;
    int       m_MinJitter;
    int       m_MaxJitter;
    int       m_SilenceSuppression;
    int       m_SilenceThreshold;
    int       m_SignalDeadband;
    int       m_SilenceDeadband;

    wxComboBox * m_selectedLID;
    wxChoice   * m_selectedAEC;
    wxComboBox * m_selectedCountry;
    void SelectedLID(wxCommandEvent & event);

    ////////////////////////////////////////
    // Video fields
    PwxString m_VideoGrabber;
    int       m_VideoGrabFormat;
    int       m_VideoGrabSource;
    int       m_VideoGrabFrameRate;
    PwxString m_VideoGrabFrameSize;
    bool      m_VideoGrabPreview;
    bool      m_VideoFlipLocal;
    bool      m_VideoAutoTransmit;
    bool      m_VideoAutoReceive;
    bool      m_VideoFlipRemote;
    PwxString m_VideoMinFrameSize;
    PwxString m_VideoMaxFrameSize;

    ////////////////////////////////////////
    // Codec fields
    wxButton   * m_AddCodec;
    wxButton   * m_RemoveCodec;
    wxButton   * m_MoveUpCodec;
    wxButton   * m_MoveDownCodec;
    wxListBox  * m_allCodecs;
    wxListBox  * m_selectedCodecs;
    wxListCtrl * m_codecOptions;
    wxTextCtrl * m_codecOptionValue;

    void AddCodec(wxCommandEvent & event);
    void RemoveCodec(wxCommandEvent & event);
    void MoveUpCodec(wxCommandEvent & event);
    void MoveDownCodec(wxCommandEvent & event);
    void SelectedCodecToAdd(wxCommandEvent & event);
    void SelectedCodec(wxCommandEvent & event);
    void SelectedCodecOption(wxListEvent & event);
    void DeselectedCodecOption(wxListEvent & event);
    void ChangedCodecOptionValue(wxCommandEvent & event);

    ////////////////////////////////////////
    // H.323 fields
    int          m_GatekeeperMode;
    PwxString    m_GatekeeperAddress;
    PwxString    m_GatekeeperIdentifier;
    int          m_GatekeeperTTL;
    PwxString    m_GatekeeperLogin;
    PwxString    m_GatekeeperPassword;
    int          m_DTMFSendMode;
    int          m_CallIntrusionProtectionLevel;
    bool         m_DisableFastStart;
    bool         m_DisableH245Tunneling;
    bool         m_DisableH245inSETUP;
    wxListBox  * m_Aliases;
    wxTextCtrl * m_NewAlias;
    wxButton   * m_AddAlias;
    wxButton   * m_RemoveAlias;

    void SelectedAlias(wxCommandEvent & event);
    void ChangedNewAlias(wxCommandEvent & event);
    void AddAlias(wxCommandEvent & event);
    void RemoveAlias(wxCommandEvent & event);

    ////////////////////////////////////////
    // SIP fields
#if OPAL_SIP
    bool      m_SIPProxyUsed;
    PwxString m_SIPProxy;
    PwxString m_SIPProxyUsername;
    PwxString m_SIPProxyPassword;

    wxListCtrl * m_Registrars;
    int          m_SelectedRegistrar;
    wxButton   * m_AddRegistrar;
    wxButton   * m_ChangeRegistrar;
    wxButton   * m_RemoveRegistrar;
    wxTextCtrl * m_RegistrarDomain;
    wxTextCtrl * m_RegistrarUser;
    wxTextCtrl * m_RegistrarPassword;
    wxSpinCtrl * m_RegistrarTimeToLive;
    wxCheckBox * m_RegistrarActive;

    void FieldsToRegistrar(RegistrarInfo & info);
    void RegistrarToList(bool overwrite, RegistrarInfo * registrar, int position);
    void AddRegistrar(wxCommandEvent & event);
    void ChangeRegistrar(wxCommandEvent & event);
    void RemoveRegistrar(wxCommandEvent & event);
    void SelectedRegistrar(wxListEvent & event);
    void DeselectedRegistrar(wxListEvent & event);
    void ChangedRegistrarInfo(wxCommandEvent & event);
#endif

    ////////////////////////////////////////
    // Routing fields
    wxListCtrl * m_Routes;
    int          m_SelectedRoute;
    wxComboBox * m_RouteSource;
    wxTextCtrl * m_RoutePattern;
    wxTextCtrl * m_RouteDestination;
    wxButton   * m_AddRoute;
    wxButton   * m_RemoveRoute;

    void AddRoute(wxCommandEvent & event);
    void RemoveRoute(wxCommandEvent & event);
    void SelectedRoute(wxListEvent & event);
    void DeselectedRoute(wxListEvent & event);
    void ChangedRouteInfo(wxCommandEvent & event);

#if PTRACING
    ////////////////////////////////////////
    // Tracing fields
    bool      m_EnableTracing;
    int       m_TraceLevelThreshold;
    bool      m_TraceLevelNumber;
    bool      m_TraceFileLine;
    bool      m_TraceBlocks;
    bool      m_TraceDateTime;
    bool      m_TraceTimestamp;
    bool      m_TraceThreadName;
    bool      m_TraceThreadAddress;
    PwxString m_TraceFileName;
#endif

    DECLARE_EVENT_TABLE()
};


class MyMedia
{
public:
  MyMedia()
    : sourceProtocol(NULL),
      preferenceOrder(-1), // -1 indicates disabled
      dirty(false)
  { }

  MyMedia(
    const char * source,
    const PString & format
  ) : sourceProtocol(source),
      mediaFormat(format),
      preferenceOrder(-1), // -1 indicates disabled
      dirty(false)
  { }

  bool operator<(const MyMedia & other) { return preferenceOrder < other.preferenceOrder; }

  const char    * sourceProtocol;
  OpalMediaFormat mediaFormat;
  int             preferenceOrder;
  bool            dirty;
};

typedef std::list<MyMedia> MyMediaList;


class MyManager : public wxFrame, public OpalManager
{
  public:
    MyManager();
    ~MyManager();

    bool Initialise();

    bool HasSpeedDialName(const wxString & name) const;
    int  GetSpeedDialIndex(const char * number, const char * ignore) const;

    void MakeCall(const PwxString & address);
    void AnswerCall();
    void RejectCall();
    void HangUpCall();
    void SendUserInput(char tone);
    void OnRinging(const OpalPCSSConnection & connection);

    PSafePtr<OpalCall> GetCall() { return FindCallWithLock(m_currentCallToken); }
    PSafePtr<OpalConnection> GetUserConnection();

  private:
    // OpalManager overrides
    virtual BOOL OnIncomingConnection(
      OpalConnection & connection
    );
    virtual void OnEstablishedCall(
      OpalCall & call   /// Call that was completed
    );
    virtual void OnClearedCall(
      OpalCall & call   /// Connection that was established
    );
    virtual BOOL OnOpenMediaStream(
      OpalConnection & connection,  /// Connection that owns the media stream
      OpalMediaStream & stream    /// New media stream being opened
    );
    virtual void OnClosedMediaStream(
      const OpalMediaStream & stream     ///<  Stream being closed
    );
    virtual void OnUserInputString(
      OpalConnection & connection,  /// Connection input has come from
      const PString & value         /// String value of indication
    );
    virtual PString ReadUserInput(
      OpalConnection & connection,        ///<  Connection to read input from
      const char * terminators = "#\r\n", ///<  Characters that can terminte input
      unsigned lastDigitTimeout = 4,      ///<  Timeout on last digit in string
      unsigned firstDigitTimeout = 30     ///<  Timeout on receiving any digits
    );
    virtual BOOL CreateVideoOutputDevice(
      const OpalConnection & connection,    ///<  Connection needing created video device
      const OpalMediaFormat & mediaFormat,  ///<  Media format for stream
      BOOL preview,                         ///<  Flag indicating is a preview output
      PVideoOutputDevice * & device,        ///<  Created device
      BOOL & autoDelete                     ///<  Flag for auto delete device
    );


    void OnClose(wxCloseEvent& event);
    void OnLogMessage(wxCommandEvent & event);
    void OnAdjustMenus(wxMenuEvent& event);
    void OnStateChange(wxCommandEvent & event);

    void OnMenuQuit(wxCommandEvent& event);
    void OnMenuAbout(wxCommandEvent& event);
    void OnMenuCall(wxCommandEvent& event);
    void OnMenuAnswer(wxCommandEvent& event);
    void OnMenuHangUp(wxCommandEvent& event);
    void OnNewSpeedDial(wxCommandEvent& event);
    void OnViewLarge(wxCommandEvent& event);
    void OnViewSmall(wxCommandEvent& event);
    void OnViewList(wxCommandEvent& event);
    void OnViewDetails(wxCommandEvent& event);
    void OnEditSpeedDial(wxCommandEvent& event);
    void OnCutSpeedDial(wxCommandEvent& event);
    void OnCopySpeedDial(wxCommandEvent& event);
    void OnPasteSpeedDial(wxCommandEvent& event);
    void OnDeleteSpeedDial(wxCommandEvent& event);
    void OnOptions(wxCommandEvent& event);
    void OnSashPositioned(wxSplitterEvent& event);
    void OnSpeedDialActivated(wxListEvent& event);
    void OnSpeedDialColumnResize(wxListEvent& event);
    void OnRightClick(wxListEvent& event);


    enum SpeedDialViews {
      e_ViewLarge,
      e_ViewSmall,
      e_ViewList,
      e_ViewDetails,
      e_NumViews
    };

    void RecreateSpeedDials(
      SpeedDialViews view
    );
    void EditSpeedDial(
      int index,
      bool newItem
    );

    enum {
      e_NameColumn,
      e_NumberColumn,
      e_AddressColumn,
      e_DescriptionColumn,
      e_NumColumns
    };

    // Controls on main frame
    enum {
      SplitterID = 100,
      SpeedDialsID
    };

    wxSplitterWindow * m_splitter;
    wxTextCtrl       * m_logWindow;
    wxListCtrl       * m_speedDials;
    wxImageList      * m_imageListNormal;
    wxImageList      * m_imageListSmall;
    wxPanel          * m_answerPanel;
    wxPanel          * m_callingPanel;
    wxPanel          * m_inCallPanel;
    wxDataFormat       m_ClipboardFormat;

    MyPCSSEndPoint   * pcssEP;
    OpalPOTSEndPoint * potsEP;

    PStringArray     m_LocalInterfaces;
    void StartAllListeners();

#if OPAL_H323
    MyH323EndPoint * h323EP;
    int              m_gatekeeperMode;
    PwxString        m_gatekeeperAddress;
    PwxString        m_gatekeeperIdentifier;
    bool StartGatekeeper();
#endif

#if OPAL_SIP
    MySIPEndPoint * sipEP;
    bool            m_SIPProxyUsed;
    RegistrarList   m_registrars;
    void StartRegistrars();
    void StopRegistrars();
#endif

#if P_EXPAT
    OpalIVREndPoint  * ivrEP;
#endif

    bool m_autoAnswer;

    bool m_VideoGrabPreview;
    int  m_localVideoFrameX;
    int  m_localVideoFrameY;
    int  m_remoteVideoFrameX;
    int  m_remoteVideoFrameY;
    PwxString m_VideoGrabFrameSize;
    PwxString m_VideoMinFrameSize;
    PwxString m_VideoMaxFrameSize;
    bool AdjustFrameSize();

    MyMediaList m_mediaInfo;
    void InitMediaInfo(const char * source, const OpalMediaFormatList & formats);
    void ApplyMediaInfo();

#if PTRACING
    bool      m_enableTracing;
    wxString  m_traceFileName;
#endif

    enum CallState {
      IdleState,
      CallingState,
      RingingState,
      AnsweringState,
      InCallState
    } m_callState;
    void SetState(CallState newState);

    PString m_ringingConnectionToken;
    PString m_currentCallToken;

    PwxString     m_RingSoundDeviceName;
    PwxString     m_RingSoundFileName;
    PSoundChannel m_RingSoundChannel;
    PTimer        m_RingSoundTimer;
    PDECLARE_NOTIFIER(PTimer, MyManager, OnRingSoundAgain);
    void StopRingSound();

    DECLARE_EVENT_TABLE()

  friend class OptionsDialog;
};


class OpenPhoneApp : public wxApp, public PProcess
{
    PCLASSINFO(OpenPhoneApp, PProcess);

  public:
    OpenPhoneApp();

    void Main(); // Dummy function

      // FUnction from wxWindows
    virtual bool OnInit();
};


#endif


// End of File ///////////////////////////////////////////////////////////////
