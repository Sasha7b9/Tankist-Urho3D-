#pragma once


void CreateListFiles();

Node *GetNodeByName(Scene *scene, char *name);

StaticModel *GetStaticModel(Scene *scene, char *name);

void AddModelToNode(Node *node, char *nameModel, const Vector3 &offset);

// Return dimensions and center bounding box
void GetDimensionsCenter(const BoundingBox &box, Vector3 &dimensions, Vector3 &center, float scale);
