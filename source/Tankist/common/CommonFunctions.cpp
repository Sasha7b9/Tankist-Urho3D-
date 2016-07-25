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


//---------------------------------------------------------------------------------------------------------------------------------------------------
Node *GetNodeByName(Scene *scene, char *name)
{
    PODVector<Node*> nodes;
    scene->GetChildren(nodes, true);

    for(Node *node : nodes)
    {
        String nameNode = node->GetName();

        if(strcmp(nameNode.CString(), name) == 0)
        {
            Vector<SharedPtr<Component>> components = node->GetComponents();

            LOG_INFOF("type name is %s, num components is %d, type name is %s", node->GetTypeName().CString(), components.Size(), components[0]->GetTypeName().CString());

            return node;
        }
    }

    return nullptr;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
StaticModel *GetStaticModel(Scene *scene, char *name)
{
    Node *node = GetNodeByName(scene, name);

    if(node)
    {
        return node->GetComponent<StaticModel>();
    }

    return nullptr;
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
void GetDimensionsCenter(const BoundingBox &box, Vector3 &dimensions, Vector3 &center)
{
    dimensions.x_ = box.max_.x_ - box.min_.x_;
    dimensions.y_ = box.max_.y_ - box.min_.y_;
    dimensions.z_ = box.max_.z_ - box.min_.z_;

    center.x_ = (box.min_.x_ + box.max_.x_) / 2.0f;
    center.y_ = (box.min_.y_ + box.max_.y_) / 2.0f;
    center.z_ = (box.min_.z_ + box.max_.z_) / 2.0f;
}
