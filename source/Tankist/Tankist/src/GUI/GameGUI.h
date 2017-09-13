#pragma once


/*
    ÃÓÉ èãðû
*/


URHO3D_EVENT(E_NEW_VAR, NewVar)
{
    URHO3D_PARAM(P_VAR, Var);
    URHO3D_PARAM(P_VALUE, Value);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class GameGUI : public Object
{
    URHO3D_OBJECT(GameGUI, Object);

public:

    GameGUI(Context *context);

    void SetVar(const String &var, const String &value);

private:

    SharedPtr<Text> window;

    Vector<String> vars;
    Vector<String> values;

    void HandleNewVar(StringHash, VariantMap&);

    GameGUI(GameGUI const&) : GameGUI(nullptr)
    {};
    GameGUI operator=(GameGUI const &)
    {};
};
