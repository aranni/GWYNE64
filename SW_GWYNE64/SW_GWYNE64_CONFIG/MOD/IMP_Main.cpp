//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <stdio.h>
#include <IniFiles.hpp>

#include "IMP_Main.h"
#include "GWY_HelpDef.hpp"
#include "GWY_About.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TGWY_Config *GWY_Config;

#define TMR_POLLING_FREC        300
#define TMR_STAT_REFRESH        300


#define TAB_SETUP               0
#define TAB_DNP                 1
#define TAB_TCP                 2
#define TAB_485                 3
#define TAB_ESTADISTICAS        4
#define TAB_CONSTANTES          5
#define TAB_VERSION             6
#define TAB_UPGRADE             7
#define TAB_PUERTOS             8
#define TAB_WEB                 9
#define TAB_VARIADORES         10
#define TAB_LOGS               11
#define TAB_TRACE              12

#define DEBUG_TEST

FILE *  ConfigFilePtr;
FILE *  WebFilePtr;
FILE *  PageFilePtr;
char    TmpParam[60];
char    TmpCntr;

UINT8   CRYP_INI;
UINT8   CRYP_MUL;
UINT8   CRYP_SUM;

const char SW_Version[] = {"003.001.F"};
const char SW_Date[] = {__DATE__};
const char SW_Time[] = {__TIME__};


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%   FUNCIONES INICIALIZACION
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
//
//---------------------------------------------------------------------------
__fastcall TGWY_Config::TGWY_Config(TComponent* Owner)
        : TForm(Owner)
{

}
//---------------------------------------------------------------------------

void __fastcall TGWY_Config::FormActivate(TObject *Sender)
{

    unsigned int AccessValid;

        TTreeNode *Node1;

        StatusBar_CFG->Panels->Items[0]->Text = "Modulo DESCONECTADO";
        ProgressBar3->Max = MAX_WEB_REG;
        ProgressBar3->Position = 0;

        if (DebugTest){
                Panel_Temperatura->Visible = true;
                Panel_EEPROM->Visible = true;
                Panel_UpdateFactory->Visible = true;
                Panel_UpdateFactory->Enabled = true;
        }
        else{
                Panel_Temperatura->Visible = false;
                Panel_EEPROM->Visible = false;
                Panel_UpdateFactory->Visible = false;
                Panel_UpdateFactory->Enabled = false;
        }



        AccessValid = (SW_Time[7]-'0')*1000 +
                      (SW_Time[6]-'0')*100 +
                      (SW_Time[4]-'0')*10 +
                      (SW_Time[3]-'0');

 //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ ABR_DEBUG: deshabilita acceso restricto
        AccessValid  = AdminCode;
 //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

        if (( AdminCode == AccessValid )||(DebugTest)){
                IMP_PageControl->Pages[TAB_UPGRADE]->Enabled = true;
                IMP_PageControl->Pages[TAB_UPGRADE]->Visible = true;
                IMP_PageControl->Pages[TAB_WEB]->Enabled = true;
                IMP_PageControl->Pages[TAB_WEB]->Visible = true;
        }
        else{
                IMP_PageControl->Pages[TAB_UPGRADE]->Enabled = false;
                IMP_PageControl->Pages[TAB_UPGRADE]->Visible = false;
                IMP_PageControl->Pages[TAB_WEB]->Enabled = false;
                IMP_PageControl->Pages[TAB_WEB]->Visible = false;
        }


        if (!(strcmp (DeviceProfile.c_str(), "VAR"))){
            Node1 = TreeView1->Items->Add(NULL, ValueList_TabNames->Keys[TAB_VARIADORES] );
            Node1->ImageIndex = 8;
            Node1->SelectedIndex = 9;
            Panel_ModoConfig->Caption = "Configuraci�n de Variadores";
            return;
        }

        if (!(strcmp (DeviceProfile.c_str(), "DNP"))){
            Node1 = TreeView1->Items->Add(NULL, ValueList_TabNames->Keys[TAB_DNP] );
            Node1->ImageIndex = 8;
            Node1->SelectedIndex = 9;
            Panel_ModoConfig->Caption = "Configuraci�n de Reconectadores";
            return;
        }


}
//---------------------------------------------------------------------------



void __fastcall TGWY_Config::FormCreate(TObject *Sender)
{
        char CharCntr, TmpRead;
        char StrKeyCode[15];
        TTreeNode *Node1;

        FirstAddrRegFile = FLASH_UPDATE_FWFILE;
        RadioButton_Upgrade->Checked = true;
        FirstAddrFatRegFile = 0;
        FirstAddrWebRegFile = 100;
        CRYP_MUL = 13;
        CRYP_SUM = 1;

        TIniFile *IniFilePtr;
        IniFilePtr = new TIniFile( ChangeFileExt( Application -> ExeName, ".ini" ) );
        AdminCode = IniFilePtr -> ReadInteger( "Settings", "AdminCode", 0 );
        DebugTest = IniFilePtr -> ReadInteger( "Settings", "ABR_Debug", 0 );
        Edit_ConnectIP -> Text = IniFilePtr->ReadString( "Settings", "IPAddress", "192.168.0.250" );
        IMP_ClientSocket->Port = IniFilePtr->ReadInteger( "Settings", "Port", 5000 );
        TMR1_PollingFrec = IniFilePtr->ReadInteger( "Settings", "PollingFrec", TMR_POLLING_FREC );
        TMR1_ReadStatFrec = IniFilePtr->ReadInteger( "Settings", "ReadStatFrec", TMR_STAT_REFRESH );
        DeviceProfile = IniFilePtr->ReadString( "Settings", "Device", "VAR" );
        ParamLabels = IniFilePtr->ReadString( "Specific", "ParamLabels", "GENERIC" );
        RTUCheckboxEnable = IniFilePtr -> ReadInteger( "Specific", "RTUCheckBox", 0 );

        IniFilePtr->ReadSectionValues( "IP",ComboBox_IP->Items);

        TreeView1->Items->Clear();
        if ( IniFilePtr->ReadInteger( "Tabs", "TAB_GENERAL", 0 )){

            Node1 = TreeView1->Items->Add(NULL, ValueList_TabNames->Keys[TAB_SETUP] );
            Node1->ImageIndex = 0;
            Node1->SelectedIndex = 1;
            if (RTUCheckboxEnable)
                CheckBox_RunCfgRTU->Visible = true;
            else
                CheckBox_RunCfgRTU->Visible = false;

        }

        if ( IniFilePtr->ReadInteger( "Tabs", "TAB_ETHERNET", 0 )){

            Node1 = TreeView1->Items->Add(NULL, ValueList_TabNames->Keys[TAB_TCP] );
            Node1->ImageIndex = 2;
            Node1->SelectedIndex = 3;
        }

        if ( IniFilePtr->ReadInteger( "Tabs", "TAB_SERIE", 0 )){

            Node1 = TreeView1->Items->Add(NULL, ValueList_TabNames->Keys[TAB_485] );
            Node1->ImageIndex = 4;
            Node1->SelectedIndex = 5;
        }

        if ( IniFilePtr->ReadInteger( "Tabs", "TAB_IO", 0 )){

            Node1 = TreeView1->Items->Add(NULL, ValueList_TabNames->Keys[TAB_PUERTOS] );
            Node1->ImageIndex = 6;
            Node1->SelectedIndex = 7;
        }

        if ( IniFilePtr->ReadInteger( "Tabs", "TAB_PARAMETROS", 0 )){

            Node1 = TreeView1->Items->Add(NULL, ValueList_TabNames->Keys[TAB_CONSTANTES] );
            Node1->ImageIndex = 18;
            Node1->SelectedIndex = 19;
            if (!(strcmp (ParamLabels.c_str(), "YPF")))
            {
                    StaticText_ConsignaFrec->Caption = "Consigna de Frec.";
                    StaticText_RelacionTx->Caption = "Rel. de Transmision";
                    StaticText_CoefResbal->Caption = "Coeficiente de Resb.";
                    StaticText_RpmEnMotor->Caption = "RPM del Motor";
            }

        }

        if ( IniFilePtr->ReadInteger( "Tabs", "TAB_VERSIONES", 0 )){

            Node1 = TreeView1->Items->Add(NULL, ValueList_TabNames->Keys[TAB_VERSION] );
            Node1->ImageIndex = 20;
            Node1->SelectedIndex = 21;
        }

        if ( IniFilePtr->ReadInteger( "Tabs", "TAB_FIRMWARE", 0 )){

            Node1 = TreeView1->Items->Add(NULL, ValueList_TabNames->Keys[TAB_UPGRADE] );
            Node1->ImageIndex = 12;
            Node1->SelectedIndex = 13;
        }

        if ( IniFilePtr->ReadInteger( "Tabs", "TAB_WEB", 0 )){

            Node1 = TreeView1->Items->Add(NULL, ValueList_TabNames->Keys[TAB_WEB] );
            Node1->ImageIndex = 10;
            Node1->SelectedIndex = 11;
        }

        if ( IniFilePtr->ReadInteger( "Tabs", "TAB_ESTADISTICAS", 0 )){

            Node1 = TreeView1->Items->Add(NULL, ValueList_TabNames->Keys[TAB_ESTADISTICAS] );
            Node1->ImageIndex = 16;
            Node1->SelectedIndex = 17;
        }

        if ( IniFilePtr->ReadInteger( "Tabs", "TAB_LOGS", 0 )){

            Node1 = TreeView1->Items->Add(NULL, ValueList_TabNames->Keys[TAB_LOGS] );
            Node1->ImageIndex = 8;
            Node1->SelectedIndex = 9;
        }

        if ( IniFilePtr->ReadInteger( "Tabs", "TAB_TRACE", 0 )){

            Node1 = TreeView1->Items->Add(NULL, ValueList_TabNames->Keys[TAB_TRACE] );
            Node1->ImageIndex = 14;
            Node1->SelectedIndex = 15;
        }

        delete IniFilePtr;

//        pWebFile = new TCfgWebFiles;
        pWebFile = &WebFileHeap;

//        ConfigFilePtr = fopen ("IMPConfig.cfg","rb");
        ConfigFilePtr = fopen ( (ChangeFileExt( Application -> ExeName, ".cfg")).c_str() ,"rb");

        if (ConfigFilePtr == NULL ) {
                CfgKeyCode = 0x00;
//                ShowMessage ("ERROR: No pudo abrirse el archivo de Configuraci�n");
                return;
        }
        else{
            CharCntr = 0;
            fseek(ConfigFilePtr, 0, SEEK_SET);
            TmpRead = (char)fgetc(ConfigFilePtr);
            do{
                StrKeyCode[CharCntr++] = TmpRead;
                TmpRead = (char)fgetc(ConfigFilePtr);
            }while ((TmpRead != '\r') && (!feof(ConfigFilePtr)));
            StrKeyCode[CharCntr] = '\0';
            CfgKeyCode = StrToInt(StrKeyCode);
            fclose (ConfigFilePtr);

            Edit_HwVersion->ReadOnly = false;
            Edit_Serial->ReadOnly = false;
            Edit_CfgCurrMAC_A->ReadOnly = false;
            Edit_CfgCurrMAC_B->ReadOnly = false;
            Edit_CfgCurrMAC_C->ReadOnly = false;
            Edit_CfgCurrMAC_D->ReadOnly = false;
            Edit_CfgCurrMAC_E->ReadOnly = false;
            Edit_CfgCurrMAC_F->ReadOnly = false;
        }


}
//---------------------------------------------------------------------------

