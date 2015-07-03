//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <stdio.h>

#include "GWY_CryptoMain.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TGWY_Crypto *GWY_Crypto;

FILE *  SourceFilePtr;
FILE *  TargetFilePtr;

char    TmpParam[60];
char    TmpCntr;

UINT8    CRYP_INI;
UINT8    CRYP_MUL;
UINT8    CRYP_SUM;

//---------------------------------------------------------------------------
__fastcall TGWY_Crypto::TGWY_Crypto(TComponent* Owner)
        : TForm(Owner)
{
        CRYP_MUL = 13;
        CRYP_SUM = 1;
        FwHeader.RestrictType           = FW_RESTRICT_FREE;
        Edit_RestrictFrom->Text         = "A-000000";
        Edit_RestrictTo->Text           = "A-000000";
        FwHeader.TrialMsgLimit          = 0;
        FwHeader.TrialTimeLimit         = 0;
        FwHeader.TrialOtherLimit        = 0;

}
//---------------------------------------------------------------------------



//****************************************************************************
//              FUNCIONES AUXILIARES.
//****************************************************************************
//


void __fastcall TGWY_Crypto::Button_CargarOrigenClick(TObject *Sender)
{

        UINT8 TmpRead;
        int  CharCntr, FileBufCntr;
        char RegCharRead[512];
        unsigned char TmpCharRead[256];
        unsigned int  RegFirstAddress;

    if (OpenDialog1->Execute()){
        SourceFilePtr = fopen (OpenDialog1->FileName.c_str(), "rb");
        if (SourceFilePtr == NULL ) {
                MessageDlg("No pudo abrirse el archivo: \n " + OpenDialog1->FileName ,
                mtError, TMsgDlgButtons() << mbOK, 0);
                return;
        }
        CharCntr = 0;
        RegFileCntr = 0;
        fseek(SourceFilePtr, 0, SEEK_SET);
        TmpRead = (char)fgetc(SourceFilePtr);

        do{
                CharCntr = 0;
                (void)fgetc(SourceFilePtr); // S
                TmpRead = tolower((char)fgetc(SourceFilePtr));
                if ((TmpRead == '9')||(TmpRead == '8')) break;  // S9 o S8 : último registro

                TmpRead = tolower((char)fgetc(SourceFilePtr));
                do{
                        RegCharRead[CharCntr++] = TmpRead;
                        TmpRead = tolower((char)fgetc(SourceFilePtr));
                }while (TmpRead != '\r');
                (void)fgetc(SourceFilePtr);        // 0x0A después del 0x0D

                RegCharRead[CharCntr] = '\0';
                HexToBin( RegCharRead, &(FwFile[RegFileCntr][0]), 510);

                RegFirstAddress = *((unsigned int*)(&(FwFile[RegFileCntr][0])));
                CHG_DWORD_ENDIAN(RegFirstAddress);
                RegFirstAddress &= 0x00FFFFFF;
                if (RegFirstAddress == 0x3C8000){
                        memcpy( &(FwHeader.VersionInfo),
                                &(FwFile[RegFileCntr][4]),
                                sizeof(TCfgFwVersion));
                }
                RegFileCntr++;

        }while (!feof(SourceFilePtr));


        fclose (SourceFilePtr);

        for (FileBufCntr = 0 ; FileBufCntr < (MAX_MSG_SIZE-1) ; FileBufCntr++)
                FwFile[0][FileBufCntr] = 0xFF;

        FwFile[0][0] = sizeof(TCfgFwHeader) + 4;  // Len del header + ADDRESS + CHECKSUM

        FwHeader.RegInFlashQtty = RegFileCntr;
        memcpy( &(FwFile[0][4]), &(FwHeader), sizeof(TCfgFwHeader));

        Edit_ArchivoOrigen->Text =  OpenDialog1->FileName;
        Edit_Registros->Text =  IntToStr(FwHeader.RegInFlashQtty);
        Edit_Version->Text = (AnsiString)(FwHeader.VersionInfo.FW_Version);
        LabeledEdit_Fecha->Text =(AnsiString)(FwHeader.VersionInfo.DateOfCompilation);
        LabeledEdit_Hora->Text =(AnsiString)(FwHeader.VersionInfo.TimeOfCompilation);
        if (FwHeader.VersionInfo.FW_Type & FW_FACTORY)
                CheckBox_Autovalid->Checked = true;
        else
                CheckBox_Autovalid->Checked = false;

        if (FwHeader.VersionInfo.FW_Type & FW_OWN_RESET)
                CheckBox_OwnReset->Checked = true;
        else
                CheckBox_OwnReset->Checked = false;
                
        if (FwHeader.VersionInfo.FW_Type & FW_TEST)
                CheckBox_Test->Checked = true;
        else
                CheckBox_Test->Checked = false;


       Edit_BackWard->Text = (AnsiString)(FwHeader.VersionInfo.FW_BackWardCompatibility);
    }

}
//---------------------------------------------------------------------------

