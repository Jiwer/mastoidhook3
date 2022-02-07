#include <iostream>

struct _ENetPeer;
struct _ENetHost;
struct _ENetChannel;
struct _iobuf;
struct CBlob;
struct CFileVtbl;

struct _ENetAddress
{
  unsigned int host;
  uint16_t port;
};

struct ENetProtocolCommandHeader
{
  char command;
  char channelID;
  char flags;
  char reserved;
  unsigned int commandLength;
  unsigned int reliableSequenceNumber;
};

struct ENetProtocolAcknowledge
{
  ENetProtocolCommandHeader header;
  unsigned int receivedReliableSequenceNumber;
  unsigned int receivedSentTime;
};

struct ENetProtocolConnect
{
  ENetProtocolCommandHeader header;
  uint16_t outgoingPeerID;
  uint16_t mtu;
  unsigned int windowSize;
  unsigned int channelCount;
  unsigned int incomingBandwidth;
  unsigned int outgoingBandwidth;
  unsigned int packetThrottleInterval;
  unsigned int packetThrottleAcceleration;
  unsigned int packetThrottleDeceleration;
};

struct ENetProtocolVerifyConnect
{
  ENetProtocolCommandHeader header;
  uint16_t outgoingPeerID;
  uint16_t mtu;
  unsigned int windowSize;
  unsigned int channelCount;
  unsigned int incomingBandwidth;
  unsigned int outgoingBandwidth;
  unsigned int packetThrottleInterval;
  unsigned int packetThrottleAcceleration;
  unsigned int packetThrottleDeceleration;
};


struct CString
{
  std::basic_string<char, std::char_traits<char>, std::allocator<char> > _str;
};

struct CPersistentVtbl
{
  /*void* (__fastcall* __vecDelDtor)(unsigned int);
  void(__fastcall* Write)(CWriter*);
  void(__fastcall* WriteMembers)(CWriter*);
  void(__fastcall* Read)(CReader*);
  void(__fastcall* ReadMember)(CReader*, int);
  void(__fastcall* InitPostRead)(CString*, int, int);
  void(__fastcall* InitPostRead)();*/
};

struct CPersistent
{
  CPersistentVtbl* vfptr;
  int _TypeToken;
};

struct CCommand : CPersistent
{
  bool _bLogged;
  int _nCommandSender;
  unsigned int _nRecipient;
  uint16_t _nRecipientPort;
  uint16_t _nTickStamp;
  bool _bTargetedAsynchronous;
  bool _bMayBeRemoved;
  unsigned int _nIdentity;
};

struct CAddPlayerCommand : CCommand
{
  CString _User;
  CString _Name;
  int _nMachineId;
  bool _bHotjoin;
  long long a6;
};

enum ERemovalReason
{
  REMOVE_PLAYER_DEFAULT = 0,
  REMOVE_PLAYER_CONNECTION_LOST = 1,
  REMOVE_PLAYER_KICKED = 2,
  REMOVE_PLAYER_BANNED = 3,
};

struct CRemovePlayerCommand : CCommand
{
  ERemovalReason _eReason;
  int _nMachineId;
  long long a4;
};

enum EFileType
{
  BINARY_FILE = 0x0,
  TEXT_FILE = 0x1,
};

enum EFileMode
{
  OPEN_READ = 0x0,
  OPEN_WRITE = 0x1,
  OPEN_WRITE_APPEND = 0x2,
};

struct CFile
{
  CFileVtbl *vfptr;
  int _nLineNo;
  char _szLastChar;
  bool _bUnget;
  EFileType _Type;
  EFileMode _Mode;
  CString _FileName;
};

struct CMemoryFile : CFile
{
  CBlob *_pFile;
  char *_pBuffer;
  int _nSize;
  int _nPos;
  int _nBufferSize;
  bool _bBad;
  bool _bDeleteBlobInDestructor;
};

struct CFileVtbl
{
  /*void *(__fastcall *__vecDelDtor)(CFile *this, unsigned int);
  _BYTE gap8[16];
  bool (__fastcall *ReOpen)(CFile *this, EFileType, EFileMode);
  bool (__fastcall *Read)(CFile *this, void *, int);
  bool (__fastcall *ReadString)(CFile *this, CBlob *);
  void (__fastcall *WriteString)(CFile *this, const char *);
  void (__fastcall *WriteByte)(CFile *this, char);
  void (__fastcall *WriteData)(CFile *this, const void *, unsigned __int64);
  bool (__fastcall *IsValid)(CFile *this);
  bool (__fastcall *SeekStart)(CFile *this, int);
  void (__fastcall *SeekEnd)(CFile *this);
  bool (__fastcall *Reset)(CFile *this);
  bool (__fastcall *Flush)(CFile *this);
  __int64 (__fastcall *GetSize)(CFile *this);
  __int64 (__fastcall *GetPosition)(CFile *this);
  _iobuf *(__fastcall *GetFilePointer)(CFile *this);
  int (__fastcall *CalculateChecksum)(CFile *this, int);*/
};

struct CPdxArrayCBlob
{
  char *_pData;
  int _nCapacity;
  int _nSize;
};

struct CBlob
{
  CPdxArrayCBlob _Data;
};

struct CGregorianDateVtbl
{
  /*void* (__fastcall* __vecDelDtor)(CGregorianDate* this, unsigned int);
  void(__fastcall* AddHours)(CGregorianDate* this, int);
  void(__fastcall* AddDays)(CGregorianDate* this, int);
  void(__fastcall* AddMonths)(CGregorianDate* this, int);
  void(__fastcall* AddYears)(CGregorianDate* this, int);*/
};

struct _iobuf
{
  void *_Placeholder;
};

struct CCountryTag
{
  int _nLinkIndex;
};

struct CDate
{
  int _nUniversalDate;
};

struct CGregorianDate : CDate
{
  CGregorianDateVtbl* vfptr;
};

struct CGameDate : CGregorianDate, CPersistent
{
};

struct CSaveFile
{
  CString _Path;
  CString _SaveName;
  int64_t _ModifiedDate;
  int _Version;
  int _FileSize;
  bool _IsValid;
  bool _IsRemote;
  bool _IsSynced;
};

struct CSaveGameMeta : CSaveFile
{
  CCountryTag _CountryTag;
  CString _CosmeticTag;
  CGameDate _GameDate;
  void* _PlayerCountries;
  void* _FlagsMask;
  void* _pIdeology;
  int _nDifficulty;
  unsigned int _nDlcMask;
  void* _Mods;
};