void __fastcall TGWY_Config::FormClose(TObject *Sender,
      TCloseAction &Action)
{
      IMP_ClientSocket->Close();
}
//---------------------------------------------------------------------------
void __fastcall TGWY_Config::IMP_ClientSocketDisconnect(TObject *Sender,
      TCustomWinSocket *Socket)
{
        StatusBar_CFG->Panels->Items[0]->Text = "Modulo DESCONECTADO";
        SpeedButton_Conectar->Enabled  = true;
        SpeedButton_Desconectar->Enabled = false;
        BitBtn_Grabar->Enabled = false;
        SpeedButton_Reset->Enabled = false;
        BitBtn_Cancelar->Enabled = false;
        Button_Sincronizar->Enabled = false;
        Timer1->Enabled = false;
        ComboBox_IP->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TGWY_Config::IMP_ClientSocketConnect(TObject *Sender,
      TCustomWinSocket *Socket)
{
        int FileBufCntr;

        StatusBar_CFG->Panels->Items[0]->Text = "Modulo CONECTADO";
        SpeedButton_Conectar->Enabled  = false;
        SpeedButton_Desconectar->Enabled = true;
        BitBtn_Grabar->Enabled = true;
        SpeedButton_Reset->Enabled = true;
        BitBtn_Cancelar->Enabled = true;
        Button_Sincronizar->Enabled = true;
        Timer1->Interval = TMR1_PollingFrec;
        Timer1->Enabled = true;
        QueCfgFrameBusy = false;
        ActualizarValores(this);
        FramesRxOk = 0x00;
        FramesTxOk = 0x00;
        NextToTx = 0x00;
        LastRx = 0x00;
        TxRetries = 0x00;
        RegFileCntr = 0x00;

        Edit_ArchivoFW->Text =  "";
        Edit_Registros->Text =  "";
        Edit_Version->Text = "";
        Edit_FechaCarga->Text = "";
        Edit_HoraCarga->Text = "";

        ComboBox_IP->Enabled = false;


        for (FileBufCntr = 0 ; FileBufCntr < (MAX_MSG_SIZE-1) ; FileBufCntr++)
                FwFile[0][FileBufCntr] = 0xFF;

        for (FileBufCntr = 1 ; FileBufCntr < MAX_FW_REG ; FileBufCntr++)
                memcpy(&(FwFile[FileBufCntr][0]),&(FwFile[0][0]), MAX_MSG_SIZE);



}
//---------------------------------------------------------------------------

void __fastcall TGWY_Config::IMP_ClientSocketError(TObject *Sender,
      TCustomWinSocket *Socket, TErrorEvent ErrorEvent, int &ErrorCode)
{
       ErrorCode = 0x00;
       MessageDlg("Falla de comunicaci�n con el m�dulo: " + IMP_ClientSocket->Host ,
               mtError, TMsgDlgButtons() << mbOK, 0);
       IMP_ClientSocket->Close();
       Timer1->Enabled = false;
       Timer2->Enabled = false;

}
//----------------------------------------------------------------------------------


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%   FUNCIONES PSEUDO DEL DRIVER DE COMUNICACIONES
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
//
// ***************************************************************************
// TRANSMISION DE MENSAJE
// ***************************************************************************
//
void __fastcall TGWY_Config::Timer1Timer(TObject *Sender)
{

        if (QueCfgFrameBusy){
                memcpy(&(ConfigFrame),&(QueueConfigFrame),sizeof(ConfigFrame));
                QueCfgFrameBusy = false;
                if (( QueueConfigFrame.OpCode == CFG_TCP_RESET)){
                        // Cerrar la conexi�n
                        Timer_Disconnect->Interval = 500;
                        Timer_Disconnect->Enabled = true;
                }

                if (( QueueConfigFrame.OpCode == CFG_TCP_UPGRADE_FW) ||
                    ( QueueConfigFrame.OpCode == CFG_TCP_RESTORE_FW) ){

                        // Cerrar la conexi�n y reconectar despu�s de unos segundos..
                        Timer_Reconect->Interval = 500;
                        Timer_Reconect->Enabled = true;
                }

        }
        else{
                ConfigFrame.Len = 1;
                switch (IMP_PageControl->ActivePageIndex){
                   case TAB_ESTADISTICAS:
                        ConfigFrame.OpCode = CFG_TCP_READ_STAT;
                        break;
                        
                   case TAB_PUERTOS:
                        ConfigFrame.OpCode = CFG_TCP_GET_IOVALUES;
                        break;

                   default:
                        ConfigFrame.OpCode = CFG_TCP_POL;
                        break;
                }
        }
        ConfigFrame.TxWindow = NextToTx;
        ConfigFrame.RxWindow = LastRx;

        IMP_ClientSocket->Socket->SendBuf(&ConfigFrame,(ConfigFrame.Len + OverHeadCfgSize));
        Timer1->Enabled = false;
        Timer2->Enabled = true;    // supervisi�n de recepci�n de respuesta.

}
//---------------------------------------------------------------------------

void __fastcall TGWY_Config::Timer2Timer(TObject *Sender)
{
        // cuando vence el Timer3 se reintenta la transmisi�n..
       Timer3->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TGWY_Config::Timer3Timer(TObject *Sender)
{
        // reintentar transmision..
       Timer3->Enabled = false;
       RetrySenBuf();
}
//---------------------------------------------------------------------------

void  TGWY_Config::RetrySenBuf (void)
{
       TxRetries++;
       if ( TxRetries > MAX_TX_RETRIES) {
              IMP_ClientSocket->Close();
              Timer1->Enabled = false;
              Timer2->Enabled = false;
              MessageDlg("Se perdi� la comunicaci�n con: " + IMP_ClientSocket->Host ,
               mtError, TMsgDlgButtons() << mbOK, 0);
              return;
       }

//     Retransmitir el mensaje..
       IMP_ClientSocket->Socket->SendBuf(&ConfigFrame,(ConfigFrame.Len + OverHeadCfgSize));
       FramesTxOk++;
       Timer2->Enabled = true;    // supervisi�n de recepci�n de respuesta.
       StatusBar_CFG->Panels->Items[1]->Text = ( "TxOK : " + IntToStr(FramesTxOk));
       StatusBar_CFG->Panels->Items[2]->Text = ( "RxOK : " + IntToStr(FramesRxOk));

}

// ***************************************************************************
// RECEPCION DE MENSAJE
// ***************************************************************************
//
void __fastcall TGWY_Config::IMP_ClientSocketRead(TObject *Sender,
      TCustomWinSocket *Socket)
{
    Socket->ReceiveBuf(&RxConfigFrame,sizeof(TConfigFrame));

        Timer2->Enabled = false;    // supervisi�n de recepci�n de respuesta.
        if ( NextToTx != RxConfigFrame.RxWindow ){
                // cuando vence el Timer3 se reintenta la transmisi�n..
                Timer3->Enabled = true;
                return;
        }

        LastRx = RxConfigFrame.TxWindow;
        switch(RxConfigFrame.OpCode){
                case CFG_TCP_READ_DNP:
                        memcpy(&(CfgMsgDnpInfo),&(RxConfigFrame.Data),sizeof(CfgMsgDnpInfo));
                        Edit_AddrDnpMaestro->Text = IntToStr(CfgMsgDnpInfo.AddrMaster);
                        Edit_AddrDnpEsclavo->Text = IntToStr(CfgMsgDnpInfo.AddrSlave);
                        break;



                case CFG_TCP_GET_SCICONFIG:
                        memcpy(&(CfgMsgSerialCfg),&(RxConfigFrame.Data),sizeof(CfgMsgSerialCfg));
                        CHG_CFG_ENDIAN( CfgMsgSerialCfg.SCIA_ConfigScanRate );
                        Edit_SRL_ScanRate->Text = IntToStr(CfgMsgSerialCfg.SCIA_ConfigScanRate );
                        CHG_CFG_ENDIAN( CfgMsgSerialCfg.SCIB_ConfigScanRate );
                        Edit_SRL_ScanRate_B->Text = IntToStr(CfgMsgSerialCfg.SCIB_ConfigScanRate );

                        CHG_CFG_ENDIAN( CfgMsgSerialCfg.SCIA_TmrTxWait );
                        Edit_SRL_TmrTxA->Text = IntToStr(CfgMsgSerialCfg.SCIA_TmrTxWait );
                        CHG_CFG_ENDIAN( CfgMsgSerialCfg.SCIA_TmrRxWait );
                        Edit_SRL_TmrRxA->Text = IntToStr(CfgMsgSerialCfg.SCIA_TmrRxWait );
                        CHG_CFG_ENDIAN( CfgMsgSerialCfg.SCIA_TmrDrive );
                        Edit_SRL_TmrDriveA->Text = IntToStr(CfgMsgSerialCfg.SCIA_TmrDrive );

                        CHG_CFG_ENDIAN( CfgMsgSerialCfg.SCIB_TmrTxWait );
                        Edit_SRL_TmrTxB->Text = IntToStr(CfgMsgSerialCfg.SCIB_TmrTxWait );
                        CHG_CFG_ENDIAN( CfgMsgSerialCfg.SCIB_TmrRxWait );
                        Edit_SRL_TmrRxB->Text = IntToStr(CfgMsgSerialCfg.SCIB_TmrRxWait );
                        CHG_CFG_ENDIAN( CfgMsgSerialCfg.SCIB_TmrDrive );
                        Edit_SRL_TmrDriveB->Text = IntToStr(CfgMsgSerialCfg.SCIB_TmrDrive );

                        Edit_SRL_Node_A->Text = IntToStr (CfgMsgSerialCfg.SCIA_ConfigNode);
                        Edit_SRL_Node_B->Text = IntToStr (CfgMsgSerialCfg.SCIB_ConfigNode);
                        BYTE    TmpCfgCode, TmpByte;
                        TmpCfgCode = CfgMsgSerialCfg.SCIA_ConfigCode;
                        TmpByte = (TmpCfgCode & CFG_SCI_BAUDRATE_MASK);
                        ComboBox_BaudRate_A->ItemIndex = TmpByte-1;     // Empieza del 001 = 300 baudios
                        
                        TmpByte = (TmpCfgCode & CFG_SCI_PARITY_MASK);
                        TmpByte >>= 4;
                        TmpByte += (TmpCfgCode & CFG_SCI_DATABITS_MASK)?3:0;
                        TmpByte--;      // Paridad es 01, 10 o 11, en cambio ItemIndex empieza en 0
                        ComboBox_Paridad_A->ItemIndex = TmpByte;
                        TmpByte = (TmpCfgCode & CFG_SCI_PHYMODE_MASK);
                        TmpByte >>= 7;
                        ComboBox_Mode_A->ItemIndex = TmpByte;   // 0: RS232, 1: RS485

                        TmpCfgCode = CfgMsgSerialCfg.SCIB_ConfigCode;
                        TmpByte = (TmpCfgCode & CFG_SCI_BAUDRATE_MASK);
                        ComboBox_BaudRate_B->ItemIndex = TmpByte-1;     // Empieza del 001 = 300 baudios
                        TmpByte = (TmpCfgCode & CFG_SCI_PARITY_MASK);
                        TmpByte >>= 4;
                        TmpByte += (TmpCfgCode & CFG_SCI_DATABITS_MASK)?3:0;
                        TmpByte--;      // Paridad es 01, 10 o 11, en cambio ItemIndex empieza en 0
                        ComboBox_Paridad_B->ItemIndex = TmpByte;
                        TmpByte = (TmpCfgCode & CFG_SCI_PHYMODE_MASK);
                        TmpByte >>= 7;
                        ComboBox_Mode_B->ItemIndex = TmpByte;   // 0: RS232, 1: RS485
                        break;



//                case CFG_TCP_WRITE:
                case CFG_TCP_READ:
                        memcpy(&(CfgMsgInfo),&(RxConfigFrame.Data),sizeof(CfgMsgInfo));


                        ComboBox_SetupDebugLevel->ItemIndex = (int)CfgMsgInfo.DebugLevel;
/*
                        if  (CfgMsgInfo.DebugLevel == INVCODE_HIT_J300){
                                RadioButton_HitachiJ300->Checked        = true;
                                RadioButton_SieMicroMaster->Checked     = false;
                                RadioButton_HitachiSJ300->Checked       = false;
                                RadioButton_SieMasterDrive->Checked     = false;
                        }
                        if  (CfgMsgInfo.DebugLevel == INVCODE_SIE_MICROMASTER){
                                RadioButton_HitachiJ300->Checked        = false;
                                RadioButton_SieMicroMaster->Checked     = true;
                                RadioButton_HitachiSJ300->Checked       = false;
                                RadioButton_SieMasterDrive->Checked     = false;
                        }
                        if  (CfgMsgInfo.DebugLevel == INVCODE_HIT_SJ300){
                                RadioButton_HitachiJ300->Checked        = false;
                                RadioButton_SieMicroMaster->Checked     = false;
                                RadioButton_HitachiSJ300->Checked       = true;
                                RadioButton_SieMasterDrive->Checked     = false;
                        }
                        if  (CfgMsgInfo.DebugLevel == INVCODE_SIE_MASTERDRIVE){
                                RadioButton_HitachiJ300->Checked        = false;
                                RadioButton_SieMicroMaster->Checked     = false;
                                RadioButton_HitachiSJ300->Checked       = false;
                                RadioButton_SieMasterDrive->Checked     = true;
                        }
*/
                        if  (CfgMsgInfo.RunConfig & CFG_RTC_RUNSTATE_MASK)
                                CheckBox_RunCfgRTC->Checked = true;
                        else
                                CheckBox_RunCfgRTC->Checked = false;

                        if  (CfgMsgInfo.RunConfig & CFG_RTU_RUNSTATE_MASK)
                                CheckBox_RunCfgRTU->Checked = true;
                        else
                                CheckBox_RunCfgRTU->Checked = false;


//                        Edit_DebugLevel->Text = IntToStr(CfgMsgInfo.DebugLevel);
                        CHG_CFG_ENDIAN( CfgMsgInfo.PollingFrec );
                        Edit_SRL_ScanRate->Text = IntToStr(CfgMsgInfo.PollingFrec);
                        CHG_CFG_ENDIAN( CfgMsgInfo.SIE_CmdWordA );
                        Edit_SIE_CmdWordA->Text = IntToStr(CfgMsgInfo.SIE_CmdWordA);

                        Edit_CfgCurrIP_A->Text = IntToStr(CfgMsgInfo.TcpIP[0]);
                        Edit_CfgCurrIP_B->Text = IntToStr(CfgMsgInfo.TcpIP[1]);
                        Edit_CfgCurrIP_C->Text = IntToStr(CfgMsgInfo.TcpIP[2]);
                        Edit_CfgCurrIP_D->Text = IntToStr(CfgMsgInfo.TcpIP[3]);

                        Edit_CfgCurrMask_A->Text = IntToStr(CfgMsgInfo.TcpMask[0]);
                        Edit_CfgCurrMask_B->Text = IntToStr(CfgMsgInfo.TcpMask[1]);
                        Edit_CfgCurrMask_C->Text = IntToStr(CfgMsgInfo.TcpMask[2]);
                        Edit_CfgCurrMask_D->Text = IntToStr(CfgMsgInfo.TcpMask[3]);

                        Edit_CfgCurrGW_A->Text = IntToStr(CfgMsgInfo.TcpGW[0]);
                        Edit_CfgCurrGW_B->Text = IntToStr(CfgMsgInfo.TcpGW[1]);
                        Edit_CfgCurrGW_C->Text = IntToStr(CfgMsgInfo.TcpGW[2]);
                        Edit_CfgCurrGW_D->Text = IntToStr(CfgMsgInfo.TcpGW[3]);

                        Edit_CfgCurrMAC_A->Text = IntToHex(CfgMsgInfo.TcpMAC[0],2);
                        Edit_CfgCurrMAC_B->Text = IntToHex(CfgMsgInfo.TcpMAC[1],2);
                        Edit_CfgCurrMAC_C->Text = IntToHex(CfgMsgInfo.TcpMAC[2],2);
                        Edit_CfgCurrMAC_D->Text = IntToHex(CfgMsgInfo.TcpMAC[3],2);
                        Edit_CfgCurrMAC_E->Text = IntToHex(CfgMsgInfo.TcpMAC[4],2);
                        Edit_CfgCurrMAC_F->Text = IntToHex(CfgMsgInfo.TcpMAC[5],2);

                        CHG_CFG_ENDIAN( CfgMsgInfo.var_TCP_INIT_RETRY_TOUT);
                        CfgMsgInfo.var_TCP_INIT_RETRY_TOUT /= TCP_TIMERTIC;
                        Edit_TcpToutPrimerRetry->Text = IntToStr(CfgMsgInfo.var_TCP_INIT_RETRY_TOUT);
                        CHG_CFG_ENDIAN( CfgMsgInfo.var_TCP_RETRY_TOUT);
                        CfgMsgInfo.var_TCP_RETRY_TOUT /= TCP_TIMERTIC;
                        Edit_TcpToutReintentos->Text = IntToStr(CfgMsgInfo.var_TCP_RETRY_TOUT);
                        Edit_TcpRetries->Text = IntToStr(CfgMsgInfo.var_TCP_DEF_RETRIES);
                        break;

                case CFG_TCP_READ_STAT:
                        float TmpPercent;
                        memcpy(&(CfgMsgStat),&(RxConfigFrame.Data),sizeof(CfgMsgStat));
                        CHG_DWORD_ENDIAN( CfgMsgStat.MsgTxOk );
                        Edit_TxOk->Text = IntToStr( CfgMsgStat.MsgTxOk );
                        CHG_DWORD_ENDIAN( CfgMsgStat.MsgTxErr );
                        Edit_TxErr->Text = IntToStr( CfgMsgStat.MsgTxErr );
                        CHG_DWORD_ENDIAN( CfgMsgStat.MsgRxOk );
                        Edit_RxOk->Text = IntToStr( CfgMsgStat.MsgRxOk );
                        CHG_DWORD_ENDIAN( CfgMsgStat.MsgRxErr );
                        Edit_RxErr->Text = IntToStr( CfgMsgStat.MsgRxErr );

                        if (( CfgMsgStat.MsgTxOk + CfgMsgStat.MsgTxErr ) != 0x00){
                                TmpPercent = (float)(CfgMsgStat.MsgTxOk *100)/(float)( CfgMsgStat.MsgTxOk + CfgMsgStat.MsgTxErr );
                                Edit_TxOkPercent->Text = ("  " + FloatToStrF ( TmpPercent, ffFixed, 5, 2 ) + "  %");
                                TmpPercent = 100 - TmpPercent;
                                Edit_TxErrPercent->Text = ("  " + FloatToStrF ( TmpPercent, ffFixed, 5, 2 ) + "  %");
                        }
                        if (( CfgMsgStat.MsgRxOk + CfgMsgStat.MsgRxErr ) != 0x00){
                                TmpPercent = (float)(CfgMsgStat.MsgRxOk *100)/(float)( CfgMsgStat.MsgRxOk + CfgMsgStat.MsgRxErr );
                                Edit_RxOkPercent->Text = ("  " + FloatToStrF ( TmpPercent, ffFixed, 5, 2 ) + "  %");
                                TmpPercent = 100 - TmpPercent;
                                Edit_RxErrPercent->Text = ("  " + FloatToStrF ( TmpPercent, ffFixed, 5, 2 ) + "  %");
                        }
                        CHG_CFG_ENDIAN(CfgMsgStat.MsgDogCntr);
                        Edit_DogCntr->Text = IntToStr( CfgMsgStat.MsgDogCntr );
                        CHG_CFG_ENDIAN(CfgMsgStat.MsgRstCntr);
                        Edit_RstCntr->Text = IntToStr( CfgMsgStat.MsgRstCntr );

                        CHG_CFG_ENDIAN(CfgMsgStat.WatchRstTmr);
                        Edit_EvtRst->Text = IntToStr( CfgMsgStat.WatchRstTmr );
                        Edit_EvtQMax->Text = IntToStr( CfgMsgStat.MaxMsgCntr );

                        Edit_FechaDia->Text = MyBcdToStr( CfgMsgStat.CurrentDateTime.Date );
                        Edit_FechaMes->Text = MyBcdToStr( CfgMsgStat.CurrentDateTime.Month );
                        Edit_FechaAnio->Text = MyBcdToStr( CfgMsgStat.CurrentDateTime.Year );

                        Edit_HoraHora->Text = MyBcdToStr( CfgMsgStat.CurrentDateTime.Hour );
                        Edit_HoraMinuto->Text = MyBcdToStr( CfgMsgStat.CurrentDateTime.Minute );
                        Edit_HoraSegundo->Text = MyBcdToStr( CfgMsgStat.CurrentDateTime.Second );

                        break;


                case CFG_TCP_GET_IOVALUES:
                        memcpy(&(CfgMsgGioState),&(RxConfigFrame.Data),sizeof(CfgMsgGioState));

                        CHG_FLOAT_ENDIAN( CfgMsgGioState.AAG_Input_A );
                        Edit_AAG_Inp1->Text = FloatToStrF(CfgMsgGioState.AAG_Input_A,ffFixed,15,3);
                        CHG_FLOAT_ENDIAN( CfgMsgGioState.AAG_Input_B );
                        Edit_AAG_Inp2->Text = FloatToStrF(CfgMsgGioState.AAG_Input_B,ffFixed,15,3);

                        CHG_FLOAT_ENDIAN( CfgMsgGioState.AAG_Output_A );
                        Edit_AAG_Out1->Text = FloatToStrF(CfgMsgGioState.AAG_Output_A,ffFixed,15,3);
                        CHG_FLOAT_ENDIAN( CfgMsgGioState.AAG_Output_B );
                        Edit_AAG_Out2->Text = FloatToStrF(CfgMsgGioState.AAG_Output_B,ffFixed,15,3);

                        CHG_FLOAT_ENDIAN( CfgMsgGioState.Temperatura );
                        Edit_Temperatura->Text = FloatToStrF(CfgMsgGioState.Temperatura,ffFixed,15,3);


/*
                        CHG_FLOAT_ENDIAN( CfgMsgGioState.AAG_Output_A );
                        Edit_AAG_Out1->Text = FloatToStrF(CfgMsgGioState.AAG_Output_A,ffFixed,15,3);
                        CHG_FLOAT_ENDIAN( CfgMsgGioState.AAG_Output_B );
                        Edit_AAG_Out2->Text = FloatToStrF(CfgMsgGioState.AAG_Output_B,ffFixed,15,3);
*/
                        CheckBox_DigInp_1->Checked = (CfgMsgGioState.DigInpVal & 0x01);
                        CheckBox_DigInp_2->Checked = (CfgMsgGioState.DigInpVal & 0x02);
                        CheckBox_DigInp_3->Checked = (CfgMsgGioState.DigInpVal & 0x04);
                        CheckBox_DigInp_4->Checked = (CfgMsgGioState.DigInpVal & 0x08);
                        CheckBox_DigInp_5->Checked = (CfgMsgGioState.DigInpVal & 0x10);
                        CheckBox_DigInp_6->Checked = (CfgMsgGioState.DigInpVal & 0x20);
 /*
                        CheckBox_DigOut_1->Checked = (CfgMsgGioState.DigOutVal & 0x01);
                        CheckBox_DigOut_2->Checked = (CfgMsgGioState.DigOutVal & 0x02);
                        CheckBox_DigOut_3->Checked = (CfgMsgGioState.DigOutVal & 0x04);
                        CheckBox_DigOut_4->Checked = (CfgMsgGioState.DigOutVal & 0x08);
                        CheckBox_DigOut_5->Checked = (CfgMsgGioState.DigOutVal & 0x10);
                        CheckBox_DigOut_6->Checked = (CfgMsgGioState.DigOutVal & 0x20);
 */
                        break;


                case CFG_TCP_READ_CTES:
 //               case CFG_TCP_WRITE_CTES:
                        memcpy(&(CfgMsgConst),&(RxConfigFrame.Data),sizeof(CfgMsgConst));
                        CHG_FLOAT_ENDIAN( CfgMsgConst.ConsignaFrec );
                        Edit_ConsignaFrec->Text = FloatToStrF(CfgMsgConst.ConsignaFrec,ffFixed,15,3);
                        CHG_FLOAT_ENDIAN( CfgMsgConst.RelacionTx );
                        Edit_RelacionTx->Text = FloatToStrF(CfgMsgConst.RelacionTx,ffFixed,15,3);
                        CHG_FLOAT_ENDIAN( CfgMsgConst.CoefResbal );
                        Edit_CoefResbal->Text = FloatToStrF(CfgMsgConst.CoefResbal,ffFixed,15,3);
                        CHG_FLOAT_ENDIAN( CfgMsgConst.RpmEnMotor );
                        Edit_RpmEnMotor->Text = FloatToStrF(CfgMsgConst.RpmEnMotor,ffFixed,15,3);

                        for (TmpCntr = 0 ; TmpCntr < SIZE_USR_TXT ; TmpCntr++)
                                TmpParam[TmpCntr] =  CfgMsgConst.UsrText[TmpCntr];
                        TmpParam[SIZE_USR_TXT] = '\0';
                        Edit_UsrText->Text = (AnsiString)TmpParam;
                        break;

                case CFG_TCP_READ_VER:
 //               case CFG_TCP_WRITE_VER:
                        memcpy(&(CfgMsgVersion),&(RxConfigFrame.Data),sizeof(CfgMsgVersion));

                        for (TmpCntr = 0 ; TmpCntr < SIZE_FW_VER ; TmpCntr++)
                                TmpParam[TmpCntr] =  CfgMsgVersion.FW_VersionHdr.FW_Version[TmpCntr];
                        TmpParam[SIZE_FW_VER] = '\0';
//                        Edit_FwVersion->Text = (AnsiString)TmpParam;
                        Edit_FwVersion->Text = (AnsiString)(CfgMsgVersion.FW_VersionHdr.FW_Version);

                        for (TmpCntr = 0 ; TmpCntr < SIZE_FW_FULL ; TmpCntr++)
                                TmpParam[TmpCntr] =  CfgMsgVersion.FW_VersionHdr.FW_VersionFullName[TmpCntr];
                        TmpParam[SIZE_FW_FULL] = '\0';
                        Edit_FwVersionFull->Text = (AnsiString)TmpParam ;

                         Edit_FechaFW->Text = (AnsiString)(CfgMsgVersion.FW_VersionHdr.DateOfCompilation );
                        Edit_HoraFW->Text = (AnsiString)(CfgMsgVersion.FW_VersionHdr.TimeOfCompilation );

                        for (TmpCntr = 0 ; TmpCntr < SIZE_HW_VER ; TmpCntr++)
                                TmpParam[TmpCntr] =  CfgMsgVersion.HW_Version[TmpCntr];
                        TmpParam[SIZE_HW_VER] = '\0';
                        Edit_HwVersion->Text = (AnsiString)TmpParam ;

                        for (TmpCntr = 0 ; TmpCntr < SIZE_SERIAL ; TmpCntr++)
                                TmpParam[TmpCntr] =  CfgMsgVersion.Serial[TmpCntr];
                        TmpParam[SIZE_SERIAL] = '\0';
                        Edit_Serial->Text = (AnsiString)TmpParam ;
                        break;

                case CFG_TCP_READ_FLASH:

                        switch (ReadFlashStage){
                            case READ_FLASH_VERIFY:
                                TxRetries = 0x00;
                                FramesRxOk++;
                                FramesTxOk++;
                                NextToTx++;
                                StatusBar_CFG->Panels->Items[1]->Text = ( "TxOK : " + IntToStr(FramesTxOk));
                                StatusBar_CFG->Panels->Items[2]->Text = ( "RxOK : " + IntToStr(FramesRxOk));
                                VerifyNextFwReg();
                                return;

                            case READ_FLASH_HDRUPDATE:
                                ReadFlashStage = READ_FLASH_HDRFACTORY;
                                QueueConfigFrame.OpCode = CFG_TCP_READ_FLASH;
                                QueueConfigFrame.Data[0] = '\0';
                                QueueConfigFrame.Parameter = FLASH_FACTORY_FWFILE;  // Direcci�n
                                CHG_CFG_ENDIAN( QueueConfigFrame.Parameter );
                                QueueConfigFrame.AuxParam  = 255;               // Longitud a leer
                                CHG_CFG_ENDIAN( QueueConfigFrame.AuxParam );
                                QueCfgFrameBusy = true;
                                QueueConfigFrame.Len = 0;

                                if ( (((TCfgFwHeader*)(&RxConfigFrame.Data[4]))->VersionInfo.FW_Version[0]) != 'G' ){
                                        strcpy ( (((TCfgFwHeader*)(&RxConfigFrame.Data[4]))->VersionInfo.FW_Version),
                                                 "---" );
                                        strcpy ( (((TCfgFwHeader*)(&RxConfigFrame.Data[4]))->VersionInfo.DateOfCompilation),
                                                 "---" );
                                        strcpy ( (((TCfgFwHeader*)(&RxConfigFrame.Data[4]))->VersionInfo.TimeOfCompilation),
                                                 "---" );
                                }

                                //ABR_TEST:2014-09-26
                                Edit_VersionDisponible->Text = (AnsiString)(((TCfgFwHeader*)(&RxConfigFrame.Data[4]))->VersionInfo.FW_VersionFullName);//FW_Version);
                                Edit_DisponibleFecha->Text = (AnsiString)(((TCfgFwHeader*)(&RxConfigFrame.Data[4]))->VersionInfo.DateOfCompilation);
                                Edit_DisponibleHora->Text = (AnsiString)(((TCfgFwHeader*)(&RxConfigFrame.Data[4]))->VersionInfo.TimeOfCompilation);

                                break;

                            case READ_FLASH_HDRFACTORY:
                                if ( (((TCfgFwHeader*)(&RxConfigFrame.Data[4]))->VersionInfo.FW_Version[0]) != 'G' ){
                                        strcpy ( (((TCfgFwHeader*)(&RxConfigFrame.Data[4]))->VersionInfo.FW_Version),
                                                 "---" );
                                        strcpy ( (((TCfgFwHeader*)(&RxConfigFrame.Data[4]))->VersionInfo.DateOfCompilation),
                                                 "---" );
                                        strcpy ( (((TCfgFwHeader*)(&RxConfigFrame.Data[4]))->VersionInfo.TimeOfCompilation),
                                                 "---" );
                                }

                                Edit_VersionDeFabrica->Text = (AnsiString)(((TCfgFwHeader*)(&RxConfigFrame.Data[4]))->VersionInfo.FW_Version);
                                Edit_FabricaFecha->Text = (AnsiString)(((TCfgFwHeader*)(&RxConfigFrame.Data[4]))->VersionInfo.DateOfCompilation);
                                Edit_FabricaHora->Text = (AnsiString)(((TCfgFwHeader*)(&RxConfigFrame.Data[4]))->VersionInfo.TimeOfCompilation);
                                break;

                            case READ_FLASH_DEBUG:
                                Edit_E2P_Data->Text = (AnsiString)((char *)(&RxConfigFrame.Data[0]));
                                break;


                            default:
                                break;
                        }
                        /*
                        for (TmpCntr = 0 ; TmpCntr < 50 ; TmpCntr++)
                            TmpParam[TmpCntr] =  RxConfigFrame.Data[TmpCntr];
                        TmpParam[50] = '\0';
                        Edit2->Text = (AnsiString)TmpParam;
*/
                        break;

                case CFG_TCP_WRITE_FLASH:
                case CFG_TCP_WRITE_FLASH256:
                        TxRetries = 0x00;
//                        Timer1->Enabled = true;     // Intervalo hasta el pr�ximo env�o.
//                        EnableSendButtons();
                        FramesRxOk++;
                        FramesTxOk++;
                        NextToTx++;
                        StatusBar_CFG->Panels->Items[1]->Text = ( "TxOK : " + IntToStr(FramesTxOk));
                        StatusBar_CFG->Panels->Items[2]->Text = ( "RxOK : " + IntToStr(FramesRxOk));

                        if (IMP_PageControl->ActivePageIndex == TAB_WEB)
                                SendNextWebReg();
                        else
                                SendNextFwReg();
                        return;

                case CFG_TCP_READ_EEPROM:
                        Edit_E2P_Data->Text = (AnsiString)((char *)(&RxConfigFrame.Data[0]));
                        break;


                case CFG_TCP_VALIDATE_FW:
                        MessageDlg(" Validaci�n CONFIRMADA del Firmware : \n " + Edit_VersionDisponible->Text,
                        mtInformation, TMsgDlgButtons() << mbOK, 0);
                        break;

                case CFG_TCP_LOG_REWIND:
                        QueueConfigFrame.Len = 0;
                        QueueConfigFrame.OpCode = CFG_TCP_LOG_READ;
                        QueueConfigFrame.Data[0] = '\0';
                        QueueConfigFrame.Parameter = 0; //  No se usa
                        CHG_CFG_ENDIAN( QueueConfigFrame.Parameter );
                        QueueConfigFrame.AuxParam  = 0;   // No se usa
                        CHG_CFG_ENDIAN( QueueConfigFrame.AuxParam );
                        QueCfgFrameBusy = true;
                        DisableSendButtons();
                        break;

                case CFG_TCP_LOG_READ:
                        if (RxConfigFrame.Len != 0)
                        {
                                Memo_LogsText->Lines->Add((AnsiString)((char *)(&RxConfigFrame.Data[0])));

                                QueueConfigFrame.Len = 0;
                                QueueConfigFrame.OpCode = CFG_TCP_LOG_READ;
                                QueueConfigFrame.Data[0] = '\0';
                                QueueConfigFrame.Parameter = 0; //  No se usa
                                CHG_CFG_ENDIAN( QueueConfigFrame.Parameter );
                                QueueConfigFrame.AuxParam  = 0;   // No se usa
                                CHG_CFG_ENDIAN( QueueConfigFrame.AuxParam );
                                QueCfgFrameBusy = true;
                                DisableSendButtons();
                        }
                        break;

                default:
                        break;
        }

        TxRetries = 0x00;
        Timer1->Enabled = true;     // Intervalo hasta el pr�ximo env�o.
        EnableSendButtons();
        FramesRxOk++;
        FramesTxOk++;
        NextToTx++;
        StatusBar_CFG->Panels->Items[1]->Text = ( "TxOK : " + IntToStr(FramesTxOk));
        StatusBar_CFG->Panels->Items[2]->Text = ( "RxOK : " + IntToStr(FramesRxOk));

}
//---------------------------------------------------------------------------



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%   FUNCIONES AUXILIARES
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
//
AnsiString  TGWY_Config::MyBcdToStr ( char BCD_Byte)
{
        AnsiString      TxtOut;

        TmpParam[0] = ((BCD_Byte & 0xF0)>>4)+'0';
        TmpParam[1] = (BCD_Byte & 0x0F)+'0';
        TmpParam[2] = '\0';

        return (AnsiString) TmpParam;

}

unsigned char  TGWY_Config::MyIntToBcd (unsigned short INT_Val)
{
        unsigned char     TmpCharVal_u, TmpCharVal_d, TmpCharVal;
        unsigned short    TmpIntVal;

        TmpIntVal = INT_Val /10;
        TmpIntVal *= 10;
        TmpCharVal_u = (unsigned char)( INT_Val - TmpIntVal);

        INT_Val /= 10;
        TmpIntVal = INT_Val /10;
        TmpIntVal *= 10;
        TmpCharVal_d = (unsigned char)( INT_Val - TmpIntVal);

        TmpCharVal = (( TmpCharVal_d & 0x0F) << 4 );
        TmpCharVal |= ( TmpCharVal_u & 0x0F);

        return TmpCharVal;

}
//---------------------------------------------------------------------------

void __fastcall TGWY_Config::RadioButton_HitachiJ300Click(TObject *Sender)
{
        StaticText_SIE_CmdWordA->Visible = false;
        Edit_SIE_CmdWordA->Visible = false;
}
//---------------------------------------------------------------------------

void __fastcall TGWY_Config::RadioButton_SieMicroMasterClick(TObject *Sender)
{
        StaticText_SIE_CmdWordA->Visible = true;
        Edit_SIE_CmdWordA->Visible = true;
}
//---------------------------------------------------------------------------


void __fastcall TGWY_Config::RadioButton_HitachiSJ300Click(TObject *Sender)
{
        StaticText_SIE_CmdWordA->Visible = false;
        Edit_SIE_CmdWordA->Visible = false;
}
//---------------------------------------------------------------------------

void __fastcall TGWY_Config::RadioButton_SieMasterDriveClick(
      TObject *Sender)
{
        StaticText_SIE_CmdWordA->Visible = false;
        Edit_SIE_CmdWordA->Visible = false;
}
//---------------------------------------------------------------------------

void  TGWY_Config::DisableSendButtons (void)
{
        BitBtn_Grabar->Enabled = false;
        SpeedButton_Reset->Enabled = false;
        BitBtn_Cancelar->Enabled = false;
        Button_Sincronizar->Enabled = false;
}
//---------------------------------------------------------------------------

void  TGWY_Config::EnableSendButtons (void)
{
        BitBtn_Grabar->Enabled = true;
        SpeedButton_Reset->Enabled = true;
        BitBtn_Cancelar->Enabled = true;
        Button_Sincronizar->Enabled = true;
}
//---------------------------------------------------------------------------


void __fastcall TGWY_Config::Button_TransferirClick(TObject *Sender)
{
        if (!RegFileCntr){
                ShowMessage ("Debe cargar un archivo primero");
                return;
        }

        NextTfrRegFile = 0;
        ProgressBar1->Position = NextTfrRegFile;

        QueueConfigFrame.Len = FwRegLen[NextTfrRegFile]; // 255;     //FwFile[NextTfrRegFile][0];
        QueueConfigFrame.OpCode = CFG_TCP_WRITE_FLASH;
        memcpy(&(QueueConfigFrame.Data),&(FwFile[NextTfrRegFile][0]), QueueConfigFrame.Len);
//        StrCat (QueueConfigFrame.Data ,Edit1->Text.c_str());
//        QueueConfigFrame.Parameter = StrToInt ( Edit3->Text );
        QueueConfigFrame.Parameter = FirstAddrRegFile + NextTfrRegFile;
        CHG_CFG_ENDIAN( QueueConfigFrame.Parameter );
        QueCfgFrameBusy = true;
        DisableSendButtons();

}
//---------------------------------------------------------------------------

void __fastcall TGWY_Config::Button_VerificarClick(TObject *Sender)
{

        if (!RegFileCntr){
                ShowMessage ("Debe cargar un archivo primero");
                return;
        }

        NextTfrRegFile = 0;
        ProgressBar1->Position = NextTfrRegFile;

        ReadFlashStage = READ_FLASH_VERIFY;
        QueueConfigFrame.Len = 0;
        QueueConfigFrame.OpCode = CFG_TCP_READ_FLASH;
        QueueConfigFrame.Data[0] = '\0';
        QueueConfigFrame.Parameter = FirstAddrRegFile + NextTfrRegFile; // Direcci�n
        CHG_CFG_ENDIAN( QueueConfigFrame.Parameter );
        QueueConfigFrame.AuxParam  = 255;               // Longitud a leer
        CHG_CFG_ENDIAN( QueueConfigFrame.AuxParam );
        QueCfgFrameBusy = true;
        DisableSendButtons();
/*
        QueueConfigFrame.Len = 255;
        QueueConfigFrame.OpCode = CFG_TCP_READ_FLASH;
        QueueConfigFrame.Data[0] = '\0';
        QueueConfigFrame.Parameter = StrToInt ( Edit3->Text );
        CHG_CFG_ENDIAN( QueueConfigFrame.Parameter );
        QueCfgFrameBusy = true;
        DisableSendButtons();
*/

}
//---------------------------------------------------------------------------



void __fastcall TGWY_Config::Button_BuscarArchivoClick(TObject *Sender)
{
        UINT8   CheckSum, TmpRead, CryptoRegLen;
        int     CharCntr, RegCntr;
        char    RegCharRead[512];
        unsigned char TmpCharRead[256];

    if (OpenDialog1->Execute()){
        ConfigFilePtr = fopen (OpenDialog1->FileName.c_str(), "rb");
        if (ConfigFilePtr == NULL ) {
                MessageDlg("No pudo abrirse el archivo: \n " + OpenDialog1->FileName ,
                mtError, TMsgDlgButtons() << mbOK, 0);
                return;
        }
        CharCntr = 0;
        RegFileCntr = 0;
        fseek(ConfigFilePtr, 0, SEEK_SET);

/*
        TmpRead = (char)fgetc(ConfigFilePtr);

        do{
                CharCntr = 0;
                (void)fgetc(ConfigFilePtr); // S
                TmpRead = tolower((char)fgetc(ConfigFilePtr));
                if ((TmpRead == '9')||(TmpRead == '8')) break;  // S9 o S8 : �ltimo registro

                TmpRead = tolower((char)fgetc(ConfigFilePtr));
                do{
                        RegCharRead[CharCntr++] = TmpRead;
                        TmpRead = tolower((char)fgetc(ConfigFilePtr));
                }while (TmpRead != '\r');
                (void)fgetc(ConfigFilePtr);        // 0x0A despu�s del 0x0D

                RegCharRead[CharCntr] = '\0';
                HexToBin( RegCharRead, &(FwFile[RegFileCntr][0]), 510);
                HexToBin( RegCharRead, TmpCharRead, 510);
                RegFileCntr++;
        }while (!feof(ConfigFilePtr));
 */

        RegCntr = 0;
        FwFile[RegCntr][0] = fgetc(ConfigFilePtr);
        FwRegLen[RegCntr] = FwFile[RegCntr][0];

        for ( CharCntr = 1; CharCntr <= FwFile[RegCntr][0] ; CharCntr++){
                FwFile[0][CharCntr] = fgetc(ConfigFilePtr);
        }
        RegCntr++;
        memcpy( &(FwHeader), &(FwFile[0][4]),  sizeof(TCfgFwHeader));
        RegFileCntr = (unsigned int)FwHeader.RegInFlashQtty;
        RegFileCntr--;

//-CRYPTO----------------------------------------------------------------
        CRYP_INI = (UINT8)(FwHeader.VersionInfo.TimeOfCompilation[7]); // unidad de segundos
        CRYP_INI = (((CRYP_INI * CRYP_MUL) + CRYP_SUM ) & 0xFF);

        do{
                FwFile[RegCntr][0] = fgetc(ConfigFilePtr);
                CryptoRegLen = FwFile[RegCntr][0] ^ CRYP_INI;
                CRYP_INI = (((CRYP_INI * CRYP_MUL) + CRYP_SUM ) & 0xFF);
                CheckSum = CryptoRegLen;
                FwRegLen[RegCntr] = CryptoRegLen;

                for ( CharCntr = 1; CharCntr <= CryptoRegLen ; CharCntr++){
                        FwFile[RegCntr][CharCntr] = fgetc(ConfigFilePtr);
                        // CRYPTO..
                        CheckSum += FwFile[RegCntr][CharCntr] ^ CRYP_INI;
                        CRYP_INI = (((CRYP_INI * CRYP_MUL) + CRYP_SUM ) & 0xFF);
                }
                if (CheckSum != 0xFF){
                        CharCntr = 0;
                        RegFileCntr = 0;
                        MessageDlg("Inconsistencia de integridad en el registro: " + IntToStr(RegCntr) + "  del archivo: \n " + OpenDialog1->FileName ,
                                     mtError, TMsgDlgButtons() << mbOK, 0);
                        fclose (ConfigFilePtr);
                        return;
//                        break;
                }

                RegCntr++;
                RegFileCntr--;
                if ((feof(ConfigFilePtr))&&(RegFileCntr)){
                        CharCntr = 0;
                        RegFileCntr = 0;
                        MessageDlg("Inconsistencia de longitud en el archivo: \n " + OpenDialog1->FileName ,
                                    mtError, TMsgDlgButtons() << mbOK, 0);
                        fclose (ConfigFilePtr);
                        return;
                }
        }while (RegFileCntr);

        fclose (ConfigFilePtr);

        RegFileCntr = FwHeader.RegInFlashQtty;
        Edit_ArchivoFW->Text =  OpenDialog1->FileName;
        Edit_Registros->Text =  IntToStr(RegFileCntr);
        if (FwHeader.VersionInfo.FW_VersionFullName)
                Edit_Version->Text = (AnsiString)FwHeader.VersionInfo.FW_VersionFullName;
        else
                Edit_Version->Text = (AnsiString)FwHeader.VersionInfo.FW_Version;

        Edit_FechaCarga->Text = (AnsiString)FwHeader.VersionInfo.DateOfCompilation;
        Edit_HoraCarga->Text = (AnsiString)FwHeader.VersionInfo.TimeOfCompilation;
        ProgressBar1->Position = 0;
        ProgressBar1->Min = 0;
        ProgressBar1->Max = RegFileCntr;

    }

}
//---------------------------------------------------------------------------

void  TGWY_Config::SendNextFwReg (void)
{

        NextTfrRegFile++;
        ProgressBar1->Position = NextTfrRegFile;

        if ( NextTfrRegFile > RegFileCntr ){
                MessageDlg(" Transferencia EXITOSA del Firmware: \n " + OpenDialog1->FileName ,
                mtInformation, TMsgDlgButtons() << mbOK, 0);

                // Encolar pedido de lectura de Versiones cargadas para actualizar campos
                ReadFlashStage = READ_FLASH_HDRUPDATE;
                QueueConfigFrame.OpCode = CFG_TCP_READ_FLASH;
                QueueConfigFrame.Data[0] = '\0';
                QueueConfigFrame.Parameter = FLASH_UPDATE_FWFILE;  // Direcci�n
                CHG_CFG_ENDIAN( QueueConfigFrame.Parameter );
                QueueConfigFrame.AuxParam  = 255;               // Longitud a leer
                CHG_CFG_ENDIAN( QueueConfigFrame.AuxParam );
                QueCfgFrameBusy = true;
                QueueConfigFrame.Len = 0;

                Timer1->Enabled = true;     // Intervalo hasta el pr�ximo env�o.
                EnableSendButtons();

                return;
        }


        ConfigFrame.Len = FwRegLen[NextTfrRegFile];  // FwFile[NextTfrRegFile][0];   //255;
        ConfigFrame.OpCode = CFG_TCP_WRITE_FLASH;
        memcpy(&(ConfigFrame.Data),&(FwFile[NextTfrRegFile][0]), ConfigFrame.Len);
        ConfigFrame.Parameter = FirstAddrRegFile + NextTfrRegFile;
        CHG_CFG_ENDIAN( ConfigFrame.Parameter );

        ConfigFrame.TxWindow = NextToTx;
        ConfigFrame.RxWindow = LastRx;

        IMP_ClientSocket->Socket->SendBuf(&ConfigFrame,(ConfigFrame.Len + OverHeadCfgSize));
        Timer1->Enabled = false;
        Timer2->Enabled = true;    // supervisi�n de recepci�n de respuesta.

}


//---------------------------------------------------------------------------

void  TGWY_Config::VerifyNextFwReg (void)
{
        unsigned char CmpCntr;

//         for (CmpCntr=0 ; CmpCntr < RxConfigFrame.Len ; CmpCntr++){
        //Len = FwFile[][0] , por encriptaci�n ahora est� en FwRegLen[]
        for (CmpCntr=0 ; CmpCntr < FwRegLen[NextTfrRegFile] ; CmpCntr++){  // FwFile[NextTfrRegFile][0]
               if ( FwFile[NextTfrRegFile][CmpCntr] !=  RxConfigFrame.Data[CmpCntr]){
                        Timer1->Enabled = true;     // Intervalo hasta el pr�ximo env�o.
                        EnableSendButtons();
                        MessageDlg(" FALL� la verificaci�n en el registro " +IntToStr(NextTfrRegFile) + " del Firmware: \n " + OpenDialog1->FileName ,
                                   mtError, TMsgDlgButtons() << mbOK, 0);
                        ProgressBar1->Position = 0;
                        return;
                }
        }

        NextTfrRegFile++;
        ProgressBar1->Position = NextTfrRegFile;

        if ( NextTfrRegFile > RegFileCntr ){
                Timer1->Enabled = true;     // Intervalo hasta el pr�ximo env�o.
                EnableSendButtons();
                MessageDlg(" Verificaci�n CORRECTA del Firmware: \n " + OpenDialog1->FileName ,
                           mtInformation, TMsgDlgButtons() << mbOK, 0);
                return;
        }

        ReadFlashStage = READ_FLASH_VERIFY;
        ConfigFrame.Len = 0;
        ConfigFrame.OpCode = CFG_TCP_READ_FLASH;
        ConfigFrame.Data[0] = '\0';
        ConfigFrame.Parameter = FirstAddrRegFile + NextTfrRegFile; // Direcci�n
        CHG_CFG_ENDIAN( ConfigFrame.Parameter );
        ConfigFrame.AuxParam  = 255;               // Longitud a leer
        CHG_CFG_ENDIAN( ConfigFrame.AuxParam );

        ConfigFrame.TxWindow = NextToTx;
        ConfigFrame.RxWindow = LastRx;

        IMP_ClientSocket->Socket->SendBuf(&ConfigFrame,(ConfigFrame.Len + OverHeadCfgSize));
        Timer1->Enabled = false;
        Timer2->Enabled = true;    // supervisi�n de recepci�n de respuesta.

}



void __fastcall TGWY_Config::RadioButton_FactoryClick(TObject *Sender)
{
        if ( RadioButton_Factory->Checked )
                FirstAddrRegFile = FLASH_FACTORY_FWFILE;
}
//---------------------------------------------------------------------------

void __fastcall TGWY_Config::RadioButton_UpgradeClick(TObject *Sender)
{
        if ( RadioButton_Upgrade->Checked )
                FirstAddrRegFile = FLASH_UPDATE_FWFILE;
}
//---------------------------------------------------------------------------

void __fastcall TGWY_Config::Button_ActualizarFWClick(TObject *Sender)
{

        if ( MessageDlg( "Se proceder� a la actualizaci�n de Firmrware\n   Presione Ok para confirmar la operaci�n ",
                            mtWarning,
                            TMsgDlgButtons() << mbOK << mbCancel,
                            Hlp_ActualizarFW) == mrCancel ){
                return;
        }

        QueueConfigFrame.Len = 0;
        QueueConfigFrame.OpCode = CFG_TCP_UPGRADE_FW;
        QueCfgFrameBusy = true;

}
//---------------------------------------------------------------------------

void __fastcall TGWY_Config::Button_ValidateClick(TObject *Sender)
{
        QueueConfigFrame.Len = 0;
        QueueConfigFrame.OpCode = CFG_TCP_VALIDATE_FW;
        QueCfgFrameBusy = true;

}
//---------------------------------------------------------------------------

void __fastcall TGWY_Config::Button_RecuperarClick(TObject *Sender)
{
        if ( MessageDlg( "Se proceder� a la recuperaci�n de Firmrware\n   Presione Ok para confirmar la operaci�n ",
                            mtWarning,
                            TMsgDlgButtons() << mbOK << mbCancel,
                            Hlp_ActualizarFW) == mrCancel ){
                return;
        }

        QueueConfigFrame.Len = 0;
        QueueConfigFrame.OpCode = CFG_TCP_RESTORE_FW;
        QueCfgFrameBusy = true;

        // Cerrar la conexi�n y reconectar despu�s de unos segundos..
        Timer_Reconect->Interval = 1000;
        Timer_Reconect->Enabled = true;

}
//---------------------------------------------------------------------------

void __fastcall TGWY_Config::Timer_ReconectTimer(TObject *Sender)
{
        if (IMP_ClientSocket->Active ){
                Timer_Reconect->Interval = 3000;
                SpeedButton_DesconectarClick(this);
                DisableSendButtons();

                MessageDlg(" Espere, se intentar� restablecer la conexi�n \n ",
                            mtInformation, TMsgDlgButtons() << mbOK, 0);

        }
        else{
                Timer_Reconect->Enabled = false;
                SpeedButton_ConectarClick(this);
        }
}
//---------------------------------------------------------------------------



void __fastcall TGWY_Config::Button_SetearSalidasClick(TObject *Sender)
{

        CfgMsgGioState.AAG_Output_A =(float)StrToFloat(Edit_AAG_OutSet1->Text);
        CHG_FLOAT_ENDIAN( CfgMsgGioState.AAG_Output_A );
        CfgMsgGioState.AAG_Output_B =(float)StrToFloat(Edit_AAG_OutSet2->Text);
        CHG_FLOAT_ENDIAN( CfgMsgGioState.AAG_Output_B );

        CfgMsgGioState.DigOutVal = 0x00;
        if (CheckBox_DigOut_1->Checked) CfgMsgGioState.DigOutVal |= 0x01;
        if (CheckBox_DigOut_2->Checked) CfgMsgGioState.DigOutVal |= 0x02;
        if (CheckBox_DigOut_3->Checked) CfgMsgGioState.DigOutVal |= 0x04;
        if (CheckBox_DigOut_4->Checked) CfgMsgGioState.DigOutVal |= 0x08;
        if (CheckBox_DigOut_5->Checked) CfgMsgGioState.DigOutVal |= 0x10;
        if (CheckBox_DigOut_6->Checked) CfgMsgGioState.DigOutVal |= 0x20;

        memcpy(&(QueueConfigFrame.Data), &(CfgMsgGioState),sizeof(CfgMsgGioState));
        QueueConfigFrame.Len = sizeof(CfgMsgGioState);
        QueueConfigFrame.OpCode = CFG_TCP_SET_IOVALUES;
        QueCfgFrameBusy = true;
        DisableSendButtons();

}
//---------------------------------------------------------------------------


void __fastcall TGWY_Config::Button_LeerEEPROMClick(TObject *Sender)
{

        QueueConfigFrame.Len = 0;
        QueueConfigFrame.OpCode = CFG_TCP_READ_EEPROM;
        QueueConfigFrame.Data[0] = '\0';
        QueueConfigFrame.Parameter = StrToInt(Edit_E2P_Addr->Text ); // Direcci�n
        CHG_CFG_ENDIAN( QueueConfigFrame.Parameter );
        QueueConfigFrame.AuxParam  = StrToInt(Edit_E2P_Len->Text );  // Longitud a leer
        CHG_CFG_ENDIAN( QueueConfigFrame.AuxParam );
        QueCfgFrameBusy = true;
        DisableSendButtons();

}
//---------------------------------------------------------------------------



void __fastcall TGWY_Config::Timer_DisconnectTimer(TObject *Sender)
{
        Timer_Disconnect->Enabled = false;
        SpeedButton_DesconectarClick(this);
        DisableSendButtons();
}
//---------------------------------------------------------------------------


void __fastcall TGWY_Config::Button_BuscarArchivoWebClick(TObject *Sender)
{

        UINT8   CheckSum, TmpRead, CryptoRegLen;
        unsigned int    FileCharCntr,FileWebSize;
        unsigned char  *FileCharPtr;

        int     FatIdxCntr, RegCntr;


    if (OpenDialog2->Execute()){
        WebFilePtr = fopen (OpenDialog2->FileName.c_str(), "r+b");
        if (WebFilePtr == NULL ) {
                if ( (MessageDlg("Archivo inexistente, desea crearlo ?\n " + OpenDialog1->FileName ,
                                mtError, TMsgDlgButtons()<<mbYes<<mbNo, 0)) == mrYes){

                        WebFilePtr = fopen (OpenDialog2->FileName.c_str(), "w+b");

                        // Borrar toda la estructura destinada al archivo web
                        FileCharPtr = (unsigned char *)(pWebFile);
                        for (FileCharCntr = 0; FileCharCntr < sizeof(TCfgWebFiles) ; FileCharCntr++){
                                *FileCharPtr = 0x00;
                                FileCharPtr++;
                                fputc (0x00,WebFilePtr);
                        }
                }
                else{
                        return;
                }
        }
        else{
                fseek(WebFilePtr, 0, SEEK_SET);
                FileCharPtr = (unsigned char *)(pWebFile);
                FileWebSize = sizeof(TCfgWebFiles);
                for (FileCharCntr = 0; FileCharCntr < FileWebSize ; FileCharCntr++){
                          *FileCharPtr = fgetc(WebFilePtr);
//                        if (*FileCharPtr == (unsigned char)EOF ){
//                                break;
//                        }
                        FileCharPtr++;
                }
                FileCharCntr++;


        }
        ProgressBar2->Max = 0;
        WebFilesQtty = 0x00;
        for (FatIdxCntr = 0 ; FatIdxCntr < SIZE_WEB_FATENTRIES ; FatIdxCntr++){
                if ( pWebFile->WebFat[FatIdxCntr].Filename[0] == '\0') break;
                Memo_WebFiles->Lines->Add((AnsiString)pWebFile->WebFat[FatIdxCntr].Filename);
                WebFilesQtty++;
                ProgressBar2->Max += 1;                                                 // por el reg de la fat
                ProgressBar2->Max += ((pWebFile->WebFat[FatIdxCntr].FileLenght)/256)+1;   // por los reg del archivo
        }
        Edit_WebFilesQtty->Text = IntToStr(WebFilesQtty);
        Edit_WebRegQtty->Text = (IntToStr(ProgressBar2->Max)+" / 512 ");
        ProgressBar3->Position = ProgressBar2->Max - WebFilesQtty;
    }

 //   fclose (WebFilePtr);
    Edit_ArchivoWeb->Text = OpenDialog2->FileName;


}
//---------------------------------------------------------------------------

void __fastcall TGWY_Config::Button_WebArgregarClick(TObject *Sender)
{

    unsigned int FirstWebReg, RegPageSize, NextFileWebReg;
    unsigned short FileSizeCntr, FileRegByteCntr, FileCurrentReg;
    char * TmpFileNamePtr;
    char TmpFileName[255];


    if (OpenDialog3->Execute()){
        PageFilePtr = fopen (OpenDialog3->FileName.c_str(), "rb");
        if (PageFilePtr == NULL ) {
                MessageDlg("No pudo abrirse el archivo: \n " + OpenDialog3->FileName ,
                mtError, TMsgDlgButtons() << mbOK, 0);
                return;
        }

        fseek(PageFilePtr, 0, SEEK_SET);

        if (WebFilesQtty){
                RegPageSize = (((unsigned int)(pWebFile->WebFat[(WebFilesQtty-1)].FileLenght))/256)+1;
                NextFileWebReg = (unsigned int)(pWebFile->WebFat[(WebFilesQtty-1)].FileAddr);
                NextFileWebReg += RegPageSize;        // REgistro (addr) donde debe comenzar el nuevo arvhivo.
                pWebFile->WebFat[WebFilesQtty].FileAddr = NextFileWebReg;
        }


        FileSizeCntr = 0;       // Tama�o total del archivo
        FileRegByteCntr  = 0;       // Tama�o dentro del registro actual
        FileCurrentReg = pWebFile->WebFat[WebFilesQtty].FileAddr;  // Registro actualmente escribiendo.

        while (!feof(PageFilePtr)){

                pWebFile->WebReg[FileCurrentReg][FileRegByteCntr] = (unsigned char)fgetc(PageFilePtr);
                FileSizeCntr++;
                FileRegByteCntr++;
                FileRegByteCntr &= 0x00FF;
                if ( !FileRegByteCntr ) FileCurrentReg++;

        }
        FileSizeCntr--;


        pWebFile->WebFat[WebFilesQtty].FileLenght = FileSizeCntr;

//        strcat (WebFile.WebFat[WebFilesQtty].Filename, OpenDialog3->FileName.c_str() );

        TmpFileName[0] = '\0';
        strcat (TmpFileName, OpenDialog3->FileName.c_str() );
        TmpFileNamePtr = TmpFileName;
        while ( *TmpFileNamePtr != '\0') TmpFileNamePtr++;
        while ( *TmpFileNamePtr != '\\') TmpFileNamePtr--;
        TmpFileNamePtr++;
        strcat (pWebFile->WebFat[WebFilesQtty].Filename, TmpFileNamePtr);
        Memo_WebFiles->Lines->Add((AnsiString)TmpFileNamePtr);

        ProgressBar2->Max += 1;                                                         // por el reg de la fat
        ProgressBar2->Max += ((pWebFile->WebFat[WebFilesQtty].FileLenght)/256)+1;         // por los reg del archivo

        WebFilesQtty++;
        Edit_WebFilesQtty->Text = IntToStr(WebFilesQtty);
        Edit_WebRegQtty->Text = (IntToStr(ProgressBar2->Max)+" / 512 ");
        ProgressBar3->Position = ProgressBar2->Max - WebFilesQtty;

    }


}
//---------------------------------------------------------------------------

void __fastcall TGWY_Config::Button_WebGrabarClick(TObject *Sender)
{
        unsigned int    FileCharCntr;
        unsigned char  *FileCharPtr;

        if (!WebFilesQtty){
                ShowMessage ("Debe cargar un archivo primero");
                return;
        }
        fseek(WebFilePtr, 0, SEEK_SET);
        FileCharPtr = (unsigned char *)(pWebFile);
        for (FileCharCntr = 0; FileCharCntr < sizeof(TCfgWebFiles) ; FileCharCntr++){
               fputc(*FileCharPtr, WebFilePtr);
                FileCharPtr++;
        }

}

//---------------------------------------------------------------------------


void  TGWY_Config::SendNextWebReg (void)
{
//   unsigned int PageByteCntr;

   if ( TransferWebStage == WEB_FAT_STAGE){
        NextTfrFatRegFile++;
        ProgressBar2->Position += 1;

        if ( NextTfrFatRegFile >= (unsigned int)WebFilesQtty ){

                TransferWebStage = WEB_FILES_STAGE;

                WebAllRegSize = (unsigned int)(pWebFile->WebFat[(WebFilesQtty-1)].FileAddr);
                WebAllRegSize += (((unsigned int)(pWebFile->WebFat[(WebFilesQtty-1)].FileLenght))/256)+1;

                NextTfrWebRegFile = 0;
                ConfigFrame.Len = 0xFF;
                ConfigFrame.OpCode = CFG_TCP_WRITE_FLASH256;

                memcpy(&(ConfigFrame.Data[0]),&(pWebFile->WebReg[NextTfrWebRegFile][0]), SIZE_WEB_REG);
//                for (PageByteCntr=0 ;PageByteCntr<SIZE_WEB_REG ;PageByteCntr++){
//                        ConfigFrame.Data[PageByteCntr] = pWebFile->WebReg[NextTfrWebRegFile][PageByteCntr];
//                }

                ConfigFrame.Parameter = FirstAddrWebRegFile + NextTfrWebRegFile;
                CHG_CFG_ENDIAN( ConfigFrame.Parameter );

                ConfigFrame.TxWindow = NextToTx;
                ConfigFrame.RxWindow = LastRx;
                IMP_ClientSocket->Socket->SendBuf(&ConfigFrame,(SIZE_WEB_REG + OverHeadCfgSize));


        }
        else{
                ConfigFrame.Len = sizeof (TCfgWebFAT);
                ConfigFrame.OpCode = CFG_TCP_WRITE_FLASH;
                memcpy(&(ConfigFrame.Data[0]),&(pWebFile->WebFat[NextTfrFatRegFile]), ConfigFrame.Len);
                CHG_DWORD_ENDIAN (((TCfgWebFAT*)(ConfigFrame.Data))->FileLenght);
                CHG_DWORD_ENDIAN (((TCfgWebFAT*)(ConfigFrame.Data))->FileAddr);

                ConfigFrame.Parameter = FirstAddrFatRegFile + NextTfrFatRegFile;
                CHG_CFG_ENDIAN( ConfigFrame.Parameter );

                ConfigFrame.TxWindow = NextToTx;
                ConfigFrame.RxWindow = LastRx;
                IMP_ClientSocket->Socket->SendBuf(&ConfigFrame,(ConfigFrame.Len + OverHeadCfgSize));

        }


        Timer1->Enabled = false;
        Timer2->Enabled = true;    // supervisi�n de recepci�n de respuesta.

   }
   else{

        NextTfrWebRegFile++;
        ProgressBar2->Position += 1;

        if ( NextTfrWebRegFile >= WebAllRegSize ){

                MessageDlg(" Transferencia EXITOSA de los archivos WEB: \n " + OpenDialog2->FileName ,
                mtInformation, TMsgDlgButtons() << mbOK, 0);

                Timer1->Enabled = true;     // Intervalo hasta el pr�ximo env�o.
                EnableSendButtons();
                return;

        }

        ConfigFrame.Len = 0xFF;
        ConfigFrame.OpCode = CFG_TCP_WRITE_FLASH256;
        
        memcpy(&(ConfigFrame.Data[0]),&(pWebFile->WebReg[NextTfrWebRegFile][0]), SIZE_WEB_REG);

        ConfigFrame.Parameter = FirstAddrWebRegFile + NextTfrWebRegFile;
        CHG_CFG_ENDIAN( ConfigFrame.Parameter );

        ConfigFrame.TxWindow = NextToTx;
        ConfigFrame.RxWindow = LastRx;

        IMP_ClientSocket->Socket->SendBuf(&ConfigFrame,(SIZE_WEB_REG + OverHeadCfgSize));
        Timer1->Enabled = false;
        Timer2->Enabled = true;    // supervisi�n de recepci�n de respuesta.

   }


}

//---------------------------------------------------------------------------

void __fastcall TGWY_Config::Button_TransferirWebClick(TObject *Sender)
{
        if (!WebFilesQtty){
                ShowMessage ("Debe cargar un archivo primero");
                return;
        }

        NextTfrFatRegFile = 0;
        ProgressBar2->Position = 0;
        TransferWebStage = WEB_FAT_STAGE;

        QueueConfigFrame.Len = sizeof (TCfgWebFAT);
        QueueConfigFrame.OpCode = CFG_TCP_WRITE_FLASH;
        memcpy(&(QueueConfigFrame.Data[0]),&(pWebFile->WebFat[NextTfrFatRegFile]), QueueConfigFrame.Len);
        CHG_DWORD_ENDIAN (((TCfgWebFAT*)(QueueConfigFrame.Data))->FileLenght);
        CHG_DWORD_ENDIAN (((TCfgWebFAT*)(QueueConfigFrame.Data))->FileAddr);

        QueueConfigFrame.Parameter = FirstAddrFatRegFile + NextTfrFatRegFile;
        CHG_CFG_ENDIAN( QueueConfigFrame.Parameter );
        QueCfgFrameBusy = true;
        DisableSendButtons();

}
//---------------------------------------------------------------------------

void __fastcall TGWY_Config::Button_ReadFlashClick(TObject *Sender)
{
        ReadFlashStage = READ_FLASH_DEBUG;
        QueueConfigFrame.Len = 0;
        QueueConfigFrame.OpCode = CFG_TCP_READ_FLASH;
        QueueConfigFrame.Data[0] = '\0';
        QueueConfigFrame.Parameter = StrToInt(Edit_E2P_Addr->Text ); // Direcci�n
        CHG_CFG_ENDIAN( QueueConfigFrame.Parameter );
        QueueConfigFrame.AuxParam  = StrToInt(Edit_E2P_Len->Text );  // Longitud a leer
        CHG_CFG_ENDIAN( QueueConfigFrame.AuxParam );
        QueCfgFrameBusy = true;
        DisableSendButtons();

}
//---------------------------------------------------------------------------

/*
void __fastcall TGWY_Config::TreeView1MouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  if ( Sender->ClassNameIs("TTreeView"))
  {
    TTreeView *pTV = (TTreeView *)Sender;
    pTV->Items->Delete(pTV->GetNodeAt(X,Y));
  }

}
*/


//----------------------------------------------------------------------------

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%   FUNCIONES ASOCIADAS A BOTONES DE COMANDOS
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
//


//void __fastcall TGWY_Config::Button_ActualizarClick(TObject *Sender)
void __fastcall TGWY_Config::ActualizarValores(TObject *Sender)
{
    if (QueCfgFrameBusy) return;          // Si hay alg�n mensaje pendiente de Tx.

    switch (IMP_PageControl->ActivePageIndex){

      case TAB_DNP:
            QueueConfigFrame.OpCode = CFG_TCP_READ_DNP;
        break;

      case TAB_485:
            QueueConfigFrame.OpCode = CFG_TCP_GET_SCICONFIG;
        break;

      case TAB_SETUP:
      case TAB_TCP:
      case TAB_VARIADORES:
        QueueConfigFrame.OpCode = CFG_TCP_READ;
        break;

      case TAB_ESTADISTICAS:
        QueueConfigFrame.OpCode = CFG_TCP_READ_STAT;
        break;

      case TAB_CONSTANTES:
        QueueConfigFrame.OpCode = CFG_TCP_READ_CTES;
        break;

      case TAB_VERSION:
        QueueConfigFrame.OpCode = CFG_TCP_READ_VER;
        break;

      case TAB_UPGRADE:
        ReadFlashStage = READ_FLASH_HDRUPDATE;
        QueueConfigFrame.OpCode = CFG_TCP_READ_FLASH;
        QueueConfigFrame.Data[0] = '\0';
        QueueConfigFrame.Parameter = FLASH_UPDATE_FWFILE;  // Direcci�n
        CHG_CFG_ENDIAN( QueueConfigFrame.Parameter );
        QueueConfigFrame.AuxParam  = 255;               // Longitud a leer
        CHG_CFG_ENDIAN( QueueConfigFrame.AuxParam );
        break;

     case TAB_PUERTOS:
        QueueConfigFrame.OpCode = CFG_TCP_GET_IOVALUES;
        break;

      default:
        return;
//        break;
   }

   QueCfgFrameBusy = true;
   QueueConfigFrame.Len = 0;
//   ConfigFrame.TxWindow= NextToTx;
//   ConfigFrame.RxWindow = LastRx;
//   IMP_ClientSocket->Socket->SendBuf(&ConfigFrame,2);

}
//---------------------------------------------------------------------------

void __fastcall TGWY_Config::Button_SincronizarClick(TObject *Sender)
{
        TDateTime  CurrDateTime = Now();
        unsigned short Year,Month,Day,Hour,Min,Sec,TmpShort;

        CurrDateTime.DecodeDate(&Year,&Month,&Day);
        CurrDateTime.DecodeTime(&Hour,&Min,&Sec,&TmpShort);

        TmpShort = Year / 100;
        TmpShort *= 100;
        Year -= TmpShort;

        Edit_FechaDia->Text = IntToStr( Day );
        Edit_FechaMes->Text = IntToStr( Month);
        Edit_FechaAnio->Text = IntToStr( Year );
        Edit_HoraHora->Text = IntToStr( Hour );
        Edit_HoraMinuto->Text = IntToStr( Min );
        Edit_HoraSegundo->Text = IntToStr( Sec);

        CfgMsgStat.CurrentDateTime.Date = MyIntToBcd (Day);
        CfgMsgStat.CurrentDateTime.Month = MyIntToBcd (Month);
        CfgMsgStat.CurrentDateTime.Year = MyIntToBcd (Year);
        CfgMsgStat.CurrentDateTime.Hour = MyIntToBcd (Hour);
        CfgMsgStat.CurrentDateTime.Minute = MyIntToBcd (Min);
        CfgMsgStat.CurrentDateTime.Second = MyIntToBcd (Sec);

        QueueConfigFrame.Len = sizeof(TMsgDateTime);
        QueueConfigFrame.OpCode = CFG_TCP_WRITE_DATE;
        memcpy(&(QueueConfigFrame.Data),&(CfgMsgStat.CurrentDateTime),sizeof(TMsgDateTime));
        QueCfgFrameBusy = true;
        DisableSendButtons();
//        IMP_ClientSocket->Socket->SendBuf(&ConfigFrame,(sizeof(TMsgDateTime)+ OverHeadCfgSize));


}
//---------------------------------------------------------------------------



void __fastcall TGWY_Config::IMP_PageControlChange(TObject *Sender)
{
/*
    switch (IMP_PageControl->ActivePageIndex){
      case TAB_ESTADISTICAS:
        Timer1->Interval = TMR1_ReadStatFrec;
        break;

      default:
        Timer1->Interval = TMR1_PollingFrec;
        break;
   }
   ActualizarValores(this);
*/
}

//---------------------------------------------------------------------------


void __fastcall TGWY_Config::TreeView1Change(TObject *Sender,
      TTreeNode *Node)
{
    int RowItem;

    // Si est� en la lista de Nombres de tabs, activar el TabIndex correspondiente
    if (ValueList_TabNames->FindRow(TreeView1->Selected->Text, RowItem) ){
        IMP_PageControl->ActivePageIndex = StrToInt(ValueList_TabNames->Values[TreeView1->Selected->Text]);
//        Edit_ConnectIP->Text = IntToStr(IMP_PageControl->ActivePageIndex);

        switch (IMP_PageControl->ActivePageIndex){
          case TAB_ESTADISTICAS:
            Timer1->Interval = TMR1_ReadStatFrec;
            break;

          default:
            Timer1->Interval = TMR1_PollingFrec;
            break;
       }
       ActualizarValores(this);

    }

}


//---------------------------------------------------------------------------

void __fastcall TGWY_Config::SpeedButton_ConectarClick(TObject *Sender)
{

        IMP_ClientSocket->Host =  Edit_ConnectIP -> Text;
        IMP_ClientSocket->Open();

 /*
        try
        {
                IMP_ClientSocket->Open();
        }
        catch (Exception &exception)
        {
                ShowMessage("No pudo estalecerse la conexi�n");
        }
*/

}
//---------------------------------------------------------------------------

void __fastcall TGWY_Config::SpeedButton_DesconectarClick(TObject *Sender)
{
      IMP_ClientSocket->Close();
      Timer1->Enabled = false;
      Timer2->Enabled = false;

}
//---------------------------------------------------------------------------

void __fastcall TGWY_Config::SpeedButton_ResetClick(TObject *Sender)
{
        QueueConfigFrame.Len = 1;
        QueueConfigFrame.OpCode = CFG_TCP_RESET;
        QueCfgFrameBusy = true;

}
//---------------------------------------------------------------------------

void __fastcall TGWY_Config::BitBtn_GrabarClick(TObject *Sender)
{
    switch (IMP_PageControl->ActivePageIndex){

      case TAB_DNP:
        CfgMsgDnpInfo.AddrMaster=(unsigned short)StrToInt(Edit_AddrDnpMaestro->Text);
        CfgMsgDnpInfo.AddrSlave=(unsigned short)StrToInt(Edit_AddrDnpEsclavo->Text);

        QueueConfigFrame.Len = sizeof(CfgMsgDnpInfo);
        QueueConfigFrame.OpCode = CFG_TCP_WRITE_DNP;
        memcpy(&(QueueConfigFrame.Data),&(CfgMsgDnpInfo),sizeof(CfgMsgDnpInfo));
        break;


      case TAB_485:
        CfgMsgSerialCfg.SCIA_ConfigScanRate = StrToInt(Edit_SRL_ScanRate->Text);
        CHG_CFG_ENDIAN( CfgMsgSerialCfg.SCIA_ConfigScanRate );
        CfgMsgSerialCfg.SCIB_ConfigScanRate = StrToInt(Edit_SRL_ScanRate_B->Text);
        CHG_CFG_ENDIAN( CfgMsgSerialCfg.SCIB_ConfigScanRate );

        CfgMsgSerialCfg.SCIA_TmrTxWait = StrToInt(Edit_SRL_TmrTxA->Text);
        CHG_CFG_ENDIAN( CfgMsgSerialCfg.SCIA_TmrTxWait );
        CfgMsgSerialCfg.SCIA_TmrRxWait = StrToInt(Edit_SRL_TmrRxA->Text);
        CHG_CFG_ENDIAN( CfgMsgSerialCfg.SCIA_TmrRxWait );
        CfgMsgSerialCfg.SCIA_TmrDrive = StrToInt(Edit_SRL_TmrDriveA->Text);
        CHG_CFG_ENDIAN( CfgMsgSerialCfg.SCIA_TmrDrive );

        CfgMsgSerialCfg.SCIB_TmrTxWait = StrToInt(Edit_SRL_TmrTxB->Text);
        CHG_CFG_ENDIAN( CfgMsgSerialCfg.SCIB_TmrTxWait );
        CfgMsgSerialCfg.SCIB_TmrRxWait = StrToInt(Edit_SRL_TmrRxB->Text);
        CHG_CFG_ENDIAN( CfgMsgSerialCfg.SCIB_TmrRxWait );
        CfgMsgSerialCfg.SCIB_TmrDrive = StrToInt(Edit_SRL_TmrDriveB->Text);
        CHG_CFG_ENDIAN( CfgMsgSerialCfg.SCIB_TmrDrive );

        CfgMsgSerialCfg.SCIA_ConfigNode = StrToInt(Edit_SRL_Node_A->Text);
        CfgMsgSerialCfg.SCIB_ConfigNode = StrToInt(Edit_SRL_Node_B->Text);

        BYTE    TmpCfgCode, TmpByte;
        TmpCfgCode = (ComboBox_BaudRate_A->ItemIndex)& CFG_SCI_BAUDRATE_MASK;
        TmpCfgCode ++;  // Empieza en 001=300 ,en cambio ItemIndex empieza en 0
        TmpByte =  ConfigSerialPort[ComboBox_Paridad_A->ItemIndex];
//        TmpByte++;      // Paridad es 01, 10 o 11, en cambio ItemIndex empieza en 0
//        TmpByte <<= 4;
//        TmpByte &= CFG_SCI_PARITY_MASK;
        TmpCfgCode |= TmpByte;
        TmpByte =  ComboBox_Mode_A->ItemIndex;
        TmpByte <<= 7;
        TmpByte &= CFG_SCI_PHYMODE_MASK;
        TmpCfgCode |= TmpByte;
        CfgMsgSerialCfg.SCIA_ConfigCode = TmpCfgCode;

        TmpCfgCode = (ComboBox_BaudRate_B->ItemIndex)& CFG_SCI_BAUDRATE_MASK;
        TmpCfgCode ++;  // Empieza en 001=300 ,en cambio ItemIndex empieza en 0
        TmpByte =  ConfigSerialPort[ComboBox_Paridad_B->ItemIndex];
//        TmpByte =  ComboBox_Paridad_B->ItemIndex;
//        TmpByte++;      // Paridad es 01, 10 o 11, en cambio ItemIndex empieza en 0
//        TmpByte <<= 4;
//        TmpByte &= CFG_SCI_PARITY_MASK;
        TmpCfgCode |= TmpByte;
        TmpByte =  ComboBox_Mode_B->ItemIndex;
        TmpByte <<= 7;
        TmpByte &= CFG_SCI_PHYMODE_MASK;
        TmpCfgCode |= TmpByte;
        CfgMsgSerialCfg.SCIB_ConfigCode = TmpCfgCode;

        QueueConfigFrame.Len = sizeof(CfgMsgSerialCfg);
        QueueConfigFrame.OpCode = CFG_TCP_SET_SCICONFIG;
        memcpy(&(QueueConfigFrame.Data),&(CfgMsgSerialCfg),sizeof(CfgMsgSerialCfg));
        break;


      case TAB_SETUP:
      case TAB_TCP:
      case TAB_VARIADORES:
/*
        if  ( RadioButton_HitachiJ300->Checked )
                CfgMsgInfo.DebugLevel = INVCODE_HIT_J300;
        if  ( RadioButton_SieMicroMaster->Checked )
                CfgMsgInfo.DebugLevel = INVCODE_SIE_MICROMASTER;
        if  ( RadioButton_HitachiSJ300->Checked )
                CfgMsgInfo.DebugLevel = INVCODE_HIT_SJ300;
        if  ( RadioButton_SieMasterDrive->Checked )
                CfgMsgInfo.DebugLevel = INVCODE_SIE_MASTERDRIVE;
 */
        CfgMsgInfo.DebugLevel = (unsigned char)ComboBox_SetupDebugLevel->ItemIndex;

        CfgMsgInfo.RunConfig = 0x00;
        if  ( CheckBox_RunCfgRTC->Checked )
                CfgMsgInfo.RunConfig |= CFG_RTC_RUNSTATE_MASK;
        if  ( CheckBox_RunCfgRTU->Checked )
                CfgMsgInfo.RunConfig |= CFG_RTU_RUNSTATE_MASK;

//        CfgMsgInfo.DebugLevel =(unsigned char)StrToInt(Edit_DebugLevel->Text);
        CfgMsgInfo.TcpIP[0]=(unsigned char)StrToInt(Edit_CfgCurrIP_A->Text);
        CfgMsgInfo.TcpIP[1]=(unsigned char)StrToInt(Edit_CfgCurrIP_B->Text);
        CfgMsgInfo.TcpIP[2]=(unsigned char)StrToInt(Edit_CfgCurrIP_C->Text);
        CfgMsgInfo.TcpIP[3]=(unsigned char)StrToInt(Edit_CfgCurrIP_D->Text);

        CfgMsgInfo.TcpMask[0]=(unsigned char)StrToInt(Edit_CfgCurrMask_A->Text);
        CfgMsgInfo.TcpMask[1]=(unsigned char)StrToInt(Edit_CfgCurrMask_B->Text);
        CfgMsgInfo.TcpMask[2]=(unsigned char)StrToInt(Edit_CfgCurrMask_C->Text);
        CfgMsgInfo.TcpMask[3]=(unsigned char)StrToInt(Edit_CfgCurrMask_D->Text);

        CfgMsgInfo.TcpGW[0]=(unsigned char)StrToInt(Edit_CfgCurrGW_A->Text);
        CfgMsgInfo.TcpGW[1]=(unsigned char)StrToInt(Edit_CfgCurrGW_B->Text);
        CfgMsgInfo.TcpGW[2]=(unsigned char)StrToInt(Edit_CfgCurrGW_C->Text);
        CfgMsgInfo.TcpGW[3]=(unsigned char)StrToInt(Edit_CfgCurrGW_D->Text);

        CfgMsgInfo.TcpMAC[0]=(unsigned char)StrToInt("0x" +(Edit_CfgCurrMAC_A->Text));
        CfgMsgInfo.TcpMAC[1]=(unsigned char)StrToInt("0x" +(Edit_CfgCurrMAC_B->Text));
        CfgMsgInfo.TcpMAC[2]=(unsigned char)StrToInt("0x" +(Edit_CfgCurrMAC_C->Text));
        CfgMsgInfo.TcpMAC[3]=(unsigned char)StrToInt("0x" +(Edit_CfgCurrMAC_D->Text));
        CfgMsgInfo.TcpMAC[4]=(unsigned char)StrToInt("0x" +(Edit_CfgCurrMAC_E->Text));
        CfgMsgInfo.TcpMAC[5]=(unsigned char)StrToInt("0x" +(Edit_CfgCurrMAC_F->Text));

        CfgMsgInfo.var_TCP_INIT_RETRY_TOUT = (unsigned char)StrToInt(Edit_TcpToutPrimerRetry->Text);
        CfgMsgInfo.var_TCP_INIT_RETRY_TOUT *= TCP_TIMERTIC;
        CHG_CFG_ENDIAN( CfgMsgInfo.var_TCP_INIT_RETRY_TOUT );
        CfgMsgInfo.var_TCP_RETRY_TOUT = (unsigned char)StrToInt(Edit_TcpToutReintentos->Text);
        CfgMsgInfo.var_TCP_RETRY_TOUT *= TCP_TIMERTIC;
        CHG_CFG_ENDIAN( CfgMsgInfo.var_TCP_RETRY_TOUT );
        CfgMsgInfo.var_TCP_DEF_RETRIES = (unsigned char)StrToInt(Edit_TcpRetries->Text);

        CfgMsgInfo.PollingFrec =(unsigned short)StrToInt(Edit_SRL_ScanRate->Text);
        CHG_CFG_ENDIAN( CfgMsgInfo.PollingFrec );
        CfgMsgInfo.SIE_CmdWordA =(unsigned short)StrToInt(Edit_SIE_CmdWordA->Text);
        CHG_CFG_ENDIAN( CfgMsgInfo.SIE_CmdWordA );

        QueueConfigFrame.Len = sizeof(CfgMsgInfo);
        QueueConfigFrame.OpCode = CFG_TCP_WRITE;
        memcpy(&(QueueConfigFrame.Data),&(CfgMsgInfo),sizeof(CfgMsgInfo));

        break;

      case TAB_CONSTANTES:
        CfgMsgConst.ConsignaFrec =(float)StrToFloat(Edit_ConsignaFrec->Text);
        CHG_FLOAT_ENDIAN( CfgMsgConst.ConsignaFrec );
        CfgMsgConst.RelacionTx =(float)StrToFloat(Edit_RelacionTx->Text);
        CHG_FLOAT_ENDIAN( CfgMsgConst.RelacionTx );
        CfgMsgConst.CoefResbal =(float)StrToFloat(Edit_CoefResbal->Text);
        CHG_FLOAT_ENDIAN( CfgMsgConst.CoefResbal );
        CfgMsgConst.RpmEnMotor =(float)StrToFloat(Edit_RpmEnMotor->Text);
        CHG_FLOAT_ENDIAN( CfgMsgConst.RpmEnMotor );
        CfgMsgConst.UsrText[0] = '\0';
        StrCat (CfgMsgConst.UsrText ,Edit_UsrText->Text.c_str());

        QueueConfigFrame.Len = sizeof(CfgMsgConst);
        QueueConfigFrame.OpCode = CFG_TCP_WRITE_CTES;
        memcpy(&(QueueConfigFrame.Data),&(CfgMsgConst),sizeof(CfgMsgConst));
        break;

      case TAB_VERSION:
//        La version de FW no se puede modificar...
//        CfgMsgVersion.FW_Version[0] = '\0';
//        StrCat (CfgMsgVersion.FW_Version ,Edit_FwVersion->Text.c_str());
        CfgMsgVersion.HW_Version[0] = '\0';
        StrCat (CfgMsgVersion.HW_Version ,Edit_HwVersion->Text.c_str());
        CfgMsgVersion.Serial[0] = '\0';
        StrCat (CfgMsgVersion.Serial ,Edit_Serial->Text.c_str());

        CfgMsgVersion.KeyCode = CfgKeyCode;
        CHG_CFG_ENDIAN( CfgMsgVersion.KeyCode );

        QueueConfigFrame.Len = sizeof(CfgMsgVersion);
        QueueConfigFrame.OpCode = CFG_TCP_WRITE_VER;
        memcpy(&(QueueConfigFrame.Data),&(CfgMsgVersion),sizeof(CfgMsgVersion));
        break;

//      case TAB_ESTADISTICAS:
      default:
        return;
//        break;
   }

   QueCfgFrameBusy = true;
   DisableSendButtons();
//   IMP_ClientSocket->Socket->SendBuf(&ConfigFrame,(ConfigFrame.Len + OverHeadCfgSize));


}
//---------------------------------------------------------------------------

void __fastcall TGWY_Config::BitBtn_CancelarClick(TObject *Sender)
{
    ActualizarValores(this);
}
//---------------------------------------------------------------------------




void __fastcall TGWY_Config::BitBtn_AyudaClick(TObject *Sender)
{

//  Application->HelpJump("SHM_contents0005");

        switch (IMP_PageControl->ActivePageIndex){
          case TAB_SETUP:
            Application->HelpCommand(HELP_CONTEXT, Hlp_General);
            break;

          case TAB_DNP:
            Application->HelpCommand(HELP_CONTEXT, Hlp_GatewayDNP3);
            break;

          case TAB_TCP:
            Application->HelpCommand(HELP_CONTEXT, Hlp_PuertoEthernet);
            break;

          case TAB_485:
            Application->HelpCommand(HELP_CONTEXT, Hlp_PuertosSerie);
            break;

          case TAB_ESTADISTICAS:
            Application->HelpCommand(HELP_CONTEXT, Hlp_Estadsticas);
            break;

          case TAB_CONSTANTES:
            Application->HelpCommand(HELP_CONTEXT, Hlp_Parmetros);
            break;

          case TAB_VERSION:
            Application->HelpCommand(HELP_CONTEXT, Hlp_IdentifiacinDel);
            break;

          case TAB_UPGRADE:
            Application->HelpCommand(HELP_CONTEXT, Hlp_ActualizarFW);
            break;

          case TAB_PUERTOS:
            Application->HelpCommand(HELP_CONTEXT, Hlp_Entradas);
            break;

          case TAB_WEB:
            Application->HelpCommand(HELP_CONTEXT, Hlp_ServidorWeb);
            break;

          case TAB_VARIADORES:
            Application->HelpCommand(HELP_CONTEXT, Hlp_Variadores);
            break;

          default:
            Application->HelpCommand(HELP_CONTEXT, Hlp_FrontPage);
            break;
       }


}
//---------------------------------------------------------------------------






void __fastcall TGWY_Config::SpeedButton_HelpClick(TObject *Sender)
{
    Application->HelpCommand(HELP_CONTEXT, Hlp_FrontPage);

}
//---------------------------------------------------------------------------




void __fastcall TGWY_Config::MenuAyudaPrincipalClick(TObject *Sender)
{
    Application->HelpCommand(HELP_CONTEXT, Hlp_FrontPage);

}
//---------------------------------------------------------------------------

void __fastcall TGWY_Config::MenuInformacionClick(TObject *Sender)
{

            Application->HelpCommand(HELP_CONTEXT, Hlp_AcercaDeGWY);

}
//---------------------------------------------------------------------------




void __fastcall TGWY_Config::MenuVersionDeSWClick(TObject *Sender)
{

    ShowMessage (AnsiString(" Versi�n de Software    : ") + AnsiString(SW_Version) +
                 AnsiString("\n Fecha de compilaci�n: ") + AnsiString(SW_Date) +
                 AnsiString("\n Hora de compilaci�n  : ") + AnsiString(SW_Time) );


}
//---------------------------------------------------------------------------


void __fastcall TGWY_Config::AcercadeGWYConfig1Click(TObject *Sender)
{

        AboutBox->Label_Version->Caption = AnsiString(" Versi�n de SW: ") + AnsiString(SW_Version);
        AboutBox->Label_Build->Caption = AnsiString("Compilaci�n: ") + AnsiString(SW_Date) +
                                        AnsiString(" - ") +  AnsiString(SW_Time);
        AboutBox->Show();
}
//---------------------------------------------------------------------------




void __fastcall TGWY_Config::Button_ActualizarLogsClick(TObject *Sender)
{

        //ReadLogStage = READ_LOG_REWIND;
        QueueConfigFrame.Len = 0;
        QueueConfigFrame.OpCode = CFG_TCP_LOG_REWIND;
        QueueConfigFrame.Data[0] = '\0';
        QueueConfigFrame.Parameter = 10; // Cantidad de logs
        CHG_CFG_ENDIAN( QueueConfigFrame.Parameter );
        QueueConfigFrame.AuxParam  = 0;   // No se usa
        CHG_CFG_ENDIAN( QueueConfigFrame.AuxParam );
        QueCfgFrameBusy = true;
        DisableSendButtons();

}
//---------------------------------------------------------------------------


void __fastcall TGWY_Config::Button_LimpiarLogsClick(TObject *Sender)
{
        Memo_LogsText->Lines->Clear();

}
//---------------------------------------------------------------------------

void __fastcall TGWY_Config::Button_DebugConectarClick(TObject *Sender)
{
        TMemoryStream *MyStream = new TMemoryStream();
        UdpDebug->RemoteHost = Edit_ConnectIP -> Text;
        //char UdpMsgTest[20]= "123";
        MyStream->Write("123",3);
        UdpDebug->SendStream(MyStream);

        //int j;
        //UdpDebug->SendBuffer(UdpMsgTest,3,j);
}
//---------------------------------------------------------------------------

void __fastcall TGWY_Config::UdpDebugDataReceived(TComponent *Sender,
      int NumberBytes, AnsiString FromIP, int Port)
{
        TDateTime  CurrDateTime = Now();

/*        unsigned short Year,Month,Day,Hour,Min,Sec,TmpShort;
        CurrDateTime.DecodeDate(&Year,&Month,&Day);
        CurrDateTime.DecodeTime(&Hour,&Min,&Sec,&TmpShort);
        TmpShort = Year / 100;
        TmpShort *= 100;
        Year -= TmpShort;
        Edit_FechaDia->Text = IntToStr( Day );
        */

        char UdpMsg[200];
        int j;
        if (NumberBytes<200)
                UdpDebug->ReadBuffer(UdpMsg, NumberBytes, j);
        UdpMsg[j-1] = 0;
        Memo_UdpLog->Lines->Add(CurrDateTime.DateTimeString()+ " > "+UdpMsg);
}
//---------------------------------------------------------------------------

void __fastcall TGWY_Config::Button_UdpLogLimpiarClick(TObject *Sender)
{
        Memo_UdpLog->Clear();
}
//---------------------------------------------------------------------------

void __fastcall TGWY_Config::ComboBox_IPSelect(TObject *Sender)
{
        int idx= ComboBox_IP->Text.AnsiPos("=");
        Edit_ConnectIP -> Text = ComboBox_IP->Text.SubString(idx+1,15);

}
//---------------------------------------------------------------------------