void __fastcall TGWY_Crypto::Button_CargarDestinoClick(TObject *Sender)
{
    if (OpenDialog2->Execute()){
         Edit_ArchivoDestino->Text =  OpenDialog2->FileName;
    }

}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//

void __fastcall TGWY_Crypto::Button_GrabarClick(TObject *Sender)
{

        int     CharCntr, RegCntr;
        UINT8   CrypRegLen;


        if (!RegFileCntr){
                ShowMessage ("Debe cargar un archivo de FW primero");
                return;
        }

        FwHeader.TrialMsgLimit = StrToInt(Edit_LimiteMsg->Text);
        CHG_DWORD_ENDIAN(FwHeader.TrialMsgLimit);

        FwHeader.TrialTimeLimit = StrToInt(Edit_LimiteTiempo->Text);
        CHG_DWORD_ENDIAN(FwHeader.TrialTimeLimit);

        strcpy (FwHeader.RestrictFromSerial, Edit_RestrictFrom->Text.c_str());
        strcpy (FwHeader.RestrictToSerial, Edit_RestrictTo->Text.c_str());
        strcpy (FwHeader.FW_Autor, Edit_Autor->Text.c_str());
        strcpy (FwHeader.FW_Info, Edit_Info->Text.c_str());

        // Por ahora actualizar siempre la info de limitaciones. Bit_1
        if ( CheckBox_TransferInfo->Checked )
                FwHeader.TransferInfo = 0x03;
        else
                FwHeader.TransferInfo = 0x02;

        memcpy( &(FwFile[0][4]), &(FwHeader), sizeof(TCfgFwHeader));

        TargetFilePtr = fopen (OpenDialog2->FileName.c_str(), "wb");
        if (!TargetFilePtr){
                ShowMessage ("Debe elegir un nombre de archivo DESTINO");
                return;
        }

        RegCntr = 0;
        fseek(TargetFilePtr, 0, SEEK_SET);

        // encripto una parte del header
        CRYP_INI = (UINT8)(FwHeader.VersionInfo.TimeOfCompilation[6]); // unidad de segundos
        CRYP_INI = (((CRYP_INI * CRYP_MUL) + CRYP_SUM ) & 0xFF);
        if (FwFile[0][0] < 106 ){
                ShowMessage("Error: incosistencia en la longitud del HEADER");
                return;
        }
        for ( CharCntr = 106; CharCntr <= FwFile[0][0] ; CharCntr++){
                (FwFile[RegCntr][CharCntr]) ^= CRYP_INI;
                CRYP_INI = (((CRYP_INI * CRYP_MUL) + CRYP_SUM ) & 0xFF);
        }

        // Grabo el header (sin encriptar la primera parte)...
        for ( CharCntr = 0; CharCntr <= FwFile[0][0] ; CharCntr++){
                fputc( (FwFile[RegCntr][CharCntr]), TargetFilePtr);
        }
        RegCntr++;
        RegFileCntr--;

        CRYP_INI = (UINT8)(FwHeader.VersionInfo.TimeOfCompilation[7]); // unidad de segundos
        CRYP_INI = (((CRYP_INI * CRYP_MUL) + CRYP_SUM ) & 0xFF);

        do{
                CrypRegLen = FwFile[RegCntr][0];

                for ( CharCntr = 0; CharCntr <= CrypRegLen ; CharCntr++){
                        fputc( ((FwFile[RegCntr][CharCntr])^CRYP_INI), TargetFilePtr);
                        CRYP_INI = (((CRYP_INI * CRYP_MUL) + CRYP_SUM ) & 0xFF);

                }

                RegCntr++;
                RegFileCntr--;
        }while (RegFileCntr);

        fclose (TargetFilePtr);

        ShowMessage ("Fin de la conversión..");

}


