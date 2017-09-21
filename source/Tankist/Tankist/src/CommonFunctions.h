#pragma once


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CreateListFiles();

Node *GetNodeByName(Scene *scene, char *name);
/// ���������� ������ � ������ name �� �����
StaticModel *GetStaticModel(Scene *scene, char *name);
/// ��������� ������ � ������ nameModel �� �������� ���� node
void AddModelToNode(Node *node, char *nameModel, const Vector3 &offset);
/// ���������� ������� � ����� box
void GetDimensionsCenter(const BoundingBox &box, Vector3 &dimensions, Vector3 &center, float scale);


