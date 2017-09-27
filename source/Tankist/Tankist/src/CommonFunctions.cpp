/* (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by */
#include <stdafx.h>
#include "CommonFunctions.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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

//---------------------------------------------------------------------------------------------------------------------------------------------------
void AddModelToNode(Node *node, char *nameModel, const Vector3 &offset)
{
    Node *nodeModel = node->CreateChild(nameModel);
    StaticModel *model = nodeModel->CreateComponent<StaticModel>();
    model->SetModel(gCache->GetResource<Model>(nameModel));
    nodeModel->SetPosition(offset);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
void GetDimensionsCenter(const BoundingBox &box, Vector3 &dimensions, Vector3 &center, float scale)
{
    dimensions.x_ = (box.max_.x_ - box.min_.x_) * scale;
    dimensions.y_ = (box.max_.y_ - box.min_.y_) * scale;
    dimensions.z_ = (box.max_.z_ - box.min_.z_) * scale;

    center.x_ = (box.min_.x_ + box.max_.x_) / 2.0f;
    center.y_ = (box.min_.y_ + box.max_.y_) / 2.0f;
    center.z_ = (box.min_.z_ + box.max_.z_) / 2.0f;
}