//------------------------------------------------------------------------------
//-------------------------------------------------------------

AnsiString  TGWY_Crypto::MyBcdToStr ( char BCD_Byte)
{
        AnsiString      TxtOut;

        TmpParam[0] = ((BCD_Byte & 0xF0)>>4)+'0';
        TmpParam[1] = (BCD_Byte & 0x0F)+'0';
        TmpParam[2] = '\0';

        return (AnsiString) TmpParam;

}

unsigned char  TGWY_Crypto::MyIntToBcd (unsigned short INT_Val)
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

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

void __fastcall TGWY_Crypto::FormCreate(TObject *Sender)
{
        int FileBufCntr;

        for (FileBufCntr = 0 ; FileBufCntr < (MAX_MSG_SIZE-1) ; FileBufCntr++)
                FwFile[0][FileBufCntr] = 0xFF;

        for (FileBufCntr = 1 ; FileBufCntr < MAX_FW_REG ; FileBufCntr++)
                memcpy(&(FwFile[FileBufCntr][0]),&(FwFile[0][0]), MAX_MSG_SIZE);


}
//---------------------------------------------------------------------------


void __fastcall TGWY_Crypto::Button_ConsultarClick(TObject *Sender)
{

    char        TmpRead;
    int         CharCntr;

    if (OpenDialog2->Execute()){
        SourceFilePtr = fopen (OpenDialog2->FileName.c_str(), "rb");
        if (SourceFilePtr == NULL ) {
                MessageDlg("No pudo abrirse el archivo: \n " + OpenDialog1->FileName ,
                mtError, TMsgDlgButtons() << mbOK, 0);
                return;
        }
        CharCntr = 0;
        fseek(SourceFilePtr, 0, SEEK_SET);

        FwFile[0][0] = fgetc(SourceFilePtr);
        for ( CharCntr = 1; CharCntr <= FwFile[0][0] ; CharCntr++){
                FwFile[0][CharCntr] = fgetc(SourceFilePtr);
        }

        // desencripto la segunda parte del header
        memcpy( &(FwHeader), &(FwFile[0][4]),  sizeof(TCfgFwHeader));  // para ver la hora
        CRYP_INI = (UINT8)(FwHeader.VersionInfo.TimeOfCompilation[6]); // unidad de segundos
        CRYP_INI = (((CRYP_INI * CRYP_MUL) + CRYP_SUM ) & 0xFF);
        if (FwFile[0][0] < 106 ){
                ShowMessage("Error: incosistencia en la longitud del HEADER");
                return;
        }
        for ( CharCntr = 106; CharCntr <= FwFile[0][0] ; CharCntr++){
                (FwFile[0][CharCntr]) ^= CRYP_INI;
                CRYP_INI = (((CRYP_INI * CRYP_MUL) + CRYP_SUM ) & 0xFF);
        }

        memcpy( &(FwHeader), &(FwFile[0][4]),  sizeof(TCfgFwHeader));

        fclose (SourceFilePtr);

        if ( FwHeader.TransferInfo & 0x01 )
                CheckBox_TransferInfo->Checked = true;
        else
                CheckBox_TransferInfo->Checked = false;

        RadioButton_Libre->Checked = false;
        RadioButton_TrialMsg->Checked = false;
        RadioButton_TrialTiempo->Checked = false;
        RadioButton_TrialSerial->Checked = false;

        switch (FwHeader.RestrictType){
                case FW_RESTRICT_FREE:
                        RadioButton_Libre->Checked = true;
                        break;

                case FW_RESTRICT_MSG:
                        RadioButton_TrialMsg->Checked = true;
                        break;

                case FW_RESTRICT_TIME:
                        RadioButton_TrialTiempo->Checked = true;
                        break;

                case FW_RESTRICT_SERIAL:
                        RadioButton_TrialSerial->Checked = true;
                        break;

        }


        Edit_Registros->Text =  IntToStr(FwHeader.RegInFlashQtty);
        Edit_Version->Text = (AnsiString)(FwHeader.VersionInfo.FW_Version);
        LabeledEdit_Fecha->Text =(AnsiString)(FwHeader.VersionInfo.DateOfCompilation);
        LabeledEdit_Hora->Text =(AnsiString)(FwHeader.VersionInfo.TimeOfCompilation);

        Edit_RestrictFrom->Text = (AnsiString)(FwHeader.RestrictFromSerial);
        Edit_RestrictTo->Text = (AnsiString)(FwHeader.RestrictToSerial);
        Edit_Autor->Text = (AnsiString)(FwHeader.FW_Autor);
        Edit_Info->Text = (AnsiString)(FwHeader.FW_Info);

        CHG_DWORD_ENDIAN(FwHeader.TrialMsgLimit);
        Edit_LimiteMsg->Text = (AnsiString)(IntToStr(FwHeader.TrialMsgLimit));

        CHG_DWORD_ENDIAN(FwHeader.TrialTimeLimit);
        Edit_LimiteTiempo->Text = (AnsiString)(IntToStr(FwHeader.TrialTimeLimit));

        Edit_LimiteMsg->Text     = IntToStr(FwHeader.TrialMsgLimit);
        Edit_LimiteTiempo->Text  = IntToStr(FwHeader.TrialTimeLimit);

        if (FwHeader.VersionInfo.FW_Type & FW_FACTORY)
                CheckBox_Autovalid->Checked = true;
        else
                CheckBox_Autovalid->Checked = false;

        if (FwHeader.VersionInfo.FW_Type & FW_OWN_RESET)
                CheckBox_OwnReset->Checked = true;
        else
                CheckBox_OwnReset->Checked = false;

        if (FwHeader.VersionInfo.FW_Type & FW_TEST)
                CheckBox_Test->Checked = true;
        else
                CheckBox_Test->Checked = false;


       Edit_BackWard->Text = (AnsiString)(FwHeader.VersionInfo.FW_BackWardCompatibility);
    }

}
//---------------------------------------------------------------------------

