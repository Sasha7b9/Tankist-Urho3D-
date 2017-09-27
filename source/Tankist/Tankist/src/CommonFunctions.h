#pragma once


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CreateListFiles();

/// Добавляет модель с именем nameModel из ресурсов ноде node
void AddModelToNode(Node *node, char *nameModel, const Vector3 &offset);
/// Возвращает размеры и центр box
void GetDimensionsCenter(const BoundingBox &box, Vector3 &dimensions, Vector3 &center, float scale);


