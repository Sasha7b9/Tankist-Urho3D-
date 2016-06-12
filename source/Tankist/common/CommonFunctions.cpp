#include <stdafx.h>

#include "CommonFunctions.h"

using namespace Urho3D;

//---------------------------------------------------------------------------------------------------------------------------------------------------
void CreateListFiles()
{
    Vector<String> files;

    gFileSystem->ScanDir(files, ".", "*", SCAN_FILES | SCAN_HIDDEN, true);

    File file(gContext, "files.txt", FILE_WRITE);

    files.Remove(".");
    files.Remove("..");
    files.Remove("files.txt");
    files.Remove("Tankist");

    for (String fileName : files)
    {
        Vector<String> parts = fileName.Split('/');

        if (parts[parts.Size() - 1] != "." && parts[parts.Size() - 1] != "..")
        {
            File nextFile(gContext, fileName);
            uint checkSum = nextFile.GetChecksum();
            uint size = nextFile.GetSize();
            nextFile.Close();

            file.WriteLine(fileName + " " + String(checkSum) + " " + String(size));
        }
    }

    file.Close();
}