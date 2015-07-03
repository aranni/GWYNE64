//---------------------------------------------------------------------------

#ifndef IMP_MainH
#define IMP_MainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Sockets.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <ScktComp.hpp>
#include <Mask.hpp>

#include "IMP_Msgdef.h"
#include <Dialogs.hpp>
#include <ImgList.hpp>
#include <Buttons.hpp>
#include <ToolWin.hpp>
#include <Grids.hpp>
#include <ValEdit.hpp>
#include <Menus.hpp>
#include <jpeg.hpp>
#include <NMUDP.hpp>


const UINT8 ConfigSerialPort[] = {
  0x10, 	                            // 8-O-1
  0x20, 	                            // 8-E-1
  0x30, 	                            // 8-N-1
  0x50, 	                            // 7-O-1
  0x60, 	                            // 7-E-1
  0x70   	                            // 7-N-1
  };


//---------------------------------------------------------------------------
class TGWY_Config : public TForm
{
__published:	// IDE-managed Components
        TPageControl *IMP_PageControl;
        TTabSheet *TabSheet_Config;
        TTabSheet *TabSheet_TCPConfig;
        TTabSheet *TabSheet_SerialConfig;
        TStatusBar *StatusBar_CFG;
        TClientSocket *IMP_ClientSocket;
        TTabSheet *TabSheet_Statistic;
        TStaticText *StaticText_SRL_ScanRate;
        TEdit *Edit_SRL_ScanRate;
        TBevel *Bevel_SRL_PollingFrec;
        TPanel *Panel1;
        TStaticText *StaticText_TxOk;
        TEdit *Edit_TxOk;
        TEdit *Edit_TxErr;
        TStaticText *StaticText_TxErr;
        TStaticText *StaticText_RxOk;
        TEdit *Edit_RxOk;
        TEdit *Edit_RxErr;
        TStaticText *StaticText_RxErr;
        TStaticText *StaticText_EvtRst;
        TEdit *Edit_EvtRst;
        TEdit *Edit_EvtQMax;
        TStaticText *StaticText_EvtQMax;
        TTimer *Timer1;
        TTabSheet *TabSheet_Constantes;
        TEdit *Edit_UsrText;
        TStaticText *StaticText_UsrText;
        TTabSheet *TabSheet_Version;
        TEdit *Edit_FwVersion;
        TStaticText *StaticText_FwVersion;
        TStaticText *StaticText_HwVersion;
        TEdit *Edit_HwVersion;
        TEdit *Edit_Serial;
        TStaticText *StaticText1;
        TBevel *Bevel_Constantes;
        TEdit *Edit_ConsignaFrec;
        TEdit *Edit_RelacionTx;
        TEdit *Edit_CoefResbal;
        TEdit *Edit_RpmEnMotor;
        TStaticText *StaticText_RpmEnMotor;
        TStaticText *StaticText_CoefResbal;
        TStaticText *StaticText_ConsignaFrec;
        TEdit *Edit_RstCntr;
        TStaticText *StaticText_RstCntr;
        TStaticText *StaticText_DogCntr;
        TEdit *Edit_DogCntr;
        TPanel *Panel__RunCfg;
        TCheckBox *CheckBox_RunCfgRTC;
        TCheckBox *CheckBox_RunCfgRTU;
    TPanel *Panel_FechaHora;
        TStaticText *StaticText_ConnectIP;
        TPanel *Panel_Variador;
        TStaticText *StaticText_InvCode;
        TPanel *Panel_InvCode;
        TRadioButton *RadioButton_HitachiJ300;
        TRadioButton *RadioButton_SieMicroMaster;
        TStaticText *StaticText_SIE_CmdWordA;
        TEdit *Edit_SIE_CmdWordA;
        TButton *Button_Sincronizar;
        TStaticText *StaticText_Fecha;
        TEdit *Edit_FechaDia;
        TEdit *Edit_FechaMes;
        TEdit *Edit_FechaAnio;
        TStaticText *StaticText_Hora;
        TEdit *Edit_HoraHora;
        TEdit *Edit_HoraMinuto;
        TEdit *Edit_HoraSegundo;
        TStaticText *StaticText6;
        TRadioButton *RadioButton_SieMasterDrive;
        TRadioButton *RadioButton_HitachiSJ300;
        TEdit *Edit_RxOkPercent;
        TEdit *Edit_RxErrPercent;
        TTimer *Timer2;
        TTimer *Timer3;
        TTabSheet *TabSheet_Actualizar;
        TEdit *Edit_ArchivoFW;
        TButton *Button_Transferir;
        TButton *Button_Verificar;
        TEdit *Edit_Registros;
        TLabel *Label1;
        TOpenDialog *OpenDialog1;
        TLabel *Label2;
        TButton *Button_BuscarArchivo;
        TProgressBar *ProgressBar1;
        TEdit *Edit_Version;
        TLabel *Label3;
        TButton *Button_ActualizarFW;
        TButton *Button_Validate;
        TBevel *Bevel6;
        TEdit *Edit_VersionDisponible;
        TEdit *Edit_VersionDeFabrica;
        TEdit *Edit_DisponibleFecha;
        TEdit *Edit_DisponibleHora;
        TEdit *Edit_FabricaFecha;
        TEdit *Edit_FabricaHora;
        TLabel *Label7;
        TLabel *Label8;
        TLabel *Label9;
        TEdit *Edit_FechaCarga;
        TEdit *Edit_HoraCarga;
        TLabel *Label10;
        TEdit *Edit_FechaFW;
        TStaticText *StaticText_FechaFW;
        TEdit *Edit_HoraFW;
        TStaticText *StaticText_HoraFW;
        TBevel *Bevel_VersionHW;
        TBevel *Bevel7;
        TButton *Button_Recuperar;
        TTimer *Timer_Reconect;
        TTabSheet *TabSheet_Puertos;
        TStaticText *StaticText_AAG_Inps;
        TEdit *Edit_AAG_Inp1;
        TBevel *Bevel8;
        TEdit *Edit_AAG_Inp2;
        TStaticText *StaticText_AAG_Outs;
        TEdit *Edit_AAG_OutSet2;
        TEdit *Edit_AAG_OutSet1;
        TBevel *Bevel9;
        TButton *Button_SetearSalidas;
        TPanel *Panel_EEPROM;
        TEdit *Edit_E2P_Data;
        TEdit *Edit_E2P_Addr;
        TEdit *Edit_E2P_Len;
        TLabel *Label12;
        TLabel *Label11;
        TButton *Button_LeerEEPROM;
        TPanel *Panel_Temperatura;
        TEdit *Edit_Temperatura;
        TStaticText *StaticText_Temperatura;
        TPanel *Panel_UpdateFactory;
        TRadioButton *RadioButton_Upgrade;
        TRadioButton *RadioButton_Factory;
        TPanel *Panel_EntradasDigitales;
        TCheckBox *CheckBox_DigInp_6;
        TCheckBox *CheckBox_DigInp_5;
        TCheckBox *CheckBox_DigInp_4;
        TCheckBox *CheckBox_DigInp_3;
        TCheckBox *CheckBox_DigInp_2;
        TCheckBox *CheckBox_DigInp_1;
        TStaticText *StaticText_DigInpVal;
        TEdit *Edit_AAG_Out1;
        TEdit *Edit_AAG_Out2;
        TPanel *Panel_SalidasDigitales;
        TCheckBox *CheckBox_DigOut_1;
        TCheckBox *CheckBox_DigOut_2;
        TCheckBox *CheckBox_DigOut_3;
        TCheckBox *CheckBox_DigOut_4;
        TCheckBox *CheckBox_DigOut_5;
        TCheckBox *CheckBox_DigOut_6;
        TStaticText *StaticText11;
        TStaticText *StaticText13;
        TEdit *Edit_ConnectIP;
        TTimer *Timer_Disconnect;
        TTabSheet *TabSheet_Web;
        TEdit *Edit_ArchivoWeb;
        TButton *Button_BuscarArchivoWeb;
        TLabel *Label_ArchivoWeb;
        TOpenDialog *OpenDialog2;
        TMemo *Memo_WebFiles;
        TButton *Button_WebArgregar;
        TOpenDialog *OpenDialog3;
        TEdit *Edit_WebFilesQtty;
        TButton *Button_WebGrabar;
        TButton *Button_TransferirWeb;
        TProgressBar *ProgressBar2;
        TLabel *Label14;
        TLabel *Label15;
        TEdit *Edit_WebRegQtty;
        TButton *Button_ReadFlash;
        TProgressBar *ProgressBar3;
    TPanel *Panel_Dnp;
    TStaticText *StaticText15;
    TPanel *Panel3;
    TStaticText *StaticText16;
    TEdit *Edit_AddrDnpMaestro;
    TStaticText *StaticText17;
    TEdit *Edit_AddrDnpEsclavo;
    TTabSheet *TabSeet_DNP3;
    TPanel *Panel_Arbol;
    TTreeView *TreeView1;
    TPanel *Panel_Show;
    TImageList *ImageList1;
    TToolBar *ToolBar1;
    TSpeedButton *SpeedButton_Conectar;
    TSpeedButton *SpeedButton_Reset;
    TPanel *Panel_ToolBar;
    TBitBtn *BitBtn_Grabar;
    TBitBtn *BitBtn_Cancelar;
    TBitBtn *BitBtn_Ayuda;
    TPanel *Panel_CommonCtrl;
    TSpeedButton *SpeedButton_Desconectar;
    TValueListEditor *ValueList_TabNames;
    TShape *Shape1;
    TShape *Shape2;
    TStaticText *StaticText2;
    TBevel *Bevel2;
    TShape *Shape3;
    TStaticText *StaticText18;
    TBevel *Bevel4;
    TPanel *Panel_Config;
    TBevel *Bevel_CfgCurrIP;
    TBevel *Bevel_CfgCurrMask;
    TBevel *Bevel_CfgCurrGW;
    TBevel *Bevel_CfgCurrMAC;
    TBevel *Bevel1;
    TBevel *Bevel3;
    TBevel *Bevel5;
    TEdit *Edit_CfgCurrIP_A;
    TEdit *Edit_CfgCurrIP_B;
    TEdit *Edit_CfgCurrIP_C;
    TEdit *Edit_CfgCurrIP_D;
    TEdit *Edit_CfgCurrMask_A;
    TEdit *Edit_CfgCurrMask_B;
    TEdit *Edit_CfgCurrMask_C;
    TEdit *Edit_CfgCurrMask_D;
    TEdit *Edit_CfgCurrGW_A;
    TEdit *Edit_CfgCurrGW_B;
    TEdit *Edit_CfgCurrGW_C;
    TEdit *Edit_CfgCurrGW_D;
    TStaticText *StaticText_CfgCurrIP;
    TStaticText *StaticText_CfgCurrMask;
    TStaticText *StaticText_Edit_CfgCurrGW;
    TStaticText *StaticText_CfgCurrMAC;
    TEdit *Edit_CfgCurrMAC_A;
    TEdit *Edit_CfgCurrMAC_B;
    TEdit *Edit_CfgCurrMAC_C;
    TEdit *Edit_CfgCurrMAC_D;
    TEdit *Edit_CfgCurrMAC_E;
    TEdit *Edit_CfgCurrMAC_F;
    TStaticText *StaticText8;
    TEdit *Edit_TcpToutReintentos;
    TStaticText *StaticText9;
    TEdit *Edit_TcpToutPrimerRetry;
    TEdit *Edit_TcpRetries;
    TStaticText *StaticText10;
    TShape *Shape4;
    TStaticText *StaticText3;
    TBevel *Bevel11;
    TShape *Shape5;
    TStaticText *StaticText4;
    TBevel *Bevel12;
    TShape *Shape6;
    TStaticText *StaticText5;
    TBevel *Bevel13;
    TShape *Shape7;
    TStaticText *StaticText7;
    TBevel *Bevel14;
    TShape *Shape8;
    TStaticText *StaticText12;
    TBevel *Bevel15;
    TShape *Shape9;
    TStaticText *StaticText14;
    TBevel *Bevel16;
    TSpeedButton *SpeedButton_Help;
    TMainMenu *MainMenu1;
    TMenuItem *MenuOpciones;
    TMenuItem *MenuArchivos;
    TMenuItem *MenuAyuda;
    TMenuItem *MenuConfiguracion;
    TMenuItem *MenuInformacion;
    TMenuItem *MenuAyudaPrincipal;
    TPanel *Panel_ModoConfig;
    TEdit *Edit_TxOkPercent;
    TEdit *Edit_TxErrPercent;
    TLabel *Label13;
    TStaticText *StaticText19;
    TShape *Shape10;
    TTabSheet *TabSheet_Variadores;
    TStaticText *StaticText20;
    TShape *Shape11;
    TBevel *Bevel19;
    TBevel *Bevel20;
    TBevel *Bevel21;
    TBevel *Bevel18;
    TMenuItem *N1;
    TBevel *Bevel10;
        TStaticText *StaticText_SRL_Nodo;
        TEdit *Edit_SRL_Node_A;
        TEdit *Edit_SRL_Node_B;
        TEdit *Edit_SRL_ScanRate_B;
        TStaticText *StaticText_SRL_BaudRate;
        TStaticText *StaticText_SRL_Paridad;
        TComboBox *ComboBox_BaudRate_A;
        TComboBox *ComboBox_Paridad_A;
        TComboBox *ComboBox_BaudRate_B;
        TComboBox *ComboBox_Paridad_B;
        TStaticText *StaticText21;
        TStaticText *StaticText22;
        TStaticText *StaticText_RelacionTx;
        TStaticText *StaticText_SRL_Mode;
        TComboBox *ComboBox_Mode_A;
        TComboBox *ComboBox_Mode_B;
        TPanel *Panel2;
        TImage *_Logo;
        TMenuItem *AcercadeGWYConfig1;
        TStaticText *StaticText_FwVersionFull;
        TEdit *Edit_FwVersionFull;
        TTabSheet *TabSheet_Logs;
        TMemo *Memo_LogsText;
        TShape *Shape12;
        TStaticText *StaticText23;
        TTabSheet *TabSheet_Trace;
        TShape *Shape13;
        TStaticText *StaticText24;
        TButton *Button_ActualizarLogs;
        TButton *Button_LimpiarLogs;
        TMemo *Memo_UdpLog;
        TNMUDP *UdpDebug;
        TButton *Button_DebugConectar;
        TButton *Button_UdpLogLimpiar;
        TComboBox *ComboBox_IP;
        TLabel *Label4;
        TBevel *Bevel22;
        TLabel *Label5;
        TLabel *Label16;
        TLabel *Label17;
        TLabel *Label18;
        TPanel *Panel_SetupDebug;
        TComboBox *ComboBox_SetupDebugLevel;
        TLabel *Label19;
        TStaticText *StaticText_TmrTxWait;
        TEdit *Edit_SRL_TmrTxA;
        TEdit *Edit_SRL_TmrTxB;
        TStaticText *StaticText_SRL_TmrRx;
        TEdit *Edit_SRL_TmrRxA;
        TEdit *Edit_SRL_TmrRxB;
        TStaticText *StaticText26;
        TEdit *Edit_SRL_TmrDriveA;
        TEdit *Edit_SRL_TmrDriveB;
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall IMP_ClientSocketDisconnect(TObject *Sender,
          TCustomWinSocket *Socket);
        void __fastcall IMP_ClientSocketConnect(TObject *Sender,
          TCustomWinSocket *Socket);
        void __fastcall IMP_ClientSocketError(TObject *Sender,
          TCustomWinSocket *Socket, TErrorEvent ErrorEvent,
          int &ErrorCode);
        void __fastcall IMP_ClientSocketRead(TObject *Sender,
          TCustomWinSocket *Socket);
        void __fastcall ActualizarValores(TObject *Sender);
        void __fastcall IMP_PageControlChange(TObject *Sender);
        void __fastcall Timer1Timer(TObject *Sender);
        void __fastcall FormActivate(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall Button_SincronizarClick(TObject *Sender);
        void __fastcall RadioButton_HitachiJ300Click(TObject *Sender);
        void __fastcall RadioButton_SieMicroMasterClick(TObject *Sender);
        void __fastcall RadioButton_HitachiSJ300Click(TObject *Sender);
        void __fastcall RadioButton_SieMasterDriveClick(TObject *Sender);
        void __fastcall Timer2Timer(TObject *Sender);
        void __fastcall Timer3Timer(TObject *Sender);
        void __fastcall Button_TransferirClick(TObject *Sender);
        void __fastcall Button_VerificarClick(TObject *Sender);
        void __fastcall Button_BuscarArchivoClick(TObject *Sender);
        void __fastcall RadioButton_FactoryClick(TObject *Sender);
        void __fastcall RadioButton_UpgradeClick(TObject *Sender);
        void __fastcall Button_ActualizarFWClick(TObject *Sender);
        void __fastcall Button_ValidateClick(TObject *Sender);
        void __fastcall Button_RecuperarClick(TObject *Sender);
        void __fastcall Timer_ReconectTimer(TObject *Sender);
        void __fastcall Button_SetearSalidasClick(TObject *Sender);
        void __fastcall Button_LeerEEPROMClick(TObject *Sender);
        void __fastcall Timer_DisconnectTimer(TObject *Sender);
        void __fastcall Button_BuscarArchivoWebClick(TObject *Sender);
        void __fastcall Button_WebArgregarClick(TObject *Sender);
        void __fastcall Button_WebGrabarClick(TObject *Sender);
        void __fastcall Button_TransferirWebClick(TObject *Sender);
        void __fastcall Button_ReadFlashClick(TObject *Sender);
    void __fastcall TreeView1Change(TObject *Sender, TTreeNode *Node);
    void __fastcall SpeedButton_ConectarClick(TObject *Sender);
    void __fastcall SpeedButton_DesconectarClick(TObject *Sender);
    void __fastcall SpeedButton_ResetClick(TObject *Sender);
    void __fastcall BitBtn_GrabarClick(TObject *Sender);
    void __fastcall BitBtn_CancelarClick(TObject *Sender);
    void __fastcall BitBtn_AyudaClick(TObject *Sender);
    void __fastcall SpeedButton_HelpClick(TObject *Sender);
    void __fastcall MenuAyudaPrincipalClick(TObject *Sender);
    void __fastcall MenuInformacionClick(TObject *Sender);
    void __fastcall MenuVersionDeSWClick(TObject *Sender);
        void __fastcall AcercadeGWYConfig1Click(TObject *Sender);
        void __fastcall Button_ActualizarLogsClick(TObject *Sender);
        void __fastcall Button_LimpiarLogsClick(TObject *Sender);
        void __fastcall Button_DebugConectarClick(TObject *Sender);
        void __fastcall UdpDebugDataReceived(TComponent *Sender,
          int NumberBytes, AnsiString FromIP, int Port);
        void __fastcall Button_UdpLogLimpiarClick(TObject *Sender);
        void __fastcall ComboBox_IPSelect(TObject *Sender);
private:	// User declarations
        unsigned int    AdminCode;         // Codigo de nivel de privilegios cargado del .ini
        unsigned int    DebugTest;          // Habilitar opciones de debug..
        unsigned int    FramesTxOk;         // Tramas transmitidas OK
        unsigned int    FramesRxOk;         // Tramas recibidas OK
        unsigned char   NextToTx;           // Proxima a transmitir
        unsigned char   LastRx;             // Ultima recibida
        unsigned char   TxRetries;          // Reintentos de transmision
        unsigned short  CfgKeyCode;         // Clave para acceso a configuración avanzada.
        bool            QueCfgFrameBusy;    // Flag de buffer de mensajes de config ocupado
        TConfigFrame    ConfigFrame;        // Trama de mensaje de configuración..
        TConfigFrame    RxConfigFrame;      // Mensaje de configuracion recibido
        TConfigFrame    QueueConfigFrame;   // Buffer para mensaje de configuración pendiente
        TMsgInfo        CfgMsgInfo;         //
        TMsgDnpInfo     CfgMsgDnpInfo;      // Mensaje de config. para comunicacion DNP3
        TMsgStat        CfgMsgStat;         // Mensaje de configuración con estadísticas
        TMsgVersion     CfgMsgVersion;      // Mensaje de config. con información de las versiones
        TMsgConst       CfgMsgConst;        // Mensaje de config. con valores de las constantes
        TMsgGioState    CfgMsgGioState;     // Mensaje de config. con los estados de los puertos
        TMsgSerialCfg   CfgMsgSerialCfg;    // Mensaje de config. con valores de configuracion del puerto serie    
        TCfgFwHeader    FwHeader;

        int             TMR1_PollingFrec;
        int             TMR1_ReadStatFrec;


        unsigned char   FwFile[MAX_FW_REG][MAX_MSG_SIZE];
        unsigned char   FwRegLen[MAX_FW_REG];
        unsigned int    FirstAddrRegFile;
        unsigned int    NextTfrRegFile;
        unsigned int    RegFileCntr;
        unsigned int    ReadFlashStage;
        //unsigned int    ReadLogStage;


//        TCfgWebFiles    WebFile;
        unsigned char   WebFilesQtty;
        unsigned int    FirstAddrWebRegFile;
        unsigned int    NextTfrWebRegFile;
        unsigned int    RegWebFileCntr;
        unsigned int    FirstAddrFatRegFile;
        unsigned int    NextTfrFatRegFile;
        unsigned int    RegFatFileCntr;
        unsigned int    TransferWebStage;
        unsigned int    WebAllRegSize;

        TCfgWebFiles *  pWebFile;
        TCfgWebFiles    WebFileHeap;

        AnsiString MyBcdToStr ( char BCD_Byte);
        unsigned char MyIntToBcd (unsigned short INT_Val);
        void  DisableSendButtons (void);
        void  EnableSendButtons (void);
        void  RetrySenBuf (void);
        void  SendNextFwReg (void);
        void  SendNextWebReg (void);
        void  VerifyNextFwReg (void);

        AnsiString  DeviceProfile;
        AnsiString  ParamLabels;
        bool        RTUCheckboxEnable;

public:		// User declarations
        __fastcall TGWY_Config(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TGWY_Config *GWY_Config;
//---------------------------------------------------------------------------
#endif
