#ifndef wAjaxToolsH
#define wAjaxToolsH

//------------------------------------------------------------------------------------
// Definiciones de CONSTANTES
//------------------------------------------------------------------------------------
//
const char https_ack_page_OK        []  = "HTTP/1.1 200 OK\r\nKeep-Alive: timeout=5, max=100\r\nConnection: Keep-Alive\r\nContent-Type: text/xml\r\n\r\n";
const char https_ack_page_Accepted  []  = "HTTP/1.0 202 Accepted\r\n";
const char https_ack_page_Void      []  = "HTTP/1.0 204 No content\r\n";
const char https_ack_page_Reset     []  = "HTTP/1.0 205 Reset Content\r\n";
const char https_ack_page_Bad       []  = "HTTP/1.0 400 Bad Request\r\n";

const char scsHeader  []  = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<GWY>\n";
const char scsTrailer []  = "</GWY>\n";

//------------------------------------------------------------------------------------
//  VARIABLES GLOBALES
//------------------------------------------------------------------------------------
//
const unsigned char scsWdDataLine   [] = "<%s>%u</%s>\n";			
//const unsigned char scsFtDataLine [] = "<%s>%.2f</%s>\n";			
const unsigned char scsSzDataLine   [] = "<%s>%s</%s>\n";			
const unsigned char scsXWdDataLine  [] = "<%s>%X</%s>\n";			

BYTE    sndbuf[450];  // ABR_ToDo: utilizar luego buffers de MEM_manager.
WORD    LogParam;
BYTE    TagFlags;
WORD    TagPolFrec;


//------------------------------------------------------------------------------------ 
// Prototipos de FUNCIONES
//------------------------------------------------------------------------------------
//
void  Fn2A          (char *pcString, float fValue, int nPrecision);
float atof1         (const char * p);
char* StripPlus     (char *ptr);
float ptrToFloat    (char *ptrA);
void  SYS_LongToStr (UINT32 val, UINT8 * DataStr );

void LoadValue(char *tmpbuf, int iBufSize, USR_DYNAMIC_FIELDS *ptrUDI);
void SaveValue(char *ptr, USR_DYNAMIC_FIELDS *ptrUDI);

UINT16 GetNameValuePair(UINT16 wLen, char* pszName, char* pszValue);
USR_DYNAMIC_FIELDS* FindTableName(char* pszName, USR_DYNAMIC_FIELDS *ptrUDI);
USR_DYNAMIC_FIELDS* FindTableNameFull(char* pszName, USR_DYNAMIC_FIELDS *ptrUDI);

char* XMLEntityOut(char* myData, char* sOut);

#endif // wAjaxToolsH