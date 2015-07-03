//---------------------------------------------------------------------------

#ifndef GWY_CryptoMainH
#define GWY_CryptoMainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>

#include "GWY_CryptoMsgdef.h"
#include <ComCtrls.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>

//---------------------------------------------------------------------------
class TGWY_Crypto : public TForm
{
__published:	// IDE-managed Components
        TEdit *Edit_ArchivoOrigen;
        TButton *Button_CargarOrigen;
        TLabel *Label1;
        TLabel *Label2;
        TEdit *Edit_ArchivoDestino;
        TButton *Button_CargarDestino;
        TOpenDialog *OpenDialog1;
        TOpenDialog *OpenDialog2;
        TButton *Button_Grabar;
        TEdit *Edit_Registros;
        TLabel *Label3;
        TLabel *Label4;
        TEdit *Edit_Version;
        TLabeledEdit *LabeledEdit_Fecha;
        TLabeledEdit *LabeledEdit_Hora;
        TBevel *Bevel1;
        TBevel *Bevel2;
        TEdit *Edit_RestrictFrom;
        TEdit *Edit_RestrictTo;
        TLabel *Label5;
        TLabel *Label6;
        TLabel *Label7;
        TEdit *Edit_LimiteMsg;
        TLabel *Label8;
        TEdit *Edit_LimiteTiempo;
        TEdit *Edit_Autor;
        TLabel *Label9;
        TEdit *Edit_Info;
        TLabel *Label10;
        TLabel *Label12;
        TEdit *Edit_BackWard;
        TButton *Button_Consultar;
        TCheckBox *CheckBox_Autovalid;
        TCheckBox *CheckBox_OwnReset;
        TCheckBox *CheckBox_Test;
        TRadioGroup *RadioGroup1;
        TRadioButton *RadioButton_TrialMsg;
        TRadioButton *RadioButton_Libre;
        TCheckBox *CheckBox_TransferInfo;
        TRadioButton *RadioButton_TrialTiempo;
        TRadioButton *RadioButton_TrialSerial;
        TBevel *Bevel3;
        void __fastcall Button_CargarOrigenClick(TObject *Sender);
        void __fastcall Button_GrabarClick(TObject *Sender);
        void __fastcall Button_CargarDestinoClick(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall Button_ConsultarClick(TObject *Sender);
        void __fastcall RadioButton_LibreClick(TObject *Sender);
        void __fastcall RadioButton_TrialMsgClick(TObject *Sender);
        void __fastcall RadioButton_TrialTiempoClick(TObject *Sender);
        void __fastcall RadioButton_TrialSerialClick(TObject *Sender);

private:	// User declarations
        unsigned char   FwFile[MAX_FW_REG][MAX_MSG_SIZE];
        unsigned int    NextTfrRegFile;
        unsigned int    RegFileCntr;

//        TCfgFwVersion   VersionInfo;
        TCfgFwHeader    FwHeader;

        AnsiString MyBcdToStr ( char BCD_Byte);
        unsigned char MyIntToBcd (unsigned short INT_Val);

public:		// User declarations
        __fastcall TGWY_Crypto(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TGWY_Crypto *GWY_Crypto;
//---------------------------------------------------------------------------
#endif
