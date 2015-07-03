object GWY_Crypto: TGWY_Crypto
  Left = 260
  Top = 118
  Width = 499
  Height = 331
  Caption = 'GWY_Crypto'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 16
    Top = 40
    Width = 47
    Height = 16
    Caption = 'Origen'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label2: TLabel
    Left = 16
    Top = 80
    Width = 54
    Height = 16
    Caption = 'Destino'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label3: TLabel
    Left = 24
    Top = 160
    Width = 68
    Height = 16
    Caption = 'Registros'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label4: TLabel
    Left = 24
    Top = 136
    Width = 54
    Height = 16
    Caption = 'Versi'#243'n'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Bevel1: TBevel
    Left = 16
    Top = 232
    Width = 449
    Height = 57
    Shape = bsFrame
  end
  object Bevel2: TBevel
    Left = 16
    Top = 120
    Width = 449
    Height = 113
    Shape = bsFrame
  end
  object Label5: TLabel
    Left = 32
    Top = 312
    Width = 131
    Height = 16
    Caption = 'Restricci'#243'n desde '
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label6: TLabel
    Left = 280
    Top = 312
    Width = 39
    Height = 16
    Caption = 'hasta'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label7: TLabel
    Left = 32
    Top = 344
    Width = 130
    Height = 16
    Caption = 'Limitaci'#243'n de MSG'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label8: TLabel
    Left = 32
    Top = 368
    Width = 150
    Height = 16
    Caption = 'Limitaci'#243'n de Tiempo'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label9: TLabel
    Left = 32
    Top = 480
    Width = 41
    Height = 16
    Caption = 'Autor:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label10: TLabel
    Left = 32
    Top = 504
    Width = 30
    Height = 16
    Caption = 'Info:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label12: TLabel
    Left = 286
    Top = 201
    Width = 108
    Height = 16
    Caption = 'Backward Com.'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Bevel3: TBevel
    Left = 16
    Top = 304
    Width = 449
    Height = 233
    Shape = bsFrame
  end
  object Edit_ArchivoOrigen: TEdit
    Left = 80
    Top = 32
    Width = 385
    Height = 24
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 0
  end
  object Button_CargarOrigen: TButton
    Left = 435
    Top = 34
    Width = 27
    Height = 18
    Caption = '...'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 1
    OnClick = Button_CargarOrigenClick
  end
  object Edit_ArchivoDestino: TEdit
    Left = 80
    Top = 72
    Width = 385
    Height = 24
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 2
  end
  object Button_CargarDestino: TButton
    Left = 435
    Top = 74
    Width = 27
    Height = 18
    Caption = '...'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 3
    OnClick = Button_CargarDestinoClick
  end
  object Button_Grabar: TButton
    Left = 256
    Top = 248
    Width = 75
    Height = 25
    Caption = 'Grabar'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 4
    OnClick = Button_GrabarClick
  end
  object Edit_Registros: TEdit
    Left = 112
    Top = 160
    Width = 137
    Height = 24
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 5
  end
  object Edit_Version: TEdit
    Left = 112
    Top = 136
    Width = 137
    Height = 24
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 6
  end
  object LabeledEdit_Fecha: TLabeledEdit
    Left = 328
    Top = 136
    Width = 121
    Height = 24
    EditLabel.Width = 52
    EditLabel.Height = 16
    EditLabel.Caption = 'Fecha: '
    EditLabel.Font.Charset = DEFAULT_CHARSET
    EditLabel.Font.Color = clWindowText
    EditLabel.Font.Height = -15
    EditLabel.Font.Name = 'MS Sans Serif'
    EditLabel.Font.Style = [fsBold]
    EditLabel.ParentFont = False
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    LabelPosition = lpLeft
    LabelSpacing = 3
    ParentFont = False
    TabOrder = 7
  end
  object LabeledEdit_Hora: TLabeledEdit
    Left = 328
    Top = 160
    Width = 121
    Height = 24
    EditLabel.Width = 51
    EditLabel.Height = 16
    EditLabel.Caption = 'Hora:   '
    EditLabel.Font.Charset = DEFAULT_CHARSET
    EditLabel.Font.Color = clWindowText
    EditLabel.Font.Height = -15
    EditLabel.Font.Name = 'MS Sans Serif'
    EditLabel.Font.Style = [fsBold]
    EditLabel.ParentFont = False
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    LabelPosition = lpLeft
    LabelSpacing = 3
    ParentFont = False
    TabOrder = 8
  end
  object Edit_RestrictFrom: TEdit
    Left = 168
    Top = 312
    Width = 105
    Height = 24
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 9
  end
  object Edit_RestrictTo: TEdit
    Left = 336
    Top = 312
    Width = 113
    Height = 24
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 10
  end
  object Edit_LimiteMsg: TEdit
    Left = 200
    Top = 344
    Width = 249
    Height = 24
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 11
    Text = '0'
  end
  object Edit_LimiteTiempo: TEdit
    Left = 200
    Top = 368
    Width = 249
    Height = 24
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 12
    Text = '0'
  end
  object Edit_Autor: TEdit
    Left = 80
    Top = 472
    Width = 273
    Height = 24
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 13
  end
  object Edit_Info: TEdit
    Left = 80
    Top = 496
    Width = 273
    Height = 24
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 14
  end
  object Edit_BackWard: TEdit
    Left = 400
    Top = 192
    Width = 49
    Height = 24
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 15
  end
  object Button_Consultar: TButton
    Left = 384
    Top = 248
    Width = 75
    Height = 25
    Caption = 'Consultar'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 16
    OnClick = Button_ConsultarClick
  end
  object CheckBox_Autovalid: TCheckBox
    Left = 96
    Top = 200
    Width = 74
    Height = 17
    Alignment = taLeftJustify
    Caption = 'Factory'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 17
  end
  object CheckBox_OwnReset: TCheckBox
    Left = 184
    Top = 200
    Width = 82
    Height = 17
    Alignment = taLeftJustify
    Caption = 'OwnRST'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 18
  end
  object CheckBox_Test: TCheckBox
    Left = 24
    Top = 200
    Width = 58
    Height = 17
    Alignment = taLeftJustify
    Caption = 'Test'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 19
  end
  object RadioGroup1: TRadioGroup
    Left = 32
    Top = 392
    Width = 417
    Height = 49
    TabOrder = 20
  end
  object RadioButton_TrialMsg: TRadioButton
    Left = 128
    Top = 409
    Width = 89
    Height = 17
    Caption = 'Por Msg'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 21
    OnClick = RadioButton_TrialMsgClick
  end
  object RadioButton_Libre: TRadioButton
    Left = 56
    Top = 410
    Width = 65
    Height = 17
    Caption = 'Libre'
    Checked = True
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 22
    TabStop = True
    OnClick = RadioButton_LibreClick
  end
  object CheckBox_TransferInfo: TCheckBox
    Left = 80
    Top = 456
    Width = 281
    Height = 17
    Caption = 'Transferir Informaci'#243'n a la EEPROM'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 23
  end
  object RadioButton_TrialTiempo: TRadioButton
    Left = 216
    Top = 409
    Width = 121
    Height = 17
    Caption = 'Por Tiempo'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 24
    OnClick = RadioButton_TrialTiempoClick
  end
  object RadioButton_TrialSerial: TRadioButton
    Left = 336
    Top = 409
    Width = 97
    Height = 17
    Caption = 'Por Serial'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 25
    OnClick = RadioButton_TrialSerialClick
  end
  object OpenDialog1: TOpenDialog
    Filter = 'S19|*.S19'
    Left = 8
    Top = 8
  end
  object OpenDialog2: TOpenDialog
    Filter = 'GWY|*.gwy'
    Left = 40
    Top = 8
  end
end
