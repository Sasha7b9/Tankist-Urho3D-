#include <stdafx.h>


#include "ChatLog.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ChatLog::ChatLog(Context *context) : Object(context)
{
    file = new File(context, "chat.log", FILE_WRITE);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
ChatLog::~ChatLog()
{
    file->Close();
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
void ChatLog::WriteMessage(const String &message)
{
    file->WriteLine(message);
    file->Flush();
}