/*
        for (CharCntr = 1 ; CharCntr <= (FwHeader.RegInFlashQtty) ; CharCntr++){

                for (FileBufCntr = 0 ; FileBufCntr <= CrypRegLen ; FileBufCntr++){
                        FwFile[CharCntr][FileBufCntr] ^= CRYP_INI;
                        CRYP_INI = (((CRYP_INI * CRYP_MUL) + CRYP_SUM ) & 0xFF);
                }
        }
*/

void __fastcall TGWY_Crypto::RadioButton_LibreClick(TObject *Sender)
{
        FwHeader.RestrictType = FW_RESTRICT_FREE;
        
}
//---------------------------------------------------------------------------

void __fastcall TGWY_Crypto::RadioButton_TrialMsgClick(TObject *Sender)
{
        FwHeader.RestrictType = FW_RESTRICT_MSG;
}
//---------------------------------------------------------------------------

void __fastcall TGWY_Crypto::RadioButton_TrialTiempoClick(TObject *Sender)
{
        FwHeader.RestrictType = FW_RESTRICT_TIME;
}
//---------------------------------------------------------------------------

void __fastcall TGWY_Crypto::RadioButton_TrialSerialClick(TObject *Sender)
{
        FwHeader.RestrictType = FW_RESTRICT_SERIAL;
}
//---------------------------------------------------------------------